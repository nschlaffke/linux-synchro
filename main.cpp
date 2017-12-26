#include <iostream>
#include "TcpSocket.h"
#include "TcpServer.h"
#include "InotifyInterface/Notifier.h"
using namespace std;

int main()
{
    TcpServer tcpServer("127.0.0.1", 8888, 20);
    TcpSocket sock = tcpServer.doAccept();
    sock.recieveFile("/home/ns/Documents/semestr5/SK2/Dropbox/TEMP.txt", 5);
}