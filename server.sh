killall -SIGKILL netbase
make -j8
if [ $APPLE ] 
then
  open http://localhost:8181/html/1
  # open /Applications/Firefox.app
  sudo ~/netbase/netbase server :de $@ 
else
  sudo ~/netbase/netbase server $@ 
fi

