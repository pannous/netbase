echo killall -SIGKILL netbase
# killall netbase   # keep other sessions running, just kill screens:
screen -ls | grep netbase | cut -d. -f1 | awk "{print $1}" | xargs kill
#sudo ~/netbase/netbase server $@ 
screen -dmS netbase # create detatched
screen -x -RR netbase -X stuff 'sudo ./netbase server^M'
screen -X hardcopy screen.dump  # print 
cat screen.dump
tail screenlog.0
screen -ls
# screen -x -RR netbase  # reattach or create   
# leave with ctrl-a ctrl-d "ade!"

