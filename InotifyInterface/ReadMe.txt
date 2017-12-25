In order to test inotify:
1. Compile .cpp files with command: g++ EventType.cpp Inotify.cpp Notifier.cpp test.cpp -std=c++11 -l boost_system -l boost_filesystem -o test.o.
2. run test.o with command ./test.o directory_path.
3. Now, you can modify files in the directory you have provided the application with. Every single modification will be immediately shown in the console.
4. The format of a single notification: "Event: [inotify_event_type]([event_number in decimal]) on [relative path to modified file] was triggered\n".
