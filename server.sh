# echo killall -SIGKILL netbase
make -j8
if [ $APPLE ] 
then
  open http://localhost:8181/html/1
  sudo ~/netbase/netbase server :de $@ 
else
  open http://localhost:8080/html/1
  sudo ~/netbase/netbase server $@ 
fi

