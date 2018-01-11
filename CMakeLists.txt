cmake_minimum_required(VERSION 3.8)
project(Dropbox)

set(SOURCE_FILES TcpSocket.cpp TcpServer.cpp Descriptor.cpp POSIXError.cpp POSIXError.h
        InotifyInterface/Inotify.cpp InotifyInterface/Notifier.cpp InotifyInterface/EventType.cpp
        Dropbox.h Dropbox.cpp)
set(SERVER_SOURCE_FILES ${SOURCE_FILES} DropboxServer.cpp DropboxServer.h)
set(CLIENT_SOURCE_FILES ${SOURCE_FILES} DropboxClient.cpp DropboxClient.h)
# TODO add clientEventReporter after fixing
set(TEST_SOURCE_FILES test/test.cpp ${SOURCE_FILES} ${SERVER_SOURCE_FILES} ${CLIENT_SOURCE_FILES})
# test inotify
set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.58.0 COMPONENTS system filesystem)
find_package(Threads)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(server.o ${SERVER_SOURCE_FILES} server.cpp)
    add_executable(client.o ${CLIENT_SOURCE_FILES} client.cpp)
    add_executable(main.o ${SOURCE_FILES} ClientEventReporter.h ClientEventReporter.cpp test/main.cpp)
    add_executable(test.o ${TEST_SOURCE_FILES} test/test.cpp)
    target_link_libraries(server.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(client.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(test.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    # TODO remove main after fixing clientEventReporter
    target_link_libraries(main.o ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    #target_link_libraries(Dropbox )
endif ()

