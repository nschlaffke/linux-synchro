//
// Created by PJ on 22.12.17.
//

#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include <sys/inotify.h>

using namespace std;

class EventType {

    int watchDescriptor;
    int eventFlag;
    boost::filesystem::path path;

    public:
        EventType(int watchDescriptor, int eventFlag, const boost::filesystem::path path);
        ~EventType();


        int getWatchDescriptor();
        void setWatchDescriptor(int watchDescriptor);
        int getWEventFlag();
        void setEventFlag(int eventFlag);
        boost::filesystem::path getPath();
        void setPath(boost::filesystem::path path);
       
        string flagsToString(int eventFlag);
};