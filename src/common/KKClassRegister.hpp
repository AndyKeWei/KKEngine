//
//  KKClassRegister.hpp
//  KKEngine
//
//  Created by kewei on 4/8/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKClassRegister_hpp
#define KKClassRegister_hpp

#include <map>
#include <string>

template <class T> void* constructor() { return (void *)(new T()); }

struct factory {
    typedef void *(*constructor_t)();
    typedef std::map<std::string, constructor_t> map_type;
    map_type m_classes;
    
    template <class T>
    void register_class(const char *n)
    {
        m_classes.insert(std::make_pair(std::string(n), &constructor<T>));
    }
    
    void *construct(std::string const &n)
    {
        auto itr = m_classes.find(n);
        if (itr == m_classes.end())
            return nullptr;
        return itr->second();
    }
};

extern factory g_factory;

#define REGISTER_CLASS(n) g_factory.register_class<n>(#n)
#define REGISTSER_CLASSAS(n, n1) g_factory.register_class<n>(n1)

#endif /* KKClassRegister_hpp */
