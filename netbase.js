// Netbase graph viewer - demo version = 1.0.2
// (C) 2014 Pannous + Quasiris

var div=document.getElementById("netbase_results");
var server="/html/"; // "http://quasiris.big:3333/html/";
var filterIds=[-10646,-50,-10508,-10910,-11566];
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

function makeStatement(statement,elem)
{  // if Wikimedia-Kategorie  BREAK!
    if(filterIds.indexOf(statement.pid)>=0)return;
    if(statement.sid==-666)return;// Error/Missing
	if(statement.predicate=="nr in context")return; 
	if(statement.predicate.match(/ID/))return;
	if(statement.predicate=="GND")return;
	if(statement.predicate=="DDC")return;
	if(statement.predicate=="VIAF")return;	
	if(statement.subject=="◊")return;
	if(statement.subject.startsWith("http"))return;
	if(statement.object.match(/rdf/))return;
	if(statement.object.match(/wikimedia.org/))return;
	if(1+statement.subject>1)return;
	if(statement.predicate=="Wappen")addImage(statement.object,div);
	var top = document.createElement("tr");
	makeLink(statement.subject.replace("_"," "),server+statement.sid,makeRow(top));
	predicate=	makeRow(top)
	makeLink("x",document.URL.replace(/html.*/,"")+"!delete $"+statement.id,predicate).style=tiny;
	makeLink(statement.predicate,server+statement.pid,predicate);
	makeLink(" ^",clean(document.URL).replace(/html/,"html/short")+"."+statement.predicate,predicate).style=tiny;// filter
	makeLink(" -",document.URL+" -"+statement.predicate,predicate).style=tiny;// filter
	makeLink(" +",statement.sid+" include "+statement.predicate,predicate).style=tiny;// filter
	makeLink(" --",statement.sid+" exclude "+statement.predicate,predicate).style=tiny;// filter
	makeLink(" -!","exclude "+statement.predicate,predicate).style=tiny;// filter
	var objectUrl=statement.object.startsWith("http")?statement.object:server+statement.oid;
	var x=makeLink(statement.object,objectUrl,makeRow(top));
	makeLink(" ^",server+ statement.predicate+":"+statement.object,x).style=tiny;// filter
	elem.appendChild(top);
	if(statement.predicate=="quasiris id")throw new Exception(); 
	// if(statement.match("quasiris"))throw new Exception();
}

function br(){append("br",div)}

var imageAdded=false;
var onerror_handled=0;
function addImage(image,div){
	if(imageAdded)return;
	if(image.contains("/Q"))return;
	image=image.replace(/ /,"_")
	var url="https://commons.wikimedia.org/wiki/"
	if(image.match(/http/))url="" // Already there
	// image=image.replace(/.150px.*/,"");
	// image=image.replace("/thumb/","/")
	var link=document.createElement("a");
	link.href=image.replace("/thumb/","/").replace(/\.jpg.*/,".jpg")
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
	makeLink("x",document.URL.replace(/html.*/,"")+"!delete "+entity.id,div).style=tiny;
	makeLink(entity.name.replace("_"," "),server+entity.name,div).style=nolink+bold+blue+big
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
	url=url.replace("/short/","/");
	url=url.replace("/long/","/");
	url=url.replace("/verbose/","/");
	url=url.replace("/all/","/");
	url=url.replace("/showview/","/");
	url=url.replace(/.limit.\d+/,"");	
	return url;
}

function parseResults(results0){
	if(results0)results=results0;
	// var results set via jsonp:
	// <script src="http://de.netbase.pannous.com:81/js/verbose/gehren"></script>
	
	div.style="display: none;"// don't render yet
	url=document.URL.replace(/%20/g," ")
	addStyle("http://files.pannous.net/styles/table.css")
	var title=decodeURIComponent(url.replace(/.*\//,"")).replace(":"," : ").replace("."," . ").replace(/\+/g," ").replace(/limit.*/,"");
	document.title=title;
	appendText(title,append("h1",div))
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
	makeLink(" tsv |",url.replace("/html","/csv"));
	makeLink(" json |",url.replace("/html","/json"));
	makeLink(" xml |",url.replace("/html","/xml"));
	makeLink(" txt ",url.replace("/html","/txt"));
	br();
	makeLink(" Normal |",clean(url).replace("/html","/html/verbose"));// :filtered
	makeLink(" Summary |",clean(url).replace("/html","/html/short"));// entity name + id
	makeLink(" ALL |",clean(url).replace("/html","/html/all"));
	makeLink(" VIEW ",clean(url).replace("/html","/html/showview"));///INCLUDES
	div.style="display: block;"// render now
}

window.onload = function (){parseResults()}