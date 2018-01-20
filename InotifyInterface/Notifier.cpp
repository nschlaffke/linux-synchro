//
// Created by PJ on 22.12.17.
//

#include "Notifier.h"

using namespace inotify;

const vector<Event> Notifier::events =
        {Event::access, Event::attrib, Event::close_write, Event::close_nowrite, Event::create,
         Event::remove, Event::remove_self, Event::close, Event::modify, Event::move_self,
         Event::moved_from, Event::moved_to, Event::move, Event::open, Event::all};

// const vector<Event> Notifier::events = {Event::modify};


string Notifier::getEventName(Event event) {
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
            return "create(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::create_dir:
            return "create_dir(" + to_string(static_cast<uint32_t>(event)) + ")";

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

        case Event::moved_to_dir:
            return "moved_to(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::move:
            return "move(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::internal_move:
            return "internal_move(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::outward_move:
            return "outward_move(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::open:
            return "open(" + to_string(static_cast<uint32_t>(event)) + ")";

        case Event::all:
            return "all(" + to_string(static_cast<uint32_t>(event)) + ")";

        default:
            throw std::runtime_error("Unknown inotify event");
    }
}

Notifier &Notifier::watchPathRecursively(boost::filesystem::path path) {
    mInotify->watchDirectoryRecursively(path);
    return *this;
}

Notifier &Notifier::watchFile(boost::filesystem::path file) {
    mInotify->watchFile(file);
    return *this;
}

Notifier &Notifier::ignoreFileOnce(string fileName) {
    mInotify->ignoreFileOnce(fileName);
    return *this;
}

Notifier &Notifier::onEvent(Event event, NotificationHandler eventObserver) {
    mInotify->setEventFlag(mInotify->getEventFlag() | static_cast<uint32_t>(event));
    mEventObserver[event] = eventObserver;
    return *this;
}

Notifier &Notifier::onEvents(vector<Event> events, NotificationHandler eventObserver) {
    for (auto event : events) {
        mInotify->setEventFlag(mInotify->getEventFlag() | static_cast<uint32_t>(event));
        mEventObserver[event] = eventObserver;
    }

    return *this;
}

Event Notifier::determineMoveType(Notification &notification) {

    vector<Notification> notifications;
    Notification tmpNotification;

    do {
        auto fileSystemEvent = mInotify->getNextEvent();
        tmpNotification.event = static_cast<Event>(fileSystemEvent.getWEventFlag());
        tmpNotification.destination = fileSystemEvent.getPath();
        notifications.push_back(tmpNotification);
    } while (mInotify->getLastErrno() != EAGAIN);

    for(Notification notif: notifications)
    {
        if(notif.event == Event::moved_to || notif.event == Event::moved_to_dir)
        {
            cout << "Internal moveE\n\n";
            notification.source = notification.destination;
            notification.destination = notif.destination;
            return Event::internal_move;
        }
    }

    cout << "Outward moveE\n\n";
    return Event::outward_move;
}

void Notifier::runOnce() {
    auto fileSystemEvent = mInotify->getNextEvent();
    Event event = static_cast<Event>(fileSystemEvent.getWEventFlag());
    Notification notification;
    notification.source = "";
    notification.destination = fileSystemEvent.getPath();

    if (event == Event::moved_from || event == Event::moved_from_dir)
    {
        event = determineMoveType(notification);
    }

    else if(event == Event::create)
    {
        this->watchFile(fileSystemEvent.getPath());
    }

    else if(event == Event::create_dir)
    {
        this->watchPathRecursively(fileSystemEvent.getPath());
    }

    notification.event = event;

    auto eventAndEventObserver = mEventObserver.find(event);
    if (eventAndEventObserver == mEventObserver.end()) {
        return;
    }

    auto eventObserver = eventAndEventObserver->second;
    eventObserver(notification);
}

void Notifier::run() {
    while (true) {
        runOnce();
    }
}
