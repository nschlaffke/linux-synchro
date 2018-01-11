#include "../DropboxClient.h"

//
// Created by ns on 11.01.18.
//
#include <boost/filesystem.hpp>
#include <iostream>

using namespace std;
int main()
{
    using namespace boost::filesystem;
    string directoryPath("/home/ns/Documents/Studia/semestr5/SK2/Dropbox");
    boost::filesystem::path dir(directoryPath);
    /*
    const string ip = "192.168.8.105";
    const short port = 8888;
    DropboxClient dropboxClient(ip, port, path);
    dropboxClient.createDirectory("temp/dupa/stefan");
    dropboxClient.moveFile("temp/dupa", "oko");
     */
    boost::filesystem::recursive_directory_iterator end;
    for(recursive_directory_iterator i(directoryPath); i!=end; i++)
    {
        string Spath(i->path().c_str());
        Spath.erase(0, directoryPath.length());
        cout << "Ścieżka: " << Spath << endl;
        cout << "Nazwa: " << i->path().filename() << endl;
        cout << "Folder: " << boost::filesystem::is_directory(i->path()) << endl;
        cout << "Plik: " << boost::filesystem::is_regular_file(i->path()) << endl;
        cout << "Czy istnieje: " << boost::filesystem::exists(i->path()) << endl;
        cout << endl;
    }
}
