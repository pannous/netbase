// Netbase Graph Viewer :: version 1.2.8
// © 2010 - 2023 Pannous

var div;// results
var editMode=false;
var link_name=true;
var server="/";//html/"; 
if(document.location.href.match(/file:/))
	server="http://netbase.pannous.com/"
else server=document.location.origin+"/html/"
	// server="http://87.118.71.26:81/" // for quasiris demo
	// server="http://big.netbase.pannous.com:81/"
	// server="http://de.netbase.pannous.com:81/"

var search = window.location.search;
var inline=false // compact view, embedded
var find_entities=false // full text netbase search
var abstract=-102;// Summary nodes
var filterIds=[-50,-11343,-10646,-10508,-10910,-11566,-10268, -10950, -10349, -11006, -10269, -10409, -11017, -10691, -10906, -11005, -10949, -10734, -11207, 12209159,-10487,-10373,4167410,-11978,-11315,/*10k*/ -101343,-100646,-100508,-100910,-101566,-100268, -100950, -100349, -101006, -100269,-100263, -100244, -100409, -101017, -100691, -100906, -101005, -100213, -100949, -100734, -101207,-100487,-100373,-101978,-101284,-101315,-100998];//  BnF-ID  etc

function setInline() {inline=true;find_entities=true}

function br(elem){
	if(!elem)elem=div;
	elem.appendChild( document.createElement("br"));
}

function append(tag,elem,clazz){
	var tl = document.createElement(tag);
	if(clazz) tl.setAttribute("class",clazz);
	elem.appendChild(tl);
	return tl;
}

function appendText(text,elem){
	if(!elem)elem=div;
	elem.appendChild(document.createTextNode(text));
	return elem;
}
	
function makeRow(elem){
	td=append("td",elem);
	td.width="30%" //400px"
	return td;
}

var nolink="border: 0 none; cursor: pointer;text-decoration:none;"
var bold="font-decoration:bold;"
var tiny=nolink+"font-size:8px;"
var big="font-size:22px;"
var blue= "color:darkblue;"
var black= "color:black;"

function makeLink(name,url,elem)
{
	if(!elem)elem=div;
	var a = document.createElement("a");
	x=document.createTextNode(name.replace(/\$(....)/,"&#x$1;"));
	a.href=url;
	if(inline) a.target="_blank"
	a.style=nolink+black
	if(name=="x") a.setAttribute("onclick","return confirm('really delete?')");
	a.setAttribute("rel","nofollow")
	a.appendChild(x);
	elem.appendChild(a);
	elem.appendChild(document.createTextNode(" "));
	return a; 
}

// function fixUmlauts(x) {
//     return x.replace(/ u([0-9A-Fa-f]{4})/g, function() { return String.fromCharCode(parseInt(arguments[1], 16)); })
// };


function filterStatement(statement){
  if(filterIds.indexOf(statement.pid)>=0)return true;
  if(statement.sid==-666)return true;// Error/Missing
	if(statement.predicate=="nr in context")return true; 
	if(statement.predicate.match(/ID/))return true;
	if(statement.predicate.match(/GND/))return true;
	if(statement.predicate=="DDC")return true;
	if(statement.predicate=="VIAF")return true;	
	if(statement.subject=="◊")return true;
	if(statement.predicate=="◊")return true;
	if(statement.object=="◊")return true;
	if(statement.subject.startsWith("http"))return true;
	if(statement.object.match(/rdf/))return true;
	if(statement.object.match(/ObjectProperty/))return true;
  if(statement.object.match(/wiki/i) && inline)return true;//  Wikimedia-Begriffsklärungsseite wikimedia.org ...
	if(1+statement.subject>1)return true;
}

function editLinks(predicate,statement,elem)
{
		makeLink("x",document.URL.replace(/html.*/,"")+"!delete $"+statement.id,predicate).style=tiny;
		makeLink(" ^",clean(document.URL).replace(/html/,"html/short")+"."+statement.predicate,predicate).style=tiny;// filter
		makeLink(" -",document.URL+" -"+statement.predicate,predicate).style=tiny;// filter
		makeLink(" +",statement.sid+" include "+statement.predicate,predicate).style=tiny;// filter
		makeLink(" --",statement.sid+" exclude "+statement.predicate,predicate).style=tiny;// filter
		makeLink(" -!","exclude "+statement.predicate,predicate).style=tiny;// filter
}


function fixName(name){
	name=name.replace("_"," ")
	name=name.replace(/%20/g," ")
	return name
}

var num=0;

function makeStatement(statement,elem,entity)
{  // if Wikimedia-Kategorie  BREAK!
  if(filterStatement(statement))return;
	if(statement.predicate.match(/Wappen/i))addImage(statement.object,div);
	if(statement.predicate.match(/Flagge/i))addImage(statement.object,div);
	if(statement.predicate.match(/karte/i))addImage(statement.object,div);
	if(statement.predicate.match(/image/))addImage(statement.object,div);
	if(statement.predicate.match(/bild/i))addImage(statement.object,div);
	if(statement.object.match(/\.svg/))addImage(statement.object,div);
	if(statement.object.match(/\.png/))addImage(statement.object,div);
	if(statement.object.match(/\.jpg/))addImage(statement.object,div);
	if(statement.object.match(/\.bmp/))addImage(statement.object,div);
	var top = document.createElement("tr");
  if(++num % 2){
  	top.classList.add('even') //.class="even"
	}  else {
  	top.classList.add('odd') //.class="odd"
	}
	if(!inline || statement.sid!=entity.id)
		makeLink(fixName(statement.subject),server+statement.sid,makeRow(top));

	predicate=	makeRow(top)
	if(editMode)editLinks(predicate,statement,elem)
	makeLink(statement.predicate,server+statement.pid,predicate);
	if(!inline || statement.oid!=entity.id){
		var objectUrl=statement.object.startsWith("http")?statement.object:server+statement.oid;
		if(statement.predicate.match("Koord"))
			objectUrl="https://www.mapquest.com/latlng/"+statement.object.replace(" ","")

		var x=makeLink(fixName(statement.object), objectUrl, makeRow(top));
	  if(editMode) makeLink(" ^",server+ statement.predicate+":"+statement.object,x).style=tiny;// filter
	}
	elem.appendChild(top);
}


// MD5 for wiki images
!function(){"use strict";function t(t){if(t)d[0]=d[16]=d[1]=d[2]=d[3]=d[4]=d[5]=d[6]=d[7]=d[8]=d[9]=d[10]=d[11]=d[12]=d[13]=d[14]=d[15]=0,this.blocks=d,this.buffer8=l;else if(a){var r=new ArrayBuffer(68);this.buffer8=new Uint8Array(r),this.blocks=new Uint32Array(r)}else this.blocks=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];this.h0=this.h1=this.h2=this.h3=this.start=this.bytes=this.hBytes=0,this.finalized=this.hashed=!1,this.first=!0}var r="input is invalid type",e="object"==typeof window,i=e?window:{};i.JS_MD5_NO_WINDOW&&(e=!1);var s=!e&&"object"==typeof self,h=!i.JS_MD5_NO_NODE_JS&&"object"==typeof process&&process.versions&&process.versions.node;h?i=global:s&&(i=self);var f=!i.JS_MD5_NO_COMMON_JS&&"object"==typeof module&&module.exports,o="function"==typeof define&&define.amd,a=!i.JS_MD5_NO_ARRAY_BUFFER&&"undefined"!=typeof ArrayBuffer,n="0123456789abcdef".split(""),u=[128,32768,8388608,-2147483648],y=[0,8,16,24],c=["hex","array","digest","buffer","arrayBuffer","base64"],p="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".split(""),d=[],l;if(a){var A=new ArrayBuffer(68);l=new Uint8Array(A),d=new Uint32Array(A)}!i.JS_MD5_NO_NODE_JS&&Array.isArray||(Array.isArray=function(t){return"[object Array]"===Object.prototype.toString.call(t)}),!a||!i.JS_MD5_NO_ARRAY_BUFFER_IS_VIEW&&ArrayBuffer.isView||(ArrayBuffer.isView=function(t){return"object"==typeof t&&t.buffer&&t.buffer.constructor===ArrayBuffer});var b=function(r){return function(e){return new t(!0).update(e)[r]()}},v=function(){var r=b("hex");h&&(r=w(r)),r.create=function(){return new t},r.update=function(t){return r.create().update(t)};for(var e=0;e<c.length;++e){var i=c[e];r[i]=b(i)}return r},w=function(t){var e=eval("require('crypto')"),i=eval("require('buffer').Buffer"),s=function(s){if("string"==typeof s)return e.createHash("md5").update(s,"utf8").digest("hex");if(null===s||void 0===s)throw r;return s.constructor===ArrayBuffer&&(s=new Uint8Array(s)),Array.isArray(s)||ArrayBuffer.isView(s)||s.constructor===i?e.createHash("md5").update(new i(s)).digest("hex"):t(s)};return s};t.prototype.update=function(t){if(!this.finalized){var e,i=typeof t;if("string"!==i){if("object"!==i)throw r;if(null===t)throw r;if(a&&t.constructor===ArrayBuffer)t=new Uint8Array(t);else if(!(Array.isArray(t)||a&&ArrayBuffer.isView(t)))throw r;e=!0}for(var s,h,f=0,o=t.length,n=this.blocks,u=this.buffer8;f<o;){if(this.hashed&&(this.hashed=!1,n[0]=n[16],n[16]=n[1]=n[2]=n[3]=n[4]=n[5]=n[6]=n[7]=n[8]=n[9]=n[10]=n[11]=n[12]=n[13]=n[14]=n[15]=0),e)if(a)for(h=this.start;f<o&&h<64;++f)u[h++]=t[f];else for(h=this.start;f<o&&h<64;++f)n[h>>2]|=t[f]<<y[3&h++];else if(a)for(h=this.start;f<o&&h<64;++f)(s=t.charCodeAt(f))<128?u[h++]=s:s<2048?(u[h++]=192|s>>6,u[h++]=128|63&s):s<55296||s>=57344?(u[h++]=224|s>>12,u[h++]=128|s>>6&63,u[h++]=128|63&s):(s=65536+((1023&s)<<10|1023&t.charCodeAt(++f)),u[h++]=240|s>>18,u[h++]=128|s>>12&63,u[h++]=128|s>>6&63,u[h++]=128|63&s);else for(h=this.start;f<o&&h<64;++f)(s=t.charCodeAt(f))<128?n[h>>2]|=s<<y[3&h++]:s<2048?(n[h>>2]|=(192|s>>6)<<y[3&h++],n[h>>2]|=(128|63&s)<<y[3&h++]):s<55296||s>=57344?(n[h>>2]|=(224|s>>12)<<y[3&h++],n[h>>2]|=(128|s>>6&63)<<y[3&h++],n[h>>2]|=(128|63&s)<<y[3&h++]):(s=65536+((1023&s)<<10|1023&t.charCodeAt(++f)),n[h>>2]|=(240|s>>18)<<y[3&h++],n[h>>2]|=(128|s>>12&63)<<y[3&h++],n[h>>2]|=(128|s>>6&63)<<y[3&h++],n[h>>2]|=(128|63&s)<<y[3&h++]);this.lastByteIndex=h,this.bytes+=h-this.start,h>=64?(this.start=h-64,this.hash(),this.hashed=!0):this.start=h}return this.bytes>4294967295&&(this.hBytes+=this.bytes/4294967296<<0,this.bytes=this.bytes%4294967296),this}},t.prototype.finalize=function(){if(!this.finalized){this.finalized=!0;var t=this.blocks,r=this.lastByteIndex;t[r>>2]|=u[3&r],r>=56&&(this.hashed||this.hash(),t[0]=t[16],t[16]=t[1]=t[2]=t[3]=t[4]=t[5]=t[6]=t[7]=t[8]=t[9]=t[10]=t[11]=t[12]=t[13]=t[14]=t[15]=0),t[14]=this.bytes<<3,t[15]=this.hBytes<<3|this.bytes>>>29,this.hash()}},t.prototype.hash=function(){var t,r,e,i,s,h,f=this.blocks;this.first?r=((r=((t=((t=f[0]-680876937)<<7|t>>>25)-271733879<<0)^(e=((e=(-271733879^(i=((i=(-1732584194^2004318071&t)+f[1]-117830708)<<12|i>>>20)+t<<0)&(-271733879^t))+f[2]-1126478375)<<17|e>>>15)+i<<0)&(i^t))+f[3]-1316259209)<<22|r>>>10)+e<<0:(t=this.h0,r=this.h1,e=this.h2,r=((r+=((t=((t+=((i=this.h3)^r&(e^i))+f[0]-680876936)<<7|t>>>25)+r<<0)^(e=((e+=(r^(i=((i+=(e^t&(r^e))+f[1]-389564586)<<12|i>>>20)+t<<0)&(t^r))+f[2]+606105819)<<17|e>>>15)+i<<0)&(i^t))+f[3]-1044525330)<<22|r>>>10)+e<<0),r=((r+=((t=((t+=(i^r&(e^i))+f[4]-176418897)<<7|t>>>25)+r<<0)^(e=((e+=(r^(i=((i+=(e^t&(r^e))+f[5]+1200080426)<<12|i>>>20)+t<<0)&(t^r))+f[6]-1473231341)<<17|e>>>15)+i<<0)&(i^t))+f[7]-45705983)<<22|r>>>10)+e<<0,r=((r+=((t=((t+=(i^r&(e^i))+f[8]+1770035416)<<7|t>>>25)+r<<0)^(e=((e+=(r^(i=((i+=(e^t&(r^e))+f[9]-1958414417)<<12|i>>>20)+t<<0)&(t^r))+f[10]-42063)<<17|e>>>15)+i<<0)&(i^t))+f[11]-1990404162)<<22|r>>>10)+e<<0,r=((r+=((t=((t+=(i^r&(e^i))+f[12]+1804603682)<<7|t>>>25)+r<<0)^(e=((e+=(r^(i=((i+=(e^t&(r^e))+f[13]-40341101)<<12|i>>>20)+t<<0)&(t^r))+f[14]-1502002290)<<17|e>>>15)+i<<0)&(i^t))+f[15]+1236535329)<<22|r>>>10)+e<<0,r=((r+=((i=((i+=(r^e&((t=((t+=(e^i&(r^e))+f[1]-165796510)<<5|t>>>27)+r<<0)^r))+f[6]-1069501632)<<9|i>>>23)+t<<0)^t&((e=((e+=(t^r&(i^t))+f[11]+643717713)<<14|e>>>18)+i<<0)^i))+f[0]-373897302)<<20|r>>>12)+e<<0,r=((r+=((i=((i+=(r^e&((t=((t+=(e^i&(r^e))+f[5]-701558691)<<5|t>>>27)+r<<0)^r))+f[10]+38016083)<<9|i>>>23)+t<<0)^t&((e=((e+=(t^r&(i^t))+f[15]-660478335)<<14|e>>>18)+i<<0)^i))+f[4]-405537848)<<20|r>>>12)+e<<0,r=((r+=((i=((i+=(r^e&((t=((t+=(e^i&(r^e))+f[9]+568446438)<<5|t>>>27)+r<<0)^r))+f[14]-1019803690)<<9|i>>>23)+t<<0)^t&((e=((e+=(t^r&(i^t))+f[3]-187363961)<<14|e>>>18)+i<<0)^i))+f[8]+1163531501)<<20|r>>>12)+e<<0,r=((r+=((i=((i+=(r^e&((t=((t+=(e^i&(r^e))+f[13]-1444681467)<<5|t>>>27)+r<<0)^r))+f[2]-51403784)<<9|i>>>23)+t<<0)^t&((e=((e+=(t^r&(i^t))+f[7]+1735328473)<<14|e>>>18)+i<<0)^i))+f[12]-1926607734)<<20|r>>>12)+e<<0,r=((r+=((h=(i=((i+=((s=r^e)^(t=((t+=(s^i)+f[5]-378558)<<4|t>>>28)+r<<0))+f[8]-2022574463)<<11|i>>>21)+t<<0)^t)^(e=((e+=(h^r)+f[11]+1839030562)<<16|e>>>16)+i<<0))+f[14]-35309556)<<23|r>>>9)+e<<0,r=((r+=((h=(i=((i+=((s=r^e)^(t=((t+=(s^i)+f[1]-1530992060)<<4|t>>>28)+r<<0))+f[4]+1272893353)<<11|i>>>21)+t<<0)^t)^(e=((e+=(h^r)+f[7]-155497632)<<16|e>>>16)+i<<0))+f[10]-1094730640)<<23|r>>>9)+e<<0,r=((r+=((h=(i=((i+=((s=r^e)^(t=((t+=(s^i)+f[13]+681279174)<<4|t>>>28)+r<<0))+f[0]-358537222)<<11|i>>>21)+t<<0)^t)^(e=((e+=(h^r)+f[3]-722521979)<<16|e>>>16)+i<<0))+f[6]+76029189)<<23|r>>>9)+e<<0,r=((r+=((h=(i=((i+=((s=r^e)^(t=((t+=(s^i)+f[9]-640364487)<<4|t>>>28)+r<<0))+f[12]-421815835)<<11|i>>>21)+t<<0)^t)^(e=((e+=(h^r)+f[15]+530742520)<<16|e>>>16)+i<<0))+f[2]-995338651)<<23|r>>>9)+e<<0,r=((r+=((i=((i+=(r^((t=((t+=(e^(r|~i))+f[0]-198630844)<<6|t>>>26)+r<<0)|~e))+f[7]+1126891415)<<10|i>>>22)+t<<0)^((e=((e+=(t^(i|~r))+f[14]-1416354905)<<15|e>>>17)+i<<0)|~t))+f[5]-57434055)<<21|r>>>11)+e<<0,r=((r+=((i=((i+=(r^((t=((t+=(e^(r|~i))+f[12]+1700485571)<<6|t>>>26)+r<<0)|~e))+f[3]-1894986606)<<10|i>>>22)+t<<0)^((e=((e+=(t^(i|~r))+f[10]-1051523)<<15|e>>>17)+i<<0)|~t))+f[1]-2054922799)<<21|r>>>11)+e<<0,r=((r+=((i=((i+=(r^((t=((t+=(e^(r|~i))+f[8]+1873313359)<<6|t>>>26)+r<<0)|~e))+f[15]-30611744)<<10|i>>>22)+t<<0)^((e=((e+=(t^(i|~r))+f[6]-1560198380)<<15|e>>>17)+i<<0)|~t))+f[13]+1309151649)<<21|r>>>11)+e<<0,r=((r+=((i=((i+=(r^((t=((t+=(e^(r|~i))+f[4]-145523070)<<6|t>>>26)+r<<0)|~e))+f[11]-1120210379)<<10|i>>>22)+t<<0)^((e=((e+=(t^(i|~r))+f[2]+718787259)<<15|e>>>17)+i<<0)|~t))+f[9]-343485551)<<21|r>>>11)+e<<0,this.first?(this.h0=t+1732584193<<0,this.h1=r-271733879<<0,this.h2=e-1732584194<<0,this.h3=i+271733878<<0,this.first=!1):(this.h0=this.h0+t<<0,this.h1=this.h1+r<<0,this.h2=this.h2+e<<0,this.h3=this.h3+i<<0)},t.prototype.hex=function(){this.finalize();var t=this.h0,r=this.h1,e=this.h2,i=this.h3;return n[t>>4&15]+n[15&t]+n[t>>12&15]+n[t>>8&15]+n[t>>20&15]+n[t>>16&15]+n[t>>28&15]+n[t>>24&15]+n[r>>4&15]+n[15&r]+n[r>>12&15]+n[r>>8&15]+n[r>>20&15]+n[r>>16&15]+n[r>>28&15]+n[r>>24&15]+n[e>>4&15]+n[15&e]+n[e>>12&15]+n[e>>8&15]+n[e>>20&15]+n[e>>16&15]+n[e>>28&15]+n[e>>24&15]+n[i>>4&15]+n[15&i]+n[i>>12&15]+n[i>>8&15]+n[i>>20&15]+n[i>>16&15]+n[i>>28&15]+n[i>>24&15]},t.prototype.toString=t.prototype.hex,t.prototype.digest=function(){this.finalize();var t=this.h0,r=this.h1,e=this.h2,i=this.h3;return[255&t,t>>8&255,t>>16&255,t>>24&255,255&r,r>>8&255,r>>16&255,r>>24&255,255&e,e>>8&255,e>>16&255,e>>24&255,255&i,i>>8&255,i>>16&255,i>>24&255]},t.prototype.array=t.prototype.digest,t.prototype.arrayBuffer=function(){this.finalize();var t=new ArrayBuffer(16),r=new Uint32Array(t);return r[0]=this.h0,r[1]=this.h1,r[2]=this.h2,r[3]=this.h3,t},t.prototype.buffer=t.prototype.arrayBuffer,t.prototype.base64=function(){for(var t,r,e,i="",s=this.array(),h=0;h<15;)t=s[h++],r=s[h++],e=s[h++],i+=p[t>>>2]+p[63&(t<<4|r>>>4)]+p[63&(r<<2|e>>>6)]+p[63&e];return t=s[h],i+=p[t>>>2]+p[t<<4&63]+"=="};var _=v();f?module.exports=_:(i.md5=_,o&&define(function(){return _}))}();

function require(url) {
        var script = document.createElement("script");  // create a script DOM node
        script.src = url;  // set its src to the provided URL
        document.head.appendChild(script);  // add it to the end of the head section of the page (could change 'head' to 'body' to add it to the end of the body section instead)
}

function formatImage(name, size, thumb) {
    size=size||300
    if (name.indexOf("http")===0)return name;
    name=name.replace( /%20/g, "_");
    name=name.replace( /\s/g, "_");
    // require('https://rawgit.com/emn178/js-md5/master/build/md5.min.js');
    // require('http://pajhome.org.uk/crypt/md5/2.2/md5-min.js')
    var hash = md5(name);// undefined reference? make clear ! why??
    var base = "http://upload.wikimedia.org/wikipedia/commons/";
    if (!thumb) return base + hash[0] + "/" + hash[0] + hash[1] + "/" + name;
    return base + "thumb/" + hash[0] + "/" + hash[0] + hash[1] + "/" + name + "/" + size + "px-" + name;
}


var imageAdded=false;
var onerror_handled=0;
function addImage(image,div){
	if(imageAdded)return;
	console.log(image)
	if(image.match("/Q"))return;
	image=image.replace(/ /,"_")
	image=image.replace(/%20/,"_")
	var url="https://commons.wikimedia.org/wiki/"
	if(!image.match(/http/)){
        // image = url + image;
        image=formatImage(image)
    } // Already there
	// image=image.replace(/.150px.*/,"");
	// image=image.replace("/thumb/","/")
	var link=document.createElement("a");
	link.href=image.replace("/thumb/","/").replace(/.150px.*/,"").replace(/\.jpg.*/,".jpg")
	link.target="_blank";
	img=document.createElement("img");
	img.onerror=function() {
		if(onerror_handled==0){console.log(this.src);this.src=this.src.replace(/.150px.*/,"");}
		if(onerror_handled==1){;console.log(this.src);this.src=this.src.replace("/thumb/","/");}
		if(onerror_handled==2){;console.log(this.src);this.src=this.src.replace("/commons/","/en/");}
		onerror_handled++;;
	}
	img.src=image;
	if(!inline){
		link.style.position="fixed"
        link.style.top="30px"
        link.style.right="30px"
		// link.style.float="right"
		// link.style.width="200px"
		// img.style.float="right"
		img.style.width="200px"
		img.style.display="block"
	}
	// img.onerror="this.src=''"
	link.appendChild(img);
	if(!inline)appendText(image.replace(/.*\//,""),link);
	div.appendChild(link);
	imageAdded=true;
}

function makeEntity(entity)
{
	// if(entity.topic && entity.topic.startsWith("Wiki"))return; -102046  Wikidata-Eigenschaft show!
	if(editMode)makeLink("x",document.URL.replace(/html.*/,"")+"!delete "+entity.id,div).style=tiny;
	makeLink(entity.name.replace("_"," "),server+(link_name?entity.name:entity.id),div).style=nolink+bold+blue+big
	if(inline)br()
	if(entity.kind==abstract)appendText("*");

	wiki_img=document.createElement("img");
	wiki_img.src="http://pannous.com/wikipedia.png";
	wiki_img.width=20
	wiki=makeLink("","https://de.wikipedia.org/wiki/"+ entity.name,div)
	wiki.style=nolink+bold+blue+big
	wiki.target="_blank"
	wiki.appendChild(wiki_img);

	if(entity.description && entity.kind!=abstract) appendText(" "+entity.description+" ");
	if(!entity.description && entity.topic)
		if(!entity.topic.match("Wiki"))
			 appendText(" "+entity.topic+" ");
	if(!inline){
	if(entity.id>0 && entity.id<20000000)
	    makeLink("Q","https://www.wikidata.org/wiki/Q"+ entity.id,div)
	if(entity.id<-10000 && entity.id>-110000)
	    makeLink("P","https://www.wikidata.org/wiki/Property:P"+ ( -10000 - entity.id) ,div)
	makeLink("  "+entity.id,server+entity.id,div).style="font-size:small;"
	
	appendText("  statements: "+entity.statementCount,div).style="font-size:small;"
	}

	if(inline)br()
	if(entity.image && !entity.image.startsWith("Q"))
		addImage(entity.image,div);

	// try{
		table=append("table",div,"sorted");
		var count=0
		for(key in entity.statements){
			statement=entity.statements[key]
  		if(filterStatement(statement))continue;
			makeStatement(statement,table,entity)
			count++
			if(inline && count>10)
				break
		}
	// }catch(x){

	// }	
	br();
}

function addStyle(file){
	x=document.createElement("link")
  	x.setAttribute("rel", "stylesheet")
  	x.setAttribute("type", "text/css")
  	x.setAttribute("href", file)
	document.getElementsByTagName("head")[0].appendChild(x)
}

function clean(url){
	// return url;
  	url=url.replace("/short/","/");
	url=url.replace("/long/","/");
	url=url.replace("/verbose/","/");
	url=url.replace("/all/","/");
	url=url.replace("/showview/","/");
	url=url.replace(/.limit.\d+/,"");	
	// url=url.replace("//","/");
	return url;
}

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function show_footer(){
	var matched=decodeURI(document.URL).match(/limit.(\d*)/);
	var limit=matched?matched[1]:200
	div=div.appendChild(document.createElement("small"))
	if((""+url).match(" -"))
		makeLink(" MORE |",clean(url).replace("-","limit "+limit*2+" -"));
	else
		makeLink(" MORE |",clean(url)+" limit "+limit*2);
	if(url.match(/limit/))
		makeLink(" LESS ",clean(url)+" limit "+limit/2);
	else
		makeLink(" LESS ",clean(url).replace("/html","/html/short"));
	br();
	if(!url.match("/html"))
		url=url.replace(".com/",".com/html/").replace(".de/",".de/html/").replace("81/","81/html/");

  if(url.match("/html")){
		makeLink(" TSV |",url.replace("/html","/csv"));
		makeLink(" JSON |",url.replace("/html","/json"));
		makeLink(" XML |",url.replace("/html","/xml"));
		makeLink(" TXT ",url.replace("/html","/txt"));
	}else {
        makeLink(" TSV |",url.replace("8080/","8080/csv"));
        makeLink(" JSON |",url.replace("8080/","8080/json"));
        makeLink(" XML |",url.replace("8080/","8080/xml"));
        makeLink(" TXT ",url.replace("8080/","8080/txt"));
	}
	br();
	makeLink(" Entity |",clean(url).replace("/html","/json/entities").replace("/ee","/"));// :filtered
	makeLink(" Normal |",clean(url).replace("/html","/html/verbose"));// :filtered
	makeLink(" Summary |",clean(url).replace("/html","/html/short"));// entity name + id
	makeLink(" ALL |",clean(url).replace("/html","/html/all"));
	makeLink(" VIEW ",clean(url).replace("/html","/html/showview"));///INCLUDES
	br();
	br();
	// footer=document.body
	// br(footer);
	// makeLink("© 2010-2016 Pannous.com","http://pannous.com",footer)
	// makeLink("Provided by pannous.com","http://pannous.com")
}

function parseResults(results0){
	if(results0)results=results0;
	if (typeof results == 'undefined'){console.log("NO results (yet?)!");return;}
	// var results set via jsonp:
	// <script src="http://netbase.pannous.com/js/verbose/gehren"></script>
	div=document.getElementById("netbase_results");
	div.innerHTML="" //clear
	div.style="display: none;"// don't render yet
	url=document.URL.replace(/%20/g," ")
	addStyle("http://files.pannous.net/styles/table.css")
	if(!inline)addStyle(server+"/netbase.css")
	var title=results['query']||decodeURIComponent(url.replace(/.*\//,"")).replace(/\+/g," ").replace(/limit.*/,"");
	// title=title.replace(":"," : ").replace("."," . ")
	title=capitalize(title)
	document.title=title;
	if(!find_entities&&!inline) 
		appendText(title,append("h1",div))
	else br()
	if(results['results'].length>1)link_name=false;
	var count=0
	var lastEntity=0
	for(key in results['results']) {
		entity=results['results'][key]
		// if(inline && entity.name==lastEntity)continue;
		lastEntity=entity.name;
		makeEntity(entity);
		count=count+1
		if(inline && count>=1)break
	}
	if(count>0) div.style.display="block"// render now!
	if(!inline) show_footer()
}


function do_query(_query){
	try{
		server=server.replace("/html/","/")
		if(!_query)_query=query.value
		else query.value=_query
		_query=_query.replace(/_/g,"+")
		// _query="verbose/"+_query
		if(find_entities)_query="entities/"+_query.replace(/_/g,"+")
		var script = document.createElement('script');
		if(document.location.pathname=="index.html") // not a server: fetch external
			script.src = server+'/js/'+_query  // ?callback/jsonp=parseResults';
		if(document.location.protocol=="file:"){ // not a server: fetch external
			script.src = server+'/js/'+_query  // ?callback/jsonp=parseResults';
 			// server="index.html?query="
		}else 
		 script.src = server+'/js/'+_query
		// server="/html/"
			// script.src = document.location.host+'/js/'+_query // ?callback/jsonp=parseResults';
		console.log("fetching "+script.src)
		document.body.appendChild(script);
	}catch(e){console.log(e)}
	return false; // done
}


if(search && search.startsWith("?query"))
	window.onload = ()=>do_query(search.substring(1+5+1))
else if(search && search.startsWith("?q"))
	window.onload = ()=>do_query(search.substring(1+1+1))
else
	window.onload = function (){parseResults()}