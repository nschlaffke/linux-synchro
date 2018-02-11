#include "Dropbox.h"
#include "ClientEventReporter.h"
//
// Created by ns on 10.01.18.

SafeSet<std::string> ClientEventReporter::ignoredPaths;
SafeSet<std::string> ClientEventReporter::permanentlyIgnored;
SafeSet<ClientEventReporter::FileInfo> ClientEventReporter::allFilesInfo;

void Dropbox::sendEvent(TcpSocket &sock, ProtocolEvent event)
{
    ProtocolEvent dataToSend = static_cast<ProtocolEvent>(htonl(event));
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
void Dropbox::sendEvent(ProtocolEvent data)
{
    sendEvent(*this, data);
}
*/
void Dropbox::receiveEvent(TcpSocket &sock, ProtocolEvent &data)
{
    ProtocolEvent dataToreceive;
    int bytes = sizeof(dataToreceive);
    char *dataPointer = reinterpret_cast<char *>(&dataToreceive);
    int received = 0;
    do
    {
        received += sock.receiveData(dataPointer, bytes);
        totalReceived += received;
        dataPointer += received;
        //received += sock.receiveData(dataPointer, bytes);
        //totalReceived += received;

    } while (bytes - received > 0);
    data = static_cast<ProtocolEvent>(ntohl(dataToreceive));
}
/*
void Dropbox::receiveEvent(ProtocolEvent &data)
{
   receiveEvent(*this, data) ;
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
    IntType dataToreceive;
    int bytes = sizeof(dataToreceive);
    char *dataPointer = reinterpret_cast<char *>(&dataToreceive);
    int received = 0;
    do
    {
        received += sock.receiveData(dataPointer, bytes);
        totalReceived += received;
        dataPointer += received;

    } while (bytes - received > 0);
    data = ntohl(dataToreceive);

}

std::string Dropbox::correctPath(std::string path)
{
    char forbiddenSign = 1;
    unsigned int pos;

    do
    {
        pos = static_cast<int>(path.find(forbiddenSign));
        forbiddenSign++;
    } while(std::string::npos == pos && forbiddenSign < 32);

    if (std::string::npos == pos)
    {
        return path;
    } else
    {
        return path.substr(0, static_cast<unsigned long>(pos - 1));
    }
}

boost::filesystem::path Dropbox::correctPath(boost::filesystem::path path)
{
    std::string path2 = path.string();
    char forbiddenSign = 1;
    unsigned int pos;

    do
    {
        pos = static_cast<int>(path2.find(forbiddenSign));
        pos = static_cast<int>(path2.find(forbiddenSign));
        forbiddenSign++;
    } while(std::string::npos == pos && forbiddenSign < 32);

    if (std::string::npos == pos)
    {
        return path;
    } else
    {
        return boost::filesystem::path(path2.substr(0, static_cast<unsigned long>(pos - 1)));
    }
}

void Dropbox::createDirectory(std::string directoryPath)
{
    if (boost::filesystem::exists(boost::filesystem::path(directoryPath)))
    {
        return;
    }

    boost::filesystem::path dir = directoryPath;
    if (!boost::filesystem::create_directory(dir))
    {
        std::string error("Failed to create folder: ");
        error += directoryPath;
        throw DropboxException(error);
    }

}

Dropbox::Dropbox(const std::string &folderPath) :
        folderPath(folderPath), totalSent(0), totalReceived(0), maxStringSize(500)
{}

void Dropbox::moveFile(std::string source, std::string destination)
{
    boost::filesystem::path sourcePath = boost::filesystem::path(source);
    boost::filesystem::path destinationPath = boost::filesystem::path(destination);

    if (rename(source.c_str(), destination.c_str()))
    {
        throw DropboxException("Move operation has failed");
    }

    for (auto it = ClientEventReporter::allFilesInfo.begin(); it != ClientEventReporter::allFilesInfo.end(); ++it)
    {
        if (it->path == sourcePath)
        {
            ClientEventReporter::allFilesInfo.erase(it);
        }
    }

    ClientEventReporter::FileInfo fileInfo;
    fileInfo.path = destinationPath;
    fileInfo.isOpen = false;

    ClientEventReporter::allFilesInfo.insert(fileInfo);
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
    if (textLen >= maxStringSize)
    {
        throw DropboxException("Dropbox::sendString error: too long string");
    }
    const char *pointer = text.c_str();
    sock.sendData(pointer, maxStringSize);
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
    sock.receiveData(buffer, maxStringSize);
    std::string tmp(buffer);
    text = tmp;
}

/*
void Dropbox::sendFile(const std::string fileName)
{
    sendFile(*this, fileName);
}
 */
void Dropbox::sendFile(TcpSocket &sock, const std::string fileName, std::size_t fileSize)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::in);
    if (!file.good())
    {
        throw DropboxException("Couldn't open file");
    }
    //size_t fileSize = getFileSize(fileName);
    char buffer[TcpSocket::CHUNK_SIZE];
    int rest = fileSize % TcpSocket::CHUNK_SIZE;
    int sent = 0;
    for (int i = 1; TcpSocket::CHUNK_SIZE * i <= fileSize; i++)
    {
        file.read(buffer, TcpSocket::CHUNK_SIZE);
        sent += sock.sendData(buffer, TcpSocket::CHUNK_SIZE);
    }
    if (rest > 0)
    {
        file.read(buffer, rest);
        rest -= sock.sendData(buffer, rest);
    }
    totalSent += fileSize;
    file.close();
}

void Dropbox::receiveFile(TcpSocket &sock, std::string fileName, size_t fileSize) // fileName jest sciezka do pliku
{
    std::ofstream file;
    file.open(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.good())
    {
        throw DropboxException("Error creating file");
    }
    char buffer[TcpSocket::CHUNK_SIZE];
    int rest = fileSize % TcpSocket::CHUNK_SIZE;
    for (int i = 1; TcpSocket::CHUNK_SIZE * i <= fileSize; i++)
    {
        size_t size = sock.receiveData(buffer, TcpSocket::CHUNK_SIZE);
        file.write(buffer, size);
    }
    while(rest > 0)
    {
        size_t size = sock.receiveData(buffer, rest);
        file.write(buffer, size);
        rest -= size;
    }
    totalReceived += fileSize;
    file.close();

    ClientEventReporter::FileInfo fileInfo;
    fileInfo.path = boost::filesystem::path(fileName);
    fileInfo.isOpen = false;

    ClientEventReporter::allFilesInfo.insert(fileInfo);
}

void Dropbox::deleteFile(std::string fileName)
{
    boost::filesystem::path path(fileName);

    if (boost::filesystem::exists(path))
    {
        if (boost::filesystem::is_directory(path))
        {
            boost::filesystem::remove_all(path);
        } else
        {
            int result = remove(fileName.c_str());
            if (result != 0)
            {
                throw DropboxException("Error deleting file: " + fileName);
            }
        }
    } else
    {
        throw std::runtime_error(
                "It is impossible to delete the file or directory, because the path does not exist. Path: " + fileName +
                "\n");
    }

    for (auto it = ClientEventReporter::allFilesInfo.begin(); it != ClientEventReporter::allFilesInfo.end(); ++it)
    {
        if (it->path == path)
        {
            ClientEventReporter::allFilesInfo.erase(it);
        }
    }
}

void Dropbox::copyFile(std::string source, std::string destination)
{
    std::ifstream sourceFile(source.c_str(), std::ios::binary | std::ios::in);
    std::ofstream destinationFile(destination.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);

    destinationFile << sourceFile.rdbuf();

    ClientEventReporter::FileInfo fileInfo;
    fileInfo.path = boost::filesystem::path(destination);
    fileInfo.isOpen = false;

    ClientEventReporter::allFilesInfo.insert(fileInfo);
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
size_t Dropbox::sendNewFileProcedure(TcpSocket sock, std::string filePath, std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, NEW_FILE);
    // std::cout << "SEND NEW FILE PROCEDURE, ABSOLUTE PATH: " << filePath << std::endl;
    std::string relativePath = generateRelativePath(filePath);
    // std::cout << "SEND NEW FILE PROCEDURE, RELATIVE PATH: " << relativePath << std::endl;
    sendString(sock, relativePath);
    IntType fileSize = getFileSize(filePath);
    sendInt(sock, fileSize);
    sendFile(sock, filePath, fileSize);
    Dropbox::IntType modificationTime = static_cast<IntType>(getModificationTime(filePath));
    sendInt(sock, modificationTime);
    clientMutex.unlock();
    return fileSize;
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
    std::string relativePath = generateRelativePath(directoryPath);
    sendString(sock, relativePath);
    clientMutex.unlock();
}

/**
 * 1. wysyła event DELETE
 * 2. wysyła ścieżkę pliku do usunięcia
 * @param sock
 * @param directoryPath ścieżka BEZWZGLĘDNA
 */
void Dropbox::sendDeletionPathProcedure(TcpSocket sock, std::string directoryPath, std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, DELETE);
    std::string relativePath = generateRelativePath(directoryPath);
    sendString(sock, relativePath);
    clientMutex.unlock();
}

/**
 * 1. wysyła event MOVE
 * 2. wysyła ścieżki źródłową i docelową pliku do usunięcia
 * @param sock
 * @param directoryPath ścieżka BEZWZGLĘDNA
 */
void Dropbox::sendMovePathsProcedure(TcpSocket sock, std::string sourcePath, std::string destinationPath,
                                     std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, MOVE);
    std::string relativeSourcePath = generateRelativePath(sourcePath);
    std::string relativeDestinationPath = generateRelativePath(destinationPath);
    sendString(sock, relativeSourcePath);
    sendString(sock, relativeDestinationPath);
    clientMutex.unlock();
}

void Dropbox::sendCopyPathsProcedure(TcpSocket sock, std::string sourcePath, std::string destinationPath,
                                     std::mutex &clientMutex)
{
    clientMutex.lock();
    sendEvent(sock, COPY);
    std::string relativeSourcePath = generateRelativePath(sourcePath);
    std::string relativeDestinationPath = generateRelativePath(destinationPath);
    sendString(sock, relativeSourcePath);
    sendString(sock, relativeDestinationPath);
    clientMutex.unlock();
}

/**
 * 1. odbiera ścieżkę do pliku
 * 2. usuwa plik
 */
std::string Dropbox::receiveDeletionPathProcedure(TcpSocket &serverSocket, std::mutex &clientMutex)
{
    clientMutex.lock();
    std::string fileName;
    receiveString(serverSocket, fileName);
    fileName = generateAbsolutPath(fileName);
    ClientEventReporter::ignoredPaths.insert(fileName);
    deleteFile(fileName);
    clientMutex.unlock();
    return fileName;
}

/**
 * 1. odbiera ścieżkę źródłową pliku
 * 2. odbiera ścieżkę docelową pliku
 * 3. wykonuję operację move
 */
std::string *Dropbox::receiveMovePathsProcedure(TcpSocket &serverSocket, std::mutex &clientMutex)
{
    std::string *paths = new std::string[2];
    clientMutex.lock();
    std::string fileName;
    receiveString(serverSocket, paths[0]);
    receiveString(serverSocket, paths[1]);
    paths[0] = generateAbsolutPath(paths[0]);
    paths[1] = generateAbsolutPath(paths[1]);
    // cout << "FROM: " << paths[0] << " TO: " << paths[1] << std::endl;
    ClientEventReporter::ignoredPaths.insert(paths[0]);
    ClientEventReporter::ignoredPaths.insert(paths[1]);
    moveFile(paths[0], paths[1]);
    clientMutex.unlock();
    return paths;
}

std::string *Dropbox::receiveCopyPathsProcedure(TcpSocket &serverSocket, std::mutex &clientMutex)
{
    std::string *paths = new std::string[2];

    clientMutex.lock();
    std::string fileName;
    receiveString(serverSocket, paths[0]);
    receiveString(serverSocket, paths[1]);
    paths[0] = generateAbsolutPath(paths[0]);
    paths[1] = generateAbsolutPath(paths[1]);
    ClientEventReporter::ignoredPaths.insert(paths[0]);
    ClientEventReporter::ignoredPaths.insert(paths[1]);
    copyFile(paths[0], paths[1]);
    clientMutex.unlock();
    return paths;
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
    // std::cout << "RECEIVE NEW FILE PROCEDURE, RELATIVE PATH: " << fileName << std::endl;
    fileName = generateAbsolutPath(fileName);

    // std::cout << "RECEIVE NEW FILE PROCEDURE, ABSOLUTE PATH: " << fileName << std::endl;

    ClientEventReporter::FileInfo fileInfo = ClientEventReporter::findByPath(boost::filesystem::path(fileName));
    if(!fileInfo.path.string().empty() && fileInfo.isOpen)
    {
        std::cerr << "A conflict has occured. Path: " << fileName << std::endl;
        ClientEventReporter::ignoredPaths.insert(fileName); // the first modifiction will be ignored
        fileName = fileInfo.path.remove_filename().string() + "/." + fileInfo.path.filename().string() + "_conflict_copy";
        ClientEventReporter::permanentlyIgnored.insert(fileName);
    }

    receiveInt(serverSocket, size);

    ClientEventReporter::permanentlyIgnored.insert(fileName);
    receiveFile(serverSocket, fileName, size);
    ClientEventReporter::permanentlyIgnored.erase(fileName);

    Dropbox::IntType modificationTime;
    receiveInt(serverSocket, modificationTime);
    changeModificationTime(fileName.c_str(), modificationTime);
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
    folder = generateAbsolutPath(folder);
    ClientEventReporter::ignoredPaths.insert(folder);
    clientMutex.unlock();
    createDirectory(folder);
    return folder;
}

Dropbox::IntType Dropbox::getModificationTime(std::string path)
{
    struct stat s;
    ClientEventReporter::permanentlyIgnored.insert(path);
    if (stat(path.c_str(), &s) != 0)
        throw DropboxException("Couldn't determine file modification time");
    changeModificationTime(path, static_cast<Dropbox::IntType>(s.st_mtim.tv_sec));
    ClientEventReporter::permanentlyIgnored.erase(path);
    return static_cast<Dropbox::IntType>(s.st_mtim.tv_sec);
}

void Dropbox::changeModificationTime(std::string path, Dropbox::IntType time)
{
    utimbuf u;
    u.modtime = static_cast<time_t>(time);
    u.actime = 0;
    ClientEventReporter::permanentlyIgnored.insert(path);
    if (utime(path.c_str(), &u) == -1)
        throw DropboxException("Failed to change file's modification time");
    ClientEventReporter::permanentlyIgnored.erase(path);
}

/* sprawdza czy plik po drugiej stronie istnieje i czy nie jest starszy
   wysyla:
   1. event
   2. sciezke do pliku
   3. date modyfikacji
   odbiera:
   1. inta z 0 lub 1
*/
bool Dropbox::askIfValid(TcpSocket &socket, std::string path)
{
    sendEvent(socket, Dropbox::ProtocolEvent::IS_VALID);
    std::string relativePath = generateRelativePath(path);
    sendString(socket, relativePath);
    Dropbox::IntType modificationTime = getModificationTime(path);
    sendInt(socket, modificationTime);
    Dropbox::IntType isValid;
    receiveInt(socket, isValid);
    if (isValid == 1)
    {
        return true;
    }
    if (isValid == 0)
    {
        return false;
    } else
    {
        throw Dropbox::DropboxException("Protocol error");
    }
}

bool Dropbox::answerIfValid(TcpSocket &socket)
{
    std::string path;
    receiveString(socket, path);
    path = generateAbsolutPath(path);

    Dropbox::IntType remoteModificationTime;
    receiveInt(socket, remoteModificationTime);

    Dropbox::IntType isValid = 0;
    // tutaj sprawdzamy czy istnieje i czy nie jest starszy
    if (boost::filesystem::exists(path))
    {
        Dropbox::IntType localModificationTime = getModificationTime(path);
        if(boost::filesystem::is_directory(path))
        {
            isValid = 1;
        }
        else if(localModificationTime >= remoteModificationTime)
        {
            isValid = 1;
        }
        else
        {
            deleteFile(path);
        }
    }
    sendInt(socket, isValid);
    return static_cast<bool>(isValid);
}