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
        struct timespec modificationTime;
        friend bool operator==(const FileInfo& first, const FileInfo& second){
            return first.path == second.path &&
                   first.modificationTime.tv_sec == second.modificationTime.tv_sec &&
                   first.modificationTime.tv_nsec == second.modificationTime.tv_nsec;
        }

        friend bool operator<(const FileInfo& first, const FileInfo& second){

            if(first.modificationTime.tv_sec != second.modificationTime.tv_sec)
            {
                return first.modificationTime.tv_sec < second.modificationTime.tv_sec;
            }
            else
            {
                return first.modificationTime.tv_nsec < second.modificationTime.tv_nsec;
            }
        }
    };

    static SafeSet<FileInfo> allFilesInfo;
    static SafeSet<std::string> ignoredPaths; // fullPaths
    static SafeQueue<Dropbox::EventMessage> messageQueue;

    ClientEventReporter(boost::filesystem::path observedDirectory);
    static bool checkIfSameFiles(boost::filesystem::path path1, boost::filesystem::path path2);
    static void makeRequest(Notification notification, Dropbox::ProtocolEvent protocolEvent);
    static bool checkIfCopied(Notification &notification);
    static void requestCreation(Notification notificationTo);
    static void requestDeletion(Notification notification);
    static void requestMoveFrom(Notification notification);
    static void requestMoveTo(Notification notification);
    static void chooseRequest(Notification notification);

private:

    boost::filesystem::path observedDirectory;

    static char* convertToCharArray(string path);
    static bool isInternalMove(Notification &notification);
    static int getFileSize(const char* filename);
    static bool isIgnored(boost::filesystem::path path);
    void collectFilePaths(boost::filesystem::path dir);


};

#endif //DROPBOX_CLIENTEVENTREPORTER_H
