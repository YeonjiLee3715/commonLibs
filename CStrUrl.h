//
// Created by 이연지 on 2022-04-22.
//

#ifndef CSTRURL_H
#define CSTRURL_H

#include "commonLibsDef.h"

#include <iostream>
#include <cstring>
#include <memory>
#include <sstream>

namespace nsCmn
{
	inline bool IsBase64( unsigned char c )
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	template<class Ch>
	inline std::string encodeBase64( Ch const* pSrc, size_t len )
	{
		std::string ret;
		int idxArr3 = 0;

		unsigned char arrChar_3[3];
		unsigned char arrChar_4[4];

		static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
												"abcdefghijklmnopqrstuvwxyz"
												"0123456789+/";

		for( size_t lenProcced = 0; lenProcced < len; ++lenProcced )
		{
			arrChar_3[idxArr3] = (*pSrc);
			++idxArr3;
			++pSrc;

			if( idxArr3 == 3 )
			{
				arrChar_4[0] = (arrChar_3[0] & 0xfc) >> 2;
				arrChar_4[1] = ((arrChar_3[0] & 0x03) << 4) + ((arrChar_3[1] & 0xf0) >> 4);
				arrChar_4[2] = ((arrChar_3[1] & 0x0f) << 2) + ((arrChar_3[2] & 0xc0) >> 6);
				arrChar_4[3] = arrChar_3[2] & 0x3f;

				for( int idxArr4 = 0; idxArr4 < 4; ++idxArr4 )
					ret += base64_chars[arrChar_4[idxArr4]];
				idxArr3 = 0;
			}
		}

		if( idxArr3 )
		{
			for( int idxReset = idxArr3; idxReset < 3; ++idxReset )
				arrChar_3[idxReset] = '\0';

			arrChar_4[0] = ( arrChar_3[0] & 0xfc) >> 2;
			arrChar_4[1] = ((arrChar_3[0] & 0x03) << 4) + ((arrChar_3[1] & 0xf0) >> 4);
			arrChar_4[2] = ((arrChar_3[1] & 0x0f) << 2) + ((arrChar_3[2] & 0xc0) >> 6);

			for ( int idxArr4 = 0; (idxArr4 < idxArr3 + 1); ++idxArr4 )
				ret += base64_chars[ arrChar_4[idxArr4] ];

			while( idxArr3 < 3 )
			{
				ret += '=';
				++idxArr3;
			}
		}

	  return ret;

	}

	inline std::string encodeUrl( const std::string& strValue )
	{
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for(std::string::const_iterator it = strValue.begin(), n = strValue.end(); it != n; ++it )
		{
			std::string::value_type vtype = (*it);

			// Keep alphanumeric and other accepted characters intact
			if( std::isalnum( vtype ) || vtype == '-' || vtype == '_' || vtype == '.' || vtype == '~')
			{
				escaped << vtype;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)vtype);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}
}


#endif //CSTRURL_H
