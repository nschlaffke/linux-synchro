//
// Created by PJ on 22.12.17.
//

#include "Inotify.h"

SafeSet<string> Inotify::mOnceIgnoredDirectories;
SafeSet<string> Inotify::mIgnoredDirectories;

Inotify::Inotify() :
    mError(0),
    mEventTimeout(0),
    mLastEventTime(0),
    mEventFlag(IN_ALL_EVENTS),
    mInotifyFileDescriptor(0)
{
        init();
}

Inotify::Inotify(int eventFlag) :
    mError(0),
    mEventTimeout(0),
    mLastEventTime(0),
    mEventFlag(eventFlag),
    mInotifyFileDescriptor(0)
{

        init();
}

Inotify::~Inotify()
{
    if(!close(mInotifyFileDescriptor))
    {
      mError = errno;
    }
}

void Inotify::init()
{
    mInotifyFileDescriptor = inotify_init();
    mDirectories.clear();
    
    if(mInotifyFileDescriptor == -1)
    {
        mError = errno;
        stringstream errorStream;
        errorStream << "Inotify initialization was unsuccessful" << strerror(mError) << ".";
        throw runtime_error(errorStream.str());
    }
}

void Inotify::watchDirectoryRecursively(boost::filesystem::path path)
{

    if(boost::filesystem::exists(path))
    {
        if(boost::filesystem::is_directory(path))
        {
            boost::filesystem::recursive_directory_iterator it(path, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            while(it != end)
            {
                boost::filesystem::path currentPath = *it;

                if(boost::filesystem::is_directory(currentPath) || boost::filesystem::is_symlink(currentPath)){
                    watchFile(currentPath);
                }

                ++it;
            }
        }
        watchFile(path);
    }
    else
    {
        throw runtime_error("It is impossible to watch the path, because the path does not exist. Path: " + path.string());
    }
}

bool Inotify::containsPath(boost::filesystem::path filePath)
{
    for (auto const& entry : mDirectories)
    {
        if(entry.second == filePath)
        {
            return true;
        }
    }

    return false;
}

void Inotify::watchFile(boost::filesystem::path filePath)
{

    if(boost::filesystem::exists(filePath))
    {
        mError = 0;
        int watchDescriptor = 0;

        if(!isIgnored(filePath.string()))
        {
            watchDescriptor = inotify_add_watch(mInotifyFileDescriptor, filePath.string().c_str(), mEventFlag);
        }

        if(watchDescriptor == -1)
        {
            mError = errno;
            stringstream errorStream;
            if(mError == 28)
            {
                errorStream << "Unable to watch " << strerror(mError) << ". Please increase the number of watches in \"/proc/sys/fs/inotify/max_user_watches\".";
                throw runtime_error(errorStream.str());
            }
            
            errorStream << "Unable to watch " << strerror(mError) << ". Path: " << filePath.string();
            throw std::runtime_error(errorStream.str());
        }
        mDirectories[watchDescriptor] = filePath;
    }
    else
    {
        throw runtime_error("It is impossible to watch the path, because the path does not exist. Path: " + filePath.string());
    }
}

void Inotify::ignoreFileOnce(boost::filesystem::path fileName)
{
    Inotify::mOnceIgnoredDirectories.insert(const_cast<string &>(fileName.string()));
}

void Inotify::removeWatch(int watchDescriptor)
{
    int result = inotify_rm_watch(mInotifyFileDescriptor, watchDescriptor);
    
    if(result == -1){
        mError = errno;
        stringstream errorStream;
        errorStream << "Failed to remove watch! " << strerror(mError) << ".";
        throw runtime_error(errorStream.str());
      }

    mDirectories.erase(watchDescriptor);
}

boost::filesystem::path Inotify::watchDescriptorToPath(int watchDescriptor)
{
    return mDirectories[watchDescriptor];
}

void Inotify::setEventFlag(int eventFlag)
{
    mEventFlag = eventFlag;
}

int Inotify::getEventFlag()
{
    return mEventFlag;
}

EventType Inotify::getNextEvent()
{
    int length = 0;
    char buffer[EVENT_BUF_LEN];
    time_t currentEventTime = time(NULL);
    vector<EventType> events;

    int flags = fcntl(mInotifyFileDescriptor, F_GETFL, 0);
    fcntl(mInotifyFileDescriptor, F_SETFL, flags | O_NONBLOCK);

    while(mEventQueue.empty())
    {
        length = 0;
        memset(&buffer, 0, EVENT_BUF_LEN);
        while(length <= 0)
        {
            length = static_cast<int>(read(mInotifyFileDescriptor, buffer, EVENT_BUF_LEN));
            currentEventTime = time(NULL);
            if(length == -1)
            {
                mError = errno;
                if(mError != EINTR)
                {
                    continue;
                }
            }
        }

        currentEventTime = time(NULL);
        int i = 0;
        while(i < length)
        {
            inotify_event *event = ((struct inotify_event*) &buffer[i]);
            boost::filesystem::path path(watchDescriptorToPath(event->wd) / string(event->name));
            if(boost::filesystem::is_directory(path))
            {
                event->mask |= IN_ISDIR;
            }
            EventType filesystemEvent(event->wd, event->mask, path);

            if(!filesystemEvent.getPath().empty())
            {
                events.push_back(filesystemEvent);
            }

            i += EVENT_SIZE + event->len;
        }

        for(auto eventIt = events.begin(); eventIt < events.end(); ++eventIt)
        {
            EventType currentEvent = *eventIt;
            if(onTimeout(currentEventTime))
            {
                events.erase(eventIt);
            }

            else if (isIgnored(currentEvent.getPath().string()))
            {
                events.erase(eventIt);
            }
            
            else
            {
                mLastEventTime = currentEventTime;
                mEventQueue.push(currentEvent);
            }

        }
    }

    EventType event = mEventQueue.front();
    mEventQueue.pop();
    return event;
}

int Inotify::getLastErrno()
{
    return mError;
}

bool Inotify::isIgnored(string fileName)
{
    if(Inotify::mIgnoredDirectories.empty() && Inotify::mOnceIgnoredDirectories.empty())
    {
        return false;
    }

    for(auto it = Inotify::mOnceIgnoredDirectories.begin(); it != Inotify::mOnceIgnoredDirectories.end(); ++it)
    {
        size_t pos = fileName.find(*it);
        if(pos != string::npos)
        {
            Inotify::mOnceIgnoredDirectories.erase(it);
            return true;
        }
    }

    for(auto it = Inotify::mOnceIgnoredDirectories.begin(); it != Inotify::mOnceIgnoredDirectories.end(); ++it)
    {
        size_t pos = fileName.find(*it);
        if(pos != string::npos)
        {
            return true;
        }
    }

    return false;
}

bool Inotify::onTimeout(time_t eventTime){
    return (mLastEventTime + mEventTimeout) > eventTime;
  }
