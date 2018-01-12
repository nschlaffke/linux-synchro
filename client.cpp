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
    const int port = 8800;
    DropboxClient dropboxClient(ip, port, "/home/ns/Documents/Studia/semestr5/SK2/Dropbox/test/client_folder");
    dropboxClient.doConnect();
    dropboxClient.run();
}
