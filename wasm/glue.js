let SHARED_MEMORY_ROOT=0x20000000//1000000 // todo
// let ONE_GB=1024*1024*1024 
let ONE_GB=1000000000
let SHARED_MEMORY_ROOT_KEY=0x69190

function _semget(segment) {
    console.log("_semget",segment)
}

function _shmat(address,amoung) {// todo amount 4GB max in wasm!
  console.log("_shmat",arguments)
  return address// from _shmget, nothing todo!
}

function _shmget(segment) {
  console.log("_shmget",segment)
  if(segment==SHARED_MEMORY_ROOT_KEY)
    return SHARED_MEMORY_ROOT
  else if(segment==SHARED_MEMORY_ROOT_KEY+1)
    return ONE_GB// SHARED_MEMORY_ROOT+200000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+2)
    return ONE_GB+300000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+3)
    return ONE_GB+ONE_GB//400000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+4)
    return ONE_GB+500000000 // todo
  else console.log("UNKNOWN SHARED_MEMORY_ROOT_KEY",segment)
    return ONE_GB*3; // todo
}