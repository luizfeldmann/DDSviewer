#ifndef _DXT_DECOMPRESSOR_H_
#define _DXT_DECOMPRESSOR_H_

// definition
template<unsigned int n> class DXT_Decompressor
{
protected:
    static unsigned int stride();
    static void DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);

public:
    static void DecompressImage(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image)
    {
        unsigned long blockCountX = (width + 3) / 4;
        unsigned long blockCountY = (height + 3) / 4;

        for (unsigned long j = 0; j < blockCountY; j++)
        {
            for (unsigned long i = 0; i < blockCountX; i++)
                DecompressBlock(i*4, j*4, width, blockStorage + i * stride(), image);

            blockStorage += blockCountX * stride();
        }
    }
};

// explicit instantiation
template<> unsigned int DXT_Decompressor<1>::stride();
template<> unsigned int DXT_Decompressor<3>::stride();
template<> unsigned int DXT_Decompressor<5>::stride();

template<> void DXT_Decompressor<1>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
template<> void DXT_Decompressor<3>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
template<> void DXT_Decompressor<5>::DecompressBlock(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);

/*template<> void DXT_Decompressor<1>::DecompressImage(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
template<> void DXT_Decompressor<3>::DecompressImage(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
template<> void DXT_Decompressor<5>::DecompressImage(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);*/

#endif // _DXT_DECOMPRESSOR_H_
