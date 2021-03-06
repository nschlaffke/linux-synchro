//
// Created by PJ on 27.12.17.
//

#include "ClientEventReporter.h"

using namespace inotify;

SafeQueue<Dropbox::EventMessage> ClientEventReporter::messageQueue;

ClientEventReporter::ClientEventReporter(boost::filesystem::path observedDirectory)
    : observedDirectory(observedDirectory)
{
    collectFilePaths(this->observedDirectory);
}

void ClientEventReporter::collectFilePaths(boost::filesystem::path path)
{
    if(boost::filesystem::exists(path))
    {
        if(boost::filesystem::is_directory(path))
        {
            boost::filesystem::recursive_directory_iterator it(path, boost::filesystem::symlink_option::recurse);
            boost::filesystem::recursive_directory_iterator end;

            FileInfo fileInfo;
            fileInfo.path = path;
            fileInfo.isOpen = false;
            ClientEventReporter::allFilesInfo.insert(fileInfo);

            while(it != end)
            {
                boost::filesystem::path currentPath = *it;

                if(boost::filesystem::is_regular_file(currentPath)){
                    FileInfo fileInfo;
                    fileInfo.path = currentPath;
                    fileInfo.isOpen = false;
                    ClientEventReporter::allFilesInfo.insert(fileInfo);
                }
                ++it;
            }
        }
    }
    else
    {
        // throw runtime_error("It is impossible to watch the path, because the path does not exist. Path: " + path.string());
    }
}

void ClientEventReporter::saveAsClosed(Notification notification)
{
    //std::cout << "Closed\n";
    FileInfo fileInfo = findByPath(notification.destination);

    if(fileInfo.path.string().empty())
    {
        if(boost::filesystem::exists(notification.destination)) {
            fileInfo.path = notification.destination;
            fileInfo.isOpen = false;
        }
        else
        {
            return;
        }
    }

    ClientEventReporter::allFilesInfo.erase(fileInfo);
    fileInfo.isOpen = false;
    ClientEventReporter::allFilesInfo.insert(fileInfo);
}

void ClientEventReporter::saveAsOpen(Notification notification)
{
    //std::cout << "Opened\n";
    notification.destination = Dropbox::correctPath(notification.destination);
    FileInfo fileInfo = findByPath(notification.destination);

    if(fileInfo.path.string().empty())
    {

        if(boost::filesystem::exists(notification.destination))
        {
            fileInfo.path = notification.destination;
            fileInfo.isOpen = true;

            ClientEventReporter::allFilesInfo.insert(fileInfo);
        }
        else
        {
            throw runtime_error("There is no info about the file");
        }
    }

    ClientEventReporter::allFilesInfo.erase(fileInfo);
    fileInfo.isOpen = true;
    ClientEventReporter::allFilesInfo.insert(fileInfo);
}

void ClientEventReporter::makeRequest(Notification notification, Dropbox::ProtocolEvent protocolEvent) //deletion = moved_from, self-deletion
{
    string path = notification.destination.string();
    //Event event = notification.event;

    Dropbox::EventMessage eventMessage;
    eventMessage.event = protocolEvent;
    if(!notification.source.empty())
    {
        eventMessage.source = notification.source.string();
    }
    eventMessage.destination = notification.destination.string();

    //std::cout << "Event: " << Notifier::getEventName(event) << " on " << path << " was triggered." << std::endl;

    ClientEventReporter::messageQueue.enqueue(eventMessage);
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
    struct stat result;
    timespec modificationTime1, modificationTime2;

    fileInfo.path = notification.destination;
    fileInfo.isOpen = false;

    if(stat(fileInfo.path.string().c_str(), &result) == 0)
    {
        modificationTime1 = result.st_mtim;
    }
    else
    {
        return false;
        //throw runtime_error("Determining modification time has failed. Path : " + fileInfo.path.string() + "\n");
    }

    auto it = ClientEventReporter::allFilesInfo.find(fileInfo);
    if(it == ClientEventReporter::allFilesInfo.end()){

        for(FileInfo fi: ClientEventReporter::allFilesInfo)
        {
            if(stat(fi.path.string().c_str(), &result) == 0)
            {
                modificationTime2 = result.st_mtim;
            }
            else
            {
                return false;
                //throw runtime_error("Determining modification time has failed. Path : " + fi.path.string() + "\n");
            }

            if(modificationTime2.tv_sec == modificationTime1.tv_sec &&
               fi.path.filename() == fileInfo.path.filename())
            {
                if(checkIfSameFiles(fileInfo.path, fi.path))
                {
                    notification.source = fi.path;
                    return true;
                }
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}

void ClientEventReporter::requestCreation(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    //std::cout << "Creation\n";
    FileInfo fileInfo;

    fileInfo.path = notification.destination;
    fileInfo.isOpen = false;

    // there is no need for creating empty directories, because their size is extremely small
    if(boost::filesystem::is_regular_file(notification.destination) && checkIfCopied(notification))
    {
        //std::cout << "Copied\n";
        makeRequest(notification, Dropbox::ProtocolEvent::COPY);
    }

    if(boost::filesystem::is_directory(notification.destination))
    {
        //std::cout << "Directory creation\n";
        makeRequest(notification, Dropbox::ProtocolEvent::NEW_DIRECTORY);
    }
    else
    {
        //std::cout << "File creation\n";
        makeRequest(notification, Dropbox::ProtocolEvent::NEW_FILE);
    }

    allFilesInfo.insert(fileInfo);
}

void ClientEventReporter::requestDeletion(Notification notification) //creation, modification, moved_to (serwer wie ze trzeba utworzyć plik)
{
    //std::cout << "Deletion\n";

    for(auto it = ClientEventReporter::allFilesInfo.begin(); it != ClientEventReporter::allFilesInfo.end(); ++it)
    {
        if(it->path == notification.destination)
        {
            ClientEventReporter::allFilesInfo.erase(it);
        }
    }

    makeRequest(notification, Dropbox::ProtocolEvent::DELETE);
}

bool ClientEventReporter::isInternalMove(Notification &notification)
{
    FileInfo fileInfo;

    fileInfo.path = notification.destination;
    fileInfo.isOpen = false;

    auto it = ClientEventReporter::allFilesInfo.find(fileInfo);
    if(it == ClientEventReporter::allFilesInfo.end())
    {
        return false;
    }
    else
    {
        notification.source = it->path;
        ClientEventReporter::allFilesInfo.erase(it);
        ClientEventReporter::allFilesInfo.insert(fileInfo);
        return true;
    }
}

void ClientEventReporter::requestMoveFrom(Notification notification)
{
    if(notification.event == Event::outward_move)
    {
        //std::cout << "Outward move\n";
        requestDeletion(notification);
    }
    else
    {
       // std::cout << "InternalMove\n";
        makeRequest(notification, Dropbox::ProtocolEvent::MOVE);
    }

}

void ClientEventReporter::requestMoveTo(Notification notification)
{
    if(isInternalMove(notification))
    {
        //std::cout << "InternalMove\n";
        makeRequest(notification, Dropbox::ProtocolEvent::MOVE);
    }
    else
    {
        //std::cout << "Inward move\n";
        ClientEventReporter::requestCreation(notification);
    }
}

bool ClientEventReporter::isIgnored(boost::filesystem::path path)
{

    if(path.filename().string()[0]  == '.')
    {
        return true;
    }

    auto it2 = permanentlyIgnored.find(path.string());
    if(it2 != permanentlyIgnored.end())
    {
        return true;
    }
    auto it = ignoredPaths.find(path.string());
    if(it == ignoredPaths.end())
    {
        return false;
    }
    else
    {
        ignoredPaths.erase(it);
        return true;
    }
}

void ClientEventReporter::chooseRequest(Notification notification)
{

    if(ClientEventReporter::isIgnored(notification.destination))
    {
        ClientEventReporter::isIgnored(notification.source);

        return;
    }

    switch(notification.event)
    {
        case Event::open:

            try
            {
                saveAsOpen(notification);
            }
            catch(std::runtime_error &e)
            {
                std::cout << e.what() << std::endl;
            }
            break;

        case Event::close:
        case Event::close_nowrite:
        case Event::close_write:
            saveAsClosed(notification);
            break;

        case Event::remove:
        case Event::remove_self:
        case Event::remove_dir:
        case Event::remove_self_dir:
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
        default:
            break;
    }
}
