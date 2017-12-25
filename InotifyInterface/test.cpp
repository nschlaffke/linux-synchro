//
// Created by PJ on 22.12.17.
//

#include "Notifier.h"
#include <iostream>

using namespace inotify;

int main(int argc, char **argv) {

    if (argc <= 1) {
        std::cout << "Usage: ./test.o /path/to/dir" << std::endl;
        exit(0);
    }

    boost::filesystem::path dir(argv[1]);

    auto handleNotification = [&](Notification notification)
    {

        std::cout << "Event: " << Notifier::getEventName(notification.event) << " on " <<  notification.path.relative_path() << " was triggered." << std::endl;
    };

    std::cout << "Setup watches for " << dir << "..." << std::endl;
    Notifier notifier = Notifier() 
        .watchPathRecursively(dir)
        .ignoreFileOnce("file")
        .onEvents(Notifier::events, handleNotification);

    std::cout << "Waiting for events..." << std::endl;
    notifier.run();

    return 0;
}
