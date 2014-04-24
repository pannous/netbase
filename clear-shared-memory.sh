echo "All instances of netbase need to be stopped to clear memory"
sudo killall netbase

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
