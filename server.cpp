//
// Created by ns on 04.01.18.
//
#include <iostream>
#include "TcpServer.h"
#include "DropboxServer.h"

#define DEBUG

#include <string>
#include <vector>
#include <mutex>
#include <thread>


using namespace std;
int main(int argc, char *argv[])
{
#ifndef DEBUG
    if(argc != 3)
    {
        cout << "enter: server.cpp [ip] [port]\n";
        return 0;
    }
#else
    const string ip = "192.168.8.105";
    const short port = 8800;
#endif
    string path("/home/ns/Documents/Studia/semestr5/SK2/Dropbox/test/server_folder");
    DropboxServer dropboxServer(ip, port, path);
    dropboxServer.run();
}

