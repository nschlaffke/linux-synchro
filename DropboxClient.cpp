//
// Created by ns on 10.01.18.
//
/// pole serverSocket to referencja na samego siebie; istnieje tylko po to, aby w funkcjach typy receiveFile nie pisać *this
#include <iostream>
#include <thread>
#include "DropboxClient.h"

// TODO jeżeli sender umiera, receiver też musi umrzec -> conditional variable
DropboxClient::DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath)
        : Dropbox(folderPath), TcpSocket(ip, port), serverSocket(*this)
{}

int DropboxClient::run()
{
    if (!isConnected())
    {
        throw DropboxException("Error: Client not connected\n");
    }
    newClientProcedure();
    // TODO usunac nastepujace linie, tylko do testow !!!!
    ///
    //EventMessage mes = {Event::NEW_FILE, "/home/ns/Documents/Studia/semestr5/SK2/Dropbox/test/client2_folder/f_from_client"};
    //EventMessage mes2 = {Event::NEW_DIRECTORY, "/home/ns/Documents/Studia/semestr5/SK2/Dropbox/test/client_folder/d_from_client"};
    //messageQueue.push(mes2);
    //messageQueue.push(mes);
    ///
    std::thread s(&DropboxClient::sender, this);
    std::thread r(&DropboxClient::receiver, this);
    s.join();
    r.join();
}

/**
 * void DropboxClient::newClientProcedure()
 * opis:
 * 1. Klient wysyła do serwera event NEW_CLIENT
 */
void DropboxClient::newClientProcedure()
{
    sendEvent(*this, Dropbox::NEW_CLIENT);
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
        while (messageQueue.empty()); // TODO wyeliminować aktywne czekanie, std::conditional_variable - inotify niech budzi sendera
        messageQueueMutex.lock();

        EventMessage eventMessage = messageQueue.front();
        messageQueue.pop();

        messageQueueMutex.unlock();

        Event event = eventMessage.event;
        std::string path = eventMessage.path;
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
        Event event;
        std::cout << "R: " << getTotalReceived() << std::endl;
        std::cout << "S: " << getTotalSent() << std::endl;
        recieveEvent(serverSocket, event);
        std::cout << "Receiver:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "NEW FILE\n";
                    receiveNewFileProcedure(serverSocket, serverMutex);
                    std::cout << "FILE RECIEVED\n";
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

