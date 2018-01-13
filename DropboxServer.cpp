//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include "DropboxServer.h"

DropboxServer::DropboxServer(const std::string &ip, const unsigned short port, const std::string path)
        : Dropbox(path), TcpServer(ip, port), maxClientsNumber(10)
{
   clients.reserve(maxClientsNumber);
}


int DropboxServer::run()
{
    std::vector<std::mutex> mutexVector(maxClientsNumber);
    std::vector<std::mutex>::iterator mutexVectorIterator = mutexVector.begin();
    while (true)
    {
        TcpSocket tmp = doAccept();
        std::thread t(&DropboxServer::clientReceiver, this, tmp, std::ref(*mutexVectorIterator));
        mutexVectorIterator++;
        t.detach();
    }
}
void DropboxServer::clientReceiver(TcpSocket client, std::mutex &clientMutex)
{
    while(true)
    {
        using namespace std;
        Event tmp;
        try
        {
            recieveEvent(client, tmp); /// tutaj usypia
        }
        catch(std::exception a)
        {
            cout << "Terminating clientReceiver: " << a.what();
            return;
        }
        string file, folder;
        switch (tmp)
        {
            case NEW_CLIENT:
                cout << "NEW CLIENT CONNECTED\n";
                try
                {
                    newClientProcedure(client, clientMutex);
                }
                catch (std::exception &a)
                {

                    cout << "NEW_CLIENT error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case NEW_FILE:
                try
                {
                    file = receiveNewFileProcedure(client, clientMutex);
                    cout << "NEW FILE: " << file << endl;
                    broadcastFile(client, file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "NEW_FILE error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    folder = receiveNewDircetoryProcedure(client, clientMutex);
                    cout << "NEW_DIRECTORY: " << folder << endl;
                    broadcastDirectory(client, folder, clientMutex);
                }
                catch (std::exception &a)
                {

                    cout << "NEW_DIRECTORY error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case DELETE_FILE:
                // TODO
                break;
            case MOVE_FILE:
                // TODO
                break;
            default:
                cout << "PROTOCOL error\n Terminating clientReceiver\n";
                return;
                break;
        }
    }
}

/**
 * Sprawdza czy są jakiekolwiek pliki do wysłania (empty):
 *      NIE: nie dzieje się nic
 *      TAK: serwer wysyła do klienta wszystkie pliki
 * @param sock
 */
void DropboxServer::newClientProcedure(TcpSocket &sock, std::mutex &clientMutex)
{
    SocketWithMutex a = {clientMutex, sock};
    boost::filesystem::recursive_directory_iterator end; // domyślny konstruktor wskazuje na koniec
    for (boost::filesystem::recursive_directory_iterator i(folderPath); i != end; i++)
    {
        boost::filesystem::path path = i->path();
        if (boost::filesystem::is_regular_file(path))
        {
            sendNewFileProcedure(sock, path.c_str(), clientMutex);
        }
        else if (boost::filesystem::is_directory(path))
        {
            sendNewDirectoryProcedure(sock, path.c_str(), clientMutex);
        }
    }
    // do listy klientów dodajemy na końcu bo:
    // zanim plik zostanie broadcastowany jest juz w systemie plikow serwera, wiec zostanie wyslany w tej procedurze,
    // a nie chcemy by zostal wyslany 2 razy
    clientsMutex.lock();
    clients.push_back(a);
    clientsMutex.unlock();
    //
}
/**
 * wysyła plik do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastFile(TcpSocket sender, std::string path, std::mutex &clientMutex)
{
    clientsMutex.lock();
    std::vector<SocketWithMutex> clientsCopy = clients;
    clientsMutex.unlock();
    for(SocketWithMutex pair: clientsCopy)
    {
        TcpSocket receiver = pair.sock;
        if(sender != receiver)
        {
            std::thread t(&DropboxServer::sendNewFileProcedure, this, receiver, generateAbsolutPath(path), std::ref(clientMutex));
            t.detach();
        }
    }
}

/**
 * wysyła folder do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastDirectory(TcpSocket &sender, std::string &path, std::mutex &clientMutex)
{
    clientsMutex.lock();
    std::vector<SocketWithMutex> clientsCopy = clients;
    clientsMutex.unlock();
    for(SocketWithMutex pair: clients)
    {
        TcpSocket receiver = pair.sock;
        if(sender != receiver)
        {
            std::thread t(&DropboxServer::sendNewDirectoryProcedure, this, receiver,
                          generateAbsolutPath(path), std::ref(clientMutex));
            t.detach();
        }
    }
}