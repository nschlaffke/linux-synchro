//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOXCLIENT_H
#define DROPBOX_DROPBOXCLIENT_H

#include "Dropbox.h"
#include "TcpSocket.h"

class DropboxClient : public Dropbox
{
public:
    DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath);

    int run() override;

private:


    void newClientProcedure();

};


#endif //DROPBOX_DROPBOXCLIENT_H
