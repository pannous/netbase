echo killall -SIGKILL netbase
#sudo ~/netbase/netbase server $@ 
screen -dmS netbase # create detatched
screen -x -RR netbase -X stuff 'ls^M'
# screen -x -RR netbase  # reattach or create   
# leave with ctrl-a ctrl-d "ade!"

