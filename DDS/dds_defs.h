#ifndef _DDS_DEFINITIONS_H_
#define _DDS_DEFINITIONS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum DDS_PIXELFORMAT_dwFlags {
    DDPF_ALPHAPIXELS = 0x1,	                // Texture contains alpha data; dwRGBAlphaBitMask contains valid data.	0x1
    DDPF_ALPHA	     = 0x2,                 // Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data)	0x2
    DDPF_FOURCC	     = 0x4,                 // Texture contains compressed RGB data; dwFourCC contains valid data.	0x4
    DDPF_RGB	     = 0x40,                // Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (dwRBitMask, dwGBitMask, dwBBitMask) contain valid data.	0x40
    DDPF_YUV	     = 0x200,               // Used in some older DDS files for YUV uncompressed data (dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask)	0x200
    DDPF_LUMINANCE	 = 0x20000,             // Used in some older DDS files for single channel color uncompressed data (dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file.	0x20000
};

struct DDS_PIXELFORMAT {
    uint32_t        dwSize;                 // structure size; set to 32 (bytes).
    uint32_t        dwFlags;                // Values which indicate what type of data is in the surface.
    uint32_t        dwFourCC;               // Four-character codes for specifying compressed or custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat member of that structure indicates the true format. When using a four-character code, dwFlags must include DDPF_FOURCC.
    uint32_t        dwRGBBitCount;          // Number of bits in an RGB (possibly including alpha) format. Valid when dwFlags includes DDPF_RGB, DDPF_LUMINANCE, or DDPF_YUV.
    uint32_t        dwRBitMask;             // Red (or lumiannce or Y) mask for reading color data. For instance, given the A8R8G8B8 format, the red mask would be 0x00ff0000.
    uint32_t        dwGBitMask;             // Green (or U) mask for reading color data. For instance, given the A8R8G8B8 format, the green mask would be 0x0000ff00.
    uint32_t        dwBBitMask;             // Blue (or V) mask for reading color data. For instance, given the A8R8G8B8 format, the blue mask would be 0x000000ff.
    uint32_t        dwABitMask;             // Alpha mask for reading alpha data. dwFlags must include DDPF_ALPHAPIXELS or DDPF_ALPHA. For instance, given the A8R8G8B8 format, the alpha mask would be 0xff000000.
};

enum DDS_HEADER_dwFlags {
    DDSD_CAPS	       = 0x1,                 // Required in every .dds file.
    DDSD_HEIGHT	       = 0x2,                 // Required in every .dds file.
    DDSD_WIDTH	       = 0x4,                 // Required in every .dds file.	0x4
    DDSD_PITCH	       = 0x8,                 // Required when pitch is provided for an uncompressed texture.
    DDSD_PIXELFORMAT   = 0x1000,              // Required in every .dds file.
    DDSD_MIPMAPCOUNT   = 0x20000,             // Required in a mipmapped texture.
    DDSD_LINEARSIZE	   = 0x80000,             // Required when pitch is provided for a compressed texture.
    DDSD_DEPTH	       = 0x800000,            // Required in a depth texture.

    DDS_HEADER_FLAGS_TEXTURE =  DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT,
    DDS_HEADER_FLAGS_MIPMAP  =  DDSD_MIPMAPCOUNT,
    DDS_HEADER_FLAGS_VOLUME  =  DDSD_DEPTH,
    DDS_HEADER_FLAGS_PITCH   =  DDSD_PITCH,
    DDS_HEADER_FLAGS_LINEARSIZE   =  DDSD_LINEARSIZE,
};

enum DDS_HEADER_dwCaps {
    DDSCAPS_COMPLEX	   = 0x8,                // Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or mipmapped volume texture).
    DDSCAPS_MIPMAP	   = 0x400000,           // Optional; should be used for a mipmap.
    DDSCAPS_TEXTURE	   = 0x1000,             // Required

    DDS_SURFACE_FLAGS_MIPMAP    = DDSCAPS_COMPLEX | DDSCAPS_MIPMAP,
    DDS_SURFACE_FLAGS_TEXTURE   = DDSCAPS_TEXTURE,
    DDS_SURFACE_FLAGS_CUBEMAP   = DDSCAPS_COMPLEX,
};

enum DDS_HEADER_dwCaps2 {
    DDSCAPS2_CUBEMAP            = 0x200,	    // Required for a cube map.
    DDSCAPS2_CUBEMAP_POSITIVEX  = 0x400,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_CUBEMAP_NEGATIVEX  = 0x800,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_CUBEMAP_POSITIVEY  = 0x1000,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_CUBEMAP_NEGATIVEY  = 0x2000,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_CUBEMAP_POSITIVEZ  = 0x4000,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_CUBEMAP_NEGATIVEZ  = 0x8000,	    // Required when these surfaces are stored in a cube map.
    DDSCAPS2_VOLUME	            = 0x200000,     // Required for a volume texture.
};

// https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dds-header
struct DDS_HEADER {
    uint32_t           dwSize;                // Size of structure. This member must be set to 124.
    uint32_t           dwFlags;               // Flags to indicate which members contain valid data.
    uint32_t           dwHeight;              // Surface height (in pixels).
    uint32_t           dwWidth;               // Surface width (in pixels).
    uint32_t           dwPitchOrLinearSize;   // The pitch or number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture.
    uint32_t           dwDepth;               // Depth of a volume texture (in pixels), otherwise unused.
    uint32_t           dwMipMapCount;         // Number of mipmap levels, otherwise unused.
    uint32_t           dwReserved1[11];
    struct DDS_PIXELFORMAT    ddspf;
    uint32_t           dwCaps;                // Specifies the complexity of the surfaces stored
    uint32_t           dwCaps2;               // Additional detail about the surfaces stored.
    uint32_t           dwCaps3;               // not used
    uint32_t           dwCaps4;               // not used
    uint32_t           dwReserved2;           // not used
};

#define MAKEFOURCC(ch0, ch1, ch2, ch3) ( ((uint32_t)(char)(ch0)) | ((uint32_t)(char)(ch1) << 8) | ((uint32_t)(char)(ch2) << 16) | ((uint32_t)(char)(ch3) << 24 ))
static const uint32_t FOURCC_DXT1 = MAKEFOURCC('D','X','T','1');
static const uint32_t FOURCC_DXT3 = MAKEFOURCC('D','X','T','3');
static const uint32_t FOURCC_DXT5 = MAKEFOURCC('D','X','T','5');

#ifdef __cplusplus
} // extern C

static_assert(sizeof(DDS_HEADER) == 124, "struct DDS_HEADER must be 124 bytes in length");
static_assert(sizeof(DDS_PIXELFORMAT) == 32, "struct DDS_PIXELFORMAT must be 32 bytes in length");

#endif

#endif // _DDS_DEFINITIONS_H_
