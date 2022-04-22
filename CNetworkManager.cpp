#include "CNetworkManager.h"

#include <netinet/in.h>
#include <map>
#include <mutex>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <CNetworkConfig.h>

#include <CLogger.h>

namespace nsCNetworkManager{
    const char* TAG = "CNetworkManager";
}

using namespace nsCNetworkManager;

#define BUFFER_BLOCK_SIZE       4096

typedef std::map< int, int >                    tyMapMsgIdToSocket;
typedef std::map< int, struct sockaddr_in* >    tyMapSockToClient;
typedef std::map< int, SSL* >                   tyMapSockToSSL;

class CNetworkManager::CNetworkManagerImpl
{
public:
    CNetworkManagerImpl();
    ~CNetworkManagerImpl();

    int createSocket( int messageId, const char* strHost , uint16_t nPort );
    int createSSLSocket( int messageId, const char* strHost , uint16_t nPort );

    void disconnectSocket(int sock );
    void disconnetAll();

    bool sendMessage( int messageId, const char* strHost, uint16_t nPort, const char* msg , int len, int timeout );
    bool sendMessage( int nSocket, const char* msg, int len, int timeout );
    bool sendMessage( int nSocket, const std::string& msg, int timeout );

    bool sendSSLMessage( int messageId, const char* strHost, uint16_t nPort, const char* msg , int len, int timeout );
    bool sendSSLMessage( int nSocket, const char* msg, int len, int timeout );
    bool sendSSLMessage( int nSocket, const std::string& msg, int timeout );

    bool receiveMessage( int messageId, const char* strHost , uint16_t nPort, char* msg, int& len, int timeout );
    bool receiveMessage( int nSocket, char* msg, int &len, int timeout );
    bool receiveMessage( int nSocket, std::stringstream& msg, int timeout );

    bool receiveSSLMessage( int messageId, const char* strHost , uint16_t nPort, char* msg, int& len, int timeout );
    bool receiveSSLMessage( int nSocket, char* msg, int &len, int timeout );
    bool receiveSSLMessage( int nSocket, std::stringstream& msg, int timeout );

private:    
    int findSocketByConnectionInfo( const char* strHost , uint16_t nPort );
    int findSocketByMsgId( int messageId );
    SSL* findSSLBySocket( int nSocket );

    void setTimeout( int nSocket, int timeout );

private:
    tyMapMsgIdToSocket  m_mapMsgIdToSocket;
    tyMapSockToClient   m_mapSocketToClient;
    tyMapSockToSSL      m_mapSocketToSSL;

    std::mutex          m_lckSocket;
};

CNetworkManager::CNetworkManager()
    : m_pImpl( nullptr )
{
    m_pImpl = std::make_unique<CNetworkManager::CNetworkManagerImpl>();
}

CNetworkManager::~CNetworkManager()
{
    if( m_pImpl != nullptr )
    {
        m_pImpl.reset();
        m_pImpl = nullptr;
    }
}

int CNetworkManager::createSocket( int messageId, const char* strHost, unsigned short int nPort )
{
    if( m_pImpl != nullptr )
        return m_pImpl->createSocket( messageId, strHost, nPort );
    
    return -1;
}

int CNetworkManager::createSSLSocket(int messageId, const char *strHost, unsigned short nPort)
{
    if( m_pImpl != nullptr )
        return m_pImpl->createSSLSocket( messageId, strHost, nPort );

    return -1;
}

void CNetworkManager::disconnectSocket( int nSocket )
{
    if( m_pImpl != nullptr )
        m_pImpl->disconnectSocket( nSocket );
}

void CNetworkManager::disconnetAll()
{
    if( m_pImpl != nullptr )
        m_pImpl->disconnetAll();
}

bool CNetworkManager::sendMessage(int nSocket, const std::string& msg, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendMessage(nSocket, msg, timeout);
    
    return false;
}

bool CNetworkManager::sendMessage(int nSocket, const char* msg, int len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendMessage(nSocket, msg, len, timeout);
    
    return false;
}

bool CNetworkManager::sendMessage(int messageId, const char* strHost, uint16_t nPort, const char* msg, int len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendMessage(messageId, strHost, nPort, msg, len, timeout);
    
    return false;
}

bool CNetworkManager::sendSSLMessage(int nSocket, const std::string& msg, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendSSLMessage(nSocket, msg, timeout);

    return false;
}

bool CNetworkManager::sendSSLMessage(int nSocket, const char* msg, int len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendSSLMessage(nSocket, msg, len, timeout);

    return false;
}

bool CNetworkManager::sendSSLMessage(int messageId, const char* strHost, uint16_t nPort, const char* msg, int len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->sendSSLMessage(messageId, strHost, nPort, msg, len, timeout);

    return false;
}

bool CNetworkManager::receiveMessage(int nSocket, std::stringstream& msg, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveMessage(nSocket, msg, timeout);
    
    return false;
}

bool CNetworkManager::receiveMessage(int nSocket, char* msg, int& len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveMessage(nSocket, msg, len, timeout);
    
    return false;
}

bool CNetworkManager::receiveMessage(int messageId, const char* strHost, uint16_t nPort, char* msg, int& len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveMessage(messageId, strHost, nPort, msg, len, timeout);
    
    return false;
}

bool CNetworkManager::receiveSSLMessage(int nSocket, std::stringstream& msg, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveSSLMessage(nSocket, msg, timeout);

    return false;
}

bool CNetworkManager::receiveSSLMessage(int nSocket, char* msg, int& len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveSSLMessage(nSocket, msg, len, timeout);

    return false;
}

bool CNetworkManager::receiveSSLMessage(int messageId, const char* strHost, uint16_t nPort, char* msg, int& len, int timeout)
{
    if( m_pImpl != nullptr )
        return m_pImpl->receiveSSLMessage(messageId, strHost, nPort, msg, len, timeout);

    return false;
}

// CNetworkManager::CNetworkManagerImpl

CNetworkManager::CNetworkManagerImpl::CNetworkManagerImpl()
{

}

CNetworkManager::CNetworkManagerImpl::~CNetworkManagerImpl()
{
    disconnetAll();
}

int CNetworkManager::CNetworkManagerImpl::createSocket( int messageId, const char* strHost, uint16_t nPort )
{
    bool isSuccess = false;

    struct hostent* host = nullptr;
    struct sockaddr_in* client = nullptr;

    int nSocket = -1;

    do
    {
        host = gethostbyname( strHost );

        if( host == nullptr || host->h_addr == nullptr )
        {
            LOGE( TAG, "Error retrieving DNS information" );
            break;
        }

        client = new struct sockaddr_in();
        memset(client, 0, sizeof(struct sockaddr_in));

        client->sin_family = AF_INET;
        client->sin_port = htons( nPort );
        memcpy(&(client->sin_addr.s_addr), host->h_addr, host->h_length );

        nSocket = socket( AF_INET, SOCK_STREAM, 0 );
        if( nSocket < 0 )
        {
            LOGE( TAG, "Error creating socket." );
            break;
        }

        int nRet = connect( nSocket, (struct sockaddr*)client, sizeof( struct sockaddr_in ) );
        if( nRet < 0 )
        {
            LOGE( TAG, "Could not connect to host: %s, port: %d" );
            break;
        }

        m_lckSocket.lock();
        m_mapMsgIdToSocket[messageId] = nSocket;
        m_mapSocketToClient[nSocket] = client;
        m_lckSocket.unlock();

        isSuccess = true;

    }while( false );

    // clear
    if( isSuccess == false )
    {
        if( client != nullptr )
        {
            delete client;
            client = nullptr;
        }

        if( nSocket >= 0 )
            disconnectSocket( nSocket );
    }

    return nSocket;
}

int CNetworkManager::CNetworkManagerImpl::createSSLSocket(int messageId, const char *strHost, uint16_t nPort)
{
    bool isSuccess = false;

    struct hostent* host = nullptr;
    struct sockaddr_in* client = nullptr;

    int nSocket = -1;

    do
    {
        host = gethostbyname( strHost );

        if( host == nullptr || host->h_addr == nullptr )
        {
            LOGE( TAG, "Error retrieving DNS information" );
            break;
        }

        client = new struct sockaddr_in();
        memset(client, 0, sizeof(struct sockaddr_in));

        client->sin_family = AF_INET;
        client->sin_port = htons( nPort );
        memcpy(&(client->sin_addr.s_addr), host->h_addr, host->h_length );

        nSocket = socket( AF_INET, SOCK_STREAM, 0 );
        if( nSocket < 0 )
        {
            LOGE( TAG, "Error creating socket." );
            break;
        }

        int nRet = connect( nSocket, (struct sockaddr*)client, sizeof( struct sockaddr_in ) );
        if( nRet < 0 )
        {
            LOGE( TAG, "Could not connect to host: %s, port: %d" );
            break;
        }

        SSL* ssl;

        SSL_library_init();
        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        const SSL_METHOD *meth = TLSv1_2_client_method();
        SSL_CTX* ctx = SSL_CTX_new (meth);
        ssl = SSL_new( ctx );

        if( ssl == nullptr )
        {
            LOGE( TAG, "Failed to create SSL." );
            break;
        }

        SSL_set_fd( ssl, nSocket );

        int err = SSL_connect(ssl);
        if( err <= 0 )
        {
            LOGE( TAG, "Failed to create SSL connection.  err=%x\n", err );
            fflush(stdout);
            break;
        }

        m_lckSocket.lock();
        m_mapMsgIdToSocket[messageId] = nSocket;
        m_mapSocketToClient[nSocket] = client;
        m_mapSocketToSSL[nSocket] = ssl;
        m_lckSocket.unlock();

        isSuccess = true;

    }while( false );

    // clear
    if( isSuccess == false )
    {
        if( client != nullptr )
        {
            delete client;
            client = nullptr;
        }

        if( nSocket >= 0 )
            disconnectSocket( nSocket );
    }

    return nSocket;
}

int CNetworkManager::CNetworkManagerImpl::findSocketByConnectionInfo( const char* strHost , uint16_t nPort )
{
    struct hostent* host = nullptr;

    int nSocket = -1;

    do
    {
        host = gethostbyname( strHost );

        if( host == nullptr )
        {
            LOGE( TAG, "Error retrieving DNS information" );
            break;
        }

        struct in_addr addr;
        memcpy( &addr, host->h_addr_list[0], sizeof(struct in_addr) );

        m_lckSocket.lock();
        if( m_mapSocketToClient.empty() == false )
        {
            tyMapSockToClient::iterator it = std::find_if( m_mapSocketToClient.begin(), m_mapSocketToClient.end()
                                                        , [&]( std::pair< int, struct sockaddr_in* > arg )
            {
                if( arg.first < 0 || arg.second == nullptr )
                    return false;

                if( arg.second->sin_addr.s_addr == addr.s_addr
                        && arg.second->sin_port == htons( nPort ) )
                    return true;

                return false;
            });

            if( it != m_mapSocketToClient.end() )
                nSocket = it->first;
        }
        m_lckSocket.unlock();

    }while( false );

    return nSocket;
}

int CNetworkManager::CNetworkManagerImpl::findSocketByMsgId( int messageId )
{
    int nSocket = -1;

    m_lckSocket.lock();
    if( m_mapMsgIdToSocket.empty() == false )
    {
        tyMapMsgIdToSocket::iterator it = m_mapMsgIdToSocket.find( messageId );

        if( it != m_mapMsgIdToSocket.end() )
            nSocket = it->second;
    }
    m_lckSocket.unlock();
    
    return nSocket;
}

SSL* CNetworkManager::CNetworkManagerImpl::findSSLBySocket(int nSocket)
{
    SSL* ssl = nullptr;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return ssl;
    }

    m_lckSocket.lock();
    if( m_mapSocketToSSL.empty() == false )
    {
        tyMapSockToSSL::iterator it = m_mapSocketToSSL.find( nSocket );

        if( it != m_mapSocketToSSL.end() )
            ssl = it->second;
    }
    m_lckSocket.unlock();

    return ssl;
}

void CNetworkManager::CNetworkManagerImpl::disconnectSocket( int nSocket )
{
    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return;
    }

    close( nSocket );

    m_lckSocket.lock();
    if( m_mapSocketToClient.empty() == false )
    {
        tyMapSockToClient::iterator it = m_mapSocketToClient.find( nSocket );
        if( it != m_mapSocketToClient.end() )
        {
            struct sockaddr_in* client = it->second;

            m_mapSocketToClient.erase( it );

            if( client != nullptr )
            {
                delete client;
                client = nullptr;
            }
        }
    }

    if( m_mapSocketToSSL.empty() == false )
    {
        tyMapSockToSSL::iterator it = m_mapSocketToSSL.find( nSocket );
        if( it != m_mapSocketToSSL.end() )
        {
            SSL* ssl = it->second;

            m_mapSocketToSSL.erase( it );

            if( ssl != nullptr )
            {
                SSL_shutdown(ssl);
                SSL_free(ssl);
                ssl = nullptr;
            }
        }
    }
    
    if( m_mapMsgIdToSocket.empty() == false )
    {
        tyMapMsgIdToSocket::iterator it = std::find_if( m_mapMsgIdToSocket.begin(), m_mapMsgIdToSocket.end()
                                                        , [&]( std::pair< int, int > arg )
        {            
            if( arg.second == nSocket )
                return true;
            
            return false;
            
        } );
        
        if( it != m_mapMsgIdToSocket.end() )
            m_mapMsgIdToSocket.erase( it );
    }
    m_lckSocket.unlock();
}

void CNetworkManager::CNetworkManagerImpl::disconnetAll()
{
    //for safety delete
    tyMapSockToClient mapRmSockets;
    tyMapSockToSSL mapRmSocketsSSL;

    m_lckSocket.lock();
    
    mapRmSockets.swap( m_mapSocketToClient );
    mapRmSocketsSSL.swap( m_mapSocketToSSL );

    m_mapSocketToClient.clear();
    m_mapSocketToSSL.clear();
    m_mapMsgIdToSocket.clear();
    
    m_lckSocket.unlock();

    for( tyMapSockToSSL::iterator it = mapRmSocketsSSL.begin()
         ; it != mapRmSocketsSSL.end(); ++it )
    {
        if( it->second == nullptr )
            continue;

        SSL_shutdown(it->second);
        SSL_free(it->second);
        it->second = nullptr;
    }

    mapRmSocketsSSL.clear();

    for( tyMapSockToClient::iterator it = mapRmSockets.begin()
         ; it != mapRmSockets.end(); ++it )
    {
        if( it->first >= 0 )
            close( it->first );

        if( it->second != nullptr )
        {
            delete it->second;
            it->second = nullptr;
        }
    }

    mapRmSockets.clear();
}

bool CNetworkManager::CNetworkManagerImpl::sendMessage( int messageId, const char* strHost , uint16_t nPort, const char* msg, int len, int timeout )
{
    int nSocket = createSocket( messageId, strHost, nPort );
    
    return sendMessage( nSocket, msg, len, timeout );
}

bool CNetworkManager::CNetworkManagerImpl::sendMessage( int nSocket, const char* msg, int len, int timeout )
{
    bool isSuccess = false;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    setTimeout( nSocket, timeout );
    
    int nSize = send( nSocket, msg, len, 0 );
    
    if( nSize == len )
        isSuccess = true;
    
    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::sendMessage(int nSocket, const std::string& msg, int timeout )
{
    bool isSuccess = false;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    setTimeout( nSocket, timeout );
    
    int nSize = static_cast<int>( send( nSocket, msg.c_str(), msg.size(), 0 ) );
    
    if( nSize == static_cast<int>( msg.size() ) )
        isSuccess = true;
    
    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::sendSSLMessage(int messageId, const char* strHost, uint16_t nPort, const char* msg, int len, int timeout)
{
    int nSocket = createSocket( messageId, strHost, nPort );

    return sendSSLMessage( nSocket, msg, len, timeout );
}

bool CNetworkManager::CNetworkManagerImpl::sendSSLMessage(int nSocket, const char* msg, int len, int timeout)
{
    bool isSuccess = false;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    SSL* ssl = findSSLBySocket( nSocket );

    if( ssl == nullptr )
    {
        LOGE( TAG, "Wrong ssl socket" );
        return isSuccess;
    }

    SSL_CTX* ctx = SSL_get_SSL_CTX( ssl );
    SSL_CTX_set_timeout( ctx, timeout );

    int nSize = SSL_write( ssl, msg, len );

    if( nSize < 0 )
    {
        int sslErrCode = SSL_get_error(ssl, nSize);
        LOGE( TAG, "SSL Socket read error: %d", sslErrCode );
    }
    else if( nSize == len )
    {
        isSuccess = true;
    }

    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::sendSSLMessage(int nSocket, const std::string &msg, int timeout)
{
    bool isSuccess = false;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    SSL* ssl = findSSLBySocket( nSocket );

    if( ssl == nullptr )
    {
        LOGE( TAG, "Wrong ssl socket" );
        return isSuccess;
    }

    SSL_CTX* ctx = SSL_get_SSL_CTX( ssl );
    SSL_CTX_set_timeout( ctx, timeout );

    int nSize = SSL_write( ssl, msg.c_str(), msg.size() );

    if( nSize < 0 )
    {
        int sslErrCode = SSL_get_error(ssl, nSize);
        LOGE( TAG, "SSL Socket read error: %d", sslErrCode );
    }
    else if( nSize == static_cast<int>( msg.size() ) )
    {
        isSuccess = true;
    }

    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::receiveMessage(int messageId, const char* strHost, uint16_t nPort, char* msg, int& len, int timeout)
{
    int nSocket = createSocket( messageId, strHost, nPort );
    
    return receiveMessage( nSocket, msg, len, timeout );
}

bool CNetworkManager::CNetworkManagerImpl::receiveMessage(int nSocket, char* msg, int& len, int timeout)
{
    bool isSuccess = false;

    int nMaxBufferSize = len;

    memset( msg, '\0', (sizeof(char)*nMaxBufferSize) );
    len = 0;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    setTimeout( nSocket, timeout );

    char buff[BUFFER_BLOCK_SIZE];
    int nRecvSize;
    bool isChunked = false;
    int nCurBuffSize = 0;

    int nChunkLenth = 0;
    int nHeader = 0;
    int nBody = 0;
    int nContentLenth = 0;
    std::stringstream tempChunk;
    int nTotalRecvSize = 0;
    bool isHeader = false;
    bool isEnd = false;

    char* pCur = msg;

    isSuccess = true;

    do
    {
        nCurBuffSize = 0;
        nRecvSize = 0;
        memset( buff, '\0', (sizeof(char)*BUFFER_BLOCK_SIZE) );

        do
        {
            nRecvSize = ::read( nSocket, buff+nCurBuffSize, BUFFER_BLOCK_SIZE-nCurBuffSize );
            if( nRecvSize <= 0 )
            {
                if( nRecvSize < 0 )
                {
                    LOGE( TAG, "Socket read error" );
                    isSuccess = false;
                }

                isEnd = true;
                break;
            }

            nCurBuffSize += nRecvSize;

        }while( false );

        if( isEnd )
            break;

        const char* pHeaderEnd = nullptr;

        if( isHeader == false )
        {
            pHeaderEnd = strstr( buff, "\r\n\r\n" );
            if( pHeaderEnd != nullptr )
            {
                pHeaderEnd += 4; //skip \r\n\r\n

                int nWriteSize = pHeaderEnd-buff;
                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                {
                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                    LOGE( TAG, "Buffer MAX" );
                    isSuccess = false;
                }

                pCur = (char*)memcpy( pCur, buff, nWriteSize );
                if( pCur == nullptr )
                {
                    LOGE( TAG, "Buffer pointer is null. " );
                    isSuccess = false;
                    break;
                }

                nTotalRecvSize += nWriteSize;
                isHeader = true;
            }
            else
            {
                int nWriteSize = nCurBuffSize;
                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                {
                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                    LOGE( TAG, "Buffer MAX" );
                    isSuccess = false;
                }

                pCur = (char*)memcpy( pCur, buff, nWriteSize );
                if( pCur == nullptr )
                {
                    LOGE( TAG, "Buffer pointer is null. " );
                    isSuccess = false;
                    break;
                }

                nTotalRecvSize += nWriteSize;
                continue;
            }

            if( isHeader )
            {
                const char* pChunked = strstr( msg, "Transfer-Encoding: chunked" );
                if( pChunked != nullptr )
                    isChunked = true;

                const char* pContentsLength = strstr( msg, NET_HEADER_CONTENT_LENGTH );
                if( pContentsLength != nullptr )
                {
                    pContentsLength += strlen( NET_HEADER_CONTENT_LENGTH );
                    const char* pLineEnd = strstr( pContentsLength, NET_API_ENDL );
                    if( pLineEnd != nullptr )
                        nContentLenth = atoi( std::string( pContentsLength, pLineEnd-pContentsLength ).c_str() );
                }

                nHeader = static_cast<int>( nTotalRecvSize );
            }
        }

        if( pHeaderEnd == nullptr )
            pHeaderEnd = buff;

        const char* pChunckPre = pHeaderEnd;
        const char* pChunckCur = pHeaderEnd;
        const char* pChunckEnd = buff + nCurBuffSize;
        const char* pLineEnd = nullptr;

        do
        {
            pLineEnd = strstr( pChunckCur, "\r\n" );

            if( pLineEnd != nullptr )
            {
                pChunckCur = pLineEnd;

                if( isChunked )
                {
                    if( nChunkLenth <= 0 )
                    {
                        {
                            std::stringstream ssChunkLenth;
                            ssChunkLenth << std::hex << std::string( pChunckPre, pChunckCur-pChunckPre ).c_str();
                            ssChunkLenth >> nChunkLenth;
                        }

                        pChunckCur +=2; //skip "\r\n"
                        pChunckPre = pChunckCur;

                        if( nChunkLenth == 0 )
                        {
                            if( tempChunk.str().empty() == false )
                            {
                                int nWriteSize = static_cast<int>( tempChunk.str().size() );
                                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                                {
                                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                                    LOGE( TAG, "Buffer MAX" );
                                    isSuccess = false;
                                }

                                pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                                if( pCur == nullptr )
                                {
                                    LOGE( TAG, "Buffer pointer is null. " );
                                    isSuccess = false;
                                    break;
                                }

                                nTotalRecvSize += nWriteSize;
                                nBody += nWriteSize;
                                tempChunk.str("");
                            }

                            isEnd = true;
                            break;
                        }
                    }
                    else
                    {
                        int nChunkSize = (pChunckCur-pChunckPre);

                        if( ( nChunkLenth-( static_cast<int>(tempChunk.str().size()) + nChunkSize ) ) > 0 )
                        {
                            pChunckCur +=2; //skip "\r\n"
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );
                        }
                        else
                        {
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );

                            int nWriteSize = static_cast<int>( tempChunk.str().size() );
                            if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                            {
                                nWriteSize = nMaxBufferSize - nTotalRecvSize;
                                LOGE( TAG, "Buffer MAX" );
                                isSuccess = false;
                                isEnd = true;
                            }

                            pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                            if( pCur == nullptr )
                            {
                                LOGE( TAG, "Buffer pointer is null. " );
                                isSuccess = false;
                                isEnd = true;
                                break;
                            }

                            nTotalRecvSize += nWriteSize;
                            nBody += nWriteSize;
                            tempChunk.str("");
                            nChunkLenth = 0;

                            pChunckCur +=2; //skip "\r\n"
                        }

                        pChunckPre = pChunckCur;
                    }
                }
            }
            else
            {
                pChunckCur = pChunckEnd;

                int nChunkSize = (pChunckCur-pChunckPre);
                if( nChunkSize > 0 )
                {
                    tempChunk.write( pChunckPre, nChunkSize );
                    pChunckPre = pChunckCur;
                }

                if( isChunked == false )
                {
                    int nWriteSize = static_cast<int>( tempChunk.str().size() );
                    if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                    {
                        nWriteSize = nMaxBufferSize - nTotalRecvSize;
                        LOGE( TAG, "Buffer MAX" );
                        isSuccess = false;
                        isEnd = true;
                    }

                    pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                    if( pCur == nullptr )
                    {
                        LOGE( TAG, "Buffer pointer is null. " );
                        isSuccess = false;
                        isEnd = true;
                        break;
                    }

                    nTotalRecvSize += nWriteSize;
                    nBody += nWriteSize;
                    tempChunk.str("");
                }
            }

        }while( pLineEnd != nullptr && pChunckCur < pChunckEnd && isEnd == false && pCur != nullptr );

    }while( nCurBuffSize > 0 && ( ( nContentLenth > 0 && nContentLenth > nBody ) || (nCurBuffSize == BUFFER_BLOCK_SIZE) || isChunked ) && isEnd == false && pCur != nullptr );

    len = nTotalRecvSize;

    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::receiveMessage(int nSocket, std::stringstream& msg, int timeout)
{
    bool isSuccess = false;

    msg.clear();

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    setTimeout( nSocket, timeout );
    
    char buff[BUFFER_BLOCK_SIZE];
    int nRecvSize;
    bool isChunked = false;
    int nCurBuffSize = 0;

    int nChunkLenth = 0;
    int nHeader = 0;
    int nBody = 0;
    int nContentLenth = 0;
    std::stringstream tempChunk;
    bool isHeader = false;
    bool isEnd = false;

    isSuccess = true;

    do
    {
        nCurBuffSize = 0;
        nRecvSize = 0;
        memset( buff, '\0', (sizeof(char)*BUFFER_BLOCK_SIZE) );

        do
        {
            nRecvSize = ::read( nSocket, buff+nCurBuffSize, BUFFER_BLOCK_SIZE-nCurBuffSize );
            if( nRecvSize <= 0 )
            {
                if( nRecvSize < 0 )
                {
                    LOGE( TAG, "Socket read error" );
                    isSuccess = false;
                }

                isEnd = true;
                break;
            }

            nCurBuffSize += nRecvSize;

        }while( false );

        if( isEnd )
            break;

        const char* pHeaderEnd = nullptr;

        if( isHeader == false )
        {
            pHeaderEnd = strstr( buff, "\r\n\r\n" );
            if( pHeaderEnd != nullptr )
            {
                pHeaderEnd += 4; //skip \r\n\r\n
                msg.write( buff, pHeaderEnd-buff );
                isHeader = true;
            }
            else
            {
                msg.write( buff, nCurBuffSize );
                continue;
            }

            if( isHeader )
            {
                const char* pChunked = strstr( msg.str().c_str(), "Transfer-Encoding: chunked" );
                if( pChunked != nullptr )
                    isChunked = true;

                const char* pContentsLength = strstr( msg.str().c_str(), NET_HEADER_CONTENT_LENGTH );
                if( pContentsLength != nullptr )
                {
                    pContentsLength += strlen( NET_HEADER_CONTENT_LENGTH );
                    const char* pLineEnd = strstr( pContentsLength, NET_API_ENDL );
                    if( pLineEnd != nullptr )
                        nContentLenth = atoi( std::string( pContentsLength, pLineEnd-pContentsLength ).c_str() );
                }

                nHeader = static_cast<int>( msg.str().length() );
            }
        }

        if( pHeaderEnd == nullptr )
            pHeaderEnd = buff;

        const char* pChunckPre = pHeaderEnd;
        const char* pChunckCur = pHeaderEnd;
        const char* pChunckEnd = buff + nCurBuffSize;
        const char* pLineEnd = nullptr;

        do
        {
            pLineEnd = strstr( pChunckCur, "\r\n" );

            if( pLineEnd != nullptr )
            {
                pChunckCur = pLineEnd;

                if( isChunked )
                {
                    if( nChunkLenth <= 0 )
                    {
                        {
                            std::stringstream ssChunkLenth;
                            ssChunkLenth << std::hex << std::string( pChunckPre, pChunckCur-pChunckPre ).c_str();
                            ssChunkLenth >> nChunkLenth;
                        }

                        pChunckCur +=2; //skip "\r\n"
                        pChunckPre = pChunckCur;

                        if( nChunkLenth == 0 )
                        {
                            if( tempChunk.str().empty() == false )
                            {
                                msg << tempChunk.str();
                                nBody += tempChunk.str().size();
                                tempChunk.str("");
                            }
                            isEnd = true;
                            break;
                        }
                    }
                    else
                    {
                        int nChunkSize = (pChunckCur-pChunckPre);

                        if( ( nChunkLenth-( static_cast<int>(tempChunk.str().size()) + nChunkSize ) ) > 0 )
                        {
                            pChunckCur +=2; //skip "\r\n"
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );
                        }
                        else
                        {
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );

                            msg << tempChunk.str();
                            nBody += tempChunk.str().size();
                            tempChunk.str("");
                            nChunkLenth = 0;

                            pChunckCur +=2; //skip "\r\n"
                        }

                        pChunckPre = pChunckCur;
                    }
                }
            }
            else
            {
                pChunckCur = pChunckEnd;

                int nChunkSize = (pChunckCur-pChunckPre);
                if( nChunkSize > 0 )
                {
                    tempChunk.write( pChunckPre, nChunkSize );
                    pChunckPre = pChunckCur;
                }

                if( isChunked == false )
                {
                    msg << tempChunk.str().c_str();
                    nBody += tempChunk.str().size();
                    tempChunk.str("");
                }
            }

        }while( pLineEnd != nullptr && pChunckCur < pChunckEnd && isEnd == false );

    }while( nCurBuffSize > 0 && ( ( nContentLenth > 0 && nContentLenth > nBody ) || (nCurBuffSize == BUFFER_BLOCK_SIZE) || isChunked ) && isEnd == false );

    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::receiveSSLMessage(int messageId, const char *strHost, uint16_t nPort, char *msg, int &len, int timeout)
{
    int nSocket = createSocket( messageId, strHost, nPort );

    return receiveSSLMessage( nSocket, msg, len, timeout );
}

bool CNetworkManager::CNetworkManagerImpl::receiveSSLMessage(int nSocket, char *msg, int &len, int timeout)
{
    bool isSuccess = false;

    int nMaxBufferSize = len;

    memset( msg, '\0', (sizeof(char)*nMaxBufferSize) );
    len = 0;

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    SSL* ssl = findSSLBySocket( nSocket );

    if( ssl == nullptr )
    {
        LOGE( TAG, "Wrong ssl socket" );
        return isSuccess;
    }

    SSL_CTX* ctx = SSL_get_SSL_CTX( ssl );
    SSL_CTX_set_timeout( ctx, timeout );

    char buff[BUFFER_BLOCK_SIZE];
    int nRecvSize;
    bool isChunked = false;
    int nCurBuffSize = 0;

    int nChunkLenth = 0;
    std::stringstream tempChunk;
    int nTotalRecvSize = 0;
    bool isHeader = false;
    bool isEnd = false;

    char* pCur = msg;

    isSuccess = true;

    do
    {
        nCurBuffSize = 0;
        nRecvSize = 0;
        memset( buff, '\0', (sizeof(char)*BUFFER_BLOCK_SIZE) );

        do
        {
            nRecvSize = SSL_read( ssl, buff+nCurBuffSize, BUFFER_BLOCK_SIZE-nCurBuffSize );
            if( nRecvSize <= 0 )
            {
                if( nRecvSize < 0 )
                {
                    int sslErrCode = SSL_get_error(ssl, nRecvSize);
                    LOGE( TAG, "SSL Socket read error: %d", sslErrCode );

                    isSuccess = false;
                }

                isEnd = true;
                break;
            }

            nCurBuffSize += nRecvSize;

        }while( false );

        if( isEnd )
            break;

        const char* pHeaderEnd = nullptr;

        if( isHeader == false )
        {
            pHeaderEnd = strstr( buff, "\r\n\r\n" );
            if( pHeaderEnd != nullptr )
            {
                pHeaderEnd += 4; //skip \r\n\r\n

                const char* pChunked = strstr( buff, "Transfer-Encoding: chunked" );
                if( pChunked != nullptr )
                    isChunked = true;

                int nWriteSize = pHeaderEnd-buff;
                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                {
                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                    LOGE( TAG, "Buffer MAX" );
                    isSuccess = false;
                }

                pCur = (char*)memcpy( pCur, buff, nWriteSize );
                if( pCur == nullptr )
                {
                    LOGE( TAG, "Buffer pointer is null. " );
                    isSuccess = false;
                    break;
                }

                nTotalRecvSize += nWriteSize;
                isHeader = true;
            }
            else
            {
                int nWriteSize = nCurBuffSize;
                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                {
                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                    LOGE( TAG, "Buffer MAX" );
                    isSuccess = false;
                }

                pCur = (char*)memcpy( pCur, buff, nWriteSize );
                if( pCur == nullptr )
                {
                    LOGE( TAG, "Buffer pointer is null. " );
                    isSuccess = false;
                    break;
                }

                nTotalRecvSize += nWriteSize;
                continue;
            }
        }

        if( pHeaderEnd == nullptr )
            pHeaderEnd = buff;

        const char* pChunckPre = pHeaderEnd;
        const char* pChunckCur = pHeaderEnd;
        const char* pChunckEnd = buff + nCurBuffSize;
        const char* pLineEnd = nullptr;

        do
        {
            pLineEnd = strstr( pChunckCur, "\r\n" );

            if( pLineEnd != nullptr )
            {
                pChunckCur = pLineEnd;

                if( isChunked )
                {
                    if( nChunkLenth <= 0 )
                    {
                        {
                            std::stringstream ssChunkLenth;
                            ssChunkLenth << std::hex << std::string( pChunckPre, pChunckCur-pChunckPre ).c_str();
                            ssChunkLenth >> nChunkLenth;
                        }

                        pChunckCur +=2; //skip "\r\n"
                        pChunckPre = pChunckCur;

                        if( nChunkLenth == 0 )
                        {
                            if( tempChunk.str().empty() == false )
                            {
                                int nWriteSize = static_cast<int>( tempChunk.str().size() );
                                if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                                {
                                    nWriteSize = nMaxBufferSize - nTotalRecvSize;
                                    LOGE( TAG, "Buffer MAX" );
                                    isSuccess = false;
                                }

                                pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                                if( pCur == nullptr )
                                {
                                    LOGE( TAG, "Buffer pointer is null. " );
                                    isSuccess = false;
                                    break;
                                }

                                nTotalRecvSize += nWriteSize;
                                tempChunk.str("");
                            }

                            isEnd = true;
                            break;
                        }
                    }
                    else
                    {
                        int nChunkSize = (pChunckCur-pChunckPre);

                        if( ( nChunkLenth-( static_cast<int>(tempChunk.str().size()) + nChunkSize ) ) > 0 )
                        {
                            pChunckCur +=2; //skip "\r\n"
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );
                        }
                        else
                        {
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );

                            int nWriteSize = static_cast<int>( tempChunk.str().size() );
                            if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                            {
                                nWriteSize = nMaxBufferSize - nTotalRecvSize;
                                LOGE( TAG, "Buffer MAX" );
                                isSuccess = false;
                                isEnd = true;
                            }

                            pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                            if( pCur == nullptr )
                            {
                                LOGE( TAG, "Buffer pointer is null. " );
                                isSuccess = false;
                                isEnd = true;
                                break;
                            }

                            nTotalRecvSize += nWriteSize;
                            tempChunk.str("");
                            nChunkLenth = 0;

                            pChunckCur +=2; //skip "\r\n"
                        }

                        pChunckPre = pChunckCur;
                    }
                }
            }
            else
            {
                pChunckCur = pChunckEnd;

                int nChunkSize = (pChunckCur-pChunckPre);
                if( nChunkSize > 0 )
                {
                    tempChunk.write( pChunckPre, nChunkSize );
                    pChunckPre = pChunckCur;
                }

                if( isChunked == false )
                {
                    int nWriteSize = static_cast<int>( tempChunk.str().size() );
                    if( nMaxBufferSize < (nWriteSize+nTotalRecvSize) )
                    {
                        nWriteSize = nMaxBufferSize - nTotalRecvSize;
                        LOGE( TAG, "Buffer MAX" );
                        isSuccess = false;
                        isEnd = true;
                    }

                    pCur = (char*)memcpy( pCur, tempChunk.str().c_str(), nWriteSize );
                    if( pCur == nullptr )
                    {
                        LOGE( TAG, "Buffer pointer is null. " );
                        isSuccess = false;
                        isEnd = true;
                        break;
                    }

                    nTotalRecvSize += nWriteSize;
                    tempChunk.str("");
                }
            }

        }while( pLineEnd != nullptr && pChunckCur < pChunckEnd && isEnd == false && pCur != nullptr );

    }while( nCurBuffSize > 0 && ( (nCurBuffSize == BUFFER_BLOCK_SIZE) || isChunked ) && isEnd == false && pCur != nullptr );

    len = nTotalRecvSize;

    return isSuccess;
}

bool CNetworkManager::CNetworkManagerImpl::receiveSSLMessage(int nSocket, std::stringstream &msg, int timeout)
{
    bool isSuccess = false;

    msg.clear();

    if( nSocket < 0 )
    {
        LOGE( TAG, "Wrong socket" );
        return isSuccess;
    }

    SSL* ssl = findSSLBySocket( nSocket );

    if( ssl == nullptr )
    {
        LOGE( TAG, "Wrong ssl socket" );
        return isSuccess;
    }

    SSL_CTX* ctx = SSL_get_SSL_CTX( ssl );
    SSL_CTX_set_timeout( ctx, timeout );

    char buff[BUFFER_BLOCK_SIZE];
    int nRecvSize;
    bool isChunked = false;
    int nCurBuffSize = 0;

    int nChunkLenth = 0;
    std::stringstream tempChunk;
    bool isHeader = false;
    bool isEnd = false;

    isSuccess = true;

    do
    {
        nCurBuffSize = 0;
        nRecvSize = 0;
        memset( buff, '\0', (sizeof(char)*BUFFER_BLOCK_SIZE) );

        do
        {
            nRecvSize = SSL_read( ssl, buff+nCurBuffSize, BUFFER_BLOCK_SIZE-nCurBuffSize );
            if( nRecvSize <= 0 )
            {
                if( nRecvSize < 0 )
                {
                    int sslErrCode = SSL_get_error(ssl, nRecvSize);
                    LOGE( TAG, "SSL Socket read error: %d", sslErrCode );

                    isSuccess = false;
                }

                isEnd = true;
                break;
            }

            nCurBuffSize += nRecvSize;

        }while( false );

        if( isEnd )
            break;

        char* pHeaderEnd = nullptr;

        if( isHeader == false )
        {
            pHeaderEnd = strstr( buff, "\r\n\r\n" );
            if( pHeaderEnd != nullptr )
            {
                pHeaderEnd += 4; //skip \r\n\r\n

                char* pChunked = strstr( buff, "Transfer-Encoding: chunked" );
                if( pChunked != nullptr )
                    isChunked = true;

                msg.write( buff, pHeaderEnd-buff );
                isHeader = true;
            }
            else
            {
                msg.write( buff, nCurBuffSize );
                continue;
            }
        }

        if( pHeaderEnd == nullptr )
            pHeaderEnd = buff;

        char* pChunckPre = pHeaderEnd;
        char* pChunckCur = pHeaderEnd;
        char* pChunckEnd = buff + nCurBuffSize;
        char* pLineEnd = nullptr;

        do
        {
            pLineEnd = strstr( pChunckCur, "\r\n" );

            if( pLineEnd != nullptr )
            {
                pChunckCur = pLineEnd;

                if( isChunked )
                {
                    if( nChunkLenth <= 0 )
                    {
                        {
                            std::stringstream ssChunkLenth;
                            ssChunkLenth << std::hex << std::string( pChunckPre, pChunckCur-pChunckPre ).c_str();
                            ssChunkLenth >> nChunkLenth;
                        }

                        pChunckCur +=2; //skip "\r\n"
                        pChunckPre = pChunckCur;

                        if( nChunkLenth == 0 )
                        {
                            if( tempChunk.str().empty() == false )
                            {
                                msg << tempChunk.str();
                                tempChunk.str("");
                            }
                            isEnd = true;
                            break;
                        }
                    }
                    else
                    {
                        int nChunkSize = (pChunckCur-pChunckPre);

                        if( ( nChunkLenth-( static_cast<int>(tempChunk.str().size()) + nChunkSize ) ) > 0 )
                        {
                            pChunckCur +=2; //skip "\r\n"
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );
                        }
                        else
                        {
                            tempChunk.write( pChunckPre, pChunckCur-pChunckPre );

                            msg << tempChunk.str();
                            tempChunk.str("");
                            nChunkLenth = 0;

                            pChunckCur +=2; //skip "\r\n"
                        }

                        pChunckPre = pChunckCur;
                    }
                }
            }
            else
            {
                pChunckCur = pChunckEnd;

                int nChunkSize = (pChunckCur-pChunckPre);
                if( nChunkSize > 0 )
                {
                    tempChunk.write( pChunckPre, nChunkSize );
                    pChunckPre = pChunckCur;
                }

                if( isChunked == false )
                {
                    msg << tempChunk.str().c_str();
                    tempChunk.str("");
                }
            }

        }while( pLineEnd != nullptr && pChunckCur < pChunckEnd && isEnd == false );

    }while( nCurBuffSize > 0 && ( (nCurBuffSize == BUFFER_BLOCK_SIZE) || isChunked ) && isEnd == false );

    return isSuccess;
}

void CNetworkManager::CNetworkManagerImpl::setTimeout(int nSocket, int timeout)
{
    //set timeout
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;
    setsockopt(nSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof( tv ) );
}