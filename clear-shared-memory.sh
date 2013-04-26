# allow bigger shared memory
# linux
sudo killall netbase
sudo /sbin/sysctl -w kernel.shmmax=2147483648
sudo /sbin/sysctl -w kernel.shmall=2147483648
# mac
sudo sysctl -w kern.sysv.shmmax=2147483648
sudo sysctl -w kern.sysv.shmall=2147483648

ipcclean
sudo ipcrm -M 0x0410669190
# sudo ipcrm -M 0x57020303 #netbase
# sudo ipcrm -M 0x00000000


ipcs -a

echo "shared memory cleaned" #return 0 for make!
