//
// Created by ns on 20.11.17.
//

#define DEBUG 0

#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
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

size_t TcpSocket::recieveData(char *buffer, size_t bufferSize)
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


void TcpSocket::sendFile(const std::string fileName)
{
    std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    if (!file.good())
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
        if (size < CHUNK_SIZE)
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

TcpSocket::TcpSocket(Descriptor tmp) : sock(tmp), connectionEstablished(true)
{}

void TcpSocket::setBlock()
{
    int flags = fcntl(sock.getVal(), F_GETFL);
    if(flags < 0)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to read flags"));
    }
    flags = flags & (~O_NONBLOCK);
    int tmp;
    tmp = fcntl(sock.getVal(), F_SETFL, flags);
    if(tmp < 0)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to set flags"));
    }
}

void TcpSocket::setNoBlock()
{
    int tmp;
    tmp = fcntl(sock.getVal(),
          F_SETFL,
          O_NONBLOCK);
    if(tmp < 0)
    {
        throw SocketException(POSIXError::getErrorMessage("Failed to set non blocking mode"));
    }

}

bool TcpSocket::hasData()
{
    int count;
    ioctl(sock.getVal(), FIONREAD, &count);
    if(count > 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TcpSocket::sendData(IntType data)
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
    }while(bytes > 0);
}

void TcpSocket::recieveData(IntType &data)
{
    IntType dataToRecieve;
    int bytes = sizeof(dataToRecieve);
    char *dataPointer = reinterpret_cast<char *>(&dataToRecieve);
    int recieved = 0;
    do
    {
       recieved += recieveData(dataPointer, bytes);

    }while(bytes - recieved > 0);
    data = ntohl(dataToRecieve);
}