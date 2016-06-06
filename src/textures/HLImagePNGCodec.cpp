//
//  HLImagePNGCodec.cpp
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImagePNGCodec.h"
#include "png.h"
#include "KKGLConfiguration.h"
#include "KKFileUtil.hpp"
#include "KKUtil.h"
#include "KKResourceManager.hpp"

NS_KK_BEGIN

/*!
 Callback function used internally to decompress a PNG file in memory.
 
 \param[in] structp The PNG structure that point to the attached MEMORY stream.
 \param[in] bytep The byte pointer to of the stream to read.
 \param[in] size The size in bytes to read from the stream.
 */
void png_memory_read( png_structp structp, png_bytep bytep, png_size_t size )
{
	KKFileData *m = ( KKFileData * ) png_get_io_ptr( structp );
    
	m->read( bytep, size );
}

HLImagePNGCodec::~HLImagePNGCodec()
{
    if (mData)
    {
        free(mData);
    }
}

bool HLImagePNGCodec::loadWithFile(std::string file)
{
    if (mData)
    {
        free(mData);
    }
    
    hasPremultipliedAlpha = false;
    
    png_structp structp;
    
	png_infop infop;
    
	png_bytep *bytep = NULL;
    
	unsigned int i = 0;
	
	int n,
    png_bit_depth,
    png_color_type;
    
	structp = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                     NULL,
                                     NULL,
                                     NULL );
    
//    char fullPath[MAX_PATH];
//    get_full_path(file.c_str(), fullPath);
//    KKFileData filedata(fullPath, "rb");
    KKFileData* filedata = KKResourceManager::getSingleton()->getFileData(file.c_str());
    
    if (!filedata)
    {
        KKLOG("file %s does not exist!\n", file.c_str());
        return false;
    }
    
	infop = png_create_info_struct( structp );
    
	png_set_read_fn( structp, ( png_voidp * )filedata, png_memory_read );
    
	png_read_info( structp, infop );
    
	png_bit_depth = png_get_bit_depth( structp, infop );
	
	png_color_type = png_get_color_type( structp, infop );
    
	if( png_color_type == PNG_COLOR_TYPE_PALETTE )
	{ png_set_expand( structp ); }
    
	if( png_color_type == PNG_COLOR_TYPE_GRAY && png_bit_depth < 8 )
	{ png_set_expand( structp ); }
	
	if( png_get_valid( structp, infop, PNG_INFO_tRNS ) )
	{ png_set_expand( structp ); }
	
	if( png_bit_depth == 16 )
	{ png_set_strip_16( structp ); }
	
	if( png_color_type == PNG_COLOR_TYPE_GRAY || png_color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
	{ png_set_gray_to_rgb( structp ); }
    
	png_read_update_info( structp, infop );
    
    png_uint_32 imageWidth=0;
    png_uint_32 imageHeight=0;
    
	png_get_IHDR( structp,
                 infop,
                 ( png_uint_32 * )( &imageWidth  ),
                 ( png_uint_32 * )( &imageHeight ),
                 &png_bit_depth,
                 &png_color_type,
                 NULL, NULL, NULL );
    
    unsigned char byte=0;
	switch( png_color_type )
	{
		case PNG_COLOR_TYPE_GRAY:
		{
			byte        = 1;
			format   = kTexturePixelFormat_I8;
			
			break;
		}
            
		case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			byte        = 2;
			format   = kTexturePixelFormat_AI88;
            
			break;
		}
            
		case PNG_COLOR_TYPE_RGB:
		{
			byte        = 3;
			format   = kTexturePixelFormat_RGB888;
			
			break;
		}
            
		case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			byte        = 4;
			format   = kTexturePixelFormat_RGBA8888;
			
			break;
		}
	}
	
	unsigned int texSize = imageWidth * imageHeight * byte;
	
	unsigned char *texel_array = ( unsigned char * ) malloc( texSize );
    
	bytep = ( png_bytep * ) malloc( imageHeight * sizeof( png_bytep ) );
    
	
	while( i < imageHeight )
	{
		n = imageHeight - ( i + 1 );
		
		bytep[ n ] = texel_array + (int)( n * imageWidth * byte );
		
		++i;
	}
    
	
	png_read_image( structp, bytep );
    
	png_read_end( structp, NULL );
    
	png_destroy_read_struct( &structp,
                            &infop,
                            NULL );
    
	free( bytep );
    
    delete filedata;
    
    mData = texel_array;
    width = imageWidth;
    height = imageHeight;
    MipmapPtr mipmap;
    mipmap.address = mData;
    mipmap.len = 0; // only used by compressed texture
    mipmaps.push_back(mipmap);
    
    KKGLConfiguration *configuration = KKGLConfiguration::getSingleton();
    if (!configuration->isSupportsNPOT())
    {
        width = (unsigned int)nextPOT(imageWidth);
        height = (unsigned int)nextPOT(imageHeight);
        if (width != imageWidth || height != imageHeight)
        {
            KKLOG("WARRING: Device not support npot images!");
            return false;
//            mData = malloc(width*height*byte);
//            memset(mData, 0, width*height*byte);
//            for (i = 0; i < imageWidth; ++i)
//            {
//                for (unsigned int j = 0; j < imageHeight; ++j)
//                {
//                    memcpy((unsigned char*)mData+(i+j*width)*byte, texel_array+(i+j*imageWidth)*byte, byte);
//                }
//            }
//            free(texel_array);
        }
    }
    
    return true;
}

NS_KK_END