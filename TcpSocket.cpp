//
// Created by ns on 20.11.17.
//

#include "TcpSocket.h"


TcpSocket::TcpSocket(string ip, unsigned short port) : ipNumber(ip), portNumber(port)
{
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1)
    {
        throw int(-1); // TODO dorobic wyjatek TcpSocketException
    }

}


void TcpSocket::doConnect()
{
    if(connectionEstablished)
    {
        throw int(-1); // TODO odpowiedni wyjatek
    }
    sockaddr_in addr =
            {.sin_family = AF_INET, .sin_port = htons(portNumber)};
    addr.sin_addr.s_addr = inet_addr(ipNumber.c_str());
    int result = connect(sock, reinterpret_cast<sockaddr *>(&addr), sizeof addr);
    if(result == -1)
    {
        throw int(-1); // TODO wyjatek
    }
}


bool TcpSocket::isConnected()
{
    return connectionEstablished;
}

void TcpSocket::closeSocket()
{
    if(!connectionEstablished)
    {
        throw int(-1); // TODO wyjatek
    }
    close(sock);
}

void TcpSocket::recieveData()
{// TODO}

void TcpSocket::sendData()
{// TODO}