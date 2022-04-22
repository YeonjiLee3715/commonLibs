#ifndef CNETWORKMANAGER_H
#define CNETWORKMANAGER_H

#include <iostream>
#include <memory>

class CNetworkManager
{
public:
    CNetworkManager();
    ~CNetworkManager();

    int createSocket( int messageId, const char* strHost, unsigned short int nPort );
    int createSSLSocket( int messageId, const char* strHost, unsigned short int nPort );

    void disconnectSocket( int nSocket );
    void disconnetAll();

    bool sendMessage( int messageId, const char* strHost , uint16_t nPort, const char* msg , int len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool sendMessage( int nSocket, const char* msg, int len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool sendMessage( int nSocket, const std::string& msg, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec

    bool sendSSLMessage( int messageId, const char* strHost, uint16_t nPort, const char* msg , int len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool sendSSLMessage( int nSocket, const char* msg, int len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool sendSSLMessage( int nSocket, const std::string& msg, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec

    bool receiveMessage( int messageId, const char* strHost , uint16_t nPort, char* msg, int& len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool receiveMessage( int nSocket, char* msg, int &len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool receiveMessage( int nSocket, std::stringstream& msg, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec

    bool receiveSSLMessage( int messageId, const char* strHost , uint16_t nPort, char* msg, int& len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool receiveSSLMessage( int nSocket, char* msg, int &len, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec
    bool receiveSSLMessage( int nSocket, std::stringstream& msg, int timeout = (10 * 1000 * 1000) ); //timeout 10 sec

private:
    class CNetworkManagerImpl;
    std::unique_ptr<CNetworkManagerImpl> m_pImpl;
};

#endif // CNETWORKMANAGER_H
