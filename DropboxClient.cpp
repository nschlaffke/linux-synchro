//
// Created by ns on 10.01.18.
//

#include <iostream>
#include "DropboxClient.h"


DropboxClient::DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath)
        : Dropbox(ip, port, folderPath)
{}

int DropboxClient::run()
{
    newClientProcedure();

}
/**
 * void DropboxClient::newClientProcedure()
 * opis:
 * 1. Klient wysyła do serwera event NEW_CLIENT
 */
void DropboxClient::newClientProcedure()
{
    const std::string errorDescription("newClientProcedure protocol error");
    sendEvent(Dropbox::NEW_CLIENT);
    Dropbox::Event tmp;
    recieveEvent(tmp);
}
