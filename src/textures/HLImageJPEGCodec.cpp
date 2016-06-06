//
//  HLImageJEPGCodec.cpp
//  HoolaiEngine
//
//  Created by zhao shuan on 13-3-25.
//  Copyright (c) 2013年 ICT. All rights reserved.
//

#include "HLImageJPEGCodec.h"
#include <stdint.h>
#include "KKResourceManager.hpp"
#include "KKFileUtil.hpp"

extern "C" {
#include "jpeglib.h"
//#include "jerror.h"
#include <setjmp.h>

typedef struct tagErrorManager {
	/// "public" fields
	struct jpeg_error_mgr pub;
	/// for return to caller
	jmp_buf setjmp_buffer;
} ErrorManager;

METHODDEF(void)
jpeg_error_exit (j_common_ptr cinfo) {
	ErrorManager* error_ptr = (ErrorManager*)cinfo->err;
    
	// always display the message
	error_ptr->pub.output_message(cinfo);
    
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message) (cinfo, buffer);
    KKLOG("jepg error: %s", buffer);
    
    jpeg_destroy(cinfo);
    longjmp(error_ptr->setjmp_buffer, 1);
}

METHODDEF(void)
jpeg_output_message (j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];
	ErrorManager* error_ptr = (ErrorManager*)cinfo->err;
    
	// create the message
	error_ptr->pub.format_message(cinfo, buffer);
	// send it to user's message proc
//	FreeImage_OutputMessageProc(s_format_id, buffer);
    printf("\n%s\n", buffer);
}
    
}

NS_KK_BEGIN

HLImageJPEGCodec::~HLImageJPEGCodec()
{
    if (mData)
    {
        free(mData);
    }
}

bool HLImageJPEGCodec::loadWithFile(std::string file)
{
//    char fullPath[MAX_PATH];
//    get_full_path(file.c_str(), fullPath);
//    KKFileData filedata(fullPath, "rb");
    hasPremultipliedAlpha = false;
    
    KKFileData* filedata = KKResourceManager::getSingleton()->getFileData(file.c_str());
    
    if (!filedata)
    {
        KKLOG("file %s does not exist!\n", file.c_str());
        return false;
    }
    
    struct jpeg_decompress_struct cinfo;
    ErrorManager fi_error_mgr;
    
    // step 1: allocate and initialize JPEG decompression object
    
    cinfo.err = jpeg_std_error(&fi_error_mgr.pub);
    fi_error_mgr.pub.error_exit     = jpeg_error_exit;
    fi_error_mgr.pub.output_message = jpeg_output_message;
    
    // establish the setjmp return context for jpeg_error_exit to use
    if (setjmp(fi_error_mgr.setjmp_buffer)) {
        // If we get here, the JPEG code has signaled an error.
        // We need to clean up the JPEG object, close the input file, and return.
        KKLOG("error happens with file %s\n", file.c_str());
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
    
    jpeg_create_decompress(&cinfo);
    
    // step 2a: specify data source (eg, a handle)
    
    jpeg_mem_src(&cinfo, filedata->buffer, filedata->size);
    
//    // step 2b: save special markers for later reading
//    
//    jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);
//    for(int m = 0; m < 16; m++) {
//        jpeg_save_markers(&cinfo, JPEG_APP0 + m, 0xFFFF);
//    }
    
    // step 3: read handle parameters with jpeg_read_header()
    
    jpeg_read_header(&cinfo, TRUE);
    
    // step 4: set parameters for decompression
    
    unsigned int scale_denom = 1;		// fraction by which to scale image
    cinfo.scale_num = 1;
    cinfo.scale_denom = scale_denom;
    
    // step 5a: start decompressor and calculate output width and height
    
    jpeg_start_decompress(&cinfo);
    
    width = cinfo.output_width;
    height = cinfo.output_height;
    
//    // step 6: read special markers
//    
//    read_markers(&cinfo, dib);
    
    if (cinfo.out_color_space == JCS_CMYK) // convert from CMYK to RGB
    {
        JSAMPARRAY buffer;		// output row buffer
        unsigned row_stride;	// physical row width in output buffer
        
        // JSAMPLEs per row in output buffer
        row_stride = cinfo.output_width * cinfo.output_components;
        
        // make a one-row-high sample array that will go away when done with image
        buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
        
        mData = (unsigned char*)malloc(width*height*3);
        unsigned char* dst = mData;
        format = kTexturePixelFormat_RGB888;
        
        while (cinfo.output_scanline < cinfo.output_height) {
            JSAMPROW src = buffer[0];
            
            jpeg_read_scanlines(&cinfo, buffer, 1);
            
            for(unsigned x = 0; x < cinfo.output_width; x++) {
                uint32_t K = (uint32_t)src[3];
                dst[0]   = (uint8_t)((K * src[0]) / 255);	// C -> R
                dst[1] = (uint8_t)((K * src[1]) / 255);	// M -> G
                dst[2]  = (uint8_t)((K * src[2]) / 255);	// Y -> B
                src += 4;
                dst += 3;
            }
        }
    }
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        mData = (unsigned char*)malloc(width*height);
        unsigned char* dst = mData;
        format = kTexturePixelFormat_I8;
        while (cinfo.output_scanline < cinfo.output_height)
        {
            jpeg_read_scanlines(&cinfo, &dst, 1);
            dst += width;
        }
    }
    else if (cinfo.out_color_space == JCS_RGB)
    {
        mData = (unsigned char*)malloc(width*height*3);
        unsigned char* dst = mData;
        format = kTexturePixelFormat_RGB888;
        while (cinfo.output_scanline < cinfo.output_height)
        {
            jpeg_read_scanlines(&cinfo, &dst, 1);
            dst += width*3;
        }
    }
    else
    {
        printf("unsupported jpeg color space!\n");
        
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
    
    // step 8: finish decompression
    
    jpeg_finish_decompress(&cinfo);
    
    // step 9: release JPEG decompression object
    
    jpeg_destroy_decompress(&cinfo);
    
    MipmapPtr mipmap;
    mipmap.address = mData;
    mipmap.len = 0; // only used by compressed texture
    mipmaps.push_back(mipmap);
    
    delete filedata;
    
    return true;
}

NS_KK_END