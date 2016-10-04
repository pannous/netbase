// Netbase Graph Viewer - version = 1.2.6
// (C) 2014 - 2016 Pannous + Quasiris

var div=document.getElementById("netbase_results");
var editMode=false;
var link_name=true;
var server="/html/"; // "http://quasiris.big:3333/html/";
var filterIds=[-50,-11343,-10646,-10508,-10910,-11566,-10268, -10950, -10349, -11006, -10269, -10409, -11017, -10691, -10906, -11005, -10949, -10734, -11207, 12209159];//   Amerigo Vespucci BnF-ID 12234845j 47674->-10268->36430981 etc
var abstract=-102;// Summary nodes
function br(){
	div.appendChild( document.createElement("br"));
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
	// a.target="_blank"
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
  // if(statement.object.match(/wikimedia/i))return true;//  Wikimedia-Begriffsklärungsseite wikimedia.org ...
	if(1+statement.subject>1)return true;
}

function makeStatement(statement,elem)
{  // if Wikimedia-Kategorie  BREAK!
  if(filterStatement(statement))return;
	if(statement.predicate=="Wappen")addImage(statement.object,div);
	if(statement.predicate=="product_image_url")addImage(statement.object,div);
	var top = document.createElement("tr");
	makeLink(statement.subject.replace("_"," "),server+statement.sid,makeRow(top));
	predicate=	makeRow(top)
	if(editMode)makeLink("x",document.URL.replace(/html.*/,"")+"!delete $"+statement.id,predicate).style=tiny;
	makeLink(statement.predicate,server+statement.pid,predicate);
	if(editMode)makeLink(" ^",clean(document.URL).replace(/html/,"html/short")+"."+statement.predicate,predicate).style=tiny;// filter
	if(editMode)makeLink(" -",document.URL+" -"+statement.predicate,predicate).style=tiny;// filter
	if(editMode)makeLink(" +",statement.sid+" include "+statement.predicate,predicate).style=tiny;// filter
	if(editMode)makeLink(" --",statement.sid+" exclude "+statement.predicate,predicate).style=tiny;// filter
	if(editMode)makeLink(" -!","exclude "+statement.predicate,predicate).style=tiny;// filter
	var objectUrl=statement.object.startsWith("http")?statement.object:server+statement.oid;
	var x=makeLink(statement.object,objectUrl,makeRow(top));
	if(editMode)makeLink(" ^",server+ statement.predicate+":"+statement.object,x).style=tiny;// filter
	elem.appendChild(top);
	if(statement.predicate=="quasiris id")throw new Exception(); 
	// if(statement.match("quasiris"))throw new Exception();
}

function br(){append("br",div)}

var imageAdded=false;
var onerror_handled=0;
function addImage(image,div){
	if(imageAdded)return;
	if(image.match("/Q"))return;
	image=image.replace(/ /,"_")
	var url="https://commons.wikimedia.org/wiki/"
	if(image.match(/http/))url="" // Already there
	// image=image.replace(/.150px.*/,"");
	// image=image.replace("/thumb/","/")
	var link=document.createElement("a");
	link.href=image.replace("/thumb/","/").replace(/.150px.*/,"").replace(/\.jpg.*/,".jpg")
	link.target="_blank"
	link.style="float:right;width:200px";
	img=document.createElement("img");
	img.onerror=function() {
		if(onerror_handled==0){console.log(this.src);this.src=this.src.replace(/.150px.*/,"");}
		if(onerror_handled==1){;console.log(this.src);this.src=this.src.replace("/thumb/","/");}
		if(onerror_handled==2){;console.log(this.src);this.src=this.src.replace("/commons/","/en/");}
		onerror_handled++;;
	}
	img.src=image;
	img.style="float:right;width:200px";
	// img.onerror="this.src=''"
	link.appendChild(img);
	appendText(entity.image.replace(/.*\//,""),link);
	div.appendChild(link);
	imageAdded=true;
}

function makeEntity(entity)
{
	if(entity.topic && entity.topic.startsWith("Wiki"))return;
	makeLink("x",document.URL.replace(/html.*/,"")+"!delete "+entity.id,div).style=tiny;
	makeLink(entity.name.replace("_"," "),server+(link_name?entity.name:entity.id),div).style=nolink+bold+blue+big

	if(entity.kind==abstract)appendText("*");
	if(entity.description && entity.kind!=abstract) appendText(" "+entity.description+" ");
	if(!entity.description && entity.topic) appendText(" "+entity.topic+" ");

	link=makeLink("","https://de.wikipedia.org/wiki/"+ entity.name,div)
	link.style=nolink+bold+blue+big
	// link.target="_blank"
	img=document.createElement("img");
	img.src="http://pannous.net/files/wikipedia.png";
	img.width=20
	link.appendChild(img);
	if(entity.id>0 && entity.id<20000000)
	    makeLink("Q","https://www.wikidata.org/wiki/Q"+ entity.id,div)
	if(entity.id<-10000)
	    makeLink("P","https://www.wikidata.org/wiki/Property:P"+ ( -10000 - entity.id) ,div)
	makeLink("  "+entity.id,server+entity.id,div).style="font-size:small;"
	if(entity.image && !entity.image.startsWith("Q"))addImage(entity.image,div);
	// addImage(entity.image,div);
	appendText("  statements: "+entity.statementCount,div).style="font-size:small;"

	
	try{
		table=append("table",div,"sorted");
		for(key in entity.statements)
			makeStatement(entity.statements[key],table)
	}catch(x){}	
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
	// url=url.replace(/.limit.\d+/,"");	
	// url=url.replace("//","/");
	return url;
}

function parseResults(results0){
	if(results0)results=results0;
	if (typeof results == 'undefined'){console.log("NO results (yet?)!");return;}
	// var results set via jsonp:
	// <script src="http://de.netbase.pannous.com:81/js/verbose/gehren"></script>
	div.innerHTML="" //clean
	div.style="display: none;"// don't render yet
	url=document.URL.replace(/%20/g," ")
	addStyle("http://files.pannous.net/styles/table.css")
	var title=results['query']||decodeURIComponent(url.replace(/.*\//,"")).replace(":"," : ").replace("."," . ").replace(/\+/g," ").replace(/limit.*/,"");
	document.title=title;
	appendText(title,append("h1",div))
	if(results['results'].length>1)link_name=false;
	for(key in results['results']) {
		entity=results['results'][key]
		makeEntity(entity);
	}
	var matched=url.match(/limit.(\d*)/);
	var limit=matched?matched[1]:200
	// br();
	if((""+url).match(" -"))
		makeLink(" MORE |",clean(url).replace("-","limit "+limit*2+" -"));
	else
		makeLink(" MORE |",clean(url)+" limit "+limit*2);
	if(url.match(/limit/))
		makeLink(" LESS ",clean(url)+" limit "+limit/2);
	else
		makeLink(" LESS ",clean(url).replace("/html","/html/short"));
	br();
	if(!url.match("/html"))url=url.replace("81/","81/html/");
	makeLink(" tsv |",url.replace("/html","/csv"));
	makeLink(" json |",url.replace("/html","/json"));
	makeLink(" xml |",url.replace("/html","/xml"));
	makeLink(" txt ",url.replace("/html","/txt"));
	br();
	makeLink(" Entity (Json)  |",clean(url).replace("/html","/json/entities").replace("/ee","/"));// :filtered
	makeLink(" Normal |",clean(url).replace("/html","/html/verbose"));// :filtered
	makeLink(" Summary |",clean(url).replace("/html","/html/short"));// entity name + id
	makeLink(" ALL |",clean(url).replace("/html","/html/all"));
	makeLink(" VIEW ",clean(url).replace("/html","/html/showview"));///INCLUDES
	div.style="display: block;"// render now
}


function do_query(_query){
	try{
		if(!_query)_query=query.value
		var script = document.createElement('script');
		if(document.location.pathname=="index.html") // not a server: fetch external
			script.src = 'http://de.netbase.pannous.com:81/js/'+_query // ?callback/jsonp=parseResults';
		if(document.location.protocol=="file:"){ // not a server: fetch external
			script.src = 'http://de.netbase.pannous.com:81/js/'+_query // ?callback/jsonp=parseResults';
 			// server="index.html?query="
 			server="http://de.netbase.pannous.com:81/html/"
		}
		else
			script.src = '/js/'+_query
			// script.src = document.location.host+'/js/'+_query // ?callback/jsonp=parseResults';
		console.log("fetching "+script.src+" ...")
		document.body.appendChild(script);
	}catch(e){console.log(e)}
	return false; // done
}


var search = window.location.search;
if(search && search.startsWith("?query"))
	window.onload = ()=>do_query(search.substring(1+5+1))
else if(search && search.startsWith("?q"))
	window.onload = ()=>do_query(search.substring(1+1+1))
else
	window.onload = function (){parseResults()}