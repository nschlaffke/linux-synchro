//
// Created by ns on 20.11.17.
//

#ifndef DROPBOX_TCPSOCKET_H
#define DROPBOX_TCPSOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

class TcpSocket
{
    using namespace std;
public:
    TcpSocket(string ip, unsigned short port);
    void doConnect();
    bool isConnected();
    void close();
    bool isConnected();

private:
    bool connectionEstablished;
    const string ipNumber;
    const unsigned short int portNumber;
    int sock;
};


#endif //DROPBOX_TCPSOCKET_H
