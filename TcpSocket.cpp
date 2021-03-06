//
// Created by ns on 20.11.17.
//

#define DEBUG 0

#include "TcpSocket.h"

#if (DEBUG == 1)
#include <iostream>
#endif

TcpSocket::TcpSocket(const std::string ip, const unsigned short port) :
        bound(false), sock(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
        ipAddress(ip), portNumber(port), connectionEstablished(false)

{
    if (!sock.isValid())
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to create a socket"));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
}

TcpSocket::TcpSocket()
{}
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

size_t TcpSocket::receiveData(char *buffer, size_t bufferSize)
{
    if (!connectionEstablished)
    {
        throw (POSIXError::getErrorMessage("Socket is not connected"));
    }
    int all = bufferSize;
    while(bufferSize > 0)
    {
        ssize_t result = read(sock.getVal(), buffer, bufferSize);
        buffer += result;
        bufferSize -= result;
        if (result == -1)
        {
            throw SocketException(POSIXError::getErrorMessage("Failed to read: "));
        }
    }
    return static_cast<size_t>(all);
}

size_t TcpSocket::sendData(const char data[], size_t size)
{
    int counter = static_cast<int>(size);
    const char *p= data;
    while(counter > 0)
    {
        ssize_t result = write(sock.getVal(), p, size);
        if (result == -1)
        {
            throw SocketException(POSIXError::getErrorMessage("Failed to write: "));
        }
        counter -= size;
        p += result;
    }
    return static_cast<size_t>(size);
}

TcpSocket::~TcpSocket()
{
    // closeSocket();
}

TcpSocket::TcpSocket(Descriptor tmp) : sock(tmp), connectionEstablished(true)
{}

bool TcpSocket::operator==(const TcpSocket &rhs) const
{
    return sock == rhs.sock;
}

bool TcpSocket::operator!=(const TcpSocket &rhs) const
{
    return !(rhs == *this);
}

void TcpSocket::setIpAddress(const std::string &ipAddress)
{
    TcpSocket::ipAddress = ipAddress;
}

void TcpSocket::setPortNumber(unsigned short portNumber)
{
    TcpSocket::portNumber = portNumber;
}

const std::string &TcpSocket::getIpAddress() const
{
    return ipAddress;
}

unsigned short TcpSocket::getPortNumber() const
{
    return portNumber;
}
