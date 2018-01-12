#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "Dropbox.h"
#include "TcpSocket.h"
//
// Created by ns on 10.01.18.

void Dropbox::sendEvent(TcpSocket &sock, Dropbox::Event event)
{
    Event dataToSend = static_cast<Event>(htonl(event));
    int bytes = sizeof(dataToSend);
    char *dataPointer = reinterpret_cast<char *>(&dataToSend);
    int sent;
    do
    {
        sent = sock.sendData(dataPointer, bytes);
        bytes -= sent;
    } while (bytes > 0);
}
/*
void Dropbox::sendEvent(Event data)
{
    sendEvent(*this, data);
}
*/
void Dropbox::recieveEvent(TcpSocket &sock, Dropbox::Event &data)
{
    Event dataToRecieve;
    int bytes = sizeof(dataToRecieve);
    char *dataPointer = reinterpret_cast<char *>(&dataToRecieve);
    int recieved = 0;
    do
    {
        recieved += sock.recieveData(dataPointer, bytes);

    } while (bytes - recieved > 0);
    data = static_cast<Event>(ntohl(dataToRecieve));
}
/*
void Dropbox::recieveEvent(Event &data)
{
   recieveEvent(*this, data) ;
}
*/
/*
void Dropbox::sendInt(IntType data)
{
   sendInt(*this, data);
}
 */
void Dropbox::sendInt(TcpSocket &sock, Dropbox::IntType data)
{
    IntType dataToSend = htonl(data);
    int bytes = sizeof(dataToSend);
    char *dataPointer = reinterpret_cast<char *>(&dataToSend);
    int sent;
    do
    {
        sent = sock.sendData(dataPointer, bytes);
        bytes -= sent;
        data += sent;
    } while (bytes > 0);
}

void Dropbox::receiveInt(TcpSocket &sock, Dropbox::IntType &data)
{
    IntType dataToRecieve;
    int bytes = sizeof(dataToRecieve);
    char *dataPointer = reinterpret_cast<char *>(&dataToRecieve);
    int recieved = 0;
    do
    {
        recieved += sock.recieveData(dataPointer, bytes);

    } while (bytes - recieved > 0);
    data = ntohl(dataToRecieve);
}
/*
void Dropbox::receiveInt(IntType &data)
{
    receiveInt(*this, data);
}
*/
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

Dropbox::Dropbox(std::string ip, int port, const std::string &folderPath) :
        folderPath(folderPath),
        maxStringSize(500) {}

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
/*
void Dropbox::sendString(std::string text)
{
    sendString(*this, text);
}
 */
void Dropbox::sendString(TcpSocket &sock, std::string text)
{
    int textLen = text.length();
    if(textLen >= maxStringSize)
    {
        throw DropboxException("Dropbox::sendString error: too long string");
    }
    const char* pointer = text.c_str();
    sock.sendData(pointer, textLen);
}
/*
void Dropbox::receiveString(std::string &text)
{
    receiveString(*this, text);
}
 */
void Dropbox::receiveString(TcpSocket &sock, std::string &text)
{
    char buffer[maxStringSize];
    sock.recieveData(buffer, maxStringSize);
    std::string tmp(buffer);
    text = tmp;
}
/*
void Dropbox::sendFile(const std::string fileName)
{
    sendFile(*this, fileName);
}
 */
void Dropbox::sendFile(TcpSocket &sock, const std::string fileName)
{
    std::ifstream file(generateAbsolutPath(fileName), std::ios::binary | std::ios::in);
    if (!file.good())
    {
        throw DropboxException("Couldn't open file");
    }
    size_t fileSize = getFileSize(fileName);
    char buffer[CHUNK_SIZE];
    for (int i = 0; CHUNK_SIZE * i <= fileSize; i++) // send file in chunks
    {
        file.read(buffer, CHUNK_SIZE);
        if (file.eof())
        {
            int size = std::min(int(fileSize) - CHUNK_SIZE * i, CHUNK_SIZE);
            sock.sendData(buffer, size);
            break;
        }
        else
        {
            sock.sendData(buffer, CHUNK_SIZE);
        }
        file.seekg(CHUNK_SIZE, std::ios::cur);
    }
    file.close();
}
/*
void Dropbox::receiveFile(const std::string fileName, size_t fileSize)
{
    receiveFile(*this, fileName, fileSize);
}
 */
void Dropbox::receiveFile(TcpSocket &sock, std::string fileName, size_t fileSize) // fileName jest sciezka do pliku
{
    std::ofstream file(generateAbsolutPath(fileName), std::ios::binary | std::ios::out | std::ios::trunc);
    char buffer[CHUNK_SIZE];

    for (int i = 0; CHUNK_SIZE * i <= fileSize; i++)
    {
        size_t size = sock.recieveData(buffer, CHUNK_SIZE);
        file.write(buffer, size);
        file.seekp(size, std::ios::cur);
        if (size < CHUNK_SIZE)
        {
            break;
        }
    }
    file.close();
}

size_t Dropbox::getFileSize(const std::string fileName)
{
    std::ifstream in(fileName.c_str(), std::ios::binary | std::ios::ate);
    size_t size = static_cast<size_t>(in.tellg());
    in.close();
    return size;
}

std::string Dropbox::generateAbsolutPath(std::string pathToFile)
{
    return folderPath + pathToFile;
}





