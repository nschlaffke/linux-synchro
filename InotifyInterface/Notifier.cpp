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

Notifier BuildNotifier() { return {};} // what is it for?

ostream& operator <<(ostream& stream, const Event& event)
{
    switch(event){
        case Event::access:
            stream << "access(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::attrib:
            stream << "attrib(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::close_write:
            stream << "close_write(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::close_nowrite:
            stream << "close_nowrite(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::create:
            stream << "close_nowrite(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::remove:
            stream << "remove(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::remove_self:
            stream << "remove_self(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::close:
            stream << "close(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::modify:
            stream << "modify(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::move_self:
            stream << "move_self(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::moved_from:
            stream << "moved_from(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::moved_to:
            stream << "moved_to(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::move:
            stream << "move(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::open:
            stream << "open(" << static_cast<uint32_t >(event) << ")";
            break;
        case Event::all:
            stream << "all(" << static_cast<uint32_t >(event) << ")";
            break;
        default:
            throw std::runtime_error("Unknown inotify event");
    }
    return stream;
}

auto Notifier::run() -> void
{
    while(true) {
        run_once();
    }
}
