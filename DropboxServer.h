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
    void sendFileProcedure(std::string filePath) override;

    void recieveFileProcedure(std::string filePath) override;

    void moveFileProcedure(std::string source, std::string destination) override;

    void deleteFileProcedure(std::string filePath) override;

    void createDirectoryProcedure(std::string directoryPath) override;

    void newClientProcedure();

public:

    DropboxServer(const std::string &ip, const unsigned short port);
    int run();

private:
    std::mutex clientsMutex;
    std::atomic<bool> newClient;
    std::vector<TcpSocket> clients;

    void acceptClients();
};


#endif //DROPBOX_DROPBOXSERVER_H
