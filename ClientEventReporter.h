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
    static boost::filesystem::path observedDirectory;
    static TcpServer serverSocket;
    static vector <boost::filesystem::path> allFilePaths;

    static char* convertToCharArray(string path);
    vector <boost::filesystem::path> collectFilePaths(boost::filesystem::path dir);

public:

    ClientEventReporter(boost::filesystem::path observedDirectory, TcpServer serverSocket);
    boost::filesystem::path getObservedDirectory();
    void setObservedDirectory(boost::filesystem::path observedDirectory);
    TcpServer getServerSocket();
    void setServerSocket(TcpServer serverSocket);
    static void requestOnly(Notification notification);
    static void requestCreation(Notification notificationTo);
    static void requestIfCreated(Notification notification);
    void handleNotification();

};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
