//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOXSERVER_H
#define DROPBOX_DROPBOXSERVER_H


#include <mutex>
#include <atomic>
#include <vector>
#include "Dropbox.h"
#include "TcpServer.h"
#include "SocketWithMutex.h"

class DropboxServer : public Dropbox, public TcpServer
{

public:
    DropboxServer(const std::string &ip, const unsigned short port, const std::string path);
    int run();

private:
    const int maxClientsNumber;
    void newClientProcedure(TcpSocket &sock, std::mutex &clientMutex);
    std::mutex clientsMutex;
    std::vector<SocketWithMutex> clients;

    void broadcastFile(TcpSocket sender, std::string path, std::mutex &clientMutex);

    void broadcastDirectory(TcpSocket &sender, std::string &path, std::mutex &clientMutex);

    void clientReceiver(TcpSocket client, std::mutex &clientMutex);
};


#endif //DROPBOX_DROPBOXSERVER_H
