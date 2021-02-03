#include "ddsLoader.h"
#include <cstring>
#include <fstream>
#include <iostream>

/// WXWIDGETS STUFF
/// ==============================================================================
template<class T> inline bool is_open(const T&);
template<> inline bool is_open<std::ifstream>(const std::ifstream& fp) {return fp.is_open();}
template<> inline bool is_open<wxInputStream>(const wxInputStream& is) {return is.CanRead();}

template<class T> inline bool good(const T&);
template<> inline bool good<std::ifstream>(const std::ifstream& fp) {return fp.good();}
template<> inline bool good<wxInputStream>(const wxInputStream& is) {return is.CanRead() && !is.Eof();}

template<class T> inline void read(T&, char* buffer, size_t size);
template<> inline void read<std::ifstream>(std::ifstream& fp, char* buffer, size_t size) { fp.read(buffer, size); }
template<> inline void read<wxInputStream>(wxInputStream& is, char* buffer, size_t size) { is.Read(buffer, size); }

template<class T> inline void close(T&);
template<> inline void close<std::ifstream>(std::ifstream& fp) { fp.close(); }
template<> inline void close<wxInputStream>(wxInputStream&)    { }


/// DDS FILE
/// ==============================================================================
template<typename T> void FileDDS::LoadFromStream(T& stream)
{
    if (!is_open(stream))
    {
        std::cerr << "Could not open DDS stream for reading"  << std::endl;
        return;
    }

    // check file code
    {
        char file_code[4];
        read(stream, file_code, sizeof(file_code));

        if ((memcmp(file_code, "DDS ", sizeof(file_code)) != 0) || !good(stream))
        {
            std::cerr << "Bad DDS file-code in header: '";
            std::cerr.write(file_code, sizeof(file_code));
            std::cout << "' " << std::endl;

            bail:
            close(stream);
            return;
        }
    }

    // read the header
    {
        memset(&header, 0, sizeof(header));
        read(stream, (char*)(void*)&header, sizeof(header));

        if (!good(stream))
        {
            std::cerr << "Unexpected end of file while parsing header" << std::endl;
            goto bail;
        }

        // check for header sanity
        if (header.dwSize != 124)
        {
            std::cerr << "Incorrect header size in dwSize: expected 124, found " << header.dwSize << std::endl;
            goto bail;
        }

        if (header.ddspf.dwSize != 32)
        {
            std::cerr << "Incorrect dwSize for pixel_format: expected 32, found " << header.ddspf.dwSize << std::endl;
            goto bail;
        }
    }

    // check other requirements
    {
        // it's a texture
        if (!(header.dwFlags & DDS_HEADER_FLAGS_TEXTURE))
        {
            std::cerr << "Required flags DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT are not set: this file is not texture" << std::endl;
            goto bail;
        }

        // make sure this is not a cubemap
        if (header.dwCaps2)
        {
            std::cerr << "DDS files containing cubemap data are not supported!" << std::endl;
            goto bail;
        }
    }

    // load the rest of the data to the buffer
    const unsigned int blockSize = GetBlockSize();
    const unsigned int mipcount = GetMipmapCount();
    unsigned int width = GetWidth();
    unsigned int height = GetHeight();

    if (width <= 0 || height <= 0 || blockSize <= 0)
        return;

    bOk = true; // file is now OK, unless we find error below

    mipmaps.reserve(mipcount);
    for (unsigned int level = 0; level < mipcount && (width || height); ++level)
    {
        // add mip map object to list
        mipmaps.emplace_back( MipmapDDS(width, height, header.ddspf.dwFourCC) );
        MipmapDDS& dest = mipmaps.back();

        // prepare the buffer
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        dest.data.reserve(size); // allocate all at once
        dest.data.resize(size);  // make sure data is accessible

        // read file to buffer
        read(stream, dest.data.data(), size);

        if (!good(stream))
        {
            bOk = false;
            close(stream);
            return;
        }

        // next mipmap is half the size...
        if ((width /= 2) < 1) width = 1;         // Deal with Non-Power-Of-Two textures.
        if ((height /= 2) < 1) height = 1;
    }

    close(stream);
}

FileDDS::FileDDS(const char* path)
{
    std::ifstream fp(path, std::ios::in | std::ios::binary);
    LoadFromStream(fp);
}

FileDDS::FileDDS(wxInputStream& input)
{
    LoadFromStream(input);
}

/// DECOMPRESSION
/// ==============================================================================
#include "dxt_decompress.h"

template<size_t n>
static bool DecompressDXT(const char* src, unsigned int w, unsigned int h, std::vector<unsigned char>& dest)
{
    const size_t rgbaSize = w * h * 4;
    dest.reserve(rgbaSize);
    dest.resize(rgbaSize);

    //std::cout << "DecompressDXT<" << n << ">: size " << w << "x" << h << " buffer size " << rgbaSize << std::endl;

    DXT_Decompressor<n>::DecompressImage(w, h, reinterpret_cast<const unsigned char*>(src), reinterpret_cast<unsigned long*>(dest.data()));

    //std::cout << "Done" << std::endl;

    return true;
}

bool MipmapDDS::Decompress(std::vector<unsigned char>& dest) const
{
    switch (FourCC)
    {
        case FOURCC_DXT1:
            return DecompressDXT<1>(data.data(), width, height, dest);
        break;

        case FOURCC_DXT3:
            return DecompressDXT<3>(data.data(), width, height, dest);
        break;

        case FOURCC_DXT5:
            return DecompressDXT<5>(data.data(), width, height, dest);
        break;

        default:
            return false;
        break;
    }
}

