//
// Created by ns on 24.11.17.
//

#include "Descriptor.h"
#include <iostream>

Descriptor::Descriptor()
{}

Descriptor::Descriptor(int s) : fd(s), isOpen(true)
{
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
    fd = d.fd;
    isOpen = d.isOpen;
}

void Descriptor::operator=(const Descriptor &rhs)
{
    fd = rhs.fd;
    isOpen = rhs.isOpen;
}

bool Descriptor::operator==(const Descriptor &rhs) const
{
    return fd == rhs.fd;
}

bool Descriptor::operator!=(const Descriptor &rhs) const
{
    return !(rhs == *this);
}


