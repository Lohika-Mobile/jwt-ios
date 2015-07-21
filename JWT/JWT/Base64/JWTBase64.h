//
//  JWTBase64.h
//
//  Created by Alexander Trishyn on 7/6/15.
//  Copyright (c) 2015 Lohika. BSD license.
//

#ifndef JWT_BASE64_H
#define JWT_BASE64_H

#include <string>

namespace jwt {

class Base64
{
public:
    static std::string decode(const std::string& data);
    static std::string encode(const std::string& data);
};
    
}

#endif // JWT_BASE64_H
