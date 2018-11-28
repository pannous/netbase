// Netbase Graph Viewer :: version 1.2.8
// © 2010 - 2017 Pannous + Quasiris

var div// results
var editMode=false
var link_name=true
var server="/"//html/"; 
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
	makeLink(" --",statement.sid+" exclude "+statement.predicate,predicate).style=tiny// filter
	makeLink(" -!","exclude "+statement.predicate,predicate).style=tiny// filter
}
var num=0;
function makeStatement(statement,elem,entity)
{  // if Wikimedia-Kategorie  BREAK!
	if(filterStatement(statement))return
	// if(statement.predicate.match(/Wappen/i))addImage(statement.object,div);
	// if(statement.predicate.match(/Flagge/i))addImage(statement.object,div);
	// if(statement.predicate.match(/karte/i))addImage(statement.object,div);
	// if(statement.predicate.match(/image/))addImage(statement.object,div);
	// if(statement.predicate.match(/bild/i))addImage(statement.object,div);
	// if(statement.object.match(/\.svg/))addImage(statement.object,div);
	// if(statement.object.match(/\.png/))addImage(statement.object,div);
	// if(statement.object.match(/\.jpg/))addImage(statement.object,div);
	// if(statement.object.match(/\.bmp/))addImage(statement.object,div);
	var top = document.createElement("tr")
    if(++num % 2){
	  top.classList.add('even') 
	}  else {
  	  top.classList.add('odd')
	}
	if(!inline || statement.sid!=entity.id)
		makeLink(statement.subject.replace("_"," "),server+statement.sid,makeRow(top))

	predicate=	makeRow(top)
	if(editMode)editLinks(predicate,statement,elem)
	makeLink(statement.predicate,server+statement.pid,predicate)
	if(!inline || statement.oid!=entity.id){
		var objectUrl=statement.object.startsWith("http")?statement.object:server+statement.oid
		if(statement.predicate.match("Koord"))
			objectUrl="https://www.mapquest.com/latlng/"+statement.object.replace(" ","")
		var x=makeLink(statement.object,objectUrl,makeRow(top))
	// if(editMode)makeLink(" ^",server+ statement.predicate+":"+statement.object,x).style=tiny;// filter
	}
	elem.appendChild(top)
}

var imageAdded=false
var onerror_handled=0
function addImage(image,div){
	// if(imageAdded)return;
	console.log(image)
	if(image.match("/Q"))return
	image=image.replace(/ /,"_")
	image=image.replace(/%20/,"_")
	var url="https://commons.wikimedia.org/wiki/"
	if(image.match(/http/))url="" // Already there
	// image=image.replace(/.150px.*/,"");
	// image=image.replace("/thumb/","/")
	var link=document.createElement("a")
	link.href=image.replace("/thumb/","/").replace(/.150px.*/,"").replace(/\.jpg.*/,".jpg")
	link.target="_blank"
	img=document.createElement("img")
	img.onerror=function() {
		if(onerror_handled==0){console.log(this.src);this.src=this.src.replace(/.150px.*/,"")}
		if(onerror_handled==1){console.log(this.src);this.src=this.src.replace("/thumb/","/")}
		if(onerror_handled==2){console.log(this.src);this.src=this.src.replace("/commons/","/en/")}
		onerror_handled++
	}
	img.src=image
	if(!inline){
		link.style.float="right"
		link.style.width="200px"
		img.style.float="right"
		img.style.width="200px"
	}
	// img.onerror="this.src=''"
	link.appendChild(img)
	if(!inline)appendText(image.replace(/.*\//,""),link)
	div.appendChild(link)
	imageAdded=true
}

function makeEntity(entity)
{
	// if(entity.topic && entity.topic.startsWith("Wiki"))return; -102046  Wikidata-Eigenschaft show!
	if(editMode)makeLink("x",document.URL.replace(/html.*/,"")+"!delete "+entity.id,div).style=tiny
	makeLink(entity.name.replace("_"," "),server+(link_name?entity.name:entity.id),div).style=nolink+bold+blue+big
	if(inline)br()
	if(entity.kind==abstract)appendText("*")

	wiki_img=document.createElement("img")
	wiki_img.src="http://pannous.net/files/wikipedia.png"
	wiki_img.width=20
	wiki=makeLink("","https://de.wikipedia.org/wiki/"+ entity.name,div)
	wiki.style=nolink+bold+blue+big
	wiki.target="_blank"
	wiki.appendChild(wiki_img)

	if(entity.description && entity.kind!=abstract) appendText(" "+entity.description+" ")
	if(!entity.description && entity.topic)
		if(!entity.topic.match("Wiki"))
			 appendText(" "+entity.topic+" ")
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
		addImage(entity.image,div)

	// try{
	table=append("table",div,"sorted")
	var count=0
	for(key in entity.statements){
		statement=entity.statements[key]
  		if(filterStatement(statement))continue
		makeStatement(statement,table,entity)
		count++
		if(inline && count>10)
			break
	}
	// }catch(x){

	// }	
	br()
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
  	url=url.replace("/short/","/")
	url=url.replace("/long/","/")
	url=url.replace("/verbose/","/")
	url=url.replace("/all/","/")
	url=url.replace("/showview/","/")
	url=url.replace(/.limit.\d+/,"")	
	// url=url.replace("//","/");
	return url
}

function capitalize(string) {
	return string.charAt(0).toUpperCase() + string.slice(1)
}

function show_footer(){
	var matched=decodeURI(document.URL).match(/limit.(\d*)/)
	var limit=matched?matched[1]:200
	div=div.appendChild(document.createElement("small"))
	if((""+url).match(" -"))
		makeLink(" MORE |",clean(url).replace("-","limit "+limit*2+" -"))
	else
		makeLink(" MORE |",clean(url)+" limit "+limit*2)
	if(url.match(/limit/))
		makeLink(" LESS ",clean(url)+" limit "+limit/2)
	else
		makeLink(" LESS ",clean(url).replace("/html","/html/short"))
	br()
	if(!url.match("/html"))url=url.replace(".com/",".com/html/").replace(".de/",".de/html/").replace("81/","81/html/")
	makeLink(" TSV |",url.replace("/html","/csv"))
	makeLink(" JSON |",url.replace("/html","/json"))
	makeLink(" XML |",url.replace("/html","/xml"))
	makeLink(" TXT ",url.replace("/html","/txt"))
	br()
	makeLink(" Entity |",clean(url).replace("/html","/json/entities").replace("/ee","/"))// :filtered
	makeLink(" Normal |",clean(url).replace("/html","/html/verbose"))// :filtered
	makeLink(" Summary |",clean(url).replace("/html","/html/short"))// entity name + id
	makeLink(" ALL |",clean(url).replace("/html","/html/all"))
	makeLink(" VIEW ",clean(url).replace("/html","/html/showview"))///INCLUDES
	br()
	br()
	// footer=document.body
	// br(footer);
	// makeLink("© 2010-2016 Pannous.com","http://pannous.com",footer)
	// makeLink("Provided by pannous.com","http://pannous.com")
}

function parseResults(results0){
	if(results0)results=results0
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
