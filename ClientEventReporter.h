//
// Created by PJ on 27.12.17.
//

#ifndef DROPBOX_CLIENTEVENTREPORTER_H
#define DROPBOX_CLIENTEVENTREPORTER_H

#include "InotifyInterface/Notifier.h"
#include "ProtocolEvent.h"
#include "SafeQueue.h"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <mutex>

#define BUFFER_SIZE 16384 // 16kB

using namespace inotify;

class ClientEventReporter
{

public:

    struct FileInfo
    {
        boost::filesystem::path path;
        int fileSize;
        friend bool operator==(const FileInfo& first, const FileInfo& second){
            return first.path == second.path && first.fileSize == second.fileSize;
        }
    };

    static vector <FileInfo> allFilesInfo;
    static SafeQueue<EventMessage> messageQueue;

    ClientEventReporter(boost::filesystem::path observedDirectory);
    static bool checkIfSameFiles(boost::filesystem::path path1, boost::filesystem::path path2);
    static void makeRequest(Notification notification, ProtocolEvent protocolEvent);
    static bool checkIfCopied(Notification &notification);
    static void requestCreation(Notification notificationTo);
    static void requestDeletion(Notification notification);
    static void requestMoveFrom(Notification notification);
    static void requestMoveTo(Notification notification);
    static void chooseRequest(Notification notification);
    void handleNotifications();

private:

    boost::filesystem::path observedDirectory;

    static char* convertToCharArray(string path);
    static bool isInternalMove(Notification &notification);
    static int getFileSize(const char* filename);
    vector <FileInfo> collectFilePaths(boost::filesystem::path dir);


};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
