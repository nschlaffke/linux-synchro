#include "DropboxClient.h"
#include "DropboxServer.h"
#include <thread>
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
        totalSent += sent;
        bytes -= sent;
        dataPointer += sent;
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
        totalReceived += recieved;
        dataPointer += recieved;

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
        totalSent += sent;
        bytes -= sent;
        data += sent;
        dataPointer += sent;
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
        totalReceived += recieved;
        dataPointer += recieved;

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
    boost::filesystem::path dir = directoryPath;
    if (!boost::filesystem::create_directory(dir))
    {
        std::string error("Failed to create folder: ");
        error += directoryPath;
        throw DropboxException(error);
    }
}

Dropbox::Dropbox(const std::string &folderPath) :
        folderPath(folderPath),
        maxStringSize(500), totalSent(0), totalReceived(0)
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
    totalSent += sock.sendData(pointer, maxStringSize);
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
    totalReceived += sock.recieveData(buffer, maxStringSize);
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
    std::ifstream file(fileName, std::ios::binary | std::ios::in);
    if (!file.good())
    {
        throw DropboxException("Couldn't open file");
    }
    size_t fileSize = getFileSize(fileName);
    char buffer[CHUNK_SIZE];
    int rest = fileSize % CHUNK_SIZE;
    int sent = 0;
    for (int i = 1; fileSize >= CHUNK_SIZE * i; i++)
    {
        file.read(buffer, CHUNK_SIZE);
        sent += sock.sendData(buffer, CHUNK_SIZE);
        file.seekg(CHUNK_SIZE, std::ios::cur);
    }
    if(rest > 0)
    {
        sent += sock.sendData(buffer, rest);
    }
    totalSent += fileSize;
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
    std::ofstream file(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
    if(!file.good())
    {
        throw DropboxException("Error creating file");
    }
    char buffer[CHUNK_SIZE];
    int rest = fileSize % CHUNK_SIZE;
    for (int i = 1; CHUNK_SIZE * i <= fileSize; i++)
    {
        size_t size = sock.recieveData(buffer, CHUNK_SIZE);
        file.write(buffer, size);
        file.seekp(size, std::ios::cur);
    }
    size_t size = sock.recieveData(buffer, rest);
    totalReceived += fileSize;
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

std::string Dropbox::generateRelativePath(std::string path)
{
    return path.erase(0, folderPath.length());
}

int Dropbox::getTotalSent()
{
    return totalSent;
}

int Dropbox::getTotalReceived() const
{
    return totalReceived;
}


/**
 * 1. wysyła event NEW_FILE
 * 2. wysyła nazwę pliku
 * 3. wysyła rozmiar pliku
 * 4. wysyła plik
 * @param sock
 * @param filePath - ścieżka BEZWZGLĘDNA do pliku
 */
void Dropbox::sendNewFileProcedure(TcpSocket sock, std::string filePath, std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, NEW_FILE);
    std::__cxx11::string relativePath = generateRelativePath(filePath);
    std::cout << "SEND FILE: " << relativePath << std::endl;
    sendString(sock, relativePath);
    IntType fileSize = getFileSize(filePath);
    sendInt(sock, fileSize);
    sendFile(sock, filePath);
    std::cout << "R: " << getTotalReceived() << std::endl;
    std::cout << "S: " << getTotalSent() << std::endl;
    clientMutex.unlock();
}

/**
 * 1. wysyła event NEW_DIRECTORY
 * 2. wysyła nazwę folderu
 * @param sock
 * @param directoryPath ścieżka BEZWZGLĘDNA do folderu
 */
void Dropbox::sendNewDirectoryProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, NEW_DIRECTORY);
    std::__cxx11::string relativePath = generateRelativePath(directoryPath);
    std::cout << "SEND DIRECTORY: " << relativePath << std::endl;
    sendString(sock, relativePath);
    std::cout << "R: " << getTotalReceived() << std::endl;
    std::cout << "S: " << getTotalSent() << std::endl;
    clientMutex.unlock();
}

/**
 * 1. odbiera nazwę pliku
 * 2. odbiera rozmiar pliku
 * 3. odbiera plik
 */
std::string Dropbox::receiveNewFileProcedure(TcpSocket &serverSocket, std::mutex &clientMutex)
{

    clientMutex.lock();
    std::string fileName;
    IntType size;
    receiveString(serverSocket, fileName);
    receiveInt(serverSocket, size);
    receiveFile(serverSocket, generateAbsolutPath(fileName), size);
    std::cout << "R: " << getTotalReceived() << std::endl;
    std::cout << "S: " << getTotalSent() << std::endl;
    clientMutex.unlock();
    return fileName;
}

/**
 * 1. odbiera od serwera ścieżkę do folderu
 * 2. tworzy folder
 */
std::string Dropbox::receiveNewDircetoryProcedure(TcpSocket &serverSocket, std::mutex &clientMutex)
{
    clientMutex.lock();
    std::string folder;
    receiveString(serverSocket, folder);
    clientMutex.unlock();
    createDirectory(generateAbsolutPath(folder));
    std::cout << "R: " << getTotalReceived() << std::endl;
    std::cout << "S: " << getTotalSent() << std::endl;
    return folder;
}
