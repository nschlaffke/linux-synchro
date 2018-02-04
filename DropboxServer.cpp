//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include "DropboxServer.h"

// TODO mutex na system plikow
// TODO sprawdzanie czy klient sie nie odlaczyl
// TODO ogarnac badziewna liste klientow
DropboxServer::DropboxServer(const std::string &ip, const unsigned short port, const std::string path)
        : Dropbox(path), TcpServer(ip, port), maxClientsNumber(10)
{}
void DropboxServer::run()
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
    while (true)
    {
        EventMessage message = queue.dequeue();
        ProtocolEvent event = message.event;
        string file = correctPath(message.destination);
        cout << endl << file << endl << endl;
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    cout << "senSENDING NEW FILE: " << file << endl;
                    sendNewFileProcedure(client, file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "senNEW_FILE error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    cout << "senSENDING NEW DIRECTORY: " << file << endl;
                    sendNewDirectoryProcedure(client, file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "senNEW_DIRECTORY error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case DELETE:
                try
                {
                    cout << "senSENDING DELETION REQUEST: " << file << endl;
                    sendDeletionPathProcedure(client, file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "senDELETION error\nTerminating clientReceiver: " << a.what();
                    return;
                }

                break;
            case MOVE:
                try
                {
                    cout << "senSENDING MOVE REQUEST FROM: " << correctPath(message.source) << " TO: " << file << endl;
                    sendMovePathsProcedure(client, correctPath(message.source), file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "senMOVE error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;

            case COPY:
                try
                {
                    cout << "SENDING COPY REQUEST FROM: " << correctPath(message.source) << " TO: " << file << endl;
                    sendCopyPathsProcedure(client, correctPath(message.source), file, clientMutex);
                }
                catch (std::exception &a)
                {
                    cout << "senCOPY error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            default:
                cout << "senPROTOCOL error\n Terminating clientReceiver\n";
                return;
                break;
        }
    }
}

void DropboxServer::clientReceiver(ClientData &clientData)
{
    TcpSocket client = clientData.sock;
    while (true)
    {
        using namespace std;
        ProtocolEvent tmp;
        try
        {
            receiveEvent(client, tmp); /// tutaj usypia
        }
        catch (std::exception &a)
        {
            cout << "Terminating clientReceiver: " << a.what();
            return;
        }
        string file, folder, *files;
        switch (tmp)
        {
            case NEW_CLIENT:
                cout << "recNEW CLIENT CONNECTED\n";
                try
                {
                    newClientProcedure(clientData);
                }
                catch (std::exception &a)
                {
                    cout << "recNEW_CLIENT error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case NEW_FILE:
                try
                {
                    file = receiveNewFileProcedure(client, clientData.sockMutex);
                    cout << "recNEW FILE: " << file << endl;
                    broadcastFile(client, file, clientData.sockMutex);
                }
                catch (std::exception &a)
                {
                    cout << "recNEW_FILE error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    folder = receiveNewDircetoryProcedure(client, clientData.sockMutex);
                    cout << "recNEW_DIRECTORY: " << folder << endl;
                    broadcastDirectory(client, folder, clientData.sockMutex);
                }
                catch (std::exception &a)
                {
                    cout << "recNEW_DIRECTORY error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case DELETE:
                try
                {
                    file = receiveDeletionPathProcedure(client, clientData.sockMutex);
                    cout << "recDELETE: " << file << endl;
                    broadcastDeletion(client, file, clientData.sockMutex);
                }
                catch (std::exception &a)
                {
                    cout << "recDELETION error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            case MOVE:
                try
                {
                    files = receiveMovePathsProcedure(client, clientData.sockMutex);
                    std::cout << "recMOVE FROM: " << files[0] << " TO: " << files[1] << std::endl;
                    broadcastMove(client, files[0], files[1], clientData.sockMutex);
                }
                catch (std::exception &a)
                {
                    cout << "recMOVE error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;

            case COPY:
                try
                {
                    files = receiveCopyPathsProcedure(client, clientData.sockMutex);
                    std::cout << "recCOPY FROM: " << files[0] << " TO: " << files[1] << std::endl;
                    broadcastCopy(client, files[0], files[1], clientData.sockMutex);
                }
                catch (std::exception &a)
                {
                    cout << "recCopy error\nTerminating clientReceiver: " << a.what();
                    return;
                }
                break;
            default:
                cout << "recPROTOCOL error\n Terminating clientReceiver\n";
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
    // TODO serwer najpierw sprawdza czy pliku nie ma u klienta, a dopiero potem wysyła
    TcpSocket &sock = clientData.sock;
    std::mutex &clientMutex = clientData.sockMutex;

    if (boost::filesystem::exists(folderPath))
    {
        if (boost::filesystem::is_directory(folderPath))
        {
            boost::filesystem::recursive_directory_iterator it(folderPath, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            while (it != end)
            {
                boost::filesystem::path currentPath = *it;

                if (boost::filesystem::is_regular_file(currentPath))
                {
                    if(!askIfValid(sock, it->path().c_str()))
                    {
                        // u klienta plik nie istnieje lub jest starszy, więc go wysyłamy
                        sendNewFileProcedure(sock, it->path().c_str(), clientMutex);
                    }
                }
                else if (boost::filesystem::is_directory(currentPath))
                {
                    // foldery wysyłamy tak czy siak: jak nie ma to sobie utworzy, jak jest to zignoruje
                    // sprawdzanie tego byloby bardziej kosztowne
                    sendNewDirectoryProcedure(sock, it->path().c_str(), clientMutex);
                }
                ++it;
            }
        }
    }
    sendEvent(sock, END_OF_SYNC);Dropbox::ProtocolEvent event;
    do
    {
        receiveEvent(sock, event);
        std::string file;
        switch(event)
        {
            case NEW_FILE:
                file = receiveNewFileProcedure(sock, clientMutex);
                broadcastFile(sock, file, clientMutex);
                break;
            case NEW_DIRECTORY:
                file = receiveNewDircetoryProcedure(sock, clientMutex);
                broadcastDirectory(sock, file, clientMutex);
                break;
            case IS_VALID:
                answerIfValid(sock);
                break;
            case END_OF_SYNC:
                break;
        }
    }while(event != Dropbox::ProtocolEvent::END_OF_SYNC);
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
    for (ClientData &clientData: clientsCopy)
    {
        TcpSocket receiver = clientData.sock;
        if (sender != receiver)
        {
            EventMessage tmp;
            tmp.event = NEW_FILE;
            tmp.destination = path;
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
    for (ClientData &clientData: clients)
    {
        TcpSocket receiver = clientData.sock;
        if (sender != receiver)
        {
            /*
            std::thread t(&DropboxServer::sendNewDirectoryProcedure, this, receiver,
                          generateAbsolutPath(path), std::ref(clientMutex));
            t.detach();
             */
            EventMessage tmp;
            tmp.event = NEW_DIRECTORY;
            tmp.destination = path;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
        }
    }
}

/**
 * wysyła ścieżkę pliku do usunięcia do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastDeletion(TcpSocket &sender, std::string &path, std::mutex &clientMutex)
{
    clientsMutex.lock();
    std::vector<std::reference_wrapper<ClientData> > clientsCopy = clients;
    clientsMutex.unlock();
    for (ClientData &clientData: clients)
    {
        TcpSocket receiver = clientData.sock;
        if (sender != receiver)
        {
            EventMessage tmp;
            tmp.event = DELETE;
            tmp.destination = path;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
        }
    }
}

/**
 * wysyła ścieżki źródłową i docelową pliku do wycięcia do wszystkich klientów oprócz nadawcy
 * @param sender
 */
void DropboxServer::broadcastMove(TcpSocket &sender, std::string &file1, std::string &file2, std::mutex &clientMutex)
{
    clientsMutex.lock();
    std::vector<std::reference_wrapper<ClientData> > clientsCopy = clients;
    clientsMutex.unlock();
    for (ClientData &clientData: clients)
    {
        TcpSocket receiver = clientData.sock;
        if (sender != receiver)
        {
            EventMessage tmp;
            tmp.event = MOVE;
            tmp.source = file1;
            tmp.destination = file2;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
        }
    }
}

void DropboxServer::broadcastCopy(TcpSocket &sender, std::string &file1, std::string &file2, std::mutex &clientMutex)
{
    clientsMutex.lock();
    std::vector<std::reference_wrapper<ClientData> > clientsCopy = clients;
    clientsMutex.unlock();
    for (ClientData &clientData: clients)
    {
        TcpSocket receiver = clientData.sock;
        if (sender != receiver)
        {
            EventMessage tmp;
            tmp.event = COPY;
            tmp.source = file1;
            tmp.destination = file2;
            tmp.sender = sender;
            clientData.safeQueue.enqueue(tmp);
        }
    }
}
