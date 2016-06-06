//
//  HLImagePVRCodec.cpp
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImagePVRCodec.h"
#include "KKUtil.h"
#include "KKFileUtil.hpp"
#include "KKGLConfiguration.h"
#include "KKResourceManager.hpp"
#include <stdint.h>

NS_KK_BEGIN

static const int PVR_TEXTURE_FLAG_TYPE_MASK = 0xff;

static bool _PVRHaveAlphaPremultiplied = false;

struct PixelFormatDescription {
    GLuint glInternalFormat;
    GLuint glFormat;
    GLuint glType;
    GLuint bpp;
    bool compressed;
};

extern PixelFormatDescription _pixelFormats[];

enum class PVR2TextureFlag
{
    Mipmap         = (1<<8),        // has mip map levels
    Twiddle        = (1<<9),        // is twiddled
    Bumpmap        = (1<<10),       // has normals encoded for a bump map
    Tiling         = (1<<11),       // is bordered for tiled pvr
    Cubemap        = (1<<12),       // is a cubemap/skybox
    FalseMipCol    = (1<<13),       // are there false colored MIP levels
    Volume         = (1<<14),       // is this a volume texture
    Alpha          = (1<<15),       // v2.1 is there transparency info in the texture
    VerticalFlip   = (1<<16),       // v2.1 is the texture vertically flipped
};

enum class PVR3TextureFlag
{
    PremultipliedAlpha	= (1<<1)	// has premultiplied alpha
};

enum class PVR2TexturePixelFormat : unsigned char
{
	RGBA4444 = 0x10,
	RGBA5551,
	RGBA8888,
	RGB565,
	RGB555,				// unsupported
	RGB888,
	I8,
	AI88,
	PVRTC2BPP_RGBA,
	PVRTC4BPP_RGBA,
	BGRA8888,
	A8,
};
    
enum class PVR3TexturePixelFormat : uint64_t
{
    PVRTC2BPP_RGB  = 0ULL,
    PVRTC2BPP_RGBA = 1ULL,
    PVRTC4BPP_RGB  = 2ULL,
    PVRTC4BPP_RGBA = 3ULL,
    PVRTC2_2BPP_RGBA = 4ULL,
    PVRTC2_4BPP_RGBA  = 5ULL,
    ETC1 = 6ULL,
    DXT1 = 7ULL,
    DXT2 = 8ULL,
    DXT3 = 9ULL,
    DXT4 = 10ULL,
    DXT5 = 11ULL,
    BC1 = 7ULL,
    BC2 = 9ULL,
    BC3 = 11ULL,
    BC4 = 12ULL,
    BC5 = 13ULL,
    BC6 = 14ULL,
    BC7 = 15ULL,
    UYVY = 16ULL,
    YUY2 = 17ULL,
    BW1bpp = 18ULL,
    R9G9B9E5 = 19ULL,
    RGBG8888 = 20ULL,
    GRGB8888 = 21ULL,
    ETC2_RGB = 22ULL,
    ETC2_RGBA = 23ULL,
    ETC2_RGBA1 = 24ULL,
    EAC_R11_Unsigned = 25ULL,
    EAC_R11_Signed = 26ULL,
    EAC_RG11_Unsigned = 27ULL,
    EAC_RG11_Signed = 28ULL,
    
    BGRA8888       = 0x0808080861726762ULL,
    RGBA8888       = 0x0808080861626772ULL,
    RGBA4444       = 0x0404040461626772ULL,
    RGBA5551       = 0x0105050561626772ULL,
    RGB565         = 0x0005060500626772ULL,
    RGB888         = 0x0008080800626772ULL,
    A8             = 0x0000000800000061ULL,
    L8             = 0x000000080000006cULL,
    LA88           = 0x000008080000616cULL,
};
    
// v2
typedef const std::map<PVR2TexturePixelFormat, KKTexturePixelFormat> _pixel2_formathash;
    
static const _pixel2_formathash::value_type v2_pixel_formathash_value[] =
{
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::BGRA8888,	    kTexturePixelFormat_BGRA8888),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA8888,	    kTexturePixelFormat_RGBA8888),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA4444,	    kTexturePixelFormat_RGBA4444),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGBA5551,	    kTexturePixelFormat_RGB5A1),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGB565,	    kTexturePixelFormat_RGB565),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::RGB888,	    kTexturePixelFormat_RGB888),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::A8,	        kTexturePixelFormat_A8),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::I8,	        kTexturePixelFormat_I8),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::AI88,	        kTexturePixelFormat_AI88),
    
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::PVRTC2BPP_RGBA,	    kTexturePixelFormat_PVRTC2),
    _pixel2_formathash::value_type(PVR2TexturePixelFormat::PVRTC4BPP_RGBA,	    kTexturePixelFormat_PVRTC4),
#endif
#endif
};
    
static const int PVR2_MAX_TABLE_ELEMENTS = sizeof(v2_pixel_formathash_value) / sizeof(v2_pixel_formathash_value[0]);
static const _pixel2_formathash v2_pixel_formathash(v2_pixel_formathash_value, v2_pixel_formathash_value + PVR2_MAX_TABLE_ELEMENTS);
    
// v3
typedef const std::map<PVR3TexturePixelFormat, KKTexturePixelFormat> _pixel3_formathash;
static _pixel3_formathash::value_type v3_pixel_formathash_value[] =
{
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::BGRA8888,	kTexturePixelFormat_BGRA8888),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA8888,	kTexturePixelFormat_RGBA8888),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA4444,	kTexturePixelFormat_RGBA4444),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGBA5551,	kTexturePixelFormat_RGB5A1),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGB565,	    kTexturePixelFormat_RGB565),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::RGB888,	    kTexturePixelFormat_RGB888),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::A8,	        kTexturePixelFormat_A8),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::L8,	        kTexturePixelFormat_I8),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::LA88,	    kTexturePixelFormat_AI88),
    
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC2BPP_RGB,	    kTexturePixelFormat_PVRTC2_NOALPHA),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC2BPP_RGBA,	    kTexturePixelFormat_PVRTC2),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC4BPP_RGB,	    kTexturePixelFormat_PVRTC4_NOALPHA),
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::PVRTC4BPP_RGBA,	    kTexturePixelFormat_PVRTC4),
#endif
#endif

#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
    _pixel3_formathash::value_type(PVR3TexturePixelFormat::ETC1,	    kTexturePixelFormat_ETC1_RGB8),
#endif
#endif
};

static const int PVR3_MAX_TABLE_ELEMENTS = sizeof(v3_pixel_formathash_value) / sizeof(v3_pixel_formathash_value[0]);
    
static const _pixel3_formathash v3_pixel_formathash(v3_pixel_formathash_value, v3_pixel_formathash_value + PVR3_MAX_TABLE_ELEMENTS);
    
typedef struct _PVRTexHeader
{
    unsigned int headerLength;
    unsigned int height;
    unsigned int width;
    unsigned int numMipmaps;
    unsigned int flags;
    unsigned int dataLength;
    unsigned int bpp;
    unsigned int bitmaskRed;
    unsigned int bitmaskGreen;
    unsigned int bitmaskBlue;
    unsigned int bitmaskAlpha;
    unsigned int pvrTag;
    unsigned int numSurfs;
} PVRv2TexHeader;

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
typedef struct
{
    uint32_t version;
    uint32_t flags;
    uint64_t pixelFormat;
    uint32_t colorSpace;
    uint32_t channelType;
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t numberOfSurfaces;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmaps;
    uint32_t metadataLength;
#ifdef _MSC_VER
} PVRv3TexHeader;
#pragma pack(pop)
#else
} __attribute__((packed)) PVRv3TexHeader;
#endif

static char gPVRTexIdentifier[5] = "PVR!";
#define PVR_TEXTURE_FLAG_TYPE_MASK    0xff
#define MAX_TABLE_ELEMENTS (sizeof(tableFormats) / sizeof(tableFormats[0]))

HLImagePVRCodec::~HLImagePVRCodec()
{
    if (mData)
    {
        delete []mData;
    }
}

bool HLImagePVRCodec::loadWithPVRv2Data(const unsigned char* data, ssize_t len)
{
    int dataLength = 0, dataOffset = 0, dataSize = 0;
    int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    int width = 0, height = 0;
    
    //Cast first sizeof(PVRTexHeader) bytes of data stream as PVRTexHeader
    const PVRv2TexHeader *header = static_cast<const PVRv2TexHeader *>(static_cast<const void*>(data));
    
    //Make sure that tag is in correct formatting
    if (memcmp(&header->pvrTag, gPVRTexIdentifier, strlen(gPVRTexIdentifier)) != 0)
    {
        return false;
    }
    
    KKGLConfiguration *configuration = KKGLConfiguration::getSingleton();
    
    hasPremultipliedAlpha = _PVRHaveAlphaPremultiplied;
    
    LittleEndianToNative<sizeof(header->flags)>((void*)&header->flags);
    PVR2TexturePixelFormat formatFlags = static_cast<PVR2TexturePixelFormat>(header->flags & PVR_TEXTURE_FLAG_TYPE_MASK);
    
    bool flipped = (header->flags & (unsigned int)PVR2TextureFlag::VerticalFlip) ? true : false;
    if (flipped)
    {
        KKLOG("WARNING: Image is flipped. Regenerate it using PVRTexTool");
    }
    
    if (!configuration->isSupportsNPOT() &&
        (header->width != nextPOT(header->width) || header->height != nextPOT(header->height)))
    {
        KKLOG("ERROR: Loding an NPOT texture (%dx%d) but is not supported on this device", header->width, header->height);
        return false;
    }
    
    auto it = v2_pixel_formathash.find(formatFlags);
    
    if (it == v2_pixel_formathash.end())
    {
        KKLOG("WARNING: Unsupported PVR Pixel Format: 0x%02X. Re-encode it with a OpenGL pixel format variant", (int)formatFlags);
        return false;
    }
    
    format = it->second;
    
    int bpp = _pixelFormats[format].bpp;
    
    LittleEndianToNative<sizeof(header->width)>((void*)&header->width);
    LittleEndianToNative<sizeof(header->height)>((void*)&header->height);
    LittleEndianToNative<sizeof(header->height)>((void*)&header->dataLength);
    this->width = width = header->width;
    this->height = height = header->height;
    
    dataLength = header->dataLength;
    
    const unsigned char* bytes = data + sizeof(PVRv2TexHeader);
    while (dataOffset < dataLength)
    {
        switch (formatFlags)
        {
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
            case PVR2TexturePixelFormat::PVRTC2BPP_RGBA:
                blockSize = 8 * 4;
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                break;
            case PVR2TexturePixelFormat::PVRTC4BPP_RGBA:
                blockSize = 4 * 4;
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                break;
#endif
#endif
            case PVR2TexturePixelFormat::BGRA8888:
                if (!configuration->isSupportsBGRA8888())
                {
                    KKLOG("BGRA8888 not supported on this device");
                    return false;
                }
            default:
                blockSize = 1;
                widthBlocks = width;
                heightBlocks = height;
                break;
        }
        
        // Clamp to minimum number of blocks
        if (widthBlocks < 2)
        {
            widthBlocks = 2;
        }
        if (heightBlocks < 2)
        {
            heightBlocks = 2;
        }
        
        dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
        int packetLength = (dataLength - dataOffset);
        packetLength = packetLength > dataSize ? dataSize : packetLength;
        
        MipmapPtr mipmap;
        mipmap.address = bytes + dataOffset;
        mipmap.len = packetLength;
        mipmaps.push_back(mipmap);
        
        dataOffset += packetLength;
        
        //Update width and height to the next lower power of two
        width = MAX(width >> 1, 1);
        height = MAX(height >> 1, 1);
    }
    
    return true;
}

bool HLImagePVRCodec::loadWithPVRv3Data(const unsigned char* data, ssize_t len)
{
    if (static_cast<size_t>(len) < sizeof(PVRv3TexHeader))
    {
        return false;
    }
    
    const PVRv3TexHeader *header = static_cast<const PVRv3TexHeader *>(static_cast<const void*>(data));
    
    BigEndianToNative<sizeof(header->version)>((void*)&header->version);
    
    if (header->version != 0x50565203)
    {
		KKLOG("PVR file version mismatch");
        return false;
    }
    
    KKGLConfiguration *configuration = KKGLConfiguration::getSingleton();
    
    // parse pixel format
    PVR3TexturePixelFormat pixelFormat = static_cast<PVR3TexturePixelFormat>(header->pixelFormat);
    
    auto it = v3_pixel_formathash.find(pixelFormat);
    if (it == v3_pixel_formathash.end())
    {
        KKLOG("WARNING: Unsupported PVR Pixel Format: 0x%016llX. Re-encode it with a OpenGL pixel format variant",
              static_cast<unsigned long long>(pixelFormat));
        return false;
    }
    
    format = it->second;
    int bpp = _pixelFormats[format].bpp;
    
    LittleEndianToNative<sizeof(header->flags)>((void*)&header->flags);
    
    if (header->flags & (unsigned int)PVR3TextureFlag::PremultipliedAlpha)
    {
        hasPremultipliedAlpha = true;
    }
    
    LittleEndianToNative<sizeof(header->width)>((void*)&header->width);
    LittleEndianToNative<sizeof(header->height)>((void*)&header->height);
    
    int width, height;
    this->width = width = header->width;
    this->height = height = header->height;
    
    ssize_t dataLength = 0;
    int dataOffset = 0, dataSize = 0;
    int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
    
    int numberOfMipmaps = header->numberOfMipmaps;
    
    dataLength = len - (sizeof(PVRv3TexHeader) + header->metadataLength);
    data += sizeof(PVRv3TexHeader) + header->metadataLength;
    
    for (int i = 0; i < numberOfMipmaps; ++i)
    {
        switch (pixelFormat)
        {
#ifdef GL_IMG_texture_compression_pvrtc
#if GL_IMG_texture_compression_pvrtc
            case PVR3TexturePixelFormat::PVRTC2BPP_RGB:
            case PVR3TexturePixelFormat::PVRTC2BPP_RGBA:
                blockSize = 8 * 4;
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                break;
            case PVR3TexturePixelFormat::PVRTC4BPP_RGB:
            case PVR3TexturePixelFormat::PVRTC4BPP_RGBA:
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                break;
#endif
#endif
#ifdef GL_OES_compressed_ETC1_RGB8_texture
#if GL_OES_compressed_ETC1_RGB8_texture
            case PVR3TexturePixelFormat::ETC1:
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                break;
#endif
#endif
            case PVR3TexturePixelFormat::BGRA8888:
                if (!configuration->isSupportsBGRA8888())
                {
                    KKLOG("BGRA8888 not supported on this device");
                    return false;
                }
                break;
            default:
                blockSize = 1;
                widthBlocks = width;
                heightBlocks = height;
                break;
        }
        
        // Clamp to minimum number of blocks
        if (widthBlocks < 2)
        {
            widthBlocks = 2;
        }
        if (heightBlocks < 2)
        {
            heightBlocks = 2;
        }
        
        dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
        auto packetLength = dataLength - dataOffset;
        packetLength = packetLength > dataSize ? dataSize : packetLength;
        
        MipmapPtr mipmap;
        mipmap.address = data + dataOffset;
        mipmap.len = packetLength;
        mipmaps.push_back(mipmap);
        
        dataOffset += packetLength;
        
        width = MAX(width >> 1, 1);
        height = MAX(height >> 1, 1);
    }
    
    return true;
}

bool HLImagePVRCodec::loadWithFile(std::string file)
{
    hasPremultipliedAlpha = false;
    
    unsigned char* data = NULL;
    int len = 0;
    
    std::string lowerCase(file);
    for (int i=0; i<lowerCase.length(); i++)
    {
        lowerCase[i] = tolower(lowerCase[i]);
    }
//    char fullPath[MAX_PATH];
//    get_full_path(file.c_str(), fullPath);
    KKFileData* fdata = KKResourceManager::getSingleton()->getFileData(file.c_str());
    
    // read file data, uncompress if neccesary
    if (lowerCase.find(".ccz") != std::string::npos)
    {
        len = inflateCCZFile(fdata, &data);
    }
//    else if (lowerCase.find(".gz") != std::string::npos)
//    {
//        len = inflateGZipFile(fdata, &data);
//    }
    else
    {
        len = (int)fdata->size;
        data = fdata->buffer;
        fdata->buffer = nullptr;
        fdata->size = 0;
    }
    delete fdata;
    
    if (!len)
    {
        KKLOG("file %s load error", file.c_str());
        if (data)
            delete []data;
        return false;
    }
    
    if (!(loadWithPVRv2Data(data, len) || loadWithPVRv3Data(data, len)))
    {
        delete []data;
        return false;
    }
    
    mData = data;
    return true;
}

void HLImagePVRCodec::setPVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied)
{
    _PVRHaveAlphaPremultiplied = haveAlphaPremultiplied;
}

NS_KK_END