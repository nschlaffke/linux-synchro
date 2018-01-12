//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include "DropboxServer.h"

DropboxServer::DropboxServer(const std::string &ip, const unsigned short port, const std::string path)
        : Dropbox(path), newClient(false), TcpServer(ip, port)
{}

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
        for (TcpSocket client: clientsCopy)
        {
            if (!client.hasData())
            {
                // TODO implement heartbeat protocol
                continue;
            }
            // pobieramy numerek zdarzenia
            Event tmp;
            recieveEvent(client, tmp);
            if (tmp < 0)
            {
                cout << "ERROR: COULDN'T RECIEVE MESSAGE\n";
                return 0;
            }
            string file, folder;
            switch (tmp)
            {
                case NEW_CLIENT:
                    cout << "NEW CLIENT CONNECTED\n";
                    newClientProcedure(client);
                    break;
                case NEW_FILE:
                    file = receiveNewFileProcedure(client);
                    cout << "NEW FILE: " << file << endl;
                    broadcastFile(client, file);
                    break;
                case NEW_DIRECTORY:
                    folder = receiveNewDircetoryProcedure(client);
                    cout << "NEW DIRECTORY: " << folder << endl;
                    broadcastDirectory(client, folder);
                    break;
                case DELETE_FILE:
                    break;
                case MOVE_FILE:
                    break;
                default:
                    cout << "UNKNOW MESSAGE: " << tmp << endl;
                    throw DropboxException("Protocol error");
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
/**
 * Sprawdza czy są jakiekolwiek pliki do wysłania (empty):
 *      NIE: nie dzieje się nic
 *      TAK: serwer wysyła do klienta wszystkie pliki
 * @param sock
 */
void DropboxServer::newClientProcedure(TcpSocket &sock)
{
    boost::filesystem::recursive_directory_iterator end; // domyślny konstruktor wskazuje na koniec
    for (boost::filesystem::recursive_directory_iterator i(folderPath); i != end; i++)
    {
        boost::filesystem::path path = i->path();
        if (boost::filesystem::is_regular_file(path))
        {
            sendNewFileProcedure(sock, path.c_str());
        }
        else if (boost::filesystem::is_directory(path))
        {
            sendNewDirectoryProcedure(sock, path.c_str());
        }
    }
}
/**
 * wysyła plik do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastFile(TcpSocket &sender, std::string &path)
{
    clientsMutex.lock();
    for(TcpSocket receiver : clients)
    {
        if(sender != receiver)
        {
            sendNewFileProcedure(receiver, generateAbsolutPath(path));
        }
    }
    clientsMutex.unlock();
}

/**
 * wysyła folder do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastDirectory(TcpSocket &sender, std::string &path)
{
    clientsMutex.lock();
    for(TcpSocket receiver : clients)
    {
        if(sender != receiver)
        {
            sendNewDirectoryProcedure(receiver, generateAbsolutPath(path));
        }
    }
    clientsMutex.unlock();
}

