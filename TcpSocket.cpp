//
// Created by ns on 20.11.17.
//

#define DEBUG 0
#include <iostream>
#include "TcpSocket.h"
#include "POSIXError.h"
#if (DEBUG == 1)
    #include <iostream>
#endif
TcpSocket::TcpSocket(const std::string ip, const unsigned short port) :
        ipAddress(ip), portNumber(port), connectionEstablished(false), bound(false), sock(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    if(!sock.isValid())
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to create a socket"));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
}

TcpSocket::TcpSocket()
{}

void TcpSocket::doClose()
{
  sock.doClose();
}

TcpSocket TcpSocket::doAccept()
{
    if (!bound)
    {
        throw SocketException(POSIXError::getErrorMessage("Accept before binding"));
    }

    socklen_t size = sizeof(addr);
    int fd = accept(sock.getVal(), reinterpret_cast<sockaddr *>(&addr), &size);

    if(fd == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to accept"));
    }

    TcpSocket tmp;
    tmp.sock = fd;
    tmp.connectionEstablished = true;
    return tmp;
}

void TcpSocket::doBind()
{
    int reuse = 1;
    int result = setsockopt(sock.getVal(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if(result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to set socket option"));
    }
    result = bind(sock.getVal(), reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
    if(result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to bind"));
    }
    else
    {
        bound = true;
    }
}
void TcpSocket::doConnect()
{
    if (connectionEstablished)
    {
        throw SocketException(POSIXError::getErrorMessage("Socket is already connected"));
    }
    int result = connect(sock.getVal(), reinterpret_cast<sockaddr *>(&addr), sizeof addr);
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to connect"));
    }
    connectionEstablished = true;
}

bool TcpSocket::isConnected()
{
    return connectionEstablished;
}

void TcpSocket::closeSocket()
{
    if (!connectionEstablished)
    {
        throw SocketException(POSIXError::getErrorMessage("Socket is not connected"));
    }
    close(sock.getVal());
}

size_t TcpSocket::recieveData(char buffer[], size_t bufferSize)
{
    if (!connectionEstablished)
    {
        throw (POSIXError::getErrorMessage("Socket is not connected"));
    }
    ssize_t result = read(sock.getVal(), buffer, bufferSize);

    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to read"));
    }
    else
        return static_cast<size_t>(result);
}

size_t TcpSocket::sendData(const char data[], size_t size)
{
    ssize_t result = write(sock.getVal(), data, size);
    if(result ==  -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to write"));
    }
    else
        return static_cast<size_t>(result);
}

TcpSocket::~TcpSocket()
{
    // closeSocket();
}

void TcpSocket::doListen(const unsigned int queueSize)
{
    listen(sock.getVal(), queueSize);
}
