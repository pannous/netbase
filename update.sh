date

cd ~/netbase  
git commit -a --allow-empty-message -m \"''\" && git push --all && git status

 # server="kfluegge@pannous.jo"
server="root@quasiris.dev" # german!
# server="root@quasiris.big" # all

echo using server $server
# find . -mmin -12 |grep -v nbproject| grep -v development |grep -v to_update|grep -v bin|grep -v build|grep -v compile.sh|grep -v update.sh|grep -v netbase.dSYM|grep -v iws |grep -v .txt|grep -v git| grep -v "./netbase$"> to_update                        
# 
# exec < to_update
# while read line
# do
# if [[ -f $line ]]; then
# echo $line
# scp "$line" "$server:/root/netbase/$line"
# fi
# done      


ssh -A "$server" "cd /root/netbase/;git pull"
# ssh -A "$server" "scp -r /root/netbase/src/* root@quasiris.big:/root/netbase/src/"
ssh "$server" "cd /root/netbase/; sed -i 's/300/30/' src/netbase.hpp "
ssh "$server" "cd /root/netbase/; sed -i 's/80/81/' src/webserver.cpp "
ssh "$server" "source .bash_profile; cd /root/netbase/;./compile.sh $@"
ssh "$server" "cd /root/netbase/; nice nohup ./restart-server.sh > logfile.log 2>&1 &"
# rm to_update.tmp                       
date
