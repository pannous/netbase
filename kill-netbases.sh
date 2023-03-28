screen -ls | grep netbase | cut -d. -f1 | awk "{print $1}" | xargs kill 
killall netbase