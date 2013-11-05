cd ~/netbase
sudo killall -SIGKILL gdb
sudo killall -SIGKILL gdb-i386-apple-darwin
sudo killall -SIGKILL netbase

ruby_include=$RVM_HOME/src/ruby-$RUBY_VERSION/include/ruby/

options="-m64 --debug -c -g -w -MMD -MP" #-MF #64bit
#   -s
#  -L$RVM_HOME/usr/lib -I$ruby_include -Ibuild/

#g++ $options -MF build/query.o.d -o build/query.o src/query.cpp
#g++ $options -MF build/init.o.d -o build/init.o src/init.cpp
#g++ $options -MF build/relations.o.d -o build/relations.o src/relations.cpp
#g++ $options -MF build/export.o.d -o build/export.o src/export.cpp
#g++ $options -MF build/console.o.d -o build/console.o src/console.cpp
#g++ $options -MF build/import.o.d -o build/import.o src/import.cpp
#g++ $options -MF build/md5.o.d -o build/md5.o src/md5.cpp
#g++ $options -MF build/netbase.o.d -o build/netbase.o src/netbase.cpp
#g++ $options -MF build/tests.o.d -o build/tests.o src/tests.cpp
#g++ $options -MF build/util.o.d -o build/util.o src/util.cpp
#g++ $options -MF build/NetbaseJNI.o.d -o build/NetbaseJNI.o src/jni/NetbaseJNI.cpp -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include
#g++ $options -L/Users/me/.rvm/usr/lib -I$ruby_include  -MF build/NetbaseRuby.o.d -o build/NetbaseRuby.o src/netbase-ruby.cpp

mv src/netbase-ruby.cpp src/netbase-ruby.cpp.x # Stupid workaround

g++ -I/opt/local/include -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -lreadline -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o netbase  && ./netbase $@

# --exclude src/netbase-ruby.cpp TODO
# mv src/netbase-ruby.cpp.x src/netbase-ruby.cpp


# cp netbase src/jni # todo

# ln -s /usr/lib/x86_64-linux-gnu/libsqlite3.so libsqlite3.dylib
#g++ -g -w src/*.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@