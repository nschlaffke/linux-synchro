//
// Created by ns on 20.11.17.
//

#ifndef DROPBOX_TCPSOCKET_H
#define DROPBOX_TCPSOCKET_H

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <exception>
#include <stdexcept>
#include <memory>
#include "Descriptor.h"

class TcpSocket
{
public:

    class SocketException : public std::runtime_error
    {
    public:
        SocketException(const std::string &s) : runtime_error(s) {}
    };

    virtual ~TcpSocket();
    TcpSocket();
    TcpSocket(const std::string ip, const unsigned short port);
    void doConnect();
    bool isConnected();
    void doClose();
    void closeSocket();
    size_t recieveData(char buffer[], const size_t bufferSize);
    size_t sendData(const char data[], const size_t size);

protected:
    TcpSocket doAccept();
    void doListen(const unsigned int queueSize);
    void doBind();

private:
    bool connectionEstablished;
    std::string ipAddress;
    unsigned short int portNumber;
    Descriptor sock;
    bool bound;
    sockaddr_in addr;
};


#endif //DROPBOX_TCPSOCKET_H
