//
// Created by 이연지 on 2022-04-22.
//

#ifndef CUUID_H
#define CUUID_H

#include "commonLibsDef.h"

#include <iostream>
#include <random>
#include <sstream>

namespace nsCmn
{
    inline unsigned char random_char()
    {
        std::random_device rd;
        std::mt19937 gen(rd()); 
        std::uniform_int_distribution<> dis(0, 255);

        return static_cast<unsigned char>(dis(gen));
    }

    inline std::string generate_hex(const unsigned int len) {
        std::stringstream ss;
        for( unsigned int idx = 0; idx < len; ++idx )
        {
            auto rc = random_char();
            std::stringstream hexstream;
            hexstream << std::hex << int(rc);
            auto hex = hexstream.str(); 
            ss << (hex.length() < 2 ? '0' + hex : hex);
        }        
        return ss.str();
    }

    inline std::string generate_uuid()
    {
        return generate_hex(32);
    }
}


#endif //CUUID_H
