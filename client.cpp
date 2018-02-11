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
        std::cout << "./client.o [ip] [port] [path]" << endl;
        return 0;
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
    std::cout << "Client was run on: " << path << std::endl;
    dropboxClient.run();
}
