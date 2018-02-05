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
    typedef u_int64_t IntType;

public:
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

    std::string correctPath(std::string path);
    boost::filesystem::path correctPath(boost::filesystem::path path);

    void createDirectory(std::string directoryPath);

    void deleteFiles(std::string filePath);

    void moveFile(std::string source, std::string destination);

    //void sendEvent(ProtocolEvent event);
    void sendEvent(TcpSocket &sock, ProtocolEvent event);

    //void receiveEvent(ProtocolEvent &event);
    void receiveEvent(TcpSocket &sock, ProtocolEvent &event);

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

    void deleteFile(std::string fileName);

    void copyFile(std::string source, std::string destination);

    const std::string folderPath;

    size_t getFileSize(const std::string fileName);

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
    int getTotalReceived() const;

    void sendNewFileProcedure(TcpSocket sock, std::string filePath, std::mutex &clientMutex);

protected:
    std::string generateRelativePath(std::string path);
    void changeModificationTime(std::string path, Dropbox::IntType time);
    IntType getModificationTime(string path);
    int getTotalSent();
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
    std::__cxx11::string source;
    std::__cxx11::string destination;
    TcpSocket sender;
};
#endif //DROPBOX_DROPBOX_H
