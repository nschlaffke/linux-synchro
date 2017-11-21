//
// Created by ns on 20.11.17.
//

#include <iostream>
#include "TcpSocket.h"

TcpSocket::TcpSocket(const string ip, const unsigned short port) : ipAddress(ip), portNumber(port), connectionEstablished(false)
{
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        throw CreationFailure();
    }
}

void TcpSocket::doConnect()
{
    if (connectionEstablished)
    {
        throw WrongState();
    }
    sockaddr_in addr =
            {.sin_family = AF_INET, .sin_port = htons(portNumber)};
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    int result = connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof addr);
    if (result == -1)
    {
        throw ConnectionFailure();
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
        throw WrongState();
    }
    close(sock);
}

size_t TcpSocket::recieveData(char buffer[], size_t bufferSize)
{
    if (!connectionEstablished)
    {
        throw WrongState();
    }
    ssize_t result = read(sock, buffer, bufferSize);

    if (result == -1)
    {
        throw ReadFailure();
    }
    else
        return static_cast<size_t>(result);
}

size_t TcpSocket::sendData(const char data[], size_t size)
{
    ssize_t result = write(sock, data, size);
    if(result ==  -1)
    {
        throw WriteFailure();
    }
    else
        return static_cast<size_t>(result);
}

TcpSocket::~TcpSocket()
{
    closeSocket();
}
