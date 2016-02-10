killall -SIGKILL netbase
make -j8
if [ $APPLE ] 
then
  # open http://localhost:81/html/1
  sudo ~/netbase/netbase server :de $@ 
else
  sudo ~/netbase/netbase server $@ 
fi

