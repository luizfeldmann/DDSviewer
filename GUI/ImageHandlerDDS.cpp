#include "ImageHandlerDDS.h"
#include "../DDS/ddsLoader.h"
#include <iostream>

wxIMPLEMENT_DYNAMIC_CLASS(ImageHandlerDDS,wxImageHandler);

bool ImageHandlerDDS::LoadFile( wxImage *image, wxInputStream& stream, bool verbose, int index)
{
    // Load the DDS file
    FileDDS dds(stream);
    //std::cout << dds.GetMipmapCount() << " mipmaps: " << dds.GetWidth() << "x" << dds.GetHeight() << std::endl;

    if (!dds.IsOk())
        return false;

    // create the image
    image->Destroy(); // delete old
    image->Create( dds.GetWidth(), dds.GetHeight() ); // alloc new

    if (!image->IsOk())
        return false;

    image->SetMask( false );
    unsigned char *rgb = image->GetData();

    // decompress DXT to RGBA
    std::vector<unsigned char> rgba;
    dds.GetMipmap(0)->Decompress(rgba);

    // image was decompressed to RGBA but wxImage stores RGB, so we must discard the 4th byte of each pixel...
    for (size_t i = 0, j = 0; i < rgba.size(); i += 4, j += 3) // j maps to rgb (stride 3), i maps to rgba(stride 4)
    {
        memcpy(&rgb[j], &(rgba.data()[i]), 3);
    }

    return true;
}

bool ImageHandlerDDS::SaveFile( wxImage *image, wxOutputStream& stream, bool verbose)
{
    return false;
}

bool ImageHandlerDDS::DoCanRead( wxInputStream& stream )
{
    return true;
}
