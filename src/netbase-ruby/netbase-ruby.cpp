#ifdef ruby
#include "ruby.h"
#include "../netbase.hpp"
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

// Our 'test1' method.. it simply returns a value of '10' for now.
// VALUE method_test1(...);
VALUE method_test2(VALUE self) {
	int x = 11;//test2();
	return INT2NUM(x);
}

extern "C"
VALUE method_test1(...) {
	int x = test2();// FROM NETBASE !!! //12;//test2();//10;//test2();
	return INT2NUM(x);
}


  // Class rb_cMySocket = define_class("MySocket", rb_cIO);
extern "C"
void Init_netbase()
{
	rb_netbase_module = rb_define_module ("Netbase");
  	// rb_netbase_node = rb_define_class_under (rb_netbase_module, "Node", rb_cObject);
  // rb_define_alloc_func (rb_netbase_node, node_allocate);

	rb_define_method(rb_netbase_module, "test1", method_test1, 0);
}
#endif
