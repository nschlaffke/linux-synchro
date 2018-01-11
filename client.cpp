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

    DropboxClient dropboxClient(ip, port, "/home/ns/Documents/Studia/semestr5/SK2/Dropbox");
    dropboxClient.run();
}
