#ifndef _DDS_FILE_LOADER_H_
#define _DDS_FILE_LOADER_H_

#include <vector>
#include "dds_defs.h"
#include <wx/stream.h>
#include <wx/artprov.h>

class MipmapDDS
{
private:
    // only constructible by the File class
    inline MipmapDDS(unsigned int w, unsigned int h, uint32_t fourcc) :
        width(w), height(h), FourCC(fourcc)
    {
    }

    // movable, but no assigning, no copying
    MipmapDDS(const MipmapDDS&) = delete;
    MipmapDDS& operator=(const MipmapDDS&) = delete;

    std::vector<char> data;

    friend class FileDDS;

public:
    MipmapDDS(MipmapDDS&&) = default; // movable

    inline const char* GetData() const { return data.data(); };
    bool Decompress(std::vector<unsigned char>& dest) const;

    const unsigned int width;
    const unsigned int height;
    const uint32_t FourCC;
};

class FileDDS
{
private:
    template<typename T> void LoadFromStream(T&);

    FileDDS(const FileDDS&) = delete;            // no copying
    FileDDS& operator=(const FileDDS&) = delete; // no assigning

    bool bOk = false;
    std::vector<MipmapDDS> mipmaps;

public:
    DDS_HEADER header;

    FileDDS(const char* path);
    FileDDS(wxInputStream& input);

    inline bool IsOk() const
    {
        return bOk;
    }

    inline unsigned int GetWidth() const
    {
        return (header.dwFlags & DDSD_WIDTH) ? (unsigned int)header.dwWidth : 0;
    }

    inline unsigned int GetHeight() const
    {
        return (header.dwFlags & DDSD_HEIGHT) ? (unsigned int)header.dwHeight : 0;
    }

    inline unsigned int GetMipmapCount() const
    {
        return (header.dwFlags & DDSD_MIPMAPCOUNT) ? (unsigned int)header.dwMipMapCount : 1;
    }

    inline unsigned int GetBlockSize() const
    {
        if (!(header.ddspf.dwFlags & DDPF_FOURCC))
            return 0; // not supported

        switch (header.ddspf.dwFourCC)
        {
            case FOURCC_DXT1:
                return 8;
            break;

            case FOURCC_DXT3:
            case FOURCC_DXT5:
                return 16;
            break;

            default:
                return 0;
            break;
        }
    }

    inline const MipmapDDS* GetMipmap(unsigned int level) const
    {
        if (level >= mipmaps.size())
            return NULL;

        return &mipmaps.at(level);
    }
};

#endif // _DDS_FILE_LOADER_H_
