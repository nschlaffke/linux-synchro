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

void DropboxClient::deleteFileProcedure(std::string filePath)
{

}

void DropboxClient::createDirectoryProcedure(std::string directoryPath)
{

}

DropboxClient::DropboxClient(const std::string &ip, const unsigned short port) : TcpSocket(ip, port)
{}

int DropboxClient::run()
{
   newClientProcedure();
}

void DropboxClient::moveFileProcedure(std::string source, std::string destination)
{

}

void DropboxClient::newClientProcedure()
{
    sendData(Dropbox::NEW_CLIENT);
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


