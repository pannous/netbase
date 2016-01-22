if [ !$APPLE ]; then rm src/webserver.cpp; git checkout src/webserver.cpp; fi
git pull 
./adjust.sh 
make -j8 
screen -R netbase ./server.sh :de