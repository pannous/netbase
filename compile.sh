sudo killall -SIGKILL gdb-i386-apple-darwin
sudo killall -SIGKILL netbase

options="--debug -c -g -w -s -MMD -MP" #-MF
#  -arch i686
# -arch x86-64 -Xarch-x86-64
# x86_64-apple-darwin

g++ $options -MF build/query.o.d -o build/query.o src/query.cpp
g++ $options -MF build/init.o.d -o build/init.o src/init.cpp
g++ $options -MF build/relations.o.d -o build/relations.o src/relations.cpp
g++ $options -MF build/export.o.d -o build/export.o src/export.cpp
g++ $options -MF build/console.o.d -o build/console.o src/console.cpp
g++ $options -MF build/import.o.d -o build/import.o src/import.cpp
g++ $options -MF build/md5.o.d -o build/md5.o src/md5.cpp
g++ $options -MF build/netbase.o.d -o build/netbase.o src/netbase.cpp
g++ $options -MF build/tests.o.d -o build/tests.o src/tests.cpp
g++ $options -MF build/util.o.d -o build/util.o src/util.cpp
g++ -g -w src/*.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@ 
# ln -s /usr/lib/x86_64-linux-gnu/libsqlite3.so libsqlite3.dylib