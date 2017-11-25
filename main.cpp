#include <iostream>
#include "TcpSocket.h"
#include "TcpServer.h"

using namespace std;

int main()
{
    TcpServer tcpServer;
    try
    {
        tcpServer = TcpServer("127.0.0.1", 7070, 10);
    }
    catch(TcpSocket::SocketException a)
    {
       cout << a.what() << endl;
        return 0;
    }
    TcpSocket tmp;
    while(true)
    {
        tmp = tcpServer.doAccept();
        cout << "new client connected\n";
    }
}