//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOX_H
#define DROPBOX_DROPBOX_H


#include "TcpSocket.h"
#include "InotifyInterface/Notifier.h"
#include <string>
#include <stdexcept>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <queue>
#include <utime.h>

class Dropbox
{
protected:

public:

    typedef u_int64_t IntType;
    enum ProtocolEvent : IntType
    {
        NEW_CLIENT,
        NEW_FILE,
        DELETE,
        MOVE,
        NEW_DIRECTORY,
        COPY,
        IS_VALID,
        END_OF_SYNC,
        TERMINATE
    };
    struct EventMessage;

    Dropbox(const std::string &folderPath);

    class DropboxException : public std::runtime_error
    {
    public:
        DropboxException(const std::string &s) : runtime_error(s)
        {}
    };

    virtual void run() = 0;

protected:
    void createDirectory(std::string directoryPath);

    void moveFile(std::string source, std::string destination);

    void sendEvent(TcpSocket &sock, ProtocolEvent event);

    void receiveEvent(TcpSocket &sock, ProtocolEvent &event);

    void sendInt(TcpSocket &sock, IntType data);

    void receiveInt(TcpSocket &sock, IntType &data);

    void sendString(TcpSocket &sock, std::string text);

    void receiveString(TcpSocket &sock, std::string &text);

    std::string generateAbsolutPath(std::string pathToFile);

    void sendFile(TcpSocket &sock, std::string fileName, std::size_t fileSize);

    void receiveFile(TcpSocket &sock, std::string fileName, size_t fileSize);

    void deleteFile(std::string fileName);

    void copyFile(std::string source, std::string destination);

    const std::string folderPath;

    size_t getFileSize(std::string fileName);

    void sendNewDirectoryProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex);

    void sendDeletionPathProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex);

    void sendMovePathsProcedure(TcpSocket sock, std::string directoryPath, std::string destinationPath,
                                std::mutex &clientMutex);

    void sendCopyPathsProcedure(TcpSocket sock, std::string directoryPath, std::string destinationPath,
                                std::mutex &clientMutex);

    std::string receiveDeletionPathProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string *receiveMovePathsProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string *receiveCopyPathsProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string receiveNewFileProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string receiveNewDircetoryProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);


public:
    static std::string correctPath(std::string path);
    static boost::filesystem::path correctPath(boost::filesystem::path path);

    size_t sendNewFileProcedure(TcpSocket sock, std::string filePath, std::mutex &clientMutex);

protected:
    std::string generateRelativePath(std::string path);
    void changeModificationTime(std::string path, Dropbox::IntType time);
    IntType getModificationTime(string path);

    bool askIfValid(TcpSocket &socket, std::string path);
    bool answerIfValid(TcpSocket &socket);
private:
    int totalSent;
    int totalReceived;
    const int maxStringSize;

};

struct Dropbox::EventMessage
{
    Dropbox::ProtocolEvent event;
    std::string source;
    std::string destination;
};
#endif //DROPBOX_DROPBOX_H
