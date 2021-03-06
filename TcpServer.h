//
// Created by ns on 22.11.17.
//

#ifndef DROPBOX_TCPSERVER_H
#define DROPBOX_TCPSERVER_H

#include "TcpSocket.h"
#include <string>
#include <stdexcept>
#include <exception>
#include "Descriptor.h"

class TcpServer : public TcpSocket
{
public:
    TcpServer();

    TcpServer(const std::string ip, const unsigned short port, const unsigned int queueSize);

    TcpServer(const std::string ip, const unsigned short port);

    TcpSocket doAccept();

protected:
    void doBind();
private:
    void doListen(const unsigned int queueSize);
};


#endif //DROPBOX_TCPSERVER_H
