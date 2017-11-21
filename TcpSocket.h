//
// Created by ns on 20.11.17.
//

#ifndef DROPBOX_TCPSOCKET_H
#define DROPBOX_TCPSOCKET_H

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

using namespace std;
class TcpSocket
{
public:

    class SocketException {};
    class CreationFailure : SocketException {};
    class ConnectionFailure : SocketException {};
    class WrongState : SocketException {};
    class ReadFailure : SocketException {};
    class WriteFailure : SocketException {};

    virtual ~TcpSocket();

    TcpSocket(const string ip, const unsigned short port);
    void doConnect();
    bool isConnected();
    void closeSocket();
    size_t recieveData(char buffer[], const size_t bufferSize);
    size_t sendData(const char data[], const size_t size);

private:
    bool connectionEstablished;
    const string ipAddress;
    const unsigned short int portNumber;
    int sock;
};


#endif //DROPBOX_TCPSOCKET_H
