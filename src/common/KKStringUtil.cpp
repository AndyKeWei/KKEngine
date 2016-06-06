//
//  KKStringUtil.cpp
//  KKEngine
//
//  Created by kewei on 1/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#include "KKStringUtil.hpp"

NS_KK_BEGIN

namespace StringUtil {
    
    std::string MD5(const char *str)
    {
        ::MD5 md5;
        md5.GenerateMD5((unsigned char *)str, (int)strlen(str));
        return md5.ToString();
    }
    
    std::string Base64Encode(const unsigned char* data, int len)
    {
        const static char EncodeTable[]= {
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
            0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
            0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
            0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
            0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
            0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
            0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
        };
        
        string strEncode;
        const unsigned maxInputBufferSize = UINT_MAX / 77 * 76 / 4 * 3 - 2;
        if (len > maxInputBufferSize)
            return strEncode;
        
        unsigned sidx = 0;
        unsigned didx = 0;
        
        unsigned outLength = ((len + 2) / 3) * 4;
        
        // Deal with the 76 character per line limit specified in RFC 2045.
        bool insertLFs = false;
        if (insertLFs)
            outLength += ((outLength - 1) / 76);
        
        int count = 0;
        strEncode.resize(outLength, 0);
        // 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
        if (len > 1)
        {
            while (sidx < len - 2)
            {
                if (insertLFs)
                {
                    if (count && !(count % 76))
                        strEncode[didx++] = '\n';
                    count += 4;
                }
                strEncode[didx++] = EncodeTable[(data[sidx] >> 2) & 077];
                strEncode[didx++] = EncodeTable[((data[sidx + 1] >> 4) & 017) | ((data[sidx] << 4) & 077)];
                strEncode[didx++] = EncodeTable[((data[sidx + 2] >> 6) & 003) | ((data[sidx + 1] << 2) & 077)];
                strEncode[didx++] = EncodeTable[data[sidx + 2] & 077];
                sidx += 3;
            }
        }
        
        if (sidx < len)
        {
            if (insertLFs && (count > 0) && !(count % 76))
                strEncode[didx++] = '\n';
            
            strEncode[didx++] = EncodeTable[(data[sidx] >> 2) & 077];
            if (sidx < len - 1)
            {
                strEncode[didx++] = EncodeTable[((data[sidx + 1] >> 4) & 017) | ((data[sidx] << 4) & 077)];
                strEncode[didx++] = EncodeTable[(data[sidx + 1] << 2) & 077];
            } else
                strEncode[didx++] = EncodeTable[(data[sidx] << 4) & 077];
        }
        
        // Add padding
        while (didx < strEncode.size())
        {
            strEncode[didx++] = '=';
            ++didx;
        }
        
        return strEncode;
    }
    
    std::vector<unsigned char> Base64Decode(const char* data)
    {
        int outLen;
        // decode table
        const static char DecodeTable[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
            0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
            0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
            0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
            0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
            0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
            0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
            0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        
        std::vector<unsigned char> dataDecode;
        outLen = 0;
        bool sawEqualsSign = false;
        size_t len = strlen(data);
        dataDecode.resize(len);
        
        for (unsigned idx = 0; idx < len; ++idx) {
            unsigned ch = data[idx];
            if (ch == '=')
                sawEqualsSign = true;
            else if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ch == '+' || ch == '/') {
                if (sawEqualsSign)
                {
                    dataDecode.clear();
                    return dataDecode;
                }
                dataDecode[outLen] = DecodeTable[ch];
                ++outLen;
            }
        }
        
        if (!outLen)
            return dataDecode;
        
        // Valid data is (n * 4 + [0,2,3]) characters long.
        if ((outLen % 4) == 1)
        {
            dataDecode.clear();
            return dataDecode;
        }
        
        // 4-byte to 3-byte conversion
        outLen -= (outLen + 3) / 4;
        if (!outLen)
        {
            dataDecode.clear();
            return dataDecode;
        }
        
        unsigned sidx = 0;
        unsigned didx = 0;
        if (outLen > 1) {
            while (didx < outLen - 2) {
                dataDecode[didx] = (((dataDecode[sidx] << 2) & 255) | ((dataDecode[sidx + 1] >> 4) & 003));
                dataDecode[didx + 1] = (((dataDecode[sidx + 1] << 4) & 255) | ((dataDecode[sidx + 2] >> 2) & 017));
                dataDecode[didx + 2] = (((dataDecode[sidx + 2] << 6) & 255) | (dataDecode[sidx + 3] & 077));
                sidx += 4;
                didx += 3;
            }
        }
        
        if (didx < outLen)
            dataDecode[didx] = (((dataDecode[sidx] << 2) & 255) | ((dataDecode[sidx + 1] >> 4) & 003));
        
        if (++didx < outLen)
            dataDecode[didx] = (((dataDecode[sidx + 1] << 4) & 255) | ((dataDecode[sidx + 2] >> 2) & 017));
        
        if (outLen < dataDecode.size())
            dataDecode.resize(outLen);
        return dataDecode;
    }
    
    std::string UrlEncode(const std::string& str)
    {
        std::string src = str;
        char hex[] = "0123456789abcdef";
        string dst;
        for (size_t i = 0; i < src.size(); ++i)
        {
            unsigned char cc = src[i];
            if (cc == ' ')
            {
                dst += "+";
            }
            else if (isalnum(cc) || strchr("-_.!~*'()", cc))
            {
                dst += cc;
            }
            else
            {
                unsigned char c = static_cast<unsigned char>(src[i]);
                dst += '%';
                dst += hex[c>>4];
                dst += hex[c&0xf];
            }
        }
        return dst;
    }
    
    std::string UrlDecode(const std::string& str)
    {
        std::string result;
        int hex = 0;
        for (size_t i = 0; i < str.length(); ++i)
        {
            switch (str[i])
            {
                case '+':
                    result += ' ';
                    break;
                case '%':
                    if (isxdigit(str[i + 1]) && isxdigit(str[i + 2]))
                    {
                        std::string hexStr = str.substr(i + 1, 2);
                        hex = (int)strtol(hexStr.c_str(), 0, 16);
                        //                        if (!((hex >= 48 && hex <= 57) || //0-9
                        //                              (hex >=97 && hex <= 122) || //a-z
                        //                              (hex >=65 && hex <= 90) || //A-Z
                        //                              //[$-_.+!*'(),] [$&+,/:;=?@]
                        //                              hex == 0x21 || hex == 0x23 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28
                        //                              || hex == 0x29 || hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e
                        //                              || hex == 0x2f || hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40
                        //                              || hex == 0x5f
                        //                              ))
                        //                        {
                        result += char(hex);
                        i += 2;
                        //                        }
                        //                        else result += '%';
                    }else {
                        result += '%';
                    }
                    break;
                default:
                    result += str[i];
                    break;
            }
        }
        return result;
        
    }
}



NS_KK_END