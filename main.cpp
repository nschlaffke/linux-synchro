#include <iostream>
#include "TcpSocket.h"
#include "TcpServer.h"
#include "InotifyInterface/Notifier.h"
#include "ClientEventReporter.h"

using namespace std;

int main(int argc, char **argv)
{
    /*TcpServer tcpServer("127.0.0.1", 8888);
    TcpSocket sock = tcpServer.doAccept();
    sock.recieveFile("/home/ns/Documents/semestr5/SK2/Dropbox/TEMP.txt", 5);*/

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