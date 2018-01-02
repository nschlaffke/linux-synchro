//
// Created by PJ on 27.12.17.
//

#ifndef DROPBOX_CLIENTEVENTREPORTER_H
#define DROPBOX_CLIENTEVENTREPORTER_H

#include "InotifyInterface/Notifier.h"
#include "TcpServer.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <mutex>

using namespace inotify;

class ClientEventReporter
{
    static vector <boost::filesystem::path> allFilePaths;

    static char* convertToCharArray(string path);
    vector <boost::filesystem::path> collectFilePaths(boost::filesystem::path dir);

public:

    ClientEventReporter(boost::filesystem::path path);
    static void requestOnly(TcpServer socket, Notification notification);
    static void requestCreation(TcpServer socket, Notification notificationTo);
    static void requestIfCreated(TcpServer socket, Notification notification);
    void handleNotification(TcpServer socket, boost::filesystem::path dir);

};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
