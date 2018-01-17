//
// Created by ns on 04.01.18.
//
#include <iostream>
#include "TcpServer.h"
#include "DropboxServer.h"


#include <string>
#include <vector>
#include <mutex>
#include <thread>


using namespace std;
int main(int argc, char *argv[])
{
    string ip;
    short port;
    if(argc != 3)
    {
        ip = "127.0.0.1";
        port = 8811;
    }
    else
    {
        // TODO podawanie numeru portu oraz IP z linii poleceń
    }
    string path("/home/piotr/Desktop/Dropbox/__testServer__");
    cout << "Running server\nIP: " << ip << " port: " << port << endl;
    DropboxServer dropboxServer(ip, port, path);
    dropboxServer.run();
}

