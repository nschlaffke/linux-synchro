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

    auto handleNotification = [&](TcpServer server, Notification notification)
    {

        std::cout << "Event: " << Notifier::getEventName(notification.event) << " on " <<  notification.path.string() << " was triggered." << std::endl;
        // if a path to a single file is passed as a parameter (argv[1]) instead of passing a path to a whole directory,
        // a gibberish sign is showed instead of the second quotation mark in the console output.
    };

    std::cout << "Setup watches for " << dir << "..." << std::endl;
    Notifier notifier = Notifier() 
        .watchPathRecursively(dir)
        .ignoreFileOnce("file")
        .onEvents(Notifier::events, handleNotification);

    std::cout << "Waiting for events..." << std::endl;

    TcpServer tcpServer("127.0.0.1", 8888);
    notifier.run(tcpServer);

    return 0;
}
