cmake_minimum_required(VERSION 3.8)
project(Dropbox)

set(SOURCE_FILES ClientEventReporter.cpp TcpSocket.cpp Server.cpp Client.cpp TcpServer.cpp Descriptor.cpp POSIXError.cpp POSIXError.h
        InotifyInterface/Inotify.cpp InotifyInterface/Notifier.cpp InotifyInterface/EventType.cpp)
# test main app
set(SOURCE_FILES ${SOURCE_FILES} main.cpp)
# test inotify
set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 COMPONENTS system filesystem)
find_package( Threads )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(Dropbox ${SOURCE_FILES})
    target_link_libraries(Dropbox ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    #target_link_libraries(Dropbox )
endif()

