//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include "DropboxServer.h"


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
                // TODO implement heartbeat protocol
                continue;
            }
            // pobieramy numerek zdarzenia
            Event tmp;
            recieveEvent(tmp);
            if (tmp < 0)
            {
                cout << "ERROR: COULDN'T RECIEVE MESSAGE\n";
                return 0;
            }
            switch (tmp)
            {
                case NEW_CLIENT:
                    cout << "NEW CLIENT CONNECTED\n";
                    newClientProcedure(sock);
                    break;

                case NEW_FILE:

                    break;
                case NEW_DIRECTORY:
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

DropboxServer::DropboxServer(const std::string &ip, const unsigned short port, const std::string path)
        : Dropbox(ip, port, path), newClient(false), empty(true)
{}

/**
 * Sprawdza czy są jakiekolwiek pliki do wysłania (empty):
 *      NIE: nie dzieje się nic
 *      TAK: serwer wysyła do klienta wszystkie pliki
 * @param sock
 */
void DropboxServer::newClientProcedure(TcpSocket &sock)
{
    if (empty)
    {
        return;
    }

    boost::filesystem::recursive_directory_iterator end; // domyślny konstruktor wskazuje na koniec
    for (boost::filesystem::recursive_directory_iterator i(folderPath); i != end; i++)
    {
        boost::filesystem::path path = i->path();
        if (boost::filesystem::is_regular_file(path))
        {
            // TODO sendRegularFile procedure
        }
        else if (boost::filesystem::is_directory(path))
        {
            // TODO send new directory
        }
    }
}
/**
 * Serwer:
 * 1. wysyła do klienta event NEW_FILE
 * 2. wysyła do klienta nazwę pliku
 * 3. wysyła do klienta rozmiar pliku
 * @param sock
 */
void DropboxServer::sendFileProcedure(TcpSocket &sock)
{
   sock.
}

