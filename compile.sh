cd ~/netbase
killall -SIGKILL gdb
killall -SIGKILL gdb-i386-apple-darwin
killall -SIGKILL netbase

platform=`uname`

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

#sed -i 's/80/81/' src/webserver.cpp 
#sed -i  's/300\*/30*/' src/netbase.hpp 

g++ -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -lreadline -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o netbase  && ./netbase exit $@


if [[ $platform == 'Darwin' ]]; then
cp netbase blueprints-netbase/lib/mac/libNetbase.dylib
cp netbase blueprints-netbase/bin/netbase-mac
else
g++ -fPIC -shared -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -lreadline -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o bin/libNetbase.so	
cp netbase blueprints-netbase/bin
cp netbase blueprints-netbase/lib/linux/libNetbase.a
cp bin/libNetbase.so blueprints-netbase/lib/linux-x86-64/
cp bin/libNetbase.so blueprints-netbase/lib/linux/
fi
cd blueprints-netbase; git pull --all; git commit -a -m "Updated library" && git push --all && git status
cd -

#  -I/opt/local/include BOOST

# --exclude src/netbase-ruby.cpp TODO
# mv src/netbase-ruby.cpp.x src/netbase-ruby.cpp


# cp netbase src/jni # todo

# ln -s /usr/lib/x86_64-linux-gnu/libsqlite3.so libsqlite3.dylib
#g++ -g -w src/*.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@
