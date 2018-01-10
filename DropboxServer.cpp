//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include "DropboxServer.h"

void DropboxServer::sendFileProcedure(std::string filePath)
{

}

void DropboxServer::recieveFileProcedure(std::string filePath)
{

}

void DropboxServer::deleteFileProcedure(std::string filePath)
{

}

void DropboxServer::createDirectoryProcedure(std::string directoryPath)
{

}

int DropboxServer::run()
{
    using namespace std;
    vector<TcpSocket> clientsCopy = clients;
    thread t1(&DropboxServer::acceptClients, this);
    while ((true))
    {
        clientsMutex.lock();
        if (newClient)
        {
            clientsCopy = clients; // kopiowanie wektora klientow czasochlonne, pozera pamiec. alternatywa - watki
            newClient = false;
        }
        clientsMutex.unlock();
        for (TcpSocket sock: clientsCopy)
        {
            clientsMutex.unlock();
            if (!sock.hasData())
            {
                // nic nie wysyla
                // TODO implement heartbeat protocol
                continue;
            }
            // pobieramy numerek zdarzenia
            TcpSocket::IntType tmp;
            sock.recieveData(tmp);
            if (tmp < 0)
            {
                cout << "ERROR: COULDN'T RECIEVE MESSAGE\n";
                return 0;
            }
            switch (tmp)
            {
                case NEW_CLIENT:
                    cout << "NEW CLIENT CONNECTED\n";

                    break;

                case NEW_FILE:
                    // klient dodal plik, serwer go pobiera nastepnie wysyla do wszystkich innych
                    cout << "NEW FILE FROM CLIENT\n";
                    // klient wysyla rozmiar pliku
                    TcpSocket::IntType size;
                    sock.recieveData(size);
                    cout << "FILE SIZE: " << size << endl;
                    // klient wysyla nazwe pliku

                    break;
                case NEW_DIRECTORY:
                    //
                    break;
                case DELETE_FILE:
                    break;

                case MOVE_FILE:
                    break;
                default:
                    cout << "UNKNOW MESSAGE: " << tmp << endl;
                    return 0;
                    break;
            }

        }
    }
}

void DropboxServer::acceptClients()
{
    while (true)
    {
        TcpSocket tmp = doAccept();
        clientsMutex.lock();
        clients.push_back(tmp);
        newClient = true;
        clientsMutex.unlock();
    }
}

DropboxServer::DropboxServer(const std::string &ip, const unsigned short port)
        : TcpServer(ip, port), newClient(false)
{}

void DropboxServer::moveFileProcedure(std::string source, std::string destination)
{

}

void DropboxServer::newClientProcedure()
{
    
}
