#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <stdexcept>
#include <csignal>

std::ofstream file;
std::string app = "";

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;

    file << app << '\n';
    file.close();

    exit(signum);
}

int main(int argc, const char *argv[])
{

    if(argc <= 1)
    {
        throw std::runtime_error("The file name was not specified");
    }

    std::string fileName = argv[1], input = "";
    signal(SIGINT, signalHandler);

    file.open(fileName, std::ios::out | std::ios::app);
    if (!file.good())
    {
        throw std::runtime_error("Opening the file has failed");
    }

    std::cout << "Type your text below. Type '~' in order to exit.\n\n";

    do
    {
        std::cin >> input;
        if(input == "~")
        {
            break;
        }

        app += input + " ";
    }while (true);

    file << app << "\n";
    file.close();

    return 0;
}