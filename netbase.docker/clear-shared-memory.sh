echo "All instances of netbase need to be stopped to clear memory"
killall netbase

ipcclean

ipcrm -M "0x00069190"
ipcrm -M "0x00069191"
ipcrm -M "0x00069192"
ipcrm -M "0x00069193"
ipcrm -M "0x00069194"
ipcrm -M "0x00069195"
ipcrm -M "0x00069196"
# ipcrm -M 0x57020303 #netbase
# ipcrm -M 0x00000000

ipcs -a

echo "shared memory cleaned" #return 0 for make!
