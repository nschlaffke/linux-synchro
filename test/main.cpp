#include <iostream>
#include "../TcpSocket.h"
#include "../TcpServer.h"
#include "../InotifyInterface/Notifier.h"
#include "../ClientEventReporter.h"

using namespace std;

int main(int argc, char **argv)
{

    if (argc <= 1) {
        std::cout << "Usage: ./test.o /path/to/dir" << std::endl;
        exit(0);
    }

    boost::filesystem::path dir(argv[1]);
    TcpServer tcpServer("127.0.0.1", 8880);

    ClientEventReporter reporter(dir);
    reporter.handleNotification(tcpServer, dir);

    return 0;
}