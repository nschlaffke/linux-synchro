//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOXCLIENT_H
#define DROPBOX_DROPBOXCLIENT_H

#include "Dropbox.h"
#include "TcpSocket.h"

class DropboxClient : public Dropbox
{

private:
    void sendFileProcedure(std::string filePath) override;

    void recieveFileProcedure(std::string filePath) override;

public:
    DropboxClient(const std::string &ip, const unsigned short port);

    int run() override;

private:

protected:
    void moveFileProcedure(std::string source, std::string destination) override;

private:
    void deleteFileProcedure(std::string filePath) override;

    void createDirectoryProcedure(std::string directoryPath) override;

    void newClientProcedure();
};


#endif //DROPBOX_DROPBOXCLIENT_H
