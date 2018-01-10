//
// Created by ns on 04.01.18.
//
#include <iostream>
#include "TcpServer.h"
#include "protocol.h"

#define DEBUG

#include <string>
#include <vector>
#include <mutex>
#include <thread>


using namespace std;
void acceptClients(TcpServer tcpServer, mutex clientsMutex, vector<TcpSocket> clients);

//TODO: Finish implementing functions responsible for answering to client requests

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
    const short port = 8888;
#endif
    std::mutex clientsMutex;
    std::vector<TcpSocket> clients;
    TcpServer tcpServer;
    std::thread acceptor(&acceptClients, tcpServer, clientsMutex, clients); // nasluchiwanie na nowych klientow
    const int bufferSize = 1024;
    const int messageSize = sizeof(char);
    while ((true))
    {
        for(TcpSocket sock : clients)
        {
            if(!sock.hasData())
            {
                // TODO implement heartbeat protocol
                continue;
            }

            int messageBuffer = 0;
            int tmp;
            tmp = sock.recieveData(reinterpret_cast<char *>(&messageBuffer), messageSize);
            if (tmp < 0)
            {
               cout << "ERROR: COULDN'T RECIEVE MESSAGE\n";
                return 0;
            }
            switch(messageBuffer)
            {
                case NEW_CLIENT:
                    break;

                case NEW_FILE:
                    break;

                case DELETE_FILE:
                    break;

                case MOVE_FILE:
                    break;
            }
        }
    }
}

void acceptClients(TcpServer tcpServer, mutex &clientsMutex, vector<TcpSocket> &clients)
{
    while(true)
    {
        TcpSocket tmp = tcpServer.doAccept();
        clientsMutex.lock();
        clients.push_back(tmp);
        clientsMutex.unlock();
    }
}
