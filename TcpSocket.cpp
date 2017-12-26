//
// Created by ns on 20.11.17.
//

#define DEBUG 0

#include <iostream>
#include "TcpSocket.h"
#include "POSIXError.h"

#if (DEBUG == 1)
#include <iostream>
#endif

TcpSocket::TcpSocket(const std::string ip, const unsigned short port) :
        ipAddress(ip), portNumber(port), connectionEstablished(false), bound(false),
        sock(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    if (!sock.isValid())
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to create a socket"));
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
}

TcpSocket::TcpSocket()
{}

void TcpSocket::doClose()
{
    sock.doClose();
}

TcpSocket TcpSocket::doAccept()
{
    if (!bound)
    {
        throw SocketException(POSIXError::getErrorMessage("Accept before binding"));
    }

    socklen_t size = sizeof(addr);
    int fd = accept(sock.getVal(), reinterpret_cast<sockaddr *>(&addr), &size);

    if (fd == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to accept"));
    }

    TcpSocket tmp;
    tmp.sock = fd;
    tmp.connectionEstablished = true;
    return tmp;
}

void TcpSocket::doBind()
{
    int reuse = 1;
    int result = setsockopt(sock.getVal(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to set socket option"));
    }
    result = bind(sock.getVal(), reinterpret_cast<const sockaddr *>(&addr), sizeof(addr));
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to bind"));
    } else
    {
        bound = true;
    }
}

void TcpSocket::doConnect()
{
    if (connectionEstablished)
    {
        throw SocketException(POSIXError::getErrorMessage("Socket is already connected"));
    }
    int result = connect(sock.getVal(), reinterpret_cast<sockaddr *>(&addr), sizeof addr);
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to connect"));
    }
    connectionEstablished = true;
}

bool TcpSocket::isConnected()
{
    return connectionEstablished;
}

void TcpSocket::closeSocket()
{
    if (!connectionEstablished)
    {
        throw SocketException(POSIXError::getErrorMessage("Socket is not connected"));
    }
    close(sock.getVal());
}

size_t TcpSocket::recieveData(char buffer[], size_t bufferSize)
{
    if (!connectionEstablished)
    {
        throw (POSIXError::getErrorMessage("Socket is not connected"));
    }
    ssize_t result = read(sock.getVal(), buffer, bufferSize);

    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to read"));
    } else
        return static_cast<size_t>(result);
}

size_t TcpSocket::sendData(const char data[], size_t size)
{
    ssize_t result = write(sock.getVal(), data, size);
    if (result == -1)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to write"));
    } else
        return static_cast<size_t>(result);
}

TcpSocket::~TcpSocket()
{
    // closeSocket();
}

void TcpSocket::doListen(const unsigned int queueSize)
{
    listen(sock.getVal(), queueSize);
}

void TcpSocket::sendFile(const std::string fileName)
{
    // TODO FILE NOT FOUND !!!
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    if(!file.good())
    {
        throw SocketException("Couldn't open file");
    }
    size_t fileSize = getFileSize(fileName);
    char buffer[CHUNK_SIZE];
    for (int i = 0; CHUNK_SIZE * i <= fileSize; i++) // send file in chunks
    {
        file.read(buffer, CHUNK_SIZE);
        if (file.eof())
        {
            int size = std::min(int(fileSize) - CHUNK_SIZE * i, CHUNK_SIZE);
            sendData(buffer, size);
            break;
        }
        else
        {
            sendData(buffer, CHUNK_SIZE);
        }
        file.seekg(CHUNK_SIZE, std::ios::cur);
    }

    file.close();
}

void TcpSocket::recieveFile(const std::string fileName, size_t fileSize) // fileName jest sciezka do pliku
{
    std::ofstream file(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
    char buffer[CHUNK_SIZE];

    for (int i = 0; CHUNK_SIZE * i <= fileSize; i++)
    {
        size_t size = recieveData(buffer, CHUNK_SIZE);
        file.write(buffer, size);
        file.seekp(size, std::ios::cur);
        if(size < CHUNK_SIZE)
        {
            break;
        }
    }
    file.close();
}
size_t TcpSocket::getFileSize(const std::string fileName)
{
    std::ifstream in(fileName.c_str(), std::ios::binary | std::ios::ate);
    size_t size = in.tellg();
    in.close();
    return size;
}
