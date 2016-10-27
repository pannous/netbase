var child_process=require('child_process') 
var system=child_process.execSync
var ffi = require('ffi');
var struct = require('ref-struct');
var ref = require('ref')
var ArrayType = require('ref-array') // ArrayType(int)=int[] vs char**=ref.refType(charPtr);  var a = new ffi.Array('int', 10)
// var wget = require('node-wget');
// var addons = require('addons'); // for C++ structures, not needed here

var SegfaultHandler = require('segfault-handler');// not always helpful 
SegfaultHandler.registerHandler("crash.log", function(signal, address, stack) {//  before the signal is forwarded 
  console.log(signal)
  console.log(address)
  console.log(stack)
});

// var cb = new ffi.Callback([ 'v', [ ffi.Array('short', 5), 'long' ] ], someFunc)

VOID='void'
BOOL=bool='bool'
DOUBLE='double'
INT=int='int'
LONG=long='long'
POINTER='pointer'
STRING='string'
// charPtr = ref.refType('char');
intPtr=ref.refType('int')
// StringArray = ArrayType('string');
// charPtrPtr = ref.refType(charPtr);
// CharPtrArray = ArrayType(charPtr);

var Node0=struct( {
    id:int,
    name:STRING,
    kind:int,
    statementCount:int,
    firstStatement:int ,
    lastStatement:int,
    value:POINTER ,
  })
Node=ref.refType(Node0)
NodeList=ref.refType(Node)
NodeArray=ArrayType(Node)
// NodeVector='pointer' // not C compatible

var Statement0=struct({
    context:int ,
    subject:int ,
    predicate:int,
    object:int,
    nextSubjectStatement:int,
    nextPredicateStatement:int,
    nextObjectStatement:int,
  })
Statement=ref.refType(Statement0)

netbase = ffi.Library(__dirname+'/lib/netbase', {
  ceil: [ DOUBLE, [ DOUBLE ]/*,abi|async|varargs*/ ],
  printf:[VOID,[STRING]] ,
  allowWipe:[VOID,[]],
  // test:[VOID,[]],
  testAll:[VOID,[]],
  setMemoryLimit:[LONG,[LONG]],
  init:[VOID,[BOOL/*relations*/]], 
  //////////
  import:[VOID,[STRING/*type*/]],
  execute:[ArrayType(Node),[STRING,intPtr]] , 
  learn:[VOID,[STRING]] ,
  // query:[VOID,[STRING]] ,
  add:[Node,[STRING/* nodeName*/]],//, int kind
  addStatement:[Statement,[Node,Node,Node]],
  addStatement4:[Statement,[int,int,int]],
  get:[Node,[int/*NodeId*/]],
  getName:[STRING,[int/* node*/]],
  getText:[STRING,[Node]],
  getData:[POINTER,[int/* node*/]],
  getAbstract:[Node,[STRING]],
  getThe:[Node,[STRING]],
  getProperty:[Node,[Node,STRING,int]],
  getNew:[Node,[STRING]],
  getStatementId:[int,[long/*pointer ?*/]],
  hasNode:[BOOL,[STRING]],
  // import:[VOID,[STRING/*type*/,STRING/*filename*/]],
  setLabel:[VOID,[Node,STRING]],
  setName:[VOID,[INT,STRING]],
  deleteNode:[VOID,[INT]],
  showNode:[VOID,[INT]],
  deleteStatement:[VOID,[INT]],
  has:[Node,[Node,Node]],
  isA:[bool,[Node,Node]],
});
netbase.parse=netbase.execute
netbase.getI=netbase.get
netbase.get=function(id){
  if(typeof id == "object" && id.kind)
    return id
  if(typeof id == "number")
    return netbase.getI(id).deref();
  else 
    return netbase.getAbstract(id).deref()
}
function get(n) {
  if(typeof n=='string')n=netbase.get(n);return n
}
Object.assign(Node.prototype, { 
  show(){ netbase.show(this);console.log(this)},
  save(){ netbase.setName(this.id,this.name);},// copy local string
  delete(){ netbase.deleteNode(this.id)},
  get(prop){return netbase.getProperty(this.ref(),prop,0).deref()},
  is(typ){return netbase.isA(this.ref(),get(typ).ref())},
  has(typ){z=netbase.has(this.ref(),get(typ).ref());return z.address()==0?false:x},
})
require('object.observe')

Object.observe(Node, ( changes ) => {
  console.log( "property "+change.name+" of " + change.object +" was "+change.type+ " to "+ change.object[change.name] );
  if(change.name=="name") netbase.setName(change.object.id,change.object.name);// copy local string
}) // multiple observers:

netbase.query=(x)=>{
  len=ref.alloc(int);
  xs=netbase.execute(x,len);
  xs.length=len.deref()
  console.log("length:"+xs.length)
  return xs.toArray().map(x=>x.deref())
}

// netbase.importTypes={all:'all',labels:'labels',wordnet:'wordnet',freebase:'freebase',geodb:'geodb',location:'geodb',dbpedia:'dbpedia',names:'names',images:'images',wikidata:'wikidata',yago:'yago',amazon:'amazon',test:'test',}
netbase.help=()=>netbase.execute('help')
netbase.importWordnet=()=>netbase.import('wordnet')
netbase.server=()=>netbase.execute(":server")

// StructType.show=function(it) {console.log(it.deref())}
netbase.show=function(it) {
  if(!it)it="<null>"
  else netbase.showNode(it.id)
	if(it.type && it.type.toString()=='[StructType]')it=it.deref()
	console.log(it)
}

netbase.shared_memory={}
KB=1024
MB=1024*1024
GB=1024*1024*1024  

// ******************** INIT NEBASE !! *********************************
old=process.cwd()
process.chdir(process.env['HOME']+"/netbase")
netbase.allowWipe()
netbase.init(true)
if(!netbase.hasNode('bug'))netbase.import.async('wordnet', () => {});
process.chdir(old)
// *********************************************************************

// netbase.execute("help")
netbase.learn("a.b=c")
b=netbase.get('hi')

// b=netbase.get(5)
// netbase.show(b)
// c=netbase.query("bug")
// netbase.show(c)
// netbase.testAll()

function importAndDownload(type,file){
  if(!type)type='all'
    if(file)netbase.import(type,file)
    if(!file){
      system( __dirname+"/import/download-"+type+".sh")
      netbase.import(type)
    }
}
// netbase.server()
// if (fun_object.isNull()) 
module.exports = {
  get:netbase.get,
  query:netbase.query,
  test:netbase.test,
  import:importAndDownload,
}
