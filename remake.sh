git pull || exit
./adjust.sh 
make -j8 || exit
#screen -X -S netbase quit
# screen -x -RR netbase 
./server.sh

