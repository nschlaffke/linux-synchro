//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include "DropboxServer.h"

DropboxServer::DropboxServer(const std::string &ip, const unsigned short port, const std::string path)
        : Dropbox(path), TcpServer(ip, port), maxClientsNumber(10)
{}


int DropboxServer::run()
{
    std::vector<ClientData> clientVector(maxClientsNumber);
    clientVectorIterator = clientVector.begin();
    while (true)
    {
        TcpSocket tmp = doAccept();
        clientVectorIterator++;
        clientVectorIterator->sock = tmp;
        std::thread receiver(&DropboxServer::clientReceiver, this, std::ref(*clientVectorIterator));
        std::thread sender(&DropboxServer::clientSender, this, std::ref(*clientVectorIterator));
        receiver.detach();
        sender.detach();
    }
}
void DropboxServer::clientSender(ClientData &clientData)
{
    using namespace std;
    TcpSocket client = clientData.sock;
    std::mutex &clientMutex = clientData.sockMutex;
    SafeQueue<EventMessage> &queue = clientData.safeQueue;
    while(true)
    {
        EventMessage message = queue.dequeue(); // tutaj usypia
        ProtocolEvent event = message.event;
        string file, folder;
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    cout << "SENDING NEW FILE: " << file << endl;
                    sendNewFileProcedure(client, generateAbsolutPath(message.source), clientMutex);
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
                    cout << "SENDING NEW DIRECTORY: " << folder << endl;
                    sendNewDirectoryProcedure(client, generateAbsolutPath(message.source), clientMutex);
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
void DropboxServer::clientReceiver(ClientData &clientData)
{
    TcpSocket client = clientData.sock;
    std::mutex &clientMutex = clientData.sockMutex;
    while(true)
    {
        using namespace std;
        ProtocolEvent tmp;
        try
        {
            receiveEvent(client, tmp); /// tutaj usypia
        }
        catch(std::exception &a)
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
                    newClientProcedure(clientData);
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
                    file = receiveNewFileProcedure(client, clientData.sockMutex);
                    cout << "NEW FILE: " << file << endl;
                    broadcastFile(client, file, clientData.sockMutex);
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
                    folder = receiveNewDircetoryProcedure(client, clientData.sockMutex);
                    cout << "NEW_DIRECTORY: " << folder << endl;
                    broadcastDirectory(client, folder, clientData.sockMutex);
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
void DropboxServer::newClientProcedure(ClientData &clientData)
{
    TcpSocket &sock = clientData.sock;
    std::mutex &clientMutex = clientData.sockMutex;
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
    clients.push_back(std::ref(clientData));
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
    std::vector<std::reference_wrapper<ClientData> > clientsCopy = clients;
    clientsMutex.unlock();
    for(ClientData &clientData: clientsCopy)
    {
        TcpSocket receiver = clientData.sock;
        if(sender != receiver)
        {
            EventMessage tmp;
            tmp.event = NEW_FILE;
            tmp.source = path;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
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
    std::vector<std::reference_wrapper<ClientData> > clientsCopy = clients;
    clientsMutex.unlock();
    for(ClientData &clientData: clients)
    {
        TcpSocket receiver = clientData.sock;
        if(sender != receiver)
        {
            /*
            std::thread t(&DropboxServer::sendNewDirectoryProcedure, this, receiver,
                          generateAbsolutPath(path), std::ref(clientMutex));
            t.detach();
             */
            EventMessage tmp;
            tmp.event = NEW_DIRECTORY;
            tmp.source = path;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
        }
    }
}