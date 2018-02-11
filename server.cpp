//
// Created by ns on 04.01.18.
//
#include <iostream>
#include "TcpServer.h"
#include "DropboxServer.h"

using namespace std;

int main(int argc, char *argv[])
{
    string ip;
    short port;
    string path;
    if (argc != 3)
    {
        std::cout << "server.o [ip] [port] [path]" << endl;
        return 0;
    }
    else
    {
        ip = argv[1];
        port = static_cast<unsigned short>(std::stoi(argv[2]));
        path = argv[3];

    }
    cout << "Running server\nIP: " << ip << " port: "
         << port << endl << "home folder: " << path << endl;
    DropboxServer dropboxServer(ip, port, path);
    dropboxServer.run();
}

