// Netbase graph viewer - demo version = 1.0.2
// (C) 2014 Pannous + Quasiris

var div=document.getElementById("results");

function br(){
	div.appendChild( document.createElement("br"));
}

function append(tag,elem,clazz){
	var tl = document.createElement(tag);
	if(clazz)tl.setAttribute("class",clazz);
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
	// server="http://quasiris.big:3333/html/";
	server="/html/";

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
	a.appendChild(x);
	elem.appendChild(a);
	elem.appendChild(document.createTextNode(" "));
	return a; // or a
}
function makeStatement(statement,elem)
{
	if(statement.predicate=="nr in context")return; 
	var top = document.createElement("tr");
	makeLink(statement.subject.replace("_"," "),server+statement.sid,makeRow(top));
	predicate=	makeRow(top)
	makeLink(statement.predicate,server+statement.pid,predicate);
	makeLink(" ^",document.URL+"."+statement.predicate,predicate).style=tiny;// filter
	makeLink(" -",document.URL+" -"+statement.predicate,predicate).style=tiny;// filter
	x=makeLink(statement.object,server+statement.oid,makeRow(top));
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
	image=image.replace(/ /,"_")
	// image=image.replace(/.150px.*/,"");
	// image=image.replace("/thumb/","/")
	var link=document.createElement("a");
	link.href=image.replace("/thumb/","/").replace(/\.jpg.*/,".jpg")
	link.target="_blank"
	link.style="float:right;width:200px";
	img=document.createElement("img");
	img.onerror=function() {
		if(onerror_handled==0){this.src=this.src.replace(/.150px.*/,"")}
		if(onerror_handled==1){this.src=this.src.replace("/thumb/","//");}
		if(onerror_handled==2){this.src=this.src.replace("/commons/","/en/");}
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
	makeLink(entity.name.replace("_"," "),server+entity.name,div).style=nolink+bold+blue+big
	makeLink("  "+entity.id,server+entity.id,div).style="font-size:small;"
	if(entity.image)addImage(entity.image,div);
	// addImage(entity.image,div);
	appendText("  statements: "+entity.statementCount,div).style="font-size:small;"
	
	try{
	table=append("table",div,"sorted");
	for(key in entity.statements){
		statement=entity.statements[key];
		makeStatement(statement,table)
	}
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


window.onload = function ()
{
	url=document.URL.replace(/%20/g," ")
	addStyle("http://files.pannous.net/styles/table.css")
	div.style="display: none;"// don't render yet
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
		makeLink(" MORE |",url.replace("/short/","/").replace(/.limit.\d+/,"").replace("-","limit "+limit*2+" -"));
	else
		makeLink(" MORE |",url.replace("/short/","/").replace(/.limit.\d+/,"")+" limit "+limit*2);
	if(url.match(/limit/))
		makeLink(" LESS ",url.replace(/.limit.\d+/,"")+" limit "+limit/2);
	else
		makeLink(" LESS ",url.replace("/html","/html/short"));
	br();
	makeLink(" tsv |",url.replace("/html","/csv"));
	makeLink(" json |",url.replace("/html","/json"));
	makeLink(" xml |",url.replace("/html","/xml"));
	makeLink(" txt ",url.replace("/html","/txt"));
	div.style="display: block;"// render now
}