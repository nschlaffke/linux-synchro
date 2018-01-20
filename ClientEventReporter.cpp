//
// Created by PJ on 27.12.17.
//

#include "ClientEventReporter.h"

using namespace inotify;

vector<ClientEventReporter::FileInfo> ClientEventReporter::allFilesInfo;
SafeQueue<EventMessage> ClientEventReporter::messageQueue;
std::mutex mtx;


//pewnie nie da sie tak inicjalizowac staticów jak poniżej
ClientEventReporter::ClientEventReporter(boost::filesystem::path observedDirectory)
    : observedDirectory(observedDirectory)
{
    ClientEventReporter::allFilesInfo = collectFilePaths(this->observedDirectory);
}

char* ClientEventReporter::convertToCharArray(string path)
{
    char *convertedPath = new char[path.size() + 1];
    std::copy(path.begin(), path.end(), convertedPath);
    convertedPath[path.size()] = '\0';

    return convertedPath;
}

int ClientEventReporter::getFileSize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return static_cast<int>(in.tellg());
}

vector <ClientEventReporter::FileInfo> ClientEventReporter::collectFilePaths(boost::filesystem::path path)
{
    vector <FileInfo> filePaths;

    if(boost::filesystem::exists(path))
    {
        if(boost::filesystem::is_directory(path))
        {
            boost::filesystem::recursive_directory_iterator it(path, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            FileInfo fileInfo;
            fileInfo.path = path;
            fileInfo.fileSize = ClientEventReporter::getFileSize(path.c_str());

            filePaths.push_back(fileInfo);

            while(it != end)
            {
                boost::filesystem::path currentPath = *it;

                if(boost::filesystem::is_regular_file(currentPath)){
                    FileInfo fileInfo;
                    fileInfo.path = currentPath;
                    fileInfo.fileSize = ClientEventReporter::getFileSize(currentPath.c_str());

                    filePaths.push_back(fileInfo);
                }

                ++it;
            }
        }
    }
    else
    {
        throw invalid_argument("It is impossible to watch the path, because the path does not exist. Path: " + path.string());
    }

    return filePaths;
}

void ClientEventReporter::makeRequest(Notification notification, ProtocolEvent protocolEvent) //deletion = moved_from, self-deletion
{
    string path = notification.destination.string();
    Event event = notification.event;

    EventMessage eventMessage;
    eventMessage.event = protocolEvent;
    if(!notification.source.empty())
    {
        eventMessage.source = notification.source.string();
    }
    eventMessage.destination = notification.destination.string();

    std::cout << "Event: " << Notifier::getEventName(event) << " on " << path << " was triggered." << std::endl;

    mtx.lock();
    ClientEventReporter::messageQueue.enqueue(eventMessage);
    mtx.unlock();
}

void ClientEventReporter::requestCreation(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Creation\n";
    if(boost::filesystem::is_directory(notification.destination))
    {
        std::cout << "Directory creation\n";
        makeRequest(notification, NEW_DIRECTORY);
    }
    else
    {
        std::cout << "File creation\n";
        makeRequest(notification, NEW_FILE);
    }


    FileInfo fileInfo;
    fileInfo.path = notification.destination;
    fileInfo.fileSize = getFileSize(notification.destination.c_str());
    allFilesInfo.push_back(fileInfo);
}

void ClientEventReporter::requestDeletion(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Deletion\n";

    if(notification.event == Event::remove_self)
    {
        notification.destination = notification.destination.remove_filename();
    }

    makeRequest(notification, DELETE);

    FileInfo fileInfo;
    fileInfo.path = notification.destination;
    fileInfo.fileSize = getFileSize(notification.destination.c_str());

    auto it = find(ClientEventReporter::allFilesInfo.begin(), ClientEventReporter::allFilesInfo.end(), fileInfo);
    ClientEventReporter::allFilesInfo.erase(it);
}

void ClientEventReporter::requestCopying(Notification notification) //copy, creation via console for example: touch
{
    FileInfo fileInfo;
    fileInfo.path = notification.destination;
    fileInfo.fileSize = getFileSize(notification.destination.c_str());

    auto it = find(ClientEventReporter::allFilesInfo.begin(), ClientEventReporter::allFilesInfo.end(), fileInfo);
    if(it == ClientEventReporter::allFilesInfo.end())
    {
        std::cout << "Attrib\n";
        makeRequest(notification, COPY_FILE);

        allFilesInfo.push_back(fileInfo);
    }
}

bool ClientEventReporter::isInternalMove(Notification &notification)
{
    FileInfo fileInfo;
    fileInfo.path = notification.destination;
    fileInfo.fileSize = getFileSize(notification.destination.c_str());

    auto it = find(ClientEventReporter::allFilesInfo.begin(), ClientEventReporter::allFilesInfo.end(), fileInfo);
    if(it == ClientEventReporter::allFilesInfo.end())
    {
        return false;
    }
    else
    {
        notification.source = it->path;
        ClientEventReporter::allFilesInfo.erase(it);
        ClientEventReporter::allFilesInfo.push_back(fileInfo);
        return true;
    }
}

void ClientEventReporter::requestMoveFrom(Notification notification)
{
    if(notification.event == Event::outward_move)
    {
        std::cout << "Outward move\n";
        requestDeletion(notification);
    }
    else
    {
        std::cout << "InternalMove\n";
        makeRequest(notification, MOVE);
    }

}

void ClientEventReporter::requestMoveTo(Notification notification)
{
    if(isInternalMove(notification))
    {
        std::cout << "InternalMove\n";
        makeRequest(notification, MOVE);
    }
    else
    {
        std::cout << "Inward move\n";
        ClientEventReporter::requestCreation(notification);
    }
}

void ClientEventReporter::chooseRequest(Notification notification)
{
    switch(notification.event)
    {
        case Event::remove:
        case Event::remove_self:
            requestDeletion(notification);
            break;

        case Event::create:
        case Event::create_dir:
        case Event::modify:
            requestCreation(notification);
            break;

        case Event::outward_move:
        case Event::internal_move:
            requestMoveFrom(notification);
            break;

        case Event::moved_to:
        case Event::moved_to_dir:
            requestMoveTo(notification);
            break;
    }
}

void ClientEventReporter::handleNotifications()
{
    Notifier notifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::remove, Event::remove_self, Event::create, Event::create_dir, Event::modify,
                       Event::outward_move, Event::internal_move, Event::moved_to, Event::moved_to_dir}, chooseRequest);

    /*Notifier newNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::create, Event::create_dir, Event::modify}, requestCreation);

    Notifier movedFromNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::outward_move, Event::internal_move}, requestMoveFrom);

    Notifier movedToNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_to}, requestMoveTo);*/

    std::cout << "Waiting for events..." << std::endl;

    std::thread t1(&Notifier::run, notifier);
    /*std::thread t2(&Notifier::run, newNotifier);
    std::thread t4(&Notifier::run, movedFromNotifier);
    std::thread t5(&Notifier::run, movedToNotifier);*/
    t1.join();
    /*t2.join();
    t4.join();
    t5.join();*/
}
