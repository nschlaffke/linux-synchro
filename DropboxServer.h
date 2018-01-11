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

class DropboxServer : public Dropbox, public TcpServer
{
private:
protected:
    void sendFileProcedure(std::string filePath) override;

private:
    void recieveFileProcedure(std::string filePath) override;


    void newClientProcedure();

public:

    DropboxServer(const std::string &ip, const unsigned short port, const std::string path);
    int run();

private:
    std::mutex clientsMutex;
    std::atomic<bool> newClient;
    std::vector<TcpSocket> clients;

    void acceptClients();
};


#endif //DROPBOX_DROPBOXSERVER_H
