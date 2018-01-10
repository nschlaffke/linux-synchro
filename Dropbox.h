//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOX_H
#define DROPBOX_DROPBOX_H

#include <string>
#include <stdexcept>
#include "TcpSocket.h"

class Dropbox : public virtual TcpSocket
{
public:
    class DropboxException : public std::runtime_error
    {
    public:
        DropboxException(const std::string &s) : runtime_error(s)
        {}
    };
    virtual int run() = 0;

protected:
    enum Event
    {
        HEARTBEAT,
        NEW_CLIENT,
        NEW_FILES,
        NEW_FILE,
        DELETE_FILE,
        MOVE_FILE,
        NEW_DIRECTORY,
        NO_FILES
    };

    virtual void sendFileProcedure(std::string filePath) = 0;

    virtual void recieveFileProcedure(std::string filePath) = 0;

    virtual void moveFileProcedure(std::string source, std::string destination) = 0;

    virtual void deleteFileProcedure(std::string filePath) = 0;

    virtual void createDirectoryProcedure(std::string directoryPath) = 0;

    virtual void sendEvent(Event event);

    virtual void recieveEvent(Event &event);

};


#endif //DROPBOX_DROPBOX_H
