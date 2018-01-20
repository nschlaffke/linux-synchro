//
// Created by ns on 22.11.17.
//
const int DEFAULT_QUEUE_SIZE = 10000;

#include "TcpServer.h"
#include "POSIXError.h"

/*TcpServer::TcpServer()
{}
 */
TcpServer::TcpServer(const std::string ip, const unsigned short port, const unsigned int queueSize)
        : TcpSocket(ip, port)
{
    doBind();
    doListen(queueSize);
}

TcpServer::TcpServer(const std::string ip, const unsigned short port)
        : TcpSocket(ip, port)
{
    doBind();
    doListen(DEFAULT_QUEUE_SIZE);
}

TcpSocket TcpServer::doAccept()
{
    // TODO do accepta podawac addr tworzonego socketa. bedzie mozna uzyskac IP i port
    if (!bound)
    {
        throw SocketException(POSIXError::getErrorMessage("Accept before binding"));
    }

    socklen_t size = sizeof(addr);
    int fd = accept(sock.getVal(), reinterpret_cast<sockaddr *>(&addr), &size);

    if (fd == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to accept"));
    }
    TcpSocket tmp(fd);
    // tmp.setNoBlock();
    return tmp;
}

void TcpServer::doListen(const unsigned int queueSize)
{
    listen(sock.getVal(), queueSize);
}

void TcpServer::doBind()
{
    int reuse = 1;
    int result = setsockopt(sock.getVal(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to set socket option"));
    }
    result = bind(sock.getVal(), reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to bind"));
    } else
    {
        bound = true;
    }
}

