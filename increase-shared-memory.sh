# increase shared memory limit
# todo : use/set in netbase

shared_memory_2GB=2147483648  
shared_memory_4GB=4294967296  
shared_memory_6GB=6442450944	
shared_memory_8GB=8589934592  
shared_memory_16GB=17179869184
shared_memory_32GB=34359738368
shared_memory_64GB=68719476736

# allow bigger shared memory
if [ $APPLE ] 
then
  export shmmax=$shared_memory_4GB
  let "shmall=$shmmax/4096" # pages,  4096 bytes per page
  sudo sysctl -w kern.sysv.shmmax=$shmmax
  sudo sysctl -w kern.sysv.shmall=$shmall
  # echo "kernel.sysv.shmmax        = $shmmax" | sudo tee -a /etc/sysctl.conf
  # echo "kernel.sysv.shmall        = $shmall" | sudo tee -a /etc/sysctl.conf
else 
  export shmmax=$shared_memory_32GB
  let "shmall=$shmmax/4096" # pages,  4096 bytes per page
  sysctl -w kernel.shmmax=$shmmax
  sysctl -w kernel.shmall=$shmall
  # sudo sysctl -w sys.kernel.shmmax=$shmmax
  #   sudo sysctl -w sys.kernel.shmmall=$shmmax
  # echo "kernel.shmmax              = $shmmax" | sudo tee --append /etc/sysctl.conf
  # echo "kernel.shmall              = $shmmax" | sudo tee --append /etc/sysctl.conf
fi
