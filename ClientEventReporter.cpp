//
// Created by PJ on 27.12.17.
//

#include "ClientEventReporter.h"

//TODO: PERFORM TESTS USING NC, VIABLE ONLY WHEN A CLIENT INTERFACE IS PROVIDED

using namespace inotify;

vector<boost::filesystem::path> ClientEventReporter::allFilePaths;
std::mutex mtx;

ClientEventReporter::ClientEventReporter(boost::filesystem::path observedDirectory, TcpServer serverSocket)
{
    this->observedDirectory = observedDirectory;
    this->serverSocket = serverSocket;
    ClientEventReporter::allFilePaths = collectFilePaths(this->observedDirectory);
}

char* ClientEventReporter::convertToCharArray(string path)
{
    char *convertedPath = new char[path.size() + 1];
    std::copy(path.begin(), path.end(), convertedPath);
    convertedPath[path.size()] = '\0';

    return convertedPath;
}

boost::filesystem::path ClientEventReporter::getObservedDirectory()
{
    return observedDirectory;
}

void ClientEventReporter::setObservedDirectory(boost::filesystem::path observedDirectory)
{
    this->observedDirectory = observedDirectory;
}

TcpServer ClientEventReporter::getServerSocket()
{
    return serverSocket;
}

void ClientEventReporter::setServerSocket(TcpServer serverSocket)
{
    this->serverSocket = serverSocket;
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

void ClientEventReporter::requestOnly(Notification notification) //deletion = moved_from, self-deletion
{
    string path = notification.path.string();
    Event event = notification.event;
    char *convertedPath = convertToCharArray(path);

    std::cout << "Event: " << Notifier::getEventName(event) << " on " << path << " was triggered." << std::endl;

    mtx.lock();
    // socket.sendData(); request do serwera z odpowiednim kodem
    ClientEventReporter::serverSocket.sendData(int(event), sizeof(event)); // send the event code
    // the server should act differently depending on the type of a deletion (self rmdir or not rm)
    ClientEventReporter::serverSocket.sendData(convertedPath, sizeof(convertedPath)); // send the path
    mtx.unlock();
}

void ClientEventReporter::requestCreation(Notification notification) //creation, modyfication, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Creation\n";
    mtx.lock();
    requestOnly(notification);
    ClientEventReporter::serverSocket.sendFile(notification.path.string());
    mtx.unlock();
}

void ClientEventReporter::requestIfCreated(Notification notification) //copy, creation via console for example: touch
{
    std::vector<boost::filesystem::path>::iterator it = find(ClientEventReporter::allFilePaths.begin(), ClientEventReporter::allFilePaths.end(), notification.path);
    if(it == ClientEventReporter::allFilePaths.end())
    {
        std::cout << "Attrib\n";

        mtx.lock();
        requestOnly(notification);
        ClientEventReporter::serverSocket.sendFile(notification.path.string());
        mtx.unlock();

        allFilePaths.push_back(notification.path);
    }
}

void runNewThread(Notifier notifier)
{
    notifier.run();
}

void ClientEventReporter::handleNotification()
{
    Notifier delNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_from, Event::remove, Event::remove_self}, requestOnly);

    Notifier newNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::moved_to, Event::create, Event::modify}, requestCreation);

    Notifier attribNotifier = Notifier()
            .watchPathRecursively(ClientEventReporter::observedDirectory)
            .ignoreFileOnce("file")
            .onEvents({Event::attrib}, requestIfCreated);

    std::cout << "Waiting for events..." << std::endl;

    std::thread t1(runNewThread, delNotifier);
    std::thread t2(runNewThread, newNotifier);
    std::thread t3(runNewThread, attribNotifier);
    t1.join();
    t2.join();
    t3.join();
}