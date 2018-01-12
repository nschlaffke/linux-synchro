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
