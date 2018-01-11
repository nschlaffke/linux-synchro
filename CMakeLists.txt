cmake_minimum_required(VERSION 3.8)
project(Dropbox)

set(SOURCE_FILES TcpSocket.cpp TcpServer.cpp Descriptor.cpp POSIXError.cpp POSIXError.h
        InotifyInterface/Inotify.cpp InotifyInterface/Notifier.cpp InotifyInterface/EventType.cpp)
# test main app
set(SERVER_SOURCE_FILES ${SOURCE_FILES} server.cpp Dropbox.h DropboxServer.cpp DropboxServer.h Dropbox.cpp)
set(CLIENT_SOURCE_FILES ${SOURCE_FILES} client.cpp Dropbox.h DropboxClient.cpp DropboxClient.h Dropbox.cpp)
# test inotify
set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 COMPONENTS system filesystem)
find_package(Threads)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(server.o ${SERVER_SOURCE_FILES})
    add_executable(client.o ${CLIENT_SOURCE_FILES})
    add_executable(main.o ${SOURCE_FILES} ClientEventReporter.h ClientEventReporter.cpp main.cpp)
    target_link_libraries(server.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(client.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(main.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    #target_link_libraries(Dropbox )
endif ()

