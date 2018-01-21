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
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "Descriptor.h"
#include "POSIXError.h"


class TcpSocket
{
public:
    class SocketException : public std::runtime_error
    {
    public:
        SocketException(const std::string &s) : runtime_error(s)
        {}
    };

    virtual ~TcpSocket();

    TcpSocket();

    TcpSocket(const std::string ip, const unsigned short port);

    void doConnect();

    bool isConnected();

    void doClose();

    void closeSocket();

    void setNoBlock();

    void setBlock();

    int hasData();

    TcpSocket(Descriptor tmp);


    size_t receiveData(char buffer[], const size_t bufferSize);

    size_t sendData(const char data[], const size_t size);

    static const size_t CHUNK_SIZE = 1024;
protected:
    bool bound;
public:
    bool operator==(const TcpSocket &rhs) const;

    bool operator!=(const TcpSocket &rhs) const;

protected:
    sockaddr_in addr;
    Descriptor sock;
private:
    std::string ipAddress;
    unsigned short int portNumber;
    bool connectionEstablished;
};


#endif //DROPBOX_TCPSOCKET_H
