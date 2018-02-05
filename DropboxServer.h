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
#include "SafeQueue.h"
#include "TcpSocket.h"

class DropboxServer : public Dropbox, public TcpServer
{

public:
    DropboxServer(const std::string &ip, const unsigned short port, const std::string path);
    void run();

private:
    struct ClientData;
    const int maxClientsNumber;
    std::vector<std::reference_wrapper<ClientData> > clients;
    SafeQueue<EventMessage> broadcasterQueue;
    void newClientProcedure(ClientData &clientData);
    std::mutex clientsMutex;
    void broadcastFile(TcpSocket sender, std::string path, std::mutex &clientMutex);
    void broadcastDirectory(TcpSocket &sender, std::string &path, std::mutex &clientMutex);
    void broadcastDeletion(TcpSocket &sender, std::string &path, std::mutex &clientMutex);
    void broadcastMove(TcpSocket &sender, std::string &file1, std::string &file2, std::mutex &clientMutex);
    void broadcastCopy(TcpSocket &sender, std::string &file1, std::string &file2, std::mutex &clientMutex);
    std::vector<ClientData>::iterator clientVectorIterator;
    void clientReceiver(ClientData &clientData);
    void clientSender(ClientData &clientData);

    void terminateClientReceiver(ClientData &clientData);
};


struct DropboxServer::ClientData
{
    std::mutex sockMutex;
    TcpSocket sock;
    SafeQueue<EventMessage> safeQueue;
    bool isAlive = false;
};
#endif //DROPBOX_DROPBOXSERVER_H
