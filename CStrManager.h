//
// Created by 이연지 on 2017-05-31.
//

#ifndef CSTRMANAGER_H
#define CSTRMANAGER_H

#include "commonLibsDef.h"

#include <algorithm>
#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <cstring>

namespace nsCmn
{
    namespace detail
    {
        typedef enum eMovePointer
        {
            MOVE_NONE = 0,
            MOVE_DST = 1,
            MOVE_SRC = 2,
            MOVE_BOTH = 3
        }eMovePointer;

        // Find length of the string
        template<class Ch>
        inline std::size_t measure(const Ch *p)
        {
            if( p == NULL )
                return 0;

            const Ch *tmp = p;
            while (*tmp)
                ++tmp;
            return tmp - p;
        }

        template<class Ch>
        inline bool compare(const Ch *p1, size_t size1, const Ch *p2, size_t size2, bool case_sensitive)
        {
            if( size1 != size2 )
                return false;

            if( case_sensitive )
            {
                for( const Ch *end = p1 + size1; p1 < end; ++p1, ++p2 )
                    if( p1 == NULL || p2 == NULL || *p1 != *p2 )
                        return false;
            }
            else
            {
                for( const Ch *end = p1 + size1; p1 < end; ++p1, ++p2 )
                    if( p1 == NULL || p2 == NULL || tolower(*p1) != tolower(*p2) )
                        return false;
            }
            return true;
        }

        template<class Ch>
        inline Ch* findFirstOf( Ch* pSrc, size_t lenSrc, Ch* keyword, size_t lenKeyword, ssize_t pos = -1, bool case_sensitive = true )
        {
            Ch* pDst = NULL;

            if( pSrc == NULL || keyword == NULL || lenSrc <= 0 || lenKeyword <= 0 || pos >= (ssize_t)lenSrc )
                return  pDst;

            Ch* pCur;
            if( pos < 0 )
                pCur = pSrc;
            else
                pCur = pSrc + pos;

            const Ch* pSrcEnd = pSrc+( (ssize_t)lenSrc - ((ssize_t)lenKeyword-1) );

            while( pCur < pSrcEnd )
            {
                if(nsCmn::detail::compare( pCur, lenKeyword, keyword, lenKeyword, case_sensitive) == true)
                {
                    pDst = pCur;
                    break;
                }
                ++pCur;
            }

            return pDst;
        }

        template<class Ch>
        inline ssize_t findFirstIndexOf( Ch* pSrc, size_t lenSrc, Ch* keyword, size_t lenKeyword, ssize_t pos = -1, bool case_sensitive = true )
        {
            ssize_t posDst = -1;

            if( pSrc == NULL || keyword == NULL || lenSrc <= 0 || lenKeyword <= 0 || pos >= (ssize_t)lenSrc )
                return  posDst;

            Ch* pCur;
            if( pos < 0 )
                pCur = pSrc;
            else
                pCur = pSrc + pos;

            const Ch* pSrcEnd = pSrc+( (ssize_t)lenSrc - ((ssize_t)lenKeyword-1) );

            while( pCur < pSrcEnd )
            {
                if(nsCmn::detail::compare( pCur, lenKeyword, keyword, lenKeyword, case_sensitive) == true)
                {
                    posDst = pCur-pSrc;
                    break;
                }
                ++pCur;
            }

            return  posDst;
        }

        template<class Ch>
        inline Ch* findLastOf( Ch* pSrc, size_t lenSrc, Ch* keyword, size_t lenKeyword, ssize_t pos = -1, bool case_sensitive = true )
        {
            Ch* pDst = NULL;

            if( pSrc == NULL || keyword == NULL || lenSrc <= 0 || lenKeyword <= 0 || pos > (ssize_t)lenSrc )
                return  pDst;

            if( pos < 0 || pos > ( (ssize_t)lenSrc - (ssize_t)lenKeyword ) )
                pos = ( (ssize_t)lenSrc - (ssize_t)lenKeyword );

            Ch* pCur = pSrc + pos;

            while( pCur >= pSrc )
            {
                if( nsCmn::detail::compare( pCur, lenKeyword, keyword, lenKeyword, case_sensitive ) == true )
                {
                    pDst = pCur;
                    break;
                }
                --pCur;
            }

            return pDst;
        }

        template<class Ch>
        inline ssize_t findLastIndexOf( Ch* pSrc, size_t lenSrc, Ch* keyword, size_t lenKeyword, ssize_t pos = -1, bool case_sensitive = true )
        {
            ssize_t posDst = -1;

            if( pSrc == NULL || keyword == NULL || lenSrc <= 0 || lenKeyword <= 0 || pos > (ssize_t)lenSrc )
                return  posDst;

            if( pos < 0 || pos > ( (ssize_t)lenSrc - (ssize_t)lenKeyword ) )
                pos = ( (ssize_t)lenSrc - (ssize_t)lenKeyword );

            Ch* pCur = pSrc + pos;

            while( pCur >= pSrc )
            {
                if( nsCmn::detail::compare( pCur, lenKeyword, keyword, lenKeyword, case_sensitive ) == true )
                {
                    posDst = pCur - pSrc;
                    break;
                }
                --pCur;
            }

            return  posDst;
        }
    }

    template<typename ... Args>
    std::string string_format( const char* format, Args ... args )
    {
        size_t size = snprintf( NULL, 0, format, args ... ) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf( new char[ size ] );
        snprintf( buf.get(), size, format, args ... );
        std::string res ( buf.get(), buf.get() + size - 1 );
        return res; // We don't want the '\0' inside
    }

    template<class Ch>
    inline bool
    compare( const Ch* p1, std::size_t size1, const Ch* p2, std::size_t size2, bool case_sensitive = true )
    {
        if(p1 == NULL && p2 == NULL)
            return true;

        if(p1 == NULL || p2 == NULL)
            return false;

        return nsCmn::detail::compare( p1, size1, p2, size2, case_sensitive );
    }

    template<class Ch>
    inline bool compare( const Ch* p1, std::size_t size1, const Ch* keyword, bool case_sensitive = true )
    {
        if(p1 == NULL && keyword == NULL)
            return true;

        if(p1 == NULL || keyword == NULL)
            return false;

        size_t size2 = nsCmn::detail::measure( keyword );

        return nsCmn::detail::compare( p1, size1, keyword, size2, case_sensitive );
    }

    template<class Ch>
    inline bool compare( const Ch* p1, const Ch* p2, bool case_sensitive = true )
    {
        if(p1 == NULL && p2 == NULL)
            return true;

        if(p1 == NULL || p2 == NULL)
            return false;

        size_t size1 = nsCmn::detail::measure( p1 );
        size_t size2 = nsCmn::detail::measure( p2 );

        return nsCmn::detail::compare( p1, size1, p2, size2, case_sensitive );
    }

    template<class Ch>
    inline Ch* findFirstOf( Ch* pSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        if( pSrc == NULL || keyword == NULL )
            return NULL;

        size_t lenSrc = detail::measure( pSrc );
        size_t lenKeyword = detail::measure( keyword );

        return detail::findFirstOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline Ch* findFirstOf( Ch* pSrc, size_t lenSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        if( keyword == NULL )
            return NULL;

        size_t lenKeyword = detail::measure( keyword );

        return detail::findFirstOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline ssize_t findFirstIndexOf( Ch* pSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        size_t lenSrc = detail::measure( pSrc );
        size_t lenKeyword = detail::measure( keyword );

        return detail::findFirstIndexOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline ssize_t findFirstIndexOf( Ch* pSrc, size_t lenSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        size_t lenKeyword = detail::measure( keyword );

        return detail::findFirstIndexOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    inline ssize_t findFirstIndexOf( const std::string& src, const std::string& keyword, ssize_t pos = std::string::npos, bool case_sensitive = true )
    {
        ssize_t posDst = std::string::npos;

        if( src.empty() || keyword.empty() ||( pos >= (ssize_t)src.size() ) )
            return posDst;

        const char*  pCur;
        if( pos <= 0 )
            pCur = src.c_str();
        else
            pCur = src.c_str() + pos;

        const char* pSrcEnd = ( src.c_str() + (ssize_t)src.size())-( (ssize_t)keyword.length()-1 );

        while( pCur < pSrcEnd )
        {
            if(nsCmn::compare( pCur, keyword.length(), keyword.c_str(), keyword.length(), case_sensitive) == true)
            {
                posDst = pCur - src.c_str();
                break;
            }
            ++pCur;
        }

        return  posDst;
    }

    template<class Ch>
    inline Ch* findLastOf( Ch* pSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        if( pSrc == NULL || keyword == NULL )
            return NULL;

        size_t lenSrc = detail::measure( pSrc );
        size_t lenKeyword = detail::measure( keyword );

        return detail::findLastOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline Ch* findLastOf( Ch* pSrc, size_t lenSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        if( keyword == NULL )
            return NULL;

        size_t lenKeyword = detail::measure( keyword );

        return detail::findLastOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline ssize_t findLastIndexOf( Ch* pSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        size_t lenSrc = detail::measure( pSrc );
        size_t lenKeyword = detail::measure( keyword );

        return detail::findLastIndexOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    template<class Ch>
    inline ssize_t findLastIndexOf( Ch* pSrc, size_t lenSrc, Ch* keyword, ssize_t pos = -1, bool case_sensitive = true )
    {
        size_t lenKeyword = detail::measure( keyword );

        return detail::findLastIndexOf( pSrc, lenSrc, keyword, lenKeyword, pos, case_sensitive );
    }

    inline ssize_t findLastIndexOf( const std::string& src, const std::string& keyword, ssize_t pos = std::string::npos, bool case_sensitive = true )
    {
        ssize_t posDst = std::string::npos;
        if( src.empty() || keyword.empty() || pos > (int)src.size() )
            return  posDst;

        if( pos < 0 || pos > ( (ssize_t)src.size() - (ssize_t)keyword.size() ) )
            pos = ( (ssize_t)src.size() - (ssize_t)keyword.size() );

        const char*  pCur = src.c_str() + pos;

        while( pCur >= src.c_str() )
        {
            if( nsCmn::compare( pCur, keyword.length(), keyword.c_str(), keyword.length(), case_sensitive ) == true )
            {
                posDst = pCur - src.c_str();
                break;
            }
            --pCur;
        }

        return  posDst;
    }

    template<class Ch>
    bool startWith( Ch* pSrc, Ch* keyword, size_t lenKeyword, bool case_sensitive = true )
    {
        if( pSrc == NULL )
            return false;

        if( nsCmn::detail::compare( pSrc, lenKeyword, keyword, lenKeyword, case_sensitive) )
            return true;

        return false;
    }

    template<class Ch>
    bool endWith( Ch* pSrc, size_t lenSrc, Ch* keyword, size_t lenKeyword, bool case_sensitive = true )
    {
        if( pSrc == NULL )
            return false;

        if( lenSrc < lenKeyword )
            return false;

        Ch* pSrcPos = pSrc+(lenSrc-lenKeyword);
        if( nsCmn::detail::compare( pSrcPos, lenKeyword, keyword, lenKeyword, case_sensitive) )
            return true;

        return false;
    }

    template<class Ch>
    Ch* getLine( Ch* pSrc, size_t lenSrc, size_t& lenLine )
    {
        if( pSrc == NULL )
        {
            lenLine = 0;
            return NULL;
        }

        Ch* pNext = NULL;
        Ch lineEnd = '\n';
        pNext = detail::findFirstOf( pSrc, lenSrc, &lineEnd, 1, 0, true );

        if( pNext != NULL )
        {
            ++pNext; //go next line
            lenLine = pNext - pSrc;

            if( pNext >= (pSrc+lenSrc) )
                pNext = NULL;
        }
        else
            lenLine = lenSrc;

        return pNext;
    }

    template<class Ch>
    size_t trim( Ch* pSrc, size_t lenSrc )
    {
        Ch* pDst = pSrc;
        Ch* pPos = pSrc;
        Ch* pEnd = pSrc + lenSrc;

        while( pPos != NULL && pPos < pEnd )
        {
            switch( *pPos )
            {
            case 0x09: break; //\t
            case 0x0A: break; //\n
            case 0x0B: break; //\v
            case 0x0D: break; //\r
            default:
                *pDst++ = *pPos;
                break;
            }

            ++pPos;
        }

        return pDst - pPos;
    }

    void trim(std::string& s);

    template<class Ch>
    char convertToByte( const Ch* value )
    {
        long ldData = 0;

        if( value == NULL )
            return 0;

        // 16진수 문자열로 작성 되어야 함. "0x4F" 등
        Ch chValue[5] = { value[0], value[1], value[2], value[3], 0x00 };
        ldData = static_cast<char>( std::strtol( reinterpret_cast<const char*>( chValue ), NULL, 16 ) );

        return static_cast<char>( ldData );
    }

    template<class Ch>
    void convertSpecialsymbol( Ch* value, size_t& len )
    {
        if( value == NULL )
            return;

        size_t pos = 0, lenConverted = 0;
        while( pos < len )
        {
            if( value[pos] == '\\' )
            {
                pos++;
                if (pos >= len)
                    break;

                switch (value[pos]) {
                    case '0':value[lenConverted++] = 0x00; break; //\0
                    case 'b': value[lenConverted++] = 0x08; break; //\b
                    case 't': value[lenConverted++] = 0x09; break; //\t
                    case 'n': value[lenConverted++] = 0x0A; break; //\n
                    case 'r': value[lenConverted++] = 0x0D; break; //\r
                    case 'f': value[lenConverted++] = 0x0C; break; //\f
                    case '\"': value[lenConverted++] = 0x22; break; //"
                    case '\'': value[lenConverted++] = 0x27; break; //'
                    case '\\': value[lenConverted++] = 0x5C; break;
                    case '|': value[lenConverted++] = 0x7C; break; //|
                    default: value[lenConverted++] = value[pos];  break;
                }
            }
            else
            {
                if( value[pos] == '0' && pos+3 < len && value[pos+1] == 'x' )
                {
                    value[lenConverted++] = convertToByte( &value[pos] );
                    pos += 3;
                }
                else
                    value[lenConverted++] = value[pos];
            }

            ++pos;
        }
        len = (size_t)lenConverted;
    }

    template<class Ch>
    inline void copyAndMovePos( Ch* &pDst, const Ch* &pSrc, size_t len, detail::eMovePointer type = detail::MOVE_DST )
    {
        if( pSrc == NULL || len <= 0 )
            return;

        memmove(pDst, pSrc, len);

        switch(type)
        {
            case detail::MOVE_NONE:
                break;
            case detail::MOVE_BOTH:
                pSrc += len;
                pDst += len;
                break;
            case detail::MOVE_SRC:
                pSrc += len;
                break;
            case detail::MOVE_DST:
                pDst += len;
                break;
        }
    }

    inline const char* boolToText( bool isTrue )
    {
        return (isTrue == true ? "true" : "false");
    }

    template<typename T>
    inline std::string toString(const T& value)
    {
        std::string ret;

        std::ostringstream oss;
        oss << value;
        ret = oss.str();

        return ret;
    }

    std::string toString(const int& value);
    std::string toString(const long& value);
    std::string toString(const double& value);
    std::string toString(const float& value);

    template<class Ch>
    inline std::string toString(Ch* value, size_t len = 0)
    {
        std::string ret;

        if( value == NULL )
            return  ret;

        if(len <= 0)
        {
            std::ostringstream oss;
            oss << value;
            ret = oss.str();
        }
        else
        {
            //1Byte 이상의 자료형은 len을 0으로 해서 처리하세요. 문자열 전용!
            if( sizeof(Ch) > 1 )
                return ret;

            ret.resize( (len*sizeof(char))+1, '\0' );
            Ch* pos = reinterpret_cast<Ch*>( &ret[0] );

            memcpy(pos, value, len*sizeof(Ch) );
        }

        return ret;
    }

    template<class Ch>
    inline bool IsTrue( const Ch* value, ssize_t len = -1 )
    {
        bool isTrue = false;

        if( len == 0 )
            return isTrue;

        size_t strLen = 0;

        if( len < 0 )
            strLen = nsCmn::detail::measure( value );
        else
            strLen = static_cast<std::size_t>( len );

        if( strLen == 0 )
            return isTrue;

        if( nsCmn::compare( value, strLen, "Y", 1, false)  )
            isTrue = true;
        else if( nsCmn::compare( value, strLen, "1", 1, false)  )
            isTrue = true;
        else if( nsCmn::compare( value, strLen, "T", 1, false)  )
            isTrue = true;
        else if( nsCmn::compare( value, strLen, "TRUE", 4, false)  )
            isTrue = true;
        else if( nsCmn::compare( value, strLen, "YES", 3, false)  )
            isTrue = true;

        return isTrue;
    }

    template<class T>
    inline bool IsTrue( const T value )
    {
        bool isTrue = false;

        if( value > 0  )
            isTrue = true;

        return isTrue;
    }

    template<class Ch>
    inline Ch* toLower(const Ch *p, std::size_t size)
    {
        if( size <= 0 || p == NULL )
            return NULL;

        Ch strLow[ size+1 ];
        std::memset( strLow, '\0', size+1 );

        Ch* pLow = strLow;

        for( const Ch *end = p + size; p < end; ++p, ++pLow )
            *pLow = ::tolower( *p );

        return strLow;
    }

    std::vector<std::string> splitWithSpecialsymbol( const std::string& value, const std::string& symbol, bool skipEmptypart = true, bool case_sensitive = true );
    std::vector<std::string> split( const std::string& value, const std::string& symbol, bool skipEmptypart = true, bool case_sensitive = true );
    std::vector<std::string> split( const std::string& value, const std::vector< std::string >& vecSymbols, bool skipEmptypart = true, bool case_sensitive = true );

    template<class Ch>
    inline void removeFrontAndBackSpace( Ch* p, std::size_t& size )
    {
        if( p == nullptr || size <= 0 )
        {
            return;
        }

        Ch* pSrc = p;
        ssize_t posStart = 0;
        ssize_t posEnd = size;
        // 앞 공백 건너뛰기
        while( posStart < size )
        {
            if(pSrc[posStart] != 0x20 /*(BYTE)' '*/)
                break;

            ++posStart;
        }

        //뒷 공백, 줄바꿈 문자 제거
        while( posEnd > 0 )
        {
            if( pSrc[posEnd-1] != 0x20 /*(BYTE)' '*/ )
            {
                if( pSrc[posEnd-1] == 0x00 )
                    --posEnd;
                else if( pSrc[posEnd-1] == 0x0D || pSrc[posEnd-1] == 0x0A )
                {
                    --posEnd;
                    if( pSrc[posEnd-1] == 0x0D )
                        --posEnd;
                }

                break;
            }

            --posEnd;
        }

        if( posStart > 0 || posEnd < size )
        {
            ssize_t lenLine = posEnd - posStart;
            memmove( pSrc, &pSrc[posStart], lenLine );

            std::memset( &pSrc[lenLine], '\0', size-lenLine );
            size = lenLine;
        }
    }

    inline std::string replace( std::string& src, const std::string& before, const std::string& after, ssize_t pos = std::string::npos, bool case_sensitive = true )
    {
        while( ( pos = findFirstIndexOf( src, before, pos, case_sensitive ) ) != std::string::npos )
        {
            src.replace( pos, before.length(), after );
            pos += (ssize_t)after.length();
        }

        return src;
    }
}


#endif //CSTRMANAGER_H
