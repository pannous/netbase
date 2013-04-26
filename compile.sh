#g++ -g -w src/tests.cpp src/util.cpp src/netbase.cpp -o netbase && ./netbase
#g++ -g -w src/tests.cpp src/netbase.cpp -o netbase && ./netbase
# g++ -g -w src/netbase.cpp src/tests.cpp -o netbase && ./netbase
# ./update.sh quit&

sudo killall -SIGKILL gdb-i386-apple-darwin
sudo killall -SIGKILL netbase
#  -arch i686
# -arch x86-64 -Xarch-x86-64
# x86_64-apple-darwin

# options=--debug -c -g -w -s -MMD -MP -MF
options=--debug -c -g -w -s -MMD -MP #-MF
#g++  		-c -g -w -s -MMD -MP -MF build/netbase.o.d -o build/netbase.o src/netbase.cpp
g++ $options -MF build/export.o.d -o build/export.o src/export.cpp
g++ $options -MF build/console.o.d -o build/console.o src/console.cpp
g++ $options -MF build/import.o.d -o build/import.o src/import.cpp
g++ $options -MF build/md5.o.d -o build/md5.o src/md5.cpp
g++ $options -MF build/netbase.o.d -o build/netbase.o src/netbase.cpp
g++ $options -MF build/tests.o.d -o build/tests.o src/tests.cpp
g++ $options -MF build/util.o.d -o build/util.o src/util.cpp
g++ -g -w src/import.cpp src/netbase.cpp src/tests.cpp src/export.cpp src/md5.cpp src/util.cpp src/console.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@
 


#g++ -g -w src/tests.cpp src/util.cpp src/netbase.cpp -o netbase && ./netbase
#g++ -g -w src/netbase.cpp  -o netbase && ./netbase
# g++ -g -w src/*.cpp  -o netbase && ./netbase
# 
# build/export.o.d 
# build/console.o.d 
# build/import.o.d 
# build/md5.o.d 	
# build/netbase.o.d
# build/tests.o.d 
# build/util.o.d 