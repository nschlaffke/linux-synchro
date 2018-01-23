//
// Created by ns on 09.01.18.
//

#include "TcpSocket.h"
#include "Dropbox.h"
#include <mutex>
#include "DropboxClient.h"
#include <iostream>

using namespace std;

int main(int argc, const char *argv[])
{
    string ip;
    unsigned short port;
    string path;
    if(argc == 4)
    {
        ip = argv[1];
        port = static_cast<unsigned short>(std::stoi(argv[2]));
        path = argv[3];
    }
    else
    {
        ip = "127.0.0.1";
        port = 8811;
        path = "/home/ns/Documents/Studia/semestr5/SK2/Dropbox/test/client2_folder";
    }

    DropboxClient dropboxClient(ip, port, path);
    try
    {
        dropboxClient.doConnect();
    }
    catch(std::exception &e)
    {
       cout << e.what();
       return 0;
    }
    dropboxClient.run();
}
