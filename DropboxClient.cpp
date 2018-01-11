//
// Created by ns on 10.01.18.
//

#include <iostream>
#include "DropboxClient.h"

void DropboxClient::sendFileProcedure(std::string filePath)
{

}

void DropboxClient::recieveFileProcedure(std::string filePath)
{

}


DropboxClient::DropboxClient(const std::string &ip, const unsigned short port, const std::string folderPath)
        : Dropbox(ip, port, folderPath)
{}

int DropboxClient::run()
{
   newClientProcedure();
}

void DropboxClient::newClientProcedure()
{
    sendEvent(Dropbox::NEW_CLIENT);
    Dropbox::Event event;
    recieveEvent(event);
    if(event == Dropbox::NO_FILES)
    {
        return;
    }
    else if(event == Dropbox::NEW_FILES)
    {
        // odbieramy pliki
    }
    else
    {
        throw DropboxException("newClientProcedure protocol error");
    }
}


