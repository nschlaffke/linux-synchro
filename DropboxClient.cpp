//
// Created by ns on 10.01.18.
//
/// pole serverSocket to referencja na samego siebie; istnieje tylko po to, aby w funkcjach typy receiveFile nie pisać *this
#include <iostream>
#include <thread>
#include "DropboxClient.h"

DropboxClient::DropboxClient(const std::string &ip, unsigned short port, const std::string folderPath)
        : Dropbox(folderPath), TcpSocket(ip, port), serverSocket(*this), folderPath(folderPath)
{
    // koniecznie może być przywrócenie notifiera w to miejsce
}

void DropboxClient::run()
{
    if (!isConnected())
    {
        throw DropboxException("Error: Client not connected\n");
    }
    newClientProcedure();

    notifier = Notifier()
            .watchPathRecursively(boost::filesystem::path(folderPath))
            .ignoreFileOnce("file")
            .onEvents({Event::open, Event::close, Event::close_nowrite, Event::close_write, Event::remove, /*Event::remove_self,*/ Event::remove_dir, Event::remove_self_dir, Event::create, Event::create_dir, Event::modify,
                       Event::outward_move, Event::internal_move, Event::moved_to, Event::moved_to_dir}, ClientEventReporter::chooseRequest);

    std::thread t(&Notifier::run, notifier);
    std::thread s(&DropboxClient::sender, this);
    std::thread r(&DropboxClient::receiver, this);

    s.join();
    return;
    r.join();
    t.join();
}

/**
 * void DropboxClient::newClientProcedure()
 * opis:
 * 1. Klient wysyła do serwera event NEW_CLIENT
 */
void DropboxClient::newClientProcedure()
{
    sendEvent(*this, NEW_CLIENT);
    Dropbox::ProtocolEvent event;
    do
    {
        receiveEvent(serverSocket, event);
        switch(event)
        {
            case NEW_FILE:
                receiveNewFileProcedure(serverSocket, serverMutex);
                break;
            case NEW_DIRECTORY:
                receiveNewDircetoryProcedure(serverSocket, serverMutex);
                break;
            case IS_VALID:
                answerIfValid(serverSocket);
                break;
            case END_OF_SYNC:
                break;
            default:
                throw DropboxException("Protocol error");
                break;
        }
    }while(event != Dropbox::ProtocolEvent::END_OF_SYNC);
    // teraz klient wysyła swoje pliki
    if (boost::filesystem::exists(folderPath))
    {
        if (boost::filesystem::is_directory(folderPath))
        {
            boost::filesystem::recursive_directory_iterator
                    it(folderPath, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            while (it != end)
            {
                boost::filesystem::path currentPath = *it;

                if (boost::filesystem::is_regular_file(currentPath))
                {
                    if(!askIfValid(serverSocket, it->path().c_str()))
                    {
                        // u klienta plik nie istnieje lub jest starszy, więc go wysyłamy
                        sendNewFileProcedure(sock, it->path().c_str(), serverMutex);
                    }
                }
                else if (boost::filesystem::is_directory(currentPath))
                {
                    if(!askIfValid(serverSocket, it->path().c_str()))
                    {
                        sendNewDirectoryProcedure(sock, it->path().c_str(), serverMutex);
                    }
                }
                ++it;
            }
        }
    }
    sendEvent(serverSocket, END_OF_SYNC);
}

/**
 * Funkcja (wątek) zczytujący wiadomości z kolejki messageQueue i wysyłająca je do serwera
 * Wiadomości na kolejce umieszcza ClientEventReporter
 */
void DropboxClient::sender()
{
    while (true)
    {
        EventMessage eventMessage = ClientEventReporter::messageQueue.dequeue();

        ProtocolEvent event = eventMessage.event;
        std::string path = correctPath(eventMessage.destination);

        std::cout << "Sender:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "senNEW FILE: " << path << std::endl;
                    notifier.watchFile(boost::filesystem::path(path));
                    sendNewFileProcedure(serverSocket, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senNEW_FILE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case DELETE:
                try
                {
                    std::cout << "senDELETE: " << path << std::endl;
                    sendDeletionPathProcedure(serverSocket, path, serverMutex);
                    // notifier.removeWatch(boost::filesystem::path(path));
                }
                catch (std::exception &e)
                {
                    std::cout << "senDELETE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case MOVE:
                try
                {
                    std::string source = correctPath(eventMessage.source);
                    std::cout << "senMOVE FROM: " << source << " TO: " << path << std::endl;

                    sendMovePathsProcedure(serverSocket, source, path, serverMutex);
                    // notifier.removeWatch(boost::filesystem::path(source));
                    notifier.watchFile(boost::filesystem::path(path));
                }
                catch (std::exception &e)
                {
                    std::cout << "senMOVE error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    sendNewDirectoryProcedure(serverSocket, path, serverMutex);
                    notifier.watchFile(boost::filesystem::path(path));
                }
                catch (std::exception &e)
                {
                    std::cout << "senNEW_DIRECTORY error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            case COPY:
                try
                {
                    std::string source = correctPath(eventMessage.source);
                    std::cout << "senCOPY FROM: " << source << " TO: " << path << std::endl;

                    notifier.watchFile(boost::filesystem::path(path));
                    sendCopyPathsProcedure(serverSocket, source, path, serverMutex);
                }
                catch (std::exception &e)
                {
                    std::cout << "senCOPY error: " << e.what()
                              << "senTerminating client\n";
                    return;
                }
                break;
            default:
                throw DropboxException("Protocol error");
                break;
        }
    }
}

/**
 * Funkcja (wątek) odbierający wiadomości od serwera
 */
void DropboxClient::receiver()
{
    ProtocolEvent event;
    std::string *files = new std::string[2];
    while (true)
    {
        std::cout << "Received: " << getTotalReceived() << std::endl;
        std::cout << "Sent: " << getTotalSent() << std::endl;
        receiveEvent(serverSocket, event);
        std::cout << "Receiver:\n";
        switch (event)
        {
            case NEW_FILE:
                try
                {
                    std::cout << "recNEW FILE" << "\n";
                    files[0] = receiveNewFileProcedure(serverSocket, serverMutex);
                    notifier.watchFile(boost::filesystem::path(files[0]));
                }
                catch (std::exception &e)
                {
                    std::cout << "recNEW_FILE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case DELETE:
                try
                {
                    std::cout << "recDELETE\n";
                    files[0] = receiveDeletionPathProcedure(serverSocket, serverMutex);
                    // notifier.removeWatch(boost::filesystem::path(files[0]));
                }
                catch (std::exception &e)
                {
                    std::cout << "recDELETE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case MOVE:
                try
                {
                    std::cout << "recMOVE\n";
                    files = receiveMovePathsProcedure(serverSocket, serverMutex);
                    // notifier.removeWatch(boost::filesystem::path(files[0]));
                    notifier.watchFile(boost::filesystem::path(files[1]));
                }
                catch (std::exception &e)
                {
                    std::cout << "recMOVE error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            case NEW_DIRECTORY:
                try
                {
                    std::cout << "recNEW DIRECTORY\n";
                    files[0] = receiveNewDircetoryProcedure(serverSocket, serverMutex);
                    notifier.watchPathRecursively(boost::filesystem::path(files[0]));
                }
                catch (std::exception &e)
                {
                    std::cout << "recNEW_DIRECTORY error:\n" << e.what() << std::endl
                              << "recTerminating client\n";
                    return;
                }
                break;
            case COPY:
                try
                {
                    std::cout << "recCOPY\n";
                    files = receiveCopyPathsProcedure(serverSocket, serverMutex);
                    notifier.watchFile(boost::filesystem::path(files[1]));
                }
                catch (std::exception &e)
                {
                    std::cout << "recCOPY error: " << e.what()
                              << "recTerminating client\n";
                    return;
                }
                break;
            default:
                throw DropboxException("recProtocol error");
                break;
        }
    }
    delete[] files;
}

