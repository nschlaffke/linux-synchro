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
    if(argc != 3)
    {
        ip = "0.0.0.0";
        port = 8811;
    }
    else
    {
        // TODO podawanie numeru portu oraz IP z linii poleceń
    }

    string path("/home/ns/semestr5/SK2/Dropbox/test/server_folder");
    cout << "Running server\nIP: " << ip << " port: " << port << endl;
    DropboxServer dropboxServer(ip, port, path);
    dropboxServer.run();
}

