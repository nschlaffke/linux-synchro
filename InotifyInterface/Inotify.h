//
// Created by PJ on 22.12.17.
//

#pragma once
#include <string>
#include <queue>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <exception>
#include <sstream>
#include <memory>
#include <boost/filesystem.hpp>
#include <sys/inotify.h>
#include "EventType.h"

#define MAX_EVENTS     4096
#define EVENT_SIZE     (sizeof (inotify_event))
#define EVENT_BUF_LEN  (MAX_EVENTS * (EVENT_SIZE + 16))

using namespace std;

class Inotify{

    int mError;
    time_t mEventTimeout;
    time_t mLastEventTime;
    int mEventFlag;
    vector<string> mIgnoredDirectories;
    vector<string> mOnceIgnoredDirectories;
    queue<EventType> mEventQueue;
    map<int, boost::filesystem::path> mDirectories;
    int mInotifyFileDescriptor;

    boost::filesystem::path watchDescriptorToPath(int watchDescriptor);
    bool isIgnored(string fileName);
    bool onTimeout(time_t eventTime);
    void removeWatch(int watchDescriptor);
    void init();

    public:
        Inotify();
        Inotify(int eventFlag);
        ~Inotify();
        void watchDirectoryRecursively(boost::filesystem::path path);
        void watchFile(boost::filesystem::path path);
        void ignoreFileOnce(boost::filesystem::path path);
        void setEventFlag(int eventFlag);
        int getEventFlag();
        EventType getNextEvent();
        int getLastErrno();
};