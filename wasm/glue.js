let SHARED_MEMORY_ROOT=0x20000000//1000000 // todo
// let CHUNK=1024*1024*1024 
// let ONE_GB=1000000000
let CHUNK=100000000

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
    return CHUNK// SHARED_MEMORY_ROOT+200000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+2)
    return CHUNK+300000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+3)
    return CHUNK+CHUNK//400000000 // todo
  else if(segment==SHARED_MEMORY_ROOT_KEY+4)
    return CHUNK+500000000 // todo
  else console.log("UNKNOWN SHARED_MEMORY_ROOT_KEY",segment)
    return CHUNK*3; // todo
}