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
    TcpSocket(string ip, unsigned short port);
    void doConnect();
    bool isConnected();
    void closeSocket();
    void recieveData();
    void sendData();

private:
    bool connectionEstablished;
    const string ipNumber;
    const unsigned short int portNumber;
    int sock;
};


#endif //DROPBOX_TCPSOCKET_H
