#include "../DropboxClient.h"

//
// Created by ns on 11.01.18.
//
#include<string>
using namespace std;
int main()
{
    const string ip = "192.168.8.105";
    const short port = 8888;
    const string path("/home/ns/Documents/Studia/semestr5/SK2/Dropbox");
    DropboxClient dropboxClient(ip, port, path);
    dropboxClient.createDirectory("temp/dupa/stefan");
    dropboxClient.moveFile("temp/dupa", "oko");
}
