//
// Created by ns on 10.01.18.
//
/// pole serverSocket to referencja na samego siebie; istnieje tylko po to, aby w funkcjach typy receiveFile nie pisać *this
#include <iostream>
#include <thread>
#include "DropboxClient.h"

// TODO jeżeli sender umiera, receiver też musi umrzec -> conditional variable
DropboxClient::DropboxClient(const std::string &ip, unsigned short port, const std::string folderPath)
        : Dropbox(folderPath), TcpSocket(ip, port), serverSocket(*this), folderPath(folderPath)
{
    notifier = Notifier()
            .watchPathRecursively(boost::filesystem::path(folderPath))
            .ignoreFileOnce("file")
            .onEvents({Event::remove, Event::remove_self, Event::remove_dir, Event::remove_self_dir, Event::create, Event::create_dir, Event::modify,
                       Event::outward_move, Event::internal_move, Event::moved_to, Event::moved_to_dir}, ClientEventReporter::chooseRequest);
}

void DropboxClient::run()
{
    if (!isConnected())
    {
        throw DropboxException("Error: Client not connected\n");
    }
    newClientProcedure();

    std::thread t(&Notifier::run, notifier);
    std::thread s(&DropboxClient::sender, this);
    std::thread r(&DropboxClient::receiver, this);

    s.join();
    r.join();
    t.join();
}

/**
 * void DropboxClient::newClientProcedure()
 * opis:
 * 1. Klient wysyła do serwera event NEW_CLIENT
 */
void DropboxClient::newClientProcedure()
{
    sendEvent(*this, NEW_CLIENT);
}

/**
 * Funkcja (wątek) zczytujący wiadomości z kolejki messageQueue i wysyłająca je do serwera
 * Wiadomości na kolejce umieszcza ClientEventReporter
 */
void DropboxClient::sender()
{
    while (true)
    {
        EventMessage eventMessage = ClientEventReporter::messageQueue.dequeue();

        ProtocolEvent event = eventMessage.event;
        std::string path = eventMessage.destination;

        std::cout << "Sender:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "senNEW FILE: " << path << std::endl;
                    sendNewFileProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senNEW_FILE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case DELETE:
                try
                {
                    std::cout << "senDELETE: " << path << std::endl;
                    sendDeletionPathProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senDELETE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case MOVE:
                try
                {
                    std::string source = eventMessage.source;
                    std::cout << "senMOVE FROM: " << source << " TO: " << path << std::endl;

                    sendMovePathsProcedure(serverSocket, source, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senMOVE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    sendNewDirectoryProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senNEW_DIRECTORY error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case COPY:
                try
                {
                    std::string source = eventMessage.source;
                    std::cout << "senCOPY FROM: " << source << " TO: " << path << std::endl;

                    sendCopyPathsProcedure(serverSocket, source, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senCOPY error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case NEW_CLIENT:
                break;
        }
    }
}

/**
 * Funkcja (wątek) odbierający wiadomości od serwera
 */
void DropboxClient::receiver()
{
    while (true)
    {
        ProtocolEvent event;
        std::cout << "Received: " << getTotalReceived() << std::endl;
        std::cout << "Sent: " << getTotalSent() << std::endl;
        receiveEvent(serverSocket, event);
        std::cout << "Receiver:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "recNEW FILE\n";
                    receiveNewFileProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "recNEW_FILE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case DELETE:
                try
                {
                    std::cout << "recDELETE\n";
                    receiveDeletionPathProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "recDELETE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case MOVE:
                try
                {
                    std::cout << "recMOVE\n";
                    receiveMovePathsProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "recMOVE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    std::cout << "recNEW DIRECTORY\n";
                    receiveNewDircetoryProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "recNEW_DIRECTORY error:\n" << e.what() << std::endl
                              << "recTerminating client\n";
                    return;
                }
                break;
            case COPY:
                try
                {
                    std::cout << "recCOPY\n";
                    receiveCopyPathsProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "recCOPY error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            default:
                throw DropboxException("recProtocol error");
                break;
        }
    }
}

