//
// Created by piotr on 14.01.18.
//

#ifndef DROPBOX_EVENT_H
#define DROPBOX_EVENT_H

enum ProtocolEvent
{
    HEARTBEAT,
    NEW_CLIENT, //
    NEW_FILE, //
    DELETE, //
    MOVE, // ???
    NEW_DIRECTORY, // ???
    COPY //
};

struct EventMessage
{
    ProtocolEvent event;
    std::string source;
    std::string destination;
    TcpSocket sender;
};

#endif //DROPBOX_EVENT_H
