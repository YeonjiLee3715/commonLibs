//
// Created by 이연지 on 2017-05-31.
//

#include "CStrManager.h"
#include <functional>

namespace nsCmn
{
    std::string toString(const int& value)
    {
        return toString<int>(value);
    }

    std::string toString(const long& value)
    {
        return toString<long>(value);
    }

    std::string toString(const double& value)
    {
        return toString<double>(value);
    }

    std::string toString(const float& value)
    {
        return toString<float>(value);
    }

    std::vector<std::string> splitWithSpecialsymbol( const std::string& value, const std::string& symbol, bool skipEmptypart, bool case_sensitive )
    {
        std::vector<std::string> vecRet;
        std::string subStr;

        if( value.empty() || symbol.empty() )
            return  vecRet;

        std::string convertedSymbol = symbol;
        size_t lenConverted = convertedSymbol.length();
        nsCmn::convertSpecialsymbol( &convertedSymbol[0], lenConverted );
        convertedSymbol.resize( lenConverted );

        std::string convertedValue = value;
        lenConverted = convertedValue.length();
        nsCmn::convertSpecialsymbol( &convertedValue[0], lenConverted );
        convertedValue.resize( lenConverted );

        ssize_t pos = 0, prePos = 0;

        do
        {
            pos = nsCmn::findFirstIndexOf( convertedValue, convertedSymbol, pos, case_sensitive );
            std::string subStr;

            if(pos == (ssize_t)std::string::npos )
            {
                if( prePos < (ssize_t)convertedValue.size() )
                    subStr = convertedValue.substr(prePos, (convertedValue.size()-prePos) );
            }
            else
            {
                subStr = convertedValue.substr( prePos, ( pos - prePos )-1 );//skip null
                pos += convertedSymbol.length(); //skip 'symbol'
            }

            if( subStr.empty() )
            {
                if( !skipEmptypart )
                    vecRet.push_back( subStr );
            }
            else
                vecRet.push_back( subStr );

            prePos = pos;
        }while( pos != (ssize_t)std::string::npos );

        return vecRet;
    }

    std::vector<std::string> split( const std::string& value, const std::string& symbol, bool skipEmptypart, bool case_sensitive )
    {
        std::vector<std::string> vecRet;

        ssize_t pos = 0, prePos = 0;

        do
        {
            pos = nsCmn::findFirstIndexOf( value, symbol, pos, case_sensitive );

            std::string subStr;

            if(pos == (ssize_t)std::string::npos )
            {
                if( prePos < (ssize_t)value.size() )
                    subStr = value.substr(prePos, (value.size()-prePos)); //skip null
            }
            else
            {
                subStr = value.substr( prePos, ( pos - prePos ) );//skip symbol
                pos = pos+symbol.length(); //skip 'symbol'
            }

            if( subStr.empty() )
            {
                if( !skipEmptypart )
                    vecRet.push_back( subStr );
            }
            else
                vecRet.push_back( subStr );

            prePos = pos;
        }while( pos != (ssize_t)std::string::npos );

        return vecRet;
    }


    std::vector<std::string> split( const std::string& value, const std::vector< std::string >& vecSymbols, bool skipEmptypart , bool case_sensitive )
    {
        std::vector<std::string> vecRet;

        if( value.empty() )
        {
            if( skipEmptypart == false )
                vecRet.push_back( std::string() );

            return vecRet;
        }

        const char* pCur = value.c_str();
        const char* pPreMatch = value.c_str();
        const char* pEnd = value.c_str() + value.size();

        if( vecSymbols.empty() )
        {
            vecRet.push_back( value );
            return vecRet;
        }

        //sort
        std::vector< std::string > vecSortedSymbols = vecSymbols;
        std::sort( vecSortedSymbols.begin(), vecSortedSymbols.end(), [](const std::string& arg1, const std::string& arg2)
        {
            ssize_t loopMax = arg1.size() < arg2.size() ? arg1.size() : arg2.size();
            const char* p1 = arg1.data();
            const char* p2 = arg2.data();

            for( ssize_t idx = 0; idx < loopMax; ++idx )
            {
                if( p1 == NULL || p2 == NULL )
                    return p1 > p2;

                if( tolower(*p1) != tolower(*p2) )
                    return *p1 > *p2;

                ++p1;
                ++p2;
            }

            return arg1.size() > arg2.size();
        });

        while( pCur != NULL && pCur < pEnd )
        {
            bool isMatch = false;

            for(  std::vector< std::string >::const_iterator it = vecSortedSymbols.cbegin()
                    ; it != vecSortedSymbols.cend(); ++it )
            {
                if( pCur + it->size() >= pEnd )
                    continue;

                if( nsCmn::compare( it->data(), it->size(), pCur, it->size(), case_sensitive ) )
                {
                    isMatch = true;
                    if( pCur == pPreMatch )
                    {
                        if( skipEmptypart == false )
                            vecRet.push_back( std::string() );
                    }
                    else
                        vecRet.push_back( std::string( pPreMatch, pCur-pPreMatch ) );

                    pCur += it->size();
                    pPreMatch = pCur;
                    isMatch = true;
                    break;
                }
            }

            if( isMatch == false )
                ++pCur;
        }

        if( pCur != NULL && pPreMatch != pCur )
            vecRet.push_back( std::string( pPreMatch, pCur-pPreMatch ) );

        return vecRet;
    }

    void trim(std::string& s)
    {
#if !defined(__APPLE__) && __cplusplus > 201402L //C++17
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !isspace( c ); } ) );
        s.erase( std::find_if( s.rbegin(), s.rend(), [](int c) { return !isspace( c ); }).base(), s.end());
#else
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
#endif
    }

}
