#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include "Dropbox.h"
//
// Created by ns on 10.01.18.

void Dropbox::sendEvent(Event data)
{
    Event dataToSend = static_cast<Event>(htonl(data));
    int bytes = sizeof(dataToSend);
    char *dataPointer = reinterpret_cast<char *>(&dataToSend);
    int sent;
    do
    {
        sent = sendData(dataPointer, bytes);
        bytes -= sent;
    } while (bytes > 0);
}

void Dropbox::recieveEvent(Event &data)
{
    Event dataToRecieve;
    int bytes = sizeof(dataToRecieve);
    char *dataPointer = reinterpret_cast<char *>(&dataToRecieve);
    int recieved = 0;
    do
    {
        recieved += recieveData(dataPointer, bytes);

    } while (bytes - recieved > 0);
    data = static_cast<Event>(ntohl(dataToRecieve));
}

void Dropbox::sendInt(IntType data)
{
    IntType dataToSend = htonl(data);
    int bytes = sizeof(dataToSend);
    char *dataPointer = reinterpret_cast<char *>(&dataToSend);
    int sent;
    do
    {
        sent = sendData(dataPointer, bytes);
        bytes -= sent;
        data += sent;
    } while (bytes > 0);
}

void Dropbox::recieveInt(IntType &data)
{
    IntType dataToRecieve;
    int bytes = sizeof(dataToRecieve);
    char *dataPointer = reinterpret_cast<char *>(&dataToRecieve);
    int recieved = 0;
    do
    {
        recieved += recieveData(dataPointer, bytes);

    } while (bytes - recieved > 0);
    data = ntohl(dataToRecieve);
}

void Dropbox::createDirectory(std::string directoryPath)
{
    boost::filesystem::path dir = createPath(directoryPath);
    if (!boost::filesystem::create_directories(dir))
    {
        std::string error("Failed to create folder: ");
        error += directoryPath;
        throw DropboxException(error);
    }
}

Dropbox::Dropbox(const std::string &ip, const unsigned short port, const std::string &folderPath) :
        TcpSocket(ip, port),
        folderPath(folderPath)
{}

void Dropbox::deleteFiles(std::string filePath)
{
    boost::filesystem::path dir = createPath(filePath);
    if (!boost::filesystem::remove_all(dir))
    {
        std::string error("Failed to delete: ");
        error += filePath;
        throw DropboxException(error);
    }
}

boost::filesystem::path Dropbox::createPath(std::string path)
{
    return boost::filesystem::path(folderPath + "/" + path);
}

void Dropbox::moveFile(std::string source, std::string destination)
{
    boost::filesystem::path sourcePath = createPath(source);
    boost::filesystem::path destinationPatth = createPath(destination);
    try
    {
        boost::filesystem::rename(sourcePath, destinationPatth);
    }
    catch(boost::filesystem::filesystem_error error)
    {
       throw DropboxException(error.what());
    }
}

