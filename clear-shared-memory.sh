# allow bigger shared memory
echo "All instances of netbase need to be stopped to clear memory"
sudo killall netbase
echo linux
# sudo /sbin/sysctl -w kernel.shmmax=4294967296
# sudo /sbin/sysctl -w kernel.shmall=4294967296
#  32 GB !!!
sudo /sbin/sysctl -w kernel.shmmax=34359738368 #32GB
sudo /sbin/sysctl -w kernel.shmall=34359738368

echo mac
# sudo sysctl -w kern.sysv.shmmax=4294967296 #4GB
# sudo sysctl -w kern.sysv.shmall=4294967296
# sudo sysctl -w kern.sysv.shmmax=6442450944 #6GB
# sudo sysctl -w kern.sysv.shmall=6442450944
sudo sysctl -w kern.sysv.shmmax=8589934592 # 8GB
sudo sysctl -w kern.sysv.shmall=8589934592
# sudo sysctl -w kern.sysv.shmmax=34359738368 # 32GB
# sudo sysctl -w kern.sysv.shmall=34359738368

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
