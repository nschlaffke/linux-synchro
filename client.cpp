//
// Created by ns on 09.01.18.
//

#include "TcpSocket.h"
#include "Dropbox.h"
#include "DropboxClient.h"
#include <iostream>

using namespace std;

int main()
{
    const string ip = "192.168.8.105";
    const short port = 8888;
    DropboxClient dropboxClient(ip, port);
    dropboxClient.run();
}
