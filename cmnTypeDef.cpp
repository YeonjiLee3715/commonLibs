//
// Created by 이연지 on 2017-07-19.
//

#include "cmnTypeDef.h"
#include "CStrManager.h"

namespace nsCmn
{
    ByteBuffer::ByteBuffer(const size_t& len)
        : m_data(nullptr), m_len(0)
    {
        m_data = newArray( len );
        m_len = len;
    }

    ByteBuffer::ByteBuffer(const unsigned char& data)
        : m_data(nullptr), m_len(0)
    {
        setBuffer( &data, 1 );
    }

    ByteBuffer::ByteBuffer(const ByteBuffer &buff)
        : m_data(nullptr), m_len(0)
    {
        setBuffer( buff );
    }

    ByteBuffer::ByteBuffer(const char* data, ssize_t len )
        : m_data( NULL ), m_len( 0 )
    {
        if( len == -1 )
            len = strlen( data );

        setBuffer( reinterpret_cast<const unsigned char*>( data ), len );
    }

    ByteBuffer::ByteBuffer(const unsigned char *data, ssize_t len)
        : m_data(NULL), m_len(0)
    {
        if( len == -1 )
            len = strlen( reinterpret_cast<const char*>( data ) );

        setBuffer( data, len );
    }

    ByteBuffer::~ByteBuffer()
    {
        clear();
    }

    unsigned char *ByteBuffer::newArray(size_t len)
    {
        unsigned char *temp = NULL;

        if (len <= 0)
            return NULL;

        temp = (unsigned char*)malloc( (len+1) * sizeof(unsigned char) );
        memset(temp, 0x00, (len+1) * sizeof(unsigned char));

        return temp;
    }

    void ByteBuffer::setBuffer(size_t len)
    {
        if (len <= 0)
            return;

        if( len == m_len )
            memset(m_data, 0x00, (len+1) * sizeof(unsigned char));
        else
        {
            if( isEmpty() )
                m_data = newArray( len );
            else
            {
                unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * ( len+1 ) );
                if( pTmp == nullptr )
                {
                    clear();
                    return;
                }

                m_data = pTmp;
                memset( m_data, 0x00, (len+1) * sizeof( unsigned char ) );
            }

            m_len = len;
        }
    }

    void ByteBuffer::setBuffer(const ByteBuffer &buff)
    {
        setBuffer( buff.data(), buff.len() );
    }

    void ByteBuffer::setBuffer( const char * data, size_t len )
    {
        setBuffer( reinterpret_cast<const unsigned char*>( data ), len );
    }

    void ByteBuffer::setBuffer(const unsigned char* data, size_t len)
    {
        if( len == m_len )
        {
            if (data == NULL || len <= 0)
                return;

            memmove(m_data, data, len);
        }
        else
        {
            if (data == nullptr || len <= 0)
                return;

            if( isEmpty() )
                m_data = newArray( len );
            else
                resize( len );

            memmove(m_data, data, sizeof(unsigned char) * len );
            m_len = len;
        }
    }

    void ByteBuffer::push_front( const unsigned char& data )
    {
        push_front( &data, 1 );
    }

    void ByteBuffer::push_front( const ByteBuffer& buff )
    {
        push_front( buff.data(), buff.len() );
    }

    void ByteBuffer::push_front( const char* data, size_t len )
    {
        push_front( reinterpret_cast<const unsigned char*>( data ), len );
    }

    void ByteBuffer::push_front( const unsigned char* data, size_t len )
    {
        if( data == nullptr || len <= 0 )
            return;

        if( isEmpty() )
            m_data = newArray( len );
        else
        {
            unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * ( m_len + len + 1 ) );

            if( pTmp == nullptr )
            {
                clear();
                return;
            }

            m_data = pTmp;
            memset( m_data + m_len, 0x00, ( len + 1 ) * sizeof( unsigned char ) );

            memmove( ( m_data + len ), m_data, sizeof( unsigned char ) * m_len );
        }

        unsigned char *pos = m_data;
        copyAndMovePos( pos, data, len, detail::MOVE_NONE );

        m_len += len;
    }

    void ByteBuffer::push_back(const unsigned char& data )
    {
        push_back( &data, 1 );
    }

    void ByteBuffer::push_back(const ByteBuffer &buff)
    {
        push_back( buff.data(), buff.len() );
    }

    void ByteBuffer::push_back( const char* data, size_t len )
    {
        push_back( reinterpret_cast<const unsigned char*>( data ), len );
    }

    void ByteBuffer::push_back(const unsigned char* data, size_t len)
    {
        if (data == nullptr || len <= 0)
            return;

        if( isEmpty() )
            m_data = newArray( len );
        else
        {
            unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * ( m_len + len + 1 ) );

            if( pTmp == nullptr )
            {
                clear();
                return;
            }

            m_data = pTmp;
            memset( m_data + m_len, 0x00, (len+1) * sizeof( unsigned char ) );
        }

        unsigned char *pos = m_data + m_len;
        copyAndMovePos( pos, data, len, detail::MOVE_NONE );

        m_len += len;
    }

    ByteBuffer ByteBuffer::substr(size_t posStart, ssize_t len) const
    {
        ByteBuffer sub;

        if (posStart < 0 || posStart >= m_len || len == 0 || len < -1)
            return sub;

        if (len == -1 || (posStart + len) > m_len)
            sub.setBuffer(&m_data[posStart], ((ssize_t)m_len)-posStart);
        else
            sub.setBuffer(&m_data[posStart], (const size_t)len);

        return sub;
    }

    void ByteBuffer::resize(size_t len)
    {
        if( m_len == len)
            return;

        if( len <= 0 )
        {
            clear();
            return;
        }

        if( isEmpty() )
            m_data = newArray( len );
        else
        {
            unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * ( len+1 ) );

            if( pTmp == nullptr )
            {
                clear();
                return;
            }

            m_data = pTmp;
            if( m_len < len )
                memset( m_data + m_len, 0x00, sizeof( unsigned char ) * ( ( len - m_len ) + 1 ) );
            else
                m_data[len] = 0x00;
        }

        m_len = len;
    }

    void ByteBuffer::removeRef( const unsigned char keyword, bool case_sensitive )
    {
        remove( &keyword, 1, case_sensitive );
    }

    void ByteBuffer::remove( const unsigned char* keyword, const size_t& lenKeyword, bool case_sensitive )
    {
        if( isEmpty() )
            return;

        const unsigned char* pPreSrc = m_data;
        const unsigned char* pSrcEnd = m_data+m_len;
        unsigned char* pDst = m_data;

        const unsigned char* pSrc = detail::findFirstOf( pPreSrc, m_len, keyword, lenKeyword, 0, case_sensitive );

        do
        {
            if( pSrc == nullptr )
                break;

            copyAndMovePos( pDst, pPreSrc, pSrc-pPreSrc );

            pSrc += lenKeyword;
            pPreSrc = pSrc;
            pSrc = detail::findFirstOf( pSrc, pSrcEnd-pSrc, keyword, lenKeyword, 0, case_sensitive );

        } while( pSrc < pSrcEnd && pDst < pSrcEnd );

        //키워드가 하나도 없었음
        if( pPreSrc == m_data )
            return;

        //키워드가 아닌 글자로 끝남
        if( pPreSrc < pSrcEnd )
            copyAndMovePos( pDst, pPreSrc, pSrcEnd-pPreSrc );

        ssize_t lenTmp = pDst - m_data;
        unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * ( lenTmp +1) );

        if( pTmp == nullptr )
        {
            clear();
            return;
        }

        m_data = pTmp;
        m_data[lenTmp] = 0x00;
        m_len = lenTmp;
    }

    void ByteBuffer::removeAt( size_t pos, size_t len )
    {
        if( isEmpty() )
            return;

        if( pos >= m_len || pos < 0 )
            return;

        if( pos+len > m_len )
            len = m_len-pos;

        if( pos == 0 && len == m_len )
        {
            clear();
            return;
        }

        ssize_t lenTemp = m_len - len;
        unsigned char *pCur = m_data;
        const unsigned char *pData = m_data;

        copyAndMovePos( pCur, pData, pos, detail::MOVE_BOTH );
        pData += len;
        copyAndMovePos( pCur, pData, lenTemp-pos, detail::MOVE_NONE );


        if( lenTemp <= 0 )
        {
            clear();
            return;
        }

        unsigned char* pTmp = (unsigned char *)realloc( m_data, sizeof( unsigned char ) * (lenTemp+1) );

        if( pTmp == nullptr )
        {
            clear();
            return;
        }

        m_data = pTmp;
        m_data[lenTemp] = 0x00;
        m_len = lenTemp;
    }

    void ByteBuffer::replaceRef( const unsigned char* src, const size_t& lenSrc, const size_t& pos, ssize_t len )
    {
        if( len == 0 )
            len = lenSrc;

        if( m_data == nullptr || src == nullptr || pos < 0 || pos+len > m_len || len == 0 || lenSrc <= 0 )
            return;

        if( pos + lenSrc == pos + len )
        {
            memmove( &m_data[pos], src, lenSrc );
        }
        else if( pos + lenSrc < pos + len )
        {
            removeAt( pos + lenSrc, len - lenSrc );
            memmove( &m_data[pos], src, lenSrc );
        }
        else
        {
            ssize_t lenOrz = m_len;

            resize( m_len + (lenSrc-len) );

            memmove( ( m_data + ( pos + lenSrc ) ), ( m_data + ( pos + len )), sizeof( unsigned char ) * ( lenOrz - ( pos + len ) ) );
            memmove( ( m_data + ( pos ) ), src, sizeof( unsigned char ) * lenSrc );
        }
    }

    void ByteBuffer::replace( const ByteBuffer& src, const size_t& pos, ssize_t len )
    {
        replaceRef( src.data(), src.len(), pos, len );
    }

    void ByteBuffer::clear()
    {
        if( m_data != nullptr )
            free( m_data );

        m_data = nullptr;
        m_len = 0;
    }

    std::vector<ByteBuffer> ByteBuffer::splitWithSpecialsymbol( const nsCmn::ByteBuffer& symbol, bool skipEmptypart, bool case_sensitive ) const
    {
        std::vector<nsCmn::ByteBuffer> vecRet;

        if( m_data == nullptr || symbol.isEmpty() )
            return  vecRet;

        nsCmn::ByteBuffer convertedSymbol = symbol;
        size_t lenConverted = convertedSymbol.len();
        nsCmn::convertSpecialsymbol( convertedSymbol.data(), lenConverted );
        convertedSymbol.resize( lenConverted );

        nsCmn::ByteBuffer convertedValue( m_data, m_len );
        lenConverted = convertedValue.len();
        nsCmn::convertSpecialsymbol( convertedValue.data(), lenConverted );
        convertedValue.resize( lenConverted );

        ssize_t pos = 0, prePos = 0;

        do
        {
            pos = nsCmn::detail::findFirstIndexOf( convertedValue.data(), convertedValue.len(), convertedSymbol.data(), convertedSymbol.len(), pos, case_sensitive );
            nsCmn::ByteBuffer subStr;

            if(pos == -1 )
            {
                if( prePos < convertedValue.len() )
                    subStr = convertedValue.substr(prePos, (convertedValue.len()-prePos));
            }
            else
            {
                subStr = convertedValue.substr( prePos, ( pos - prePos ) );//skip symbol
                pos += convertedSymbol.len(); //skip 'symbol'
            }

            if( subStr.isEmpty() )
            {
                if( !skipEmptypart  )
                    vecRet.push_back( subStr );
            }
            else
                vecRet.push_back( subStr );

            prePos = pos;
        }while( pos != -1 );

        return vecRet;
    }

    std::vector<ByteBuffer> ByteBuffer::split( const nsCmn::ByteBuffer& symbol, bool skipEmptypart, bool case_sensitive ) const
    {
        std::vector<nsCmn::ByteBuffer> vecRet;

        ssize_t pos = 0, prePos = 0;

        do
        {
            pos = nsCmn::detail::findFirstIndexOf( m_data, m_len, symbol.data(), symbol.len(), pos, case_sensitive );
            nsCmn::ByteBuffer subStr;
            if(pos == -1 )
            {
                if( prePos < m_len )
                    subStr = substr(prePos, (m_len-prePos));
            }
            else
            {
                subStr = substr( prePos, ( pos - prePos ) );//skip symbol
                pos += symbol.len(); //skip 'symbol'
            }

            if( subStr.isEmpty() )
            {
                if( !skipEmptypart  )
                    vecRet.push_back( subStr );
            }
            else
                vecRet.push_back( subStr );

            prePos = pos;
        }while( pos != -1 );

        return vecRet;
    }

    std::vector<ByteBuffer> ByteBuffer::split( const std::vector< nsCmn::ByteBuffer >& vecSymbols, bool skipEmptypart, bool case_sensitive ) const
    {
        std::vector<nsCmn::ByteBuffer> vecRet;

        if( m_data == nullptr || m_len <= 0 )
        {
            if( skipEmptypart == false )
                vecRet.push_back( nsCmn::ByteBuffer() );

            return vecRet;
        }

        const unsigned char* pCur = m_data;
        const unsigned char* pPreMatch = m_data;
        const unsigned char* pEnd = m_data + m_len;

        if( vecSymbols.empty() )
        {
            vecRet.push_back( nsCmn::ByteBuffer( m_data, m_len ) );
            return vecRet;
        }

        //sort
        std::vector< nsCmn::ByteBuffer > vecSortedSymbols = vecSymbols;
        std::sort( vecSortedSymbols.begin(), vecSortedSymbols.end(), [](const nsCmn::ByteBuffer& arg1, const nsCmn::ByteBuffer& arg2)
        {
            size_t loopMax = arg1.len() < arg2.len() ? arg1.len() : arg2.len();
            const unsigned char* p1 = arg1.data();
            const unsigned char* p2 = arg2.data();

            for( size_t idx = 0; idx < loopMax; ++idx )
            {
                if( p1 == nullptr || p2 == nullptr )
                    return p1 > p2;

                if( tolower(*p1) != tolower(*p2) )
                    return *p1 > *p2;

                ++p1;
                ++p2;
            }

            return arg1.len() > arg2.len();
        });

        while( pCur != nullptr && pCur < pEnd )
        {
            bool isMatch = false;

            for(  std::vector< nsCmn::ByteBuffer >::const_iterator it = vecSortedSymbols.cbegin()
                    ; it != vecSortedSymbols.cend(); ++it )
            {
                if( pCur + it->len() >= pEnd )
                    continue;

                if( nsCmn::compare( it->data(), it->len(), pCur, it->len(), case_sensitive ) )
                {
                    isMatch = true;
                    if( pCur == pPreMatch )
                    {
                        if( skipEmptypart == false )
                            vecRet.push_back( nsCmn::ByteBuffer() );
                    }
                    else
                        vecRet.push_back( nsCmn::ByteBuffer( pPreMatch, pCur-pPreMatch ) );

                    pCur += it->len();
                    pPreMatch = pCur;
                    isMatch = true;
                    break;
                }
            }

            if( isMatch == false )
                ++pCur;
        }

        if( pCur != nullptr && pPreMatch != pCur )
            vecRet.push_back( nsCmn::ByteBuffer( pPreMatch, pCur-pPreMatch ) );

        return vecRet;
    }

    ByteBuffer * ByteBuffer::operator()( const char* data, size_t len )
    {
        this->setBuffer( data, len );
        return this;
    }

    ByteBuffer *ByteBuffer::operator()(const unsigned char *data, size_t len)
    {
        this->setBuffer(data, len);
        return this;
    }

    ByteBuffer *ByteBuffer::operator()(const ByteBuffer &buff)
    {
        this->setBuffer(buff.data(), buff.len());
        return this;
    }

    ByteBuffer *ByteBuffer::operator=(const ByteBuffer &buff)
    {
        this->setBuffer(buff.data(), buff.len());
        return this;
    }

    ByteBuffer *ByteBuffer::operator+(const ByteBuffer &buff)
    {
        this->push_back(buff.data(), buff.len());
        return this;
    }
}