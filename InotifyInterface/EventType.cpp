//
// Created by PJ on 22.12.17.
//

#include "EventType.h"

EventType::EventType(const int watchDescriptor, int eventFlag, const boost::filesystem::path path) :
    watchDescriptor(watchDescriptor),
    eventFlag(eventFlag),
    path(path){
}

int EventType::getWatchDescriptor()
{
    return watchDescriptor;
}

void EventType::setWatchDescriptor(int watchDescriptor)
{
    this->watchDescriptor = watchDescriptor;
}

int EventType::getWEventFlag()
{
    return eventFlag;
}

void EventType::setEventFlag(int eventFlag)
{
    this->eventFlag = eventFlag;
}

boost::filesystem::path EventType::getPath()
{
    return path;
}

void EventType::setPath(boost::filesystem::path path)
{
    this->path = path;
}

string EventType::flagsToString(int eventFlag)
{
    string flagsString = "";
    
      if(IN_ACCESS & eventFlag)
        flagsString.append("IN_ACCESS ");
      if(IN_ATTRIB & eventFlag)
        flagsString.append("IN_ATTRIB ");
      if(IN_CLOSE_WRITE & eventFlag)
        flagsString.append("IN_CLOSE_WRITE ");
      if(IN_CLOSE_NOWRITE & eventFlag)
        flagsString.append("IN_CLOSE_NOWRITE ");
      if(IN_CREATE & eventFlag)
        flagsString.append("IN_CREATE ");
      if(IN_DELETE & eventFlag)
        flagsString.append("IN_DELETE ");
      if(IN_DELETE_SELF & eventFlag)
        flagsString.append("IN_DELETE_SELF ");
      if(IN_MODIFY & eventFlag)
        flagsString.append("IN_MODIFY ");
      if(IN_MOVE_SELF & eventFlag)
        flagsString.append("IN_MOVE_SELF ");
      if(IN_MOVED_FROM & eventFlag)
        flagsString.append("IN_MOVED_FROM ");
      if(IN_MOVED_TO & eventFlag)
        flagsString.append("IN_MOVED_TO ");
      if(IN_OPEN & eventFlag)
        flagsString.append("IN_OPEN ");
      if(IN_ISDIR & eventFlag)
        flagsString.append("IN_ISDIR ");
      if(IN_UNMOUNT & eventFlag)
        flagsString.append("IN_UNMOUNT ");
      if(IN_Q_OVERFLOW & eventFlag)
        flagsString.append("IN_Q_OVERFLOW ");
      if(IN_CLOSE & eventFlag)
        flagsString.append("IN_CLOSE ");
      if(IN_IGNORED & eventFlag)
        flagsString.append("IN_IGNORED ");
      if(IN_ONESHOT & eventFlag)
        flagsString.append("IN_ONESHOT ");
    
    return flagsString;
}