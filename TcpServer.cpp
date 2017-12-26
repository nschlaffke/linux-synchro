//
// Created by ns on 22.11.17.
//
const int DEFAULT_QUEUE_SIZE  = 10;
#include "TcpServer.h"
TcpServer::TcpServer(const std::string ip, const unsigned short port, const unsigned int queueSize)
        : TcpSocket(ip, port)
{
   doBind();
   TcpSocket::doListen(queueSize);
}

TcpServer::TcpServer(const std::string ip, const unsigned short port)
        : TcpSocket(ip, port)
{
   doBind();
   TcpSocket::doListen(DEFAULT_QUEUE_SIZE);
}

TcpSocket TcpServer::doAccept()
{
   return TcpSocket::doAccept();
}

TcpServer::TcpServer()
{}

