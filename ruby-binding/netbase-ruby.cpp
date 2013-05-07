#include "ruby.h"
#include "util.hpp"
#include "netbase.hpp"
#include "import.hpp"
#include "relations.hpp"
#include "console.hpp"
#include "query.hpp"
// SEE http://home.vr-web.de/juergen.katins/ruby/buch/ext_ruby.html !!!!

typedef VALUE (ruby_method)(...);

//RString
static VALUE rb_netbase_module=Qnil;
// static VALUE rb_netbase_node;
//
// static void node_mark (rb_netbase_node w)
// {}
//
// static void node_free (rb_netbase_node w)
// {
//   // nodeDispose (w);
// }
//
// static VALUE node_allocate (VALUE klass)
// {
// 	rb_netbase_node node = 1;//nodeNew(NULL, NULL, NULL);
//   	return Data_Wrap_Struct (klass, node_mark, node_free, node);
// }


#define STR2CSTR StringValuePtr
#define CSTR2STR rb_str_new2

extern "C"
VALUE method_query(VALUE self, VALUE anObject){
	printf("querying: %s\n",STR2CSTR(anObject));
	init();
	Query q;
	q.keyword=getAbstract(STR2CSTR(anObject));
	string result=query(q);//string  	parse("?");

VALUE results = rb_ary_new();
rb_define_variable("$results", &results);
rb_ary_push(results, rb_str_new2("test"));
//	for_each()
//	rb_ary_push(results, rb_str_new2(node));
	return rb_str_new2(result.c_str());
}


extern "C" VALUE method_parse(	VALUE self, VALUE anObject){
//rb_eval_string("anObject.each{|x| x.clearFlag }");// USE RUBY INSIDE C++ !!!!
	printf("parsing: %s\n",STR2CSTR(anObject));
	parse(STR2CSTR(anObject));
	return rb_str_new2("\nOK");
}


  // Class rb_cMySocket = define_class("MySocket", rb_cIO);
extern "C"
void Init_netbase()
{
	rb_netbase_module = rb_define_module ("Netbase");
  	// rb_netbase_node = rb_define_class_under (rb_netbase_module, "Node", rb_cObject);
  // rb_define_alloc_func (rb_netbase_node, node_allocate);
//	rb_define_method(rb_netbase_module, "test1", method_test1, 0);
	rb_define_method(rb_netbase_module, "parse", (ruby_method*) &method_parse, 1);
	rb_define_method(rb_netbase_module, "query", (ruby_method*) &method_query, 1);
}