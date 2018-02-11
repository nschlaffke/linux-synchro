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
        ip = "0.0.0.0";
        port = 8811;
        path = "/home/ns/semestr5/SK2/Dropbox/test/server_folder";
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

