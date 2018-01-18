//
// Created by PJ on 27.12.17.
//

#ifndef DROPBOX_CLIENTEVENTREPORTER_H
#define DROPBOX_CLIENTEVENTREPORTER_H

#include "InotifyInterface/Notifier.h"
#include "ProtocolEvent.h"
#include "SafeQueue.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <mutex>

using namespace inotify;

class ClientEventReporter
{
    boost::filesystem::path observedDirectory;

    static char* convertToCharArray(string path);
    static bool checkInternalMovement();
    vector <boost::filesystem::path> collectFilePaths(boost::filesystem::path dir);

public:

    struct fileInfo
    {
        boost::filesystem::path path;

    };

    static vector <boost::filesystem::path> allFilePaths;
    static SafeQueue<EventMessage> messageQueue;

    ClientEventReporter(boost::filesystem::path observedDirectory);
    static void makeRequest(Notification notification, ProtocolEvent protocolEvent);
    static void requestCreation(Notification notificationTo);
    static void requestDeletion(Notification notification);
    static void requestCopying(Notification notification);
    static void requestMovement(Notification notification);
    void handleNotifications();
};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
