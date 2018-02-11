//
// Created by PJ on 27.12.17.
//

#ifndef DROPBOX_CLIENTEVENTREPORTER_H
#define DROPBOX_CLIENTEVENTREPORTER_H

#include "InotifyInterface/Notifier.h"
#include "SafeQueue.h"
#include "Dropbox.h"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 16384 // 16kB

using namespace inotify;

class ClientEventReporter
{

public:

    struct FileInfo
    {
        boost::filesystem::path path;
        bool isOpen;
        friend bool operator==(const FileInfo& first, const FileInfo& second){
            return first.path == second.path;
        }

        friend bool operator<(const FileInfo& first, const FileInfo& second){
            return first.path < second.path;
        }
    };

    static SafeSet<FileInfo> allFilesInfo;
    static SafeSet<std::string> ignoredPaths; // fullPaths
    static SafeSet<std::string> permanentlyIgnored;
    static SafeQueue<Dropbox::EventMessage> messageQueue;

    ClientEventReporter(boost::filesystem::path observedDirectory);
    static bool checkIfSameFiles(boost::filesystem::path path1, boost::filesystem::path path2);
    static void saveAsClosed(Notification notification);
    static void saveAsOpen(Notification notification);
    static void makeRequest(Notification notification, Dropbox::ProtocolEvent protocolEvent);
    static bool checkIfCopied(Notification &notification);
    static void requestCreation(Notification notificationTo);
    static void requestDeletion(Notification notification);
    static void requestMoveFrom(Notification notification);
    static void requestMoveTo(Notification notification);
    static void chooseRequest(Notification notification);

    static FileInfo findByPath(boost::filesystem::path path)
    {
        for(FileInfo fileInfo: allFilesInfo){
            if (fileInfo.path == path)
            {
                return fileInfo;
            }
        }

        FileInfo empty;
        empty.path = boost::filesystem::path("");

        return empty;
    }

private:

    boost::filesystem::path observedDirectory;

    static bool isInternalMove(Notification &notification);
    static bool isIgnored(boost::filesystem::path path);
    void collectFilePaths(boost::filesystem::path dir);
};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
