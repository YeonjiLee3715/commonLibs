//
// Created by 이연지 on 2017-06-14.
//

#ifndef AROUNDVIEW_CMNTYPEDEF_H
#define AROUNDVIEW_CMNTYPEDEF_H

#include "commonLibsDef.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <cctype>
#include <functional>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

namespace nsCmn
{
    class ByteBuffer
    {
    public:

        ByteBuffer():m_data( NULL), m_len(0){}
        ByteBuffer( const size_t& len );
        ByteBuffer( const unsigned char& data );
        ByteBuffer( const ByteBuffer& buff );
        ByteBuffer( const char* data, ssize_t len = -1 );
        ByteBuffer( const unsigned char* data, ssize_t len = -1 );

        ~ByteBuffer();

        void clear();

        void setBuffer( size_t len);
        void setBuffer( const ByteBuffer& buff );
        void setBuffer( const char* data, size_t len );
        void setBuffer( const unsigned char* data, size_t len);

        void push_front( const unsigned char& data );
        void push_front( const ByteBuffer& buff );
        void push_front( const char* data, size_t len );
        void push_front( const unsigned char* data, size_t len );

        void push_back( const unsigned char& data );
        void push_back( const ByteBuffer& buff );
        void push_back( const char* data, size_t len );
        void push_back( const unsigned char* data, size_t len);

        ByteBuffer substr( size_t posStart = 0, ssize_t len = -1 ) const;

        void resize( size_t len );

        void removeRef( const unsigned char keyword, bool case_sensitive = true );
        void remove( const unsigned char* keyword, const size_t& lenKeyword, bool case_sensitive = true );
        void removeAt( size_t pos, size_t len );

        unsigned char* data() const { return m_data; }
        size_t len() const { return m_len; }

        unsigned char* data() { return m_data; }

        void replaceRef( const unsigned char* src, const size_t& lenSrc, const size_t& pos, ssize_t len = 0);
        void replace( const ByteBuffer& src, const size_t& pos, ssize_t len = 0 );

        unsigned char* seek( const ssize_t& pos ) const { if( pos < 0 || pos >= m_len ) return NULL; else return m_data + pos; }
        unsigned char* seek( const ssize_t& pos ) { if( pos < 0 || pos >= m_len ) return NULL; else return m_data + pos; }

        unsigned char at( const ssize_t& pos ) const { if( pos < 0 || pos >= m_len ) return 0x00; else return *(m_data + pos); }
        unsigned char at( const ssize_t& pos ) { if( pos < 0 || pos >= m_len ) return 0x00; else return *(m_data + pos); }

        unsigned char front() const { if(isEmpty()) return 0x00; return *m_data; }
        unsigned char back() const { if(isEmpty()) return 0x00; return *(m_data + m_len-1); }

        unsigned char* begin() const { return m_data; }
        unsigned char* end() const { return m_data + m_len; }

        unsigned char* begin() { return m_data; }
        unsigned char* end() { return m_data + m_len; }

        bool isEmpty() const { return ( m_data == NULL || m_len == 0 ); }

        const char* c_str() const { return reinterpret_cast<const char*>( m_data ); }

        std::string toStdString() const { return std::string( c_str(), len() ); }

        std::vector<ByteBuffer> splitWithSpecialsymbol( const ByteBuffer& symbol, bool skipEmptypart = true, bool case_sensitive = true ) const;
        std::vector<ByteBuffer> split( const ByteBuffer& symbol, bool skipEmptypart = true, bool case_sensitive = true ) const;
        std::vector<ByteBuffer> split( const std::vector< nsCmn::ByteBuffer >& vecSymbols, bool skipEmptypart = true, bool case_sensitive = true ) const;

        ByteBuffer* operator()( const char* data, size_t len );
        ByteBuffer* operator()( const unsigned char* data, size_t len);
        ByteBuffer* operator()( const ByteBuffer& buff );
        ByteBuffer* operator= ( const ByteBuffer& buff );
        ByteBuffer* operator+ ( const ByteBuffer& buff );

    private:
        unsigned char* newArray( size_t len );

    private:

        unsigned char* m_data;
        size_t m_len;
    };

#if !defined(__APPLE__) && __cplusplus > 201402L //C++17
    template<class Arg1, class Arg2, class Result>
    struct binary_function
    {
        using first_argument_type = Arg1;
        using second_argument_type = Arg2;
        using result_type = Result;
    };

    struct byte_less: binary_function<nsCmn::ByteBuffer, nsCmn::ByteBuffer, bool>
    {
        // case-independent (ci) compare_less binary function
        struct nocase_compare: public binary_function<unsigned char, unsigned char, bool>
        {
            bool operator() ( const unsigned char& c1, const unsigned char& c2 ) const
            {
                return tolower( c1 ) < tolower( c2 );
            }
        };
        bool operator() ( const nsCmn::ByteBuffer& s1, const nsCmn::ByteBuffer& s2 ) const
        {
            return std::lexicographical_compare
            ( s1.begin(), s1.end(),   // source range
                s2.begin(), s2.end(),   // dest range
                nocase_compare() );  // comparison
        }
    };

    struct byte_less_desc: binary_function<nsCmn::ByteBuffer, nsCmn::ByteBuffer, bool>
    {
        bool operator() ( const nsCmn::ByteBuffer& s1, const nsCmn::ByteBuffer& s2 ) const
        {

            size_t loopMax = s1.len() < s2.len() ? s1.len() : s2.len();
            const char* p1 = s1.c_str();
            const char* p2 = s2.c_str();

            for( size_t idx = 0; idx < loopMax; ++idx )
            {
                if( p1 == NULL || p2 == NULL )
                    return p1 > p2;

                if( tolower( *p1 ) != tolower( *p2 ) )
                    return *p1 > * p2;

                ++p1;
                ++p2;
            }

            return s1.len() > s2.len();
        }
    };

    struct ci_less: binary_function<std::string, std::string, bool>
    {
        // case-independent (ci) compare_less binary function
        struct nocase_compare: public binary_function<unsigned char, unsigned char, bool>
        {
            bool operator() ( const unsigned char& c1, const unsigned char& c2 ) const
            {
                return tolower( c1 ) < tolower( c2 );
            }
        };
        bool operator() ( const std::string& s1, const std::string& s2 ) const
        {
            return std::lexicographical_compare
            ( s1.begin(), s1.end(),   // source range
                s2.begin(), s2.end(),   // dest range
                nocase_compare() );  // comparison
        }
    };
#else
    struct byte_less : std::binary_function<nsCmn::ByteBuffer, nsCmn::ByteBuffer, bool>
    {
        // case-independent (ci) compare_less binary function
        struct nocase_compare : public std::binary_function<unsigned char,unsigned char,bool>
        {
            bool operator() (const unsigned char& c1, const unsigned char& c2) const {
                return tolower (c1) < tolower (c2);
            }
        };
        bool operator() (const nsCmn::ByteBuffer& s1, const nsCmn::ByteBuffer& s2) const {
            return std::lexicographical_compare
                    (s1.begin(), s1.end(),   // source range
                     s2.begin(), s2.end(),   // dest range
                     nocase_compare ());  // comparison
        }
    };

    struct byte_less_desc : std::binary_function<nsCmn::ByteBuffer, nsCmn::ByteBuffer, bool>
    {
        bool operator() (const nsCmn::ByteBuffer& s1, const nsCmn::ByteBuffer& s2) const {

                size_t loopMax = s1.len() < s2.len() ? s1.len() : s2.len();
                const char* p1 = s1.c_str();
                const char* p2 = s2.c_str();

                for( size_t idx = 0; idx < loopMax; ++idx )
                {
                    if( p1 == NULL || p2 == NULL )
                        return p1 > p2;

                    if( tolower(*p1) != tolower(*p2) )
                        return *p1 > *p2;

                    ++p1;
                    ++p2;
                }

                return s1.len() > s2.len();
        }
    };

    struct ci_less : std::binary_function<std::string, std::string, bool>
    {
        // case-independent (ci) compare_less binary function
        struct nocase_compare : public std::binary_function<unsigned char,unsigned char,bool>
        {
            bool operator() (const unsigned char& c1, const unsigned char& c2) const {
                return tolower (c1) < tolower (c2);
            }
        };
        bool operator() (const std::string & s1, const std::string & s2) const {
            return std::lexicographical_compare
                    (s1.begin(), s1.end(),   // source range
                     s2.begin(), s2.end(),   // dest range
                     nocase_compare ());  // comparison
        }
    };
#endif

    typedef enum eFileType
    {
        FILE_TYPE_CAM = 1,
        FILE_TYPE_MP4 = 2,
        FILE_TYPE_AVI = 3,
        FILE_TYPE_JPG = 4,
        FILE_TYPE_PNG = 5,
        FILE_TYPE_VBB = 6,
        FILE_TYPE_PGM = 7

    }eFileType;
}

#endif //AROUNDVIEW_CMNTYPEDEF_H
