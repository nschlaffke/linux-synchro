//
// Created by PJ on 22.12.17.
//

#include "Notifier.h"

using namespace inotify;

const vector<Event> Notifier::events =
        {Event::access, Event::attrib, Event::close_write, Event::close_nowrite, Event::create,
         Event::remove, Event::remove_self, Event::close, Event::modify, Event::move_self,
         Event::moved_from, Event::moved_to, Event::move, Event::open, Event::all};

string Notifier::getEventName(Event event)
{
    switch (event) {
        case Event::access:
            return "access(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::attrib:
            return "attrib(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::close_write:
            return "close_write(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::close_nowrite:
            return "close_nowrite(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::create:
            return "close_nowrite(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::remove:
            return "remove(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::remove_self:
            return "remove_self(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::close:
            return "close(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::modify:
            return "modify(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::move_self:
            return "move_self(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::moved_from:
            return "moved_from(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::moved_to:
            return "moved_to(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::move:
            return "move(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::open:
            return "open(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::all:
            return "all(" + to_string(static_cast<uint32_t>(event)) + ")";

        default:
            throw std::runtime_error("Unknown inotify event");
    }
}

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
    for(auto event : events)
    {
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
    if (eventAndEventObserver == mEventObserver.end())
    {
        return;
    }

    Notification notification;
    notification.event = event;
    notification.path = fileSystemEvent.getPath();

    auto eventObserver = eventAndEventObserver->second;
    eventObserver(notification);
}

//Notifier BuildNotifier() { return {};} // not needed for now

auto Notifier::run() -> void
{
    while(true) {
        run_once();
    }
}
