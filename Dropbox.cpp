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
    }while(bytes > 0);
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

    }while(bytes - recieved > 0);
    data = static_cast<Event>(ntohl(dataToRecieve));
}