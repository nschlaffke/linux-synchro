cmake_minimum_required(VERSION 3.8)
project(Dropbox)

set(SOURCE_FILES TcpSocket.cpp TcpServer.cpp Descriptor.cpp POSIXError.cpp POSIXError.h
        InotifyInterface/Inotify.cpp InotifyInterface/Notifier.cpp InotifyInterface/EventType.cpp
        Dropbox.h Dropbox.cpp SafeQueue.h ClientEventReporter.h ClientEventReporter.cpp)
set(SERVER_SOURCE_FILES ${SOURCE_FILES} DropboxServer.cpp DropboxServer.h)
set(CLIENT_SOURCE_FILES ${SOURCE_FILES} DropboxClient.cpp DropboxClient.h)
set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 COMPONENTS system filesystem)
find_package(Threads)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(server.o ${SERVER_SOURCE_FILES} server.cpp)
    add_executable(client.o ${CLIENT_SOURCE_FILES} client.cpp)
    add_executable(queue.o ${SOURCE_FILES} test/queue.cpp)
    target_link_libraries(server.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(client.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(queue.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif ()

