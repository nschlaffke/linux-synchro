//
// Created by ns on 13.01.18.
//

#ifndef DROPBOX_SOCKETWITHMUTEX_H
#define DROPBOX_SOCKETWITHMUTEX_H


#include <mutex>
#include "TcpSocket.h"

struct SocketWithMutex
{
    std::mutex &sockMutex;
    TcpSocket sock;
};


#endif //DROPBOX_SOCKETWITHMUTEX_H
