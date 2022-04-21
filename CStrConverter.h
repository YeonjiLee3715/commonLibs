//
// Created by 이연지 on 2018-12-31.
//

#ifndef _CSTRCONVERTER_H
#define _CSTRCONVERTER_H

#include <string>
#include <vector>
#include <locale>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <stdexcept>

namespace nsCmn
{
    namespace detail {}

//    template<class Ch>
//    void convertUtf8ToUnicodeLE( const Ch* pSrc, int lenSrc, Ch* pDest, int& lenDest )
//    {
//        int lenConverted = 0;
//        const Ch* pDestEnd = pDest + lenDest;
//
//        if( pSrc == nullptr || lenSrc == 0 )
//            return;
//
//        memset( pDest, 0x00, lenDest );
//
//        wchar_t* pWchDest = reinterpret_cast< wchar_t* >( pDest );
//        int szOne = sizeof( wchar_t );
//
//        for( int idx = 0; idx < lenSrc && idx < lenDest; ++idx )
//        {
//            if( pSrc[idx] <= 127 ) // ansi
//            {
//                pWchDest = pSrc[idx];
//            }
//            else if ((pSrc[idx] & 0xF0) == 0xC0) // 2byte
//            {
//                pWchDest = (( pSrc[idx] & 0x1F ) << 6) + ( pSrc[idx+1] & 0x3F );
//                idx += 1;
//            }
//            else if ((pSrc[idx] & 0xF0) == 0xE0) // 3byte
//            {
//                pWchDest = ((pSrc[idx] & 0x0F) << 12) + ((pSrc[idx+1] & 0x3F) << 6) + (pSrc[idx+2] & 0x3F);
//                idx += 2;
//            }
//            else
//            {
//                return; // ignore 4byte
//            }
//
//            if( reinterpret_cast<const Ch*>(pWchDest+1)+1 >= pDestEnd )
//                break;
//
//            ++pWchDest;
//            lenConverted += szOne;
//        }
//
//        lenDest = lenConverted;
//    }
//
//    template<class Ch>
//    void convertUnicodeLEToUtf8( const Ch* pSrc, int lenSrc, Ch* pDest, int& lenDest )
//    {
//        int lenConverted = 0;
//
//        if( pSrc == nullptr || lenSrc == 0 )
//            return;
//
//        memset( pDest, 0x00, lenDest );
//
//        const wchar_t* pWchSrc = reinterpret_cast< const wchar_t* >( pSrc );
//        int szOne = sizeof( wchar_t );
//        int lenWchSrc = lenSrc/szOne;
//        int lenWchDest = lenDest/szOne;
//
//        for( int idx = 0 ; idx < lenWchSrc && idx < lenWchDest; ++idx )
//        {
//            if( pWchSrc[idx] <= 0x7F )
//            {
//                *(pDest++) = pWchSrc[idx];
//                lenConverted++;
//            }
//            else if(pWchSrc[idx] >= 0x80 && pWchSrc[idx] <=0x7FF)
//            {
//                wchar_t tmp = pWchSrc[idx];
//                char first = 0, second = 0, third = 0;
//                for(int idxUtf8 = 0; idxUtf8 < 3 ; idxUtf8++)
//                {
//                    wchar_t tmp_quota = tmp%16;
//                    switch( idxUtf8 )
//                    {
//                        case 0: third    = tmp_quota; break;
//                        case 1: second   = tmp_quota; break;
//                        case 2: first    = tmp_quota; break;
//                    }
//                    tmp /= 16;
//                }
//
//                *(pDest++) = 0xC0 + (first<<2) + (second>>2);
//                *(pDest++) = 0x80 + (((second%8)%4) << 4) + third;
//                lenConverted +=2;
//            }
//            else if(pWchSrc[idx] >= 0x800 && pWchSrc[idx] <=0xFFFF)
//            {
//                wchar_t tmp = pWchSrc[idx];
//                char chFirst = 0, chSecond = 0, chThird = 0, chFourth = 0;
//                for(int j = 0; j < 4 ; j++)
//                {
//                    wchar_t tmp_quota = tmp%16;
//                    switch(j)
//                    {
//                        case 0: chFourth   = tmp_quota; break;
//                        case 1: chThird    = tmp_quota; break;
//                        case 2: chSecond   = tmp_quota; break;
//                        case 3: chFirst    = tmp_quota; break;
//                    }
//                    tmp /= 16;
//                }
//
//                *(pDest++) = 0xE0 + chFirst;
//                *(pDest++) = 0x80 + chSecond << 2 + chThird >> 2;
//                *(pDest++) = 0x80 + (((chThird%8)%4) << 4) + chFourth;
//                lenConverted +=3;
//            }
//            else
//                return;
//        }
//
//        lenDest = lenConverted;
//    }
//
//    template<class Ch>
//    void convertAnsiToUnicodeLE( const Ch* pSrc, int lenSrc, Ch* pDest, int& lenDest, std::locale const& loc = std::locale())
//    {
//        typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
//        codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
//        std::mbstate_t state = std::mbstate_t();
//
//        int lenConverted = 0;
//        const Ch* pDestEnd = pDest + lenDest;
//
//        if( pSrc == nullptr || lenSrc == 0 )
//            return;
//
//        memset( pDest, 0x00, lenDest );
//
//        int szOne = sizeof( wchar_t );
//        Ch const* pCur = pSrc;
//
//        wchar_t* pWchDest = reinterpret_cast< wchar_t* >( pDest );
//        wchar_t* pWchDestEnd = reinterpret_cast< wchar_t* >( pDest+(lenDest-(szOne/2)) );
//
//        std::codecvt_base::result r = codecvt.in(state,
//                                                 pSrc, pSrc + lenSrc, pCur,
//                                                 pWchDest, pWchDestEnd, pWchDest);
//        if (r == std::codecvt_base::error)
//            throw std::runtime_error("can't convert string to wstring");
//
//        lenConverted = pWchDestEnd;
//    }
//
//    std::string convertUnicodeLEToAnsi( std::wstring const& str, std::locale const& loc = std::locale())
//    {
//        typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
//        codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
//        std::mbstate_t state = std::mbstate_t();
//        std::vector<char> buf((str.size() + 1) * codecvt.max_length());
//        wchar_t const* in_next = str.c_str();
//        char* out_next = &buf[0];
//        std::codecvt_base::result r = codecvt.out(state,
//                                                  str.c_str(), str.c_str() + str.size(), in_next,
//                                                  &buf[0], &buf[0] + buf.size(), out_next);
//        if (r == std::codecvt_base::error)
//            throw std::runtime_error("can't convert wstring to string");
//        return std::string(&buf[0]);
//    }
}


#endif //_CSTRCONVERTER_H
