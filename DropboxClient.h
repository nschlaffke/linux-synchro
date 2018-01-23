//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOXCLIENT_H
#define DROPBOX_DROPBOXCLIENT_H

#include "Dropbox.h"
#include "TcpSocket.h"
#include "SafeQueue.h"
#include "ClientEventReporter.h"
#include <mutex>

class DropboxClient : public Dropbox, public TcpSocket
{
public:

    DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath);
    void run() override;

private:

    std::mutex serverMutex;

    TcpSocket &serverSocket;
    std::string folderPath;

    Notifier notifier;

    void newClientProcedure();

    void sender();

    void receiver();

};


#endif //DROPBOX_DROPBOXCLIENT_H
