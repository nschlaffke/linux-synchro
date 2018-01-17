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
{}

int DropboxClient::run()
{
    if (!isConnected())
    {
        throw DropboxException("Error: Client not connected\n");
    }
    newClientProcedure();

    std::thread t(&ClientEventReporter::handleNotifications, ClientEventReporter(folderPath));
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
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true)
    {
        while (ClientEventReporter::messageQueue.empty()); // TODO wyeliminować aktywne czekanie, std::conditional_variable - inotify niech budzi sendera

        EventMessage eventMessage = ClientEventReporter::messageQueue.dequeue();

        ProtocolEvent event = eventMessage.event;
        std::string path = eventMessage.source;

        std::cout << "Sender:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "NEW FILE: " << path << std::endl;
                    sendNewFileProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "NEW_FILE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case DELETE_FILE:
                // TODO sender DELETE_FILE
                break;
            case MOVE_FILE:
                // TODO sender MOVE_FILE
                break;
            case NEW_DIRECTORY:
                try
                {
                    sendNewDirectoryProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "NEW_DIRECTORY error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case COPY_FILE:
                // TODO sender COPY_FILE
                break;
        }
    }
#pragma clang diagnostic pop
}

/**
 * Funkcja (wątek) odbierający wiadomości od serwera
 */
void DropboxClient::receiver()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true)
    {
        ProtocolEvent event;
        std::cout << "R: " << getTotalReceived() << std::endl;
        std::cout << "S: " << getTotalSent() << std::endl;
        receiveEvent(serverSocket, event);
        std::cout << "Receiver:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "NEW FILE\n";
                    receiveNewFileProcedure(serverSocket, serverMutex);
                    std::cout << "FILE receiveD\n";
                }
                catch (std::exception &e)
                {
                    std::cout << "NEW_DIRECTORY error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case DELETE_FILE:
                // TODO receiver DELETE_FILE
                break;
            case MOVE_FILE:
                // TODO receiver MOVE_FILE
                break;
            case NEW_DIRECTORY:
                try
                {
                    std::cout << "NEW DIRECTORY\n";
                    receiveNewDircetoryProcedure(serverSocket, serverMutex);
                    std::cout << "NEW DIRECTORY CREATED\n";
                }
                catch(std::exception &e)
                {
                    std::cout << "NEW_DIRECTORY error:\n" << e.what() << std::endl
                              << "Terminating client\n";
                    return;
                }
                break;
            case COPY_FILE:
                // TODO receiver COPY_FILE
                break;
            default:
                throw DropboxException("Protocol error");
                break;
        }
    }
#pragma clang diagnostic pop
}

