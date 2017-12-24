//
// Created by PJ on 22.12.17.
//

#pragma once

#include "Inotify.h"
#include <boost/filesystem.hpp>
#include <string>
#include <memory>

using namespace std;


namespace inotify {

    enum class Event
    {
        access = IN_ACCESS,
        attrib = IN_ATTRIB,
        close_write = IN_CLOSE_WRITE,
        close_nowrite = IN_CLOSE_NOWRITE,
        close = IN_CLOSE,
        create = IN_CREATE,
        remove = IN_DELETE,
        remove_self = IN_DELETE_SELF,
        modify = IN_MODIFY,
        move_self = IN_MOVE_SELF,
        moved_from = IN_MOVED_FROM,
        moved_to = IN_MOVED_TO,
        move = IN_MOVE,
        open = IN_OPEN,
        all = IN_ALL_EVENTS
    };

    struct Notification
    {
        Event event;
        boost::filesystem::path path;
    };

    class Notifier 
    {

        shared_ptr<Inotify> mInotify;
        map<Event, function<void(Notification)>> mEventObserver;

        public:
            Notifier() : mInotify(make_shared<Inotify>()){}

            auto run() -> void;
            auto run_once() -> void;
            auto watchPathRecursively(boost::filesystem::path path) -> Notifier&;
            auto watchFile(boost::filesystem::path file) -> Notifier&;
            auto ignoreFileOnce(string fileName) -> Notifier&;
            auto onEvent(Event event, function<void(Notification)>) -> Notifier&;
            auto onEvents(vector<Event> event, function<void(Notification)>) -> Notifier&;
    };

    Notifier BuildNotifier() { return {};}
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
}