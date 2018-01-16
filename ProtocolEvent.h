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
    DELETE_FILE, //
    MOVE_FILE, // ???
    NEW_DIRECTORY, // ???
    COPY_FILE //
};

struct EventMessage
{
    ProtocolEvent event;
    std::string source;
    std::string destination;
    TcpSocket sender;
};

#endif //DROPBOX_EVENT_H
