//
// Created by ns on 10.01.18.
//

#include <iostream>
#include <thread>
#include "DropboxClient.h"


DropboxClient::DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath)
        : Dropbox(ip, port, folderPath), TcpSocket(ip, port)
{}

int DropboxClient::run()
{
    if(!isConnected())
    {
        throw DropboxException("Error: Client not connected\n");
    }
    newClientProcedure();
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

void DropboxClient::sender()
{
    // TODO PIOTR tutaj się będą działy cuda INOTIFYOWE
    // fajnie by było jak byś drobny szablonik machnął, łatwiej mi będzię się wgryźć ;)
}

void DropboxClient::receiver()
{
    while (true)
    {
        Event event;
        recieveEvent(*this, event);
        switch (event)
        {
            case HEARTBEAT:
                // TODO
                break;
            case NEW_FILES:
                break;
            case NEW_FILE:
                receiveNewFileProcedure();
                break;
            case DELETE_FILE:
                break;
            case MOVE_FILE:
                break;
            case NEW_DIRECTORY:
                break;
            case NO_FILES:
                break;
        }
    }
}
/**
 * Klient:
 * 1. odbiera nazwę pliku
 * 2. odbiera rozmiar pliku
 * 3. odbiera plik
 */
void DropboxClient::receiveNewFileProcedure()
{
    std::string fileName;
    IntType size;
    receiveString(*this, fileName);
    receiveInt(*this, size);
    receiveFile(*this, generateAbsolutPath(fileName), size);
}
