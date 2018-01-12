//
// Created by ns on 24.11.17.
//

#ifndef DROPBOX_DESCRIPTOR_H
#define DROPBOX_DESCRIPTOR_H

#include <unistd.h>
#include <stdexcept>
#include <exception>

class Descriptor
{
    friend class TcpSocket;

public:
    Descriptor(const Descriptor &d);

    void operator=(const Descriptor &rhs);

    bool operator==(const Descriptor &rhs) const;

    bool operator!=(const Descriptor &rhs) const;

    Descriptor();
    Descriptor(int s);
    void doClose();
    int getVal();

private:
    bool isOpen;
    bool isValid();
    int fd;

};


#endif //DROPBOX_DESCRIPTOR_H
