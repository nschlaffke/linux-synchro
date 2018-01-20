//
// Created by PJ on 27.12.17.
//

#include "ClientEventReporter.h"

using namespace inotify;

vector<ClientEventReporter::FileInfo> ClientEventReporter::allFilesInfo;
SafeQueue<Dropbox::EventMessage> ClientEventReporter::messageQueue;
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
            fileInfo.fileSize = -1;
            cout << fileInfo.path << endl;
            filePaths.push_back(fileInfo);

            while(it != end)
            {
                boost::filesystem::path currentPath = *it;

                if(boost::filesystem::is_regular_file(currentPath)){
                    FileInfo fileInfo;
                    fileInfo.path = currentPath;
                    fileInfo.fileSize = ClientEventReporter::getFileSize(currentPath.c_str());

                    cout << fileInfo.path << endl;
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

void ClientEventReporter::makeRequest(Notification notification, Dropbox::ProtocolEvent protocolEvent) //deletion = moved_from, self-deletion
{
    string path = notification.destination.string();
    Event event = notification.event;

    Dropbox::EventMessage eventMessage;
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

bool ClientEventReporter::checkIfSameFiles(boost::filesystem::path path1, boost::filesystem::path path2)
{
    std::ifstream file1(path1.c_str(), std::ifstream::in | std::ifstream::binary);
    std::ifstream file2(path2.c_str(), std::ifstream::in | std::ifstream::binary);

    if(!file1.is_open() || !file2.is_open())
    {
        return false;
    }

    char *buffer1 = new char[BUFFER_SIZE]();
    char *buffer2 = new char[BUFFER_SIZE]();

    do {
        file1.read(buffer1, BUFFER_SIZE);
        file2.read(buffer2, BUFFER_SIZE);

        if (std::memcmp(buffer1, buffer2, BUFFER_SIZE) != 0)
        {
            delete[] buffer1;
            delete[] buffer2;
            return false;
        }
    } while (file1.good() || file2.good());

    delete[] buffer1;
    delete[] buffer2;
    return true;
}

bool ClientEventReporter::checkIfCopied(Notification &notification)
{
    FileInfo fileInfo;
    fileInfo.path = notification.destination;
    fileInfo.fileSize = getFileSize(notification.destination.c_str());

    auto it = find(ClientEventReporter::allFilesInfo.begin(), ClientEventReporter::allFilesInfo.end(), fileInfo);
    if(it == ClientEventReporter::allFilesInfo.end()){

        for(FileInfo fi: ClientEventReporter::allFilesInfo)
        {
            if(fi.fileSize == fileInfo.fileSize && fi.path.filename() == fileInfo.path.filename())
            {
                if(checkIfSameFiles(fileInfo.path, fi.path))
                {
                    notification.source = fi.path;
                    return true;
                }
            }
        }

        return false; //new file
    }
    else // ordinary modyfication
    {
        return false;
    }
}

void ClientEventReporter::requestCreation(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Creation\n";
    FileInfo fileInfo;
    fileInfo.path = notification.destination;

    // there is no need for creating empty directories, because their size is extremely small
    if(boost::filesystem::is_regular_file(notification.destination) && checkIfCopied(notification))
    {
        std::cout << "Copied\n";
        fileInfo.fileSize = getFileSize(notification.destination.c_str());
        makeRequest(notification, Dropbox::ProtocolEvent::COPY);
    }

    if(boost::filesystem::is_directory(notification.destination))
    {
        std::cout << "Directory creation\n";
        fileInfo.fileSize = -1;
        makeRequest(notification, Dropbox::ProtocolEvent::NEW_DIRECTORY);
    }
    else
    {
        std::cout << "File creation\n";
        fileInfo.fileSize = getFileSize(notification.destination.c_str());
        makeRequest(notification, Dropbox::ProtocolEvent::NEW_FILE);
    }

    allFilesInfo.push_back(fileInfo);
}

void ClientEventReporter::requestDeletion(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    std::cout << "Deletion\n";
    FileInfo fileInfo;

    if(notification.event == Event::remove_self)
    {
        notification.destination = notification.destination.remove_filename();
        fileInfo.fileSize = -1;
    }
    else
    {
        fileInfo.fileSize = getFileSize(notification.destination.c_str());
    }

    fileInfo.path = notification.destination;

    auto it = find(ClientEventReporter::allFilesInfo.begin(), ClientEventReporter::allFilesInfo.end(), fileInfo);

    ClientEventReporter::allFilesInfo.erase(it);

    makeRequest(notification, Dropbox::ProtocolEvent::DELETE);
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
        makeRequest(notification, Dropbox::ProtocolEvent::MOVE);
    }

}

void ClientEventReporter::requestMoveTo(Notification notification)
{
    if(isInternalMove(notification))
    {
        std::cout << "InternalMove\n";
        makeRequest(notification, Dropbox::ProtocolEvent::MOVE);
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
