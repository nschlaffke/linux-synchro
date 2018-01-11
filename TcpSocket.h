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
#include "Descriptor.h"

#define CHUNK_SIZE 1024

class TcpSocket
{
public:
    typedef uint32_t IntType;
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

    void receiveFile(const std::string fileName, size_t fileSize); // fileName jest sciezka do pliku
    void sendFile(const std::string fileName);

    void setNoBlock();

    void setBlock();

    bool hasData();

    TcpSocket(Descriptor tmp);

    void recieveData(IntType &data);

    void sendData(IntType data);

    size_t recieveData(char buffer[], const size_t bufferSize);

    size_t sendData(const char data[], const size_t size);

protected:
    bool bound;
    sockaddr_in addr;
    Descriptor sock;
private:
    size_t getFileSize(const std::string fileName);

    bool connectionEstablished;
    std::string ipAddress;
    unsigned short int portNumber;

};


#endif //DROPBOX_TCPSOCKET_H
