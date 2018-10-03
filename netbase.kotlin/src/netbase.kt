import com.pannous.mark.parse
import com.pannous.extensions.*
import kotlin.collections.mutableListOf as list

//class Json
typealias Json=MutableMap<String,Any>
private var api="http://de.netbase.pannous.com:8080/json/verbose/"

class Netbase{
	companion object {
		fun get(entity:String ): Node{
			val json = download(api + entity)
			val data = parse(json)
			val results = data["results"] as List<*>
			val result=	results[0] as Json
			return Node(result)
		}
	}
}
class Node(val data: Json) {
	val id: Int     by data
	val seo: String by data
	val name: String by data
	val description: String by data
	val statementCount: Int     by data
	val typeid: Int     by data

	private val edges=list<Statement>()
	val statements: MutableList<Statement>
		get() {
			if(edges.size==0)
				for (s in this.data["statements"] as ArrayList<*>)
					edges+=Statement(s as Json)
			return edges// this.data["statements"] as Json
		}
}

class Statement(val data: Json){
	val id: Int     by data
	val sid: Int     by data
	val pid: Int     by data
	val oid: Int     by data
	val subject: String by data
	val predicate: String by data
	val `object`: String by data
	override fun toString(): String {
		val ids="\t"+sid+"⇨"+pid+"⇨"+oid
		return subject+"⇨"+predicate+"⇨"+`object`  +ids
	}
}


fun main(args: Array<String>) {
	val result=	Netbase.get("hi")
//	val results=	Netbase.all("hi")
	val id= result.id
	println(id)
	println(result.data)
	println(result.statements[0])
	print("OK")
}
