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
}


bool TcpSocket::isConnected()
{
    return connectionEstablished;
}

void TcpSocket::close()
{
    if(!connectionEstablished)
    {
        throw int(-1); // TODO wyjatek
    }
    close(sock);
}
