//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOX_H
#define DROPBOX_DROPBOX_H

#include <string>
#include <stdexcept>
#include "TcpSocket.h"
#include <boost/filesystem/path.hpp>

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

    Dropbox(const std::string &ip, const unsigned short port, const std::string &folderPath);

protected:

    void createDirectory(std::string directoryPath);

    void deleteFiles(std::string filePath);

    void moveFile(std::string source, std::string destination);

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

    typedef uint32_t IntType;

    void sendEvent(Event event);
    void sendEvent(TcpSocket &sock, Event event);

    void recieveEvent(Event &event);
    void recieveEvent(TcpSocket &sock, Event &event);

    void sendInt(IntType data);
    void sendInt(TcpSocket &sock, IntType data);
    
    void receiveInt(TcpSocket &sock, IntType &data);
    void receiveInt(IntType &data);

    boost::filesystem::path createPath(std::string path);

    void sendString(TcpSocket &sock, std::string text);
    void sendString(std::string text);

    void receiveString(std::string &text);
    void receiveString(TcpSocket &sock, std::string &text);

    const std::string folderPath;

private:
    const int maxStringSize;

};


#endif //DROPBOX_DROPBOX_H
