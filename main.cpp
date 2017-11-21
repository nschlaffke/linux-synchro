#include <iostream>
#include "TcpSocket.h"

int main()
{
    using namespace std;

    const int MAX = 50;
    TcpSocket socket("127.0.0.1", 8080);
    socket.doConnect();
    while(true)
    {
        string text;
        cin >> text;
        socket.sendData(reinterpret_cast<const TcpSocket::byte *>(text.c_str()), text.length());
    }
}