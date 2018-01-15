//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOX_H
#define DROPBOX_DROPBOX_H

#include <string>
#include <stdexcept>
#include "TcpSocket.h"
#include <boost/filesystem/path.hpp>
#include <mutex>
#include <queue>

class Dropbox
{
public:

    Dropbox(const std::string &folderPath);

    class DropboxException : public std::runtime_error
    {
    public:
        DropboxException(const std::string &s) : runtime_error(s)
        {}
    };

    virtual int run() = 0;

protected:

    void createDirectory(std::string directoryPath);

    void deleteFiles(std::string filePath);

    void moveFile(std::string source, std::string destination);

    enum Event
    {
        HEARTBEAT,
        NEW_CLIENT,
        NEW_FILE,
        DELETE_FILE,
        MOVE_FILE,
        NEW_DIRECTORY,
        COPY_FILE
    };

    typedef uint32_t IntType;

    //void sendEvent(Event event);
    void sendEvent(TcpSocket &sock, Event event);

    //void recieveEvent(Event &event);
    void recieveEvent(TcpSocket &sock, Event &event);

    //void sendInt(IntType data);
    void sendInt(TcpSocket &sock, IntType data);

    void receiveInt(TcpSocket &sock, IntType &data);
    //void receiveInt(IntType &data);

    boost::filesystem::path createPath(std::string path);

    void sendString(TcpSocket &sock, std::string text);
    //void sendString(std::string text);

    //void receiveString(std::string &text);
    void receiveString(TcpSocket &sock, std::string &text);


    std::string generateAbsolutPath(std::string pathToFile);

    void sendFile(TcpSocket &sock, std::string fileName);
    //void sendFile(const std::string fileName);

    void receiveFile(TcpSocket &sock, std::string fileName, size_t fileSize);
    //void receiveFile(const std::string fileName, size_t fileSize);

    const std::string folderPath;

    size_t getFileSize(const std::string fileName);


    void sendNewDirectoryProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex);

    std::string receiveNewFileProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string receiveNewDircetoryProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    struct EventMessage
    {
        Event event;
        std::string source;
        std::string destination;
        TcpSocket sender;
    };
public:
    int getTotalReceived() const;
    void sendNewFileProcedure(TcpSocket sock, std::string filePath, std::mutex &clientMutex);

protected:
    std::string generateRelativePath(std::string path);

    int getTotalSent();
private:
    int totalSent;
    int totalReceived;
    const int maxStringSize;

};


#endif //DROPBOX_DROPBOX_H
