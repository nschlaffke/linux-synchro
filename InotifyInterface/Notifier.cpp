//
// Created by PJ on 22.12.17.
//

#include "Notifier.h"

using namespace inotify;

auto Notifier::watchPathRecursively(boost::filesystem::path path) -> Notifier&
{
    mInotify->watchDirectoryRecursively(path);
    return *this;
}

auto Notifier::watchFile(boost::filesystem::path file) -> Notifier&
{
    mInotify->watchFile(file);
    return *this;
}

auto Notifier::ignoreFileOnce(string fileName) -> Notifier&
{
    mInotify->ignoreFileOnce(fileName);
    return *this;
}

auto Notifier::onEvent(Event event, std::function<void(Notification)> eventObserver) -> Notifier&
{
    mInotify->setEventFlag(mInotify->getEventFlag() | static_cast<uint32_t>(event));
    mEventObserver[event] = eventObserver;
    return *this;
}

auto Notifier::onEvents(vector<Event> events, std::function<void(Notification)> eventObserver) -> Notifier&
{
    for(auto event : events){
        mInotify->setEventFlag(mInotify->getEventFlag() | static_cast<uint32_t>(event));
        mEventObserver[event] = eventObserver;
    }

    return *this;
}

auto Notifier::run_once() -> void
{
    auto fileSystemEvent = mInotify->getNextEvent();
    Event event = static_cast<Event>(fileSystemEvent.getWEventFlag());

    auto eventAndEventObserver = mEventObserver.find(event);
    if (eventAndEventObserver == mEventObserver.end()) {
        return;
    }

    Notification notification;
    notification.event = event;
    notification.path = fileSystemEvent.getPath();

    auto eventObserver = eventAndEventObserver->second;
    eventObserver(notification);
}

auto Notifier::run() -> void
{
    while(true) {
        run_once();
    }
}