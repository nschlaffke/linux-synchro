//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOXCLIENT_H
#define DROPBOX_DROPBOXCLIENT_H

#include <queue>
#include <mutex>
#include "Dropbox.h"
#include "TcpSocket.h"

class DropboxClient : public Dropbox, public TcpSocket
{
public:
    DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath);

    int run() override;

private:

    std::mutex serverMutex;

    struct EventMessage
    {
        Dropbox::Event event;
        std::string path;
    };

    std::queue<EventMessage> messageQueue;
    std::mutex messageQueueMutex;

    TcpSocket &serverSocket;

    void newClientProcedure();

    void sender();

    void receiver();

};


#endif //DROPBOX_DROPBOXCLIENT_H
