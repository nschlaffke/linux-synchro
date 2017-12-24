//
// Created by PJ on 22.12.17.
//

#include "Notifier.h"
#include <boost/filesystem.hpp>
#include <iostream>

using namespace inotify;

int main(int argc, char **argv) {

    std::cout << argv[1] << " " << argv[2] << endl << endl;

    if (argc <= 1) {
        std::cout << "Usage: ./inotify_example /path/to/dir" << std::endl;
        exit(0);
    }

    boost::filesystem::path dir(argv[1]);

    auto handleNotification = [&](Notification notification) {
        std::cout << "Event" << notification.event << " on " <<  notification.path << " was triggered." << std::endl;
    };

    std::cout << "Setup watches for " << dir << "..." << std::endl;
    auto notifier = BuildNotifier()
        .watchPathRecursively(dir)
        .ignoreFileOnce("file")
        .onEvents({Event::create, Event::modify, Event::remove, Event::move}, handleNotification);

    std::cout << "Waiting for events..." << std::endl;
    notifier.run();

    return 0;
}