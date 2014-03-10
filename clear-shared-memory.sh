echo "All instances of netbase need to be stopped to clear memory"
sudo killall netbase

# shared memory limit
# export shmmax=2147483648 	# 2GB
export shmmax=4294967296	# 4GB
# export shmmax=6442450944	# 6GB
# export shmmax=8589934592	# 8GB
# export shmmax=17179869184	# 16GB
# export shmmax=34359738368	# 32GB
# export shmmax=68719476736 	# 64GB

# todo : use/set in netbase

# allow bigger shared memory
if [ $1 ] 
	then
		# echo "kernel.shmmax=$shmmax" >> /etc/sysctl.conf
		# echo "kernel.shmall=$shmmax" >> /etc/sysctl.conf
	if [ $APPLE ] 
	then
sudo sysctl -w kern.sysv.shmmax=$shmmax && sudo sysctl -w kern.sysv.shmall=$shmmax
	else 
sysctl -w kernel.shmall=$shmmax && sysctl -w kernel.shmmax=$shmmax
#sudo sysctl -w sys.kernel.shmmax=$shmmax && sudo sysctl -w sys.kernel.shmmall=$shmmax
	fi
fi

ipcclean

sudo ipcrm -M "0x00069190"
sudo ipcrm -M "0x00069191"
sudo ipcrm -M "0x00069192"
sudo ipcrm -M "0x00069193"
sudo ipcrm -M "0x00069194"
sudo ipcrm -M "0x00069195"
# sudo ipcrm -M 0x57020303 #netbase
# sudo ipcrm -M 0x00000000


ipcs -a

echo "shared memory cleaned" #return 0 for make!
