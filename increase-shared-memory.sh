# increase shared memory limit
# todo : use/set in netbase

# export shmmax=2147483648 	# 2GB
export shmmax=4294967296	# 4GB
# export shmmax=6442450944	# 6GB
# export shmmax=8589934592	# 8GB
# export shmmax=17179869184	# 16GB
# export shmmax=34359738368	# 32GB
# export shmmax=68719476736 # 64GB

# allow bigger shared memory
if [[ $APPLE == 1 ]] 
then
			sudo sysctl -w kern.sysv.shmmax=$shmmax && sudo sysctl -w kern.sysv.shmall=$shmmax
else 
		#echo "kernel.shmmax=$shmmax" >> /etc/sysctl.conf
		#echo "kernel.shmall=$shmmax" >> /etc/sysctl.conf
sudo sysctl -w kernel.shmall=$shmmax && sudo sysctl -w kernel.shmmax=$shmmax
#sudo sysctl -w sys.kernel.shmmax=$shmmax && sudo sysctl -w sys.kernel.shmmall=$shmmax
fi
