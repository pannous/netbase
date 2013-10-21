cd ~/netbase
sudo killall -SIGKILL gdb
sudo killall -SIGKILL gdb-i386-apple-darwin
sudo killall -SIGKILL netbase

ruby_include=/Users/me/.rvm/src/ruby-1.9.2-p318/include/ruby/

options="-m64 --debug -c -g -w -s -MMD -MP" #-MF #64bit

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
g++ $options -MF build/NetbaseJNI.o.d -o build/NetbaseJNI.o src/jni/NetbaseJNI.cpp -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include
g++ $options -L/Users/me/.rvm/usr/lib -I$ruby_include  -MF build/NetbaseRuby.o.d -o build/NetbaseRuby.o src/netbase-ruby.cpp 

#g++ -g -w src/*.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@ 

g++ -L/Users/me/.rvm/usr/lib -I$ruby_include -Ibuild/  -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -lreadline -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o netbase  && ./netbase $@ 
# cp netbase src/jni # todo
 # --exclude src/netbase-ruby.cpp
# ln -s /usr/lib/x86_64-linux-gnu/libsqlite3.so libsqlite3.dylib
