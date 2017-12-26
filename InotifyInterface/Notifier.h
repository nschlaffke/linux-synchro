//
// Created by PJ on 22.12.17.
//

#ifndef NOTIFIER_H
#define NOTIFIER_H


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

            void run();
            void run_once();
            Notifier& watchPathRecursively(boost::filesystem::path path);
            Notifier& watchFile(boost::filesystem::path file);
            Notifier& ignoreFileOnce(string fileName);
            Notifier& onEvent(Event event, function<void(Notification)>);
            Notifier& onEvents(vector<Event> event, function<void(Notification)>);

            static const vector <Event> events;
            static string getEventName(Event event);
    };
}

#endif