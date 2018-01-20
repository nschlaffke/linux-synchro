//
// Created by ns on 10.01.18.
//
/// pole serverSocket to referencja na samego siebie; istnieje tylko po to, aby w funkcjach typy receiveFile nie pisać *this
#include <iostream>
#include <thread>
#include "DropboxClient.h"

// TODO jeżeli sender umiera, receiver też musi umrzec -> conditional variable
DropboxClient::DropboxClient(const std::string &ip, unsigned short port, const std::string folderPath)
        : Dropbox(folderPath), TcpSocket(ip, port), serverSocket(*this), folderPath(folderPath) {}

int DropboxClient::run() {
    if (!isConnected()) {
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
void DropboxClient::newClientProcedure() {
    sendEvent(*this, NEW_CLIENT);
}

/**
 * Funkcja (wątek) zczytujący wiadomości z kolejki messageQueue i wysyłająca je do serwera
 * Wiadomości na kolejce umieszcza ClientEventReporter
 */
void DropboxClient::sender() {
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        while (ClientEventReporter::messageQueue.empty());

        EventMessage eventMessage = ClientEventReporter::messageQueue.dequeue();

        ProtocolEvent event = eventMessage.event;
        std::string path = eventMessage.destination;

        std::cout << "Sender:\n";
        switch (event) {
            case NEW_FILE:
                try {
                    std::cout << "NEW FILE: " << path << std::endl;
                    sendNewFileProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "NEW_FILE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case DELETE:
                try {
                    std::cout << "DELETE: " << path << std::endl;
                    sendDeletionPathProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "DELETE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case MOVE:
                try {
                    std::string source = eventMessage.source;
                    std::cout << "MOVE FROM: " << source << " TO: " << path << std::endl;

                    sendMovePathsProcedure(serverSocket, source, path, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "MOVE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try {
                    sendNewDirectoryProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "NEW_DIRECTORY error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case COPY:
                try {
                    std::string source = eventMessage.source;
                    std::cout << "COPY FROM: " << source << " TO: " << path << std::endl;

                    sendCopyPathsProcedure(serverSocket, source, path, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "COPY error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
        }
    }
#pragma clang diagnostic pop
}

/**
 * Funkcja (wątek) odbierający wiadomości od serwera
 */
void DropboxClient::receiver() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        ProtocolEvent event;
        std::cout << "Received: " << getTotalReceived() << std::endl;
        std::cout << "Sent: " << getTotalSent() << std::endl;
        receiveEvent(serverSocket, event);
        std::cout << "Receiver:\n";
        switch (event) {
            case NEW_FILE:
                try {
                    std::cout << "NEW FILE\n";
                    receiveNewFileProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "NEW_FILE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case DELETE:
                try {
                    std::cout << "DELETE\n";
                    receiveDeletionPathProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "DELETE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case MOVE:
                try {
                    std::cout << "MOVE\n";
                    receiveMovePathsProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "MOVE error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try {
                    std::cout << "NEW DIRECTORY\n";
                    receiveNewDircetoryProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "NEW_DIRECTORY error:\n" << e.what() << std::endl
                              << "Terminating client\n";
                    return;
                }
                break;
            case COPY:
                try {
                    std::cout << "COPY\n";
                    receiveCopyPathsProcedure(serverSocket, serverMutex);
                }
                catch (std::exception &e) {
                    std::cout << "COPY error: " << e.what()
                              << "Terminating client\n";
                    return;
                }
                break;
            default:
                throw DropboxException("Protocol error");
                break;
        }
    }
#pragma clang diagnostic pop
}

