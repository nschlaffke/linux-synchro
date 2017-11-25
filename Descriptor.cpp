//
// Created by ns on 24.11.17.
//

#include "Descriptor.h"
#include <iostream>

Descriptor::Descriptor()
{}

Descriptor::Descriptor(int s) : fd(s), isOpen(true), copied(false)
{
}

Descriptor::~Descriptor()
{
    if(isOpen and !copied)
    {
        close(fd);
        std::cout << "Closing " << fd << std::endl;
    }
}

int Descriptor::getVal()
{
    return fd;
}

void Descriptor::doClose()
{
   close(fd);
   isOpen = false;
}

bool Descriptor::isValid()
{
   if(fd != -1)
   {
       return true;
   }
   else
   {
       return false;
   }
}

Descriptor::Descriptor(const Descriptor &d)
{
    copied = true;
    fd = d.fd;
    isOpen = d.isOpen;
}

void Descriptor::operator=(const Descriptor &rhs)
{
    fd = rhs.fd;
    isOpen = rhs.isOpen;
    copied = false;
    rhs.copied = true;
}


