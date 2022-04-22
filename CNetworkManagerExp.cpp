#include <memory>

#include "CNetworkConfig.h"
#include "CNetworkManager.h"

#include <sstream>

#include <nlohmann/json.hpp>
#include <CLogger.h>

namespace nsCNetworkManagerExp{
    const char* TAG = "CNetworkManagerExp";
}

using namespace nsCNetworkManagerExp;

using json = nlohmann::json;

class CNetworkManagerExp{
public:

    CNetworkManagerExp();
    ~CNetworkManagerExp();

    void makeSendHttpMessage(std::vector<std::string>& header, std::stringstream& ssMessage,
                             const std::string& uri, const std::string& method, const std::string& jsonBody, const std::string& strParams = std::string(""));
    bool isValidSendDataAndNetwork(const std::string& strMessage);
    bool workNetworkProcess(int& nSocket, const std::string& strMessage, std::stringstream& recvBuff);

private:
    std::unique_ptr<CNetworkManager>    m_pNetworkManager;
};

CNetworkManagerExp::CNetworkManagerExp()
    : m_pNetworkManager(nullptr)
{
    m_pNetworkManager = std::make_unique<HNetworkManager>();
}

CNetworkManagerExp::~CNetworkManagerExp()
{
    if( m_pNetworkManager != nullptr )
    {
        m_pNetworkManager.reset();
        m_pNetworkManager = nullptr;
    }
}

void CNetworkManagerExp::makeSendHttpMessage(std::vector<std::string>& header, std::stringstream& ssMessage,
                                                               const std::string& uri, const std::string& method, const std::string& jsonBody, const std::string& strParams)
{
    ssMessage << method;
    ssMessage << NET_API_PF;
    ssMessage << uri;

    if (strParams.length() > 0)
    {
        ssMessage << strParams;
    }

    ssMessage << NET_PROTOCOL_HTTP_1_1 << NET_API_ENDL;
    ssMessage << NET_HEADER_HOST << NET_SVR_DOMAIN << NET_API_ENDL;

    if ( (strParams.length() > 0) || (jsonBody.size() > 0) )
    {
        ssMessage << NET_HEADER_ACCEPT << NET_ACCEPT_JSON << NET_API_ENDL;
    }

    if (jsonBody.size() > 0)
    {
        ssMessage << NET_HEADER_CONTENT_TYPE << NET_CONTENT_TYPE_JSON << NET_API_ENDL;
        ssMessage << NET_HEADER_USER_AGENT << NET_USER_AGENT << NET_API_ENDL;
        ssMessage << NET_HEADER_CONTENT_LENGTH << jsonBody.size() << NET_API_ENDL;
        ssMessage << NET_HEADER_ACCEPT_ENCODING << NET_ACCEPT_ENCODING << NET_API_ENDL;
    }

    for( std::vector< std::string >::const_iterator it = header.cbegin(); it != header.cend(); ++it )
        ssMessage << (*it) << NET_API_ENDL;

    ssMessage << NET_API_ENDL;

    if (jsonBody.size() > 0)
    {
        ssMessage << jsonBody << NET_API_ENDL;
    }

    LOGD(TAG, "ssMessage : %s", ssMessage.str().c_str() );
}

bool CNetworkManagerExp::isValidSendDataAndNetwork(const std::string& strMessage)
{
    if( strMessage.empty() )
    {
        LOGE( TAG, "Failed to create request message" );
        return false;
    }

    if( m_pNetworkManager == nullptr )
    {
        LOGE( TAG, "Network manager is nullptr" );
        return false;
    }

    return true;
}

bool CNetworkManagerExp::workNetworkProcess(int& nSocket, const std::string& strMessage, std::stringstream& recvBuff)
{
    bool bRet = false;

    do
    {
        bool isSuccess = false;
        int messageId = std::rand();
        
        nSocket = m_pNetworkManager->createSocket( messageId, NET_SVR_DOMAIN, NET_SVR_PORT );

        if( nSocket < 0 )
        {
            LOGE( TAG, "Failed to create socket" );
            break;
        }

        isSuccess = m_pNetworkManager->sendMessage( nSocket, strMessage );
        if( isSuccess == false )
        {
            LOGE( TAG, "Failed to send message" );
            break;
        }

        isSuccess = m_pNetworkManager->receiveMessage( nSocket, recvBuff );
        if( isSuccess == false )
        {
            LOGE( TAG, "Failed to receive message" );
            break;
        }

        bRet = true;

        LOGD(TAG, "receiveMessageLen : %d", recvBuff.str().length());
        LOGD(TAG, "receiveMessage : %s", recvBuff.str().c_str());

    } while(false);

    return bRet;
}