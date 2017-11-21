cmake_minimum_required(VERSION 3.8)
project(Dropbox)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp TcpSocket.cpp Server.cpp Client.cpp TcpServer.h)
add_executable(Dropbox ${SOURCE_FILES})