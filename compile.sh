cd ~/netbase
#killall -SIGKILL gdb
#killall -SIGKILL gdb-i386-apple-darwin
#killall -SIGKILL netbase
eval `ssh-agent -s`
ssh-add ~/.ssh/github_jini
git pull #|| exit
./adjust.sh

platform=`uname`

# ruby_include=$RVM_HOME/src/ruby-$RUBY_VERSION/include/ruby/

# options="-m64 --debug -c -g -w -MMD -MP" #-MF #64bit cannot specify -o with -c or -S with multiple files
options="-m64 --debug  -std=c++1z"  # 0x = old -std=c++11 once no jserver
#   -s -std=c++11 for sorting array!
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

# mv src/netbase-ruby.cpp src/netbase-ruby.cpp.x # Stupid workaround

g++ $options -g -w  src/*.cpp -o netbase -lreadline -lz && ./netbase :exit $@

if [[ $platform == 'Darwin' ]]; then
	# g++ $options -fPIC -shared -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -lreadline -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o bin/libNetbase.so	
	# cp netbase lib/libNetbase.dylib
	cp netbase bin/netbase-mac
else
	g++ $options -fPIC -shared -I$JAVA_HOME/include/$arch -I$JAVA_HOME/include -DNO_READLINE -lreadline -lz -g -w  src/*.cpp src/jni/NetbaseJNI.cpp -o bin/libNetbase.so	
	cp netbase bin/
	cp netbase lib/linux/
	cp netbase lib/linux/libNetbase.a
	cp bin/libNetbase.so lib/linux-x86-64/
	cp bin/libNetbase.so lib/linux/
	cp bin/libNetbase.so lib/
fi
# cp -r bin blueprints-netbase/
# cp -r lib blueprints-netbase/
# cd blueprints-netbase; git pull --all; git commit -a -m "Updated library" && git push --all && git status
cd -

#  -I/opt/local/include BOOST

# --exclude src/netbase-ruby.cpp TODO
# mv src/netbase-ruby.cpp.x src/netbase-ruby.cpp


# cp netbase src/jni # todo

# ln -s /usr/lib/x86_64-linux-gnu/libsqlite3.so libsqlite3.dylib
#g++ -g -w src/*.cpp /usr/lib/libsqlite3.dylib -o netbase  && ./netbase $@
