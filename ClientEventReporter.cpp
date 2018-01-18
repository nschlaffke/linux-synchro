//
// Created by PJ on 27.12.17.
//

#include "ClientEventReporter.h"

using namespace inotify;

vector<boost::filesystem::path> ClientEventReporter::allFilePaths;
SafeQueue<EventMessage> ClientEventReporter::messageQueue;
std::mutex mtx;


//pewnie nie da sie tak inicjalizowac staticów jak poniżej
ClientEventReporter::ClientEventReporter(boost::filesystem::path observedDirectory)
    : observedDirectory(observedDirectory)
{
    ClientEventReporter::allFilePaths = collectFilePaths(this->observedDirectory);
}

char* ClientEventReporter::convertToCharArray(string path)
{
    char *convertedPath = new char[path.size() + 1];
    std::copy(path.begin(), path.end(), convertedPath);
    convertedPath[path.size()] = '\0';

    return convertedPath;
}

vector <boost::filesystem::path> ClientEventReporter::collectFilePaths(boost::filesystem::path path)
{
    vector <boost::filesystem::path> filePaths;

    if(boost::filesystem::exists(path))
    {
        if(boost::filesystem::is_directory(path))
        {
            boost::filesystem::recursive_directory_iterator it(path, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            while(it != end)
            {
                boost::filesystem::path currentPath = *it;

                if(boost::filesystem::is_regular_file(currentPath) || boost::filesystem::is_symlink(currentPath)){
                    filePaths.push_back(currentPath);
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
    string path = notification.path.string();
    Event event = notification.event;

    EventMessage eventMessage;
    eventMessage.event = protocolEvent;
    eventMessage.source = notification.path.string();

    std::cout << "Event: " << Notifier::getEventName(event) << " on " << path << " was triggered." << std::endl;

    mtx.lock();
    ClientEventReporter::messageQueue.enqueue(eventMessage);
    mtx.unlock();
}

void ClientEventReporter::requestCreation(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Creation\n";
    if(boost::filesystem::is_directory(notification.path))
    {
        makeRequest(notification, NEW_DIRECTORY);
    }
    else
    {
        makeRequest(notification, NEW_FILE);
    }
    allFilePaths.push_back(notification.path);
}

void ClientEventReporter::requestDeletion(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Deletion\n";
    makeRequest(notification, DELETE);
    std::vector<boost::filesystem::path>::iterator it = find(ClientEventReporter::allFilePaths.begin(), ClientEventReporter::allFilePaths.end(), notification.path);
    ClientEventReporter::allFilePaths.erase(it);
}

void ClientEventReporter::requestCopying(Notification notification) //copy, creation via console for example: touch
{
    std::vector<boost::filesystem::path>::iterator it = find(ClientEventReporter::allFilePaths.begin(), ClientEventReporter::allFilePaths.end(), notification.path);
    if(it == ClientEventReporter::allFilePaths.end())
    {
        std::cout << "Attrib\n";
        makeRequest(notification, COPY_FILE);

        allFilePaths.push_back(notification.path);
    }
}

void ClientEventReporter::requestMovement(Notification notification)
{
    // TODO: To be implemented
}

void ClientEventReporter::handleNotifications()
{
    Notifier delNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_from, Event::remove, Event::remove_self}, requestDeletion);

    Notifier newNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_to, Event::create, Event::create_dir, Event::modify}, requestCreation);

    Notifier attribNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::attrib}, requestCopying);

    Notifier movementNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_from, Event::moved_from}, requestMovement);

    std::cout << "Waiting for events..." << std::endl;

    std::thread t1(&Notifier::run, delNotifier);
    std::thread t2(&Notifier::run, newNotifier);
    std::thread t3(&Notifier::run, attribNotifier);
    //std::thread t4(&Notifier::run, requestMovement);
    t1.join();
    t2.join();
    t3.join();
    //t4.join();
}