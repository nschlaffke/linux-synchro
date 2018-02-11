//
// Created by PJ on 22.12.17.
//

#ifndef INOTIFY_H
#define INOTIFY_H

#include <string>
#include <set>
#include <queue>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>
#include <cassert>
#include <cerrno>
#include <ctime>
#include <string>
#include <exception>
#include <sstream>
#include <memory>
#include <sys/inotify.h>
#include <fcntl.h>
#include <iostream>
#include "EventType.h"
#include "../SafeSet.h"

#define MAX_EVENTS     4096
#define EVENT_SIZE     (sizeof (inotify_event))
#define EVENT_BUF_LEN  (MAX_EVENTS * (EVENT_SIZE + 16))

using namespace std;

class Inotify{

    int mError;
    time_t mEventTimeout;
    time_t mLastEventTime;
    int mEventFlag;
    queue<EventType> mEventQueue;
    map<int, boost::filesystem::path> mDirectories;
    int mInotifyFileDescriptor;

    bool containsPath(boost::filesystem::path filePath);
    boost::filesystem::path watchDescriptorToPath(int watchDescriptor);
    int pathToWatchDescriptor(boost::filesystem::path path);
    bool isIgnored(string fileName);
    bool onTimeout(time_t eventTime);
    std::string correctPath(std::string path);
    void init();

    public:

        static SafeSet<string> mIgnoredDirectories;
        static SafeSet<string> mOnceIgnoredDirectories;

        Inotify();
        Inotify(int eventFlag);
        ~Inotify();
        void watchDirectoryRecursively(boost::filesystem::path path);
        void watchFile(boost::filesystem::path path);
        void removeWatch(boost::filesystem::path filePath);
        void ignoreFileOnce(boost::filesystem::path path);
        void setEventFlag(int eventFlag);
        int getEventFlag();
        EventType getNextEvent();
        int getLastErrno();
};

#endif