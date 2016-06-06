//
//  KKStringUtil.hpp
//  KKEngine
//
//  Created by kewei on 1/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKStringUtil_hpp
#define KKStringUtil_hpp

#include "KKMacros.h"
#include "md5.h"
#include <vector>

#define LEN 100

NS_KK_BEGIN

namespace StringUtil {
    
    inline void StringToUpper(std::string &str)
    {
        if (str.empty())
        {
            return;
        }
        
        size_t size = str.size();
        for (size_t i = 0; i < size; ++i)
        {
            str[i] = toupper(str[i]);
        }
    }
    
    inline void StringToLower(std::string &str)
    {
        if (str.empty())
        {
            return;
        }
        
        size_t size = str.size();
        for (size_t i = 0; i < size; ++i)
        {
            str[i] = tolower(str[i]);
        }
    }
    
    inline std::string & ReplaceAll(std::string &str, const char *toReplace, const char *replace)
    {
        size_t pos = str.find(toReplace);
        size_t len = strlen(toReplace);
        
        while (pos != std::string::npos) {
            str = str.replace(pos, len, replace);
            pos = str.find(toReplace, pos+1);
        }
        
        return str;
    }
    
    inline std::string& Trim(std::string& str)
    {
        std::string::size_type p = str.find_first_not_of(" \t\r\n");
        if (p == std::string::npos)
        {
            str = "";
            return str;
        }
        std::string::size_type q = str.find_last_not_of(" \t\r\n");
        str = str.substr(p, q-p+1);
        return str;
    }
    
    inline unsigned long Hash(const char * str)
    {
        const unsigned long prime1 = 4224542477ul;
        const unsigned long prime2 = 3264857ul;
        unsigned long hash = 0;
        for (const char * c = str; *c; c++)
        {
            hash += prime1 * hash + *c + 1 + (hash % prime2);
        }
        return hash;
    }
    
    inline unsigned int BKDRHash(const char *str)
    {
        unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
        unsigned int hash = 0;
        
        while (*str)
        {
            hash = hash * seed + (*str++);
        }
        
        return (hash & 0x7FFFFFFF);
    }
    
    inline void Split(std::vector<std::string>& vs,
                      const std::string& line,
                      char dmt)
    {
        std::string::size_type p=0;
        std::string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            std::string str = line.substr(p,q-p);
            Trim(str);
            vs.push_back(str);
            if(q == std::string::npos) break;
            p = q+1;
        }
    }
    
    inline void Split(std::vector<std::string>& vs,
                      const std::string& line,
                      const std::string& dmt)
    {
        std::string::size_type p=0;
        std::string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            std::string str = line.substr(p,q-p);
            Trim(str);
            vs.push_back(str);
            if(q == std::string::npos) break;
            p = q+dmt.length();
        }
    }
    
    inline void SplitInt(std::vector<int>& vs,
                         const std::string& line,
                         char dmt)
    {
        std::string::size_type p=0;
        std::string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            std::string str = line.substr(p,q-p);
            Trim(str);
            if(!str.empty()) vs.push_back(atoi(str.c_str()));
            if(q == std::string::npos) break;
            p = q+1;
        }
    }
    
    inline std::string Format(const char * fmt, ...)
    {
        std::string str;
        int size = LEN;
        va_list ap;
        while (1) {
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
            va_end(ap);
            if (n > -1 && n < size)
            {
                str.resize(n);
                return str;
            }
            else
                size *= 2;
        }
        return str;
    }
    
    std::string MD5(const char *str);
    
    std::string Base64Encode(const unsigned char* data, int len);
    
    std::vector<unsigned char> Base64Decode(const char* data);
    
    std::string UrlEncode(const std::string& str);
    std::string UrlDecode(const std::string& str);
}


NS_KK_END

#define KK_STRING_FORMAT(format, ...) cocos2d::StringUtil::Format(format, ##__VA_ARGS__);
#define KK_STRING_BKDRHASH(str) cocos2d::StringUtil::BKDRHash(#str);

#endif /* KKStringUtil_hpp */
