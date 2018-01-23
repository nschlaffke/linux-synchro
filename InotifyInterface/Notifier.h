//
// Created by PJ on 22.12.17.
//

#ifndef NOTIFIER_H
#define NOTIFIER_H


#include "Inotify.h"
#include "../TcpServer.h"
#include <boost/filesystem.hpp>
#include <string>
#include <memory>
#include <thread>

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
        create_dir = IN_CREATE | IN_ISDIR,
        remove = IN_DELETE,
        remove_self = IN_DELETE_SELF,
        remove_dir = IN_DELETE | IN_ISDIR,
        remove_self_dir = IN_DELETE_SELF | IN_ISDIR,
        modify = IN_MODIFY,
        move_self = IN_MOVE_SELF,
        moved_from = IN_MOVED_FROM,
        moved_from_dir = IN_MOVED_FROM | IN_ISDIR,
        moved_to = IN_MOVED_TO,
        moved_to_dir = IN_MOVED_TO | IN_ISDIR,
        move = IN_MOVE,
        open = IN_OPEN,
        all = IN_ALL_EVENTS,
        outward_move = 1111,
        internal_move = 2222,
    };

    struct Notification
    {
        Event event;
        boost::filesystem::path source;
        boost::filesystem::path destination;
    };

    using NotificationHandler = std::function<void(Notification)>;

    class Notifier 
    {

        shared_ptr<Inotify> mInotify;
        map<Event, NotificationHandler> mEventObserver;
        Event determineMoveType(Notification &notification);

        public:
            Notifier() : mInotify(make_shared<Inotify>()){}

            void run();
            void runOnce();
            Notifier& watchPathRecursively(boost::filesystem::path path);
            Notifier& watchFile(boost::filesystem::path file);
            Notifier& ignoreFileOnce(string fileName);
            Notifier& onEvent(Event event, NotificationHandler);
            Notifier& onEvents(vector<Event> event, NotificationHandler);

            static const vector <Event> events;
            static string getEventName(Event event);
    };
}

#endif