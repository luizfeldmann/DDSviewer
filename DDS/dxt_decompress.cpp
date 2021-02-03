#include "dxt_decompress.h"

/// INTERNAL
/// ================================================================================================
template<> unsigned int DXT_Decompressor<1>::stride() {return 8; }
template<> unsigned int DXT_Decompressor<3>::stride() {return 16;}
template<> unsigned int DXT_Decompressor<5>::stride() {return 16;}

static unsigned long PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return r | (g << 8) | (b << 16) | (a << 24);
}

static void DecompressBlockDXT13(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image, const unsigned char* alphaValues)
{
    unsigned short color0 = *reinterpret_cast<const unsigned short *>(blockStorage);
    unsigned short color1 = *reinterpret_cast<const unsigned short *>(blockStorage + 2);

    unsigned long temp;

    temp = (color0 >> 11) * 255 + 16;
    unsigned char r0 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
    unsigned char g0 = (unsigned char)((temp/64 + temp)/64);
    temp = (color0 & 0x001F) * 255 + 16;
    unsigned char b0 = (unsigned char)((temp/32 + temp)/32);

    temp = (color1 >> 11) * 255 + 16;
    unsigned char r1 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
    unsigned char g1 = (unsigned char)((temp/64 + temp)/64);
    temp = (color1 & 0x001F) * 255 + 16;
    unsigned char b1 = (unsigned char)((temp/32 + temp)/32);

    unsigned long code = *reinterpret_cast<const unsigned long *>(blockStorage + 4);

    for (int j=0; j < 4; j++)
    {
        for (int i=0; i < 4; i++)
        {
            unsigned long finalColor = 0;
            unsigned char positionCode = (code >>  2*(4*j+i)) & 0x03;
            unsigned char alpha = alphaValues[j*4+i];

            if (color0 > color1)
            {
                switch (positionCode)
                {
                    case 0:
                        finalColor = PackRGBA(r0, g0, b0, alpha);
                        break;
                    case 1:
                        finalColor = PackRGBA(r1, g1, b1, alpha);
                        break;
                    case 2:
                        finalColor = PackRGBA((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3, alpha);
                        break;
                    case 3:
                        finalColor = PackRGBA((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3, alpha);
                        break;
                }
            }
            else
            {
                switch (positionCode)
                {
                    case 0:
                        finalColor = PackRGBA(r0, g0, b0, alpha);
                        break;
                    case 1:
                        finalColor = PackRGBA(r1, g1, b1, alpha);
                        break;
                    case 2:
                        finalColor = PackRGBA((r0+r1)/2, (g0+g1)/2, (b0+b1)/2, alpha);
                        break;
                    case 3:
                        finalColor = PackRGBA(0, 0, 0, alpha);
                        break;
                }
            }

            if (x + i < width)
                image[(y + j)*width + (x + i)] = finalColor;
        }
    }
}

/// SINGLE BLOCK DECOMPRESSION
/// ================================================================================================
template<> void DXT_Decompressor<1>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image)
{
    static const unsigned char const_alpha [] = {
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255
	};

    DecompressBlockDXT13(x, y, width, blockStorage, image, const_alpha);
}

template<> void DXT_Decompressor<3>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image)
{
    unsigned char alphaValues [16] = { 0 };

	for (unsigned char i = 0; i < 4; ++i)
    {
		const short* alphaData = (const short*)(blockStorage);

		alphaValues [i * 4 + 0] = (((*alphaData) >> 0) & 0xF) * 17;
		alphaValues [i * 4 + 1] = (((*alphaData) >> 4) & 0xF) * 17;
		alphaValues [i * 4 + 2] = (((*alphaData) >> 8) & 0xF) * 17;
		alphaValues [i * 4 + 3] = (((*alphaData) >> 12) & 0xF) * 17;

		blockStorage += 2;
	}

	DecompressBlockDXT13(x, y, width, blockStorage, image, alphaValues);
}

template<> void DXT_Decompressor<5>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image)
{
    unsigned char alpha0 = *reinterpret_cast<const unsigned char *>(blockStorage);
    unsigned char alpha1 = *reinterpret_cast<const unsigned char *>(blockStorage + 1);

    const unsigned char *bits = blockStorage + 2;
    unsigned long alphaCode1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
    unsigned short alphaCode2 = bits[0] | (bits[1] << 8);

    unsigned short color0 = *reinterpret_cast<const unsigned short *>(blockStorage + 8);
    unsigned short color1 = *reinterpret_cast<const unsigned short *>(blockStorage + 10);

    unsigned long temp;

    temp = (color0 >> 11) * 255 + 16;
    unsigned char r0 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
    unsigned char g0 = (unsigned char)((temp/64 + temp)/64);
    temp = (color0 & 0x001F) * 255 + 16;
    unsigned char b0 = (unsigned char)((temp/32 + temp)/32);

    temp = (color1 >> 11) * 255 + 16;
    unsigned char r1 = (unsigned char)((temp/32 + temp)/32);
    temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
    unsigned char g1 = (unsigned char)((temp/64 + temp)/64);
    temp = (color1 & 0x001F) * 255 + 16;
    unsigned char b1 = (unsigned char)((temp/32 + temp)/32);

    unsigned long code = *reinterpret_cast<const unsigned long *>(blockStorage + 12);

    for (int j=0; j < 4; j++)
    {
        for (int i=0; i < 4; i++)
        {
            int alphaCodeIndex = 3*(4*j+i);
            int alphaCode;

            if (alphaCodeIndex <= 12)
            {
                alphaCode = (alphaCode2 >> alphaCodeIndex) & 0x07;
            }
            else if (alphaCodeIndex == 15)
            {
                alphaCode = (alphaCode2 >> 15) | ((alphaCode1 << 1) & 0x06);
            }
            else // alphaCodeIndex >= 18 && alphaCodeIndex <= 45
            {
                alphaCode = (alphaCode1 >> (alphaCodeIndex - 16)) & 0x07;
            }

            unsigned char finalAlpha;
            if (alphaCode == 0)
            {
                finalAlpha = alpha0;
            }
            else if (alphaCode == 1)
            {
                finalAlpha = alpha1;
            }
            else
            {
                if (alpha0 > alpha1)
                {
                    finalAlpha = ((8-alphaCode)*alpha0 + (alphaCode-1)*alpha1)/7;
                }
                else
                {
                    if (alphaCode == 6)
                        finalAlpha = 0;
                    else if (alphaCode == 7)
                        finalAlpha = 255;
                    else
                        finalAlpha = ((6-alphaCode)*alpha0 + (alphaCode-1)*alpha1)/5;
                }
            }

            unsigned char colorCode = (code >> 2*(4*j+i)) & 0x03;

            unsigned long finalColor = 0;
            switch (colorCode)
            {
                case 0:
                    finalColor = PackRGBA(r0, g0, b0, finalAlpha);
                    break;
                case 1:
                    finalColor = PackRGBA(r1, g1, b1, finalAlpha);
                    break;
                case 2:
                    finalColor = PackRGBA((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3, finalAlpha);
                    break;
                case 3:
                    finalColor = PackRGBA((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3, finalAlpha);
                    break;
            }

            if (x + i < width)
                image[(y + j)*width + (x + i)] = finalColor;
        }
    }
}
