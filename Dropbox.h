//
// Created by ns on 10.01.18.
//

#ifndef DROPBOX_DROPBOX_H
#define DROPBOX_DROPBOX_H

#include "TcpSocket.h"
#include "ProtocolEvent.h"

#include <string>
#include <stdexcept>
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

    typedef uint32_t IntType;

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

    void deleteFile(std:: string fileName);

    const std::string folderPath;

    size_t getFileSize(const std::string fileName);

    void sendNewDirectoryProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex);

    void sendDeletionPathProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex);

    void sendMovePathsProcedure(TcpSocket sock, std::string directoryPath, std::string destinationPath, std::mutex &clientMutex);

    std::string receiveDeletionPathProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string* receiveMovePathsProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string receiveNewFileProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);

    std::string receiveNewDircetoryProcedure(TcpSocket &serverSocket, std::mutex &clientMutex);


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
