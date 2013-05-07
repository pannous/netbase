#include <jni.h>
#include <stdio.h>
#include "../netbase.hpp"
#include "../init.hpp"
#include "../console.hpp"
//#include "../netbase-c.h"
#include "HelloJNI.h"

JNIEXPORT void JNICALL Java_HelloJNI_sayHello(JNIEnv *env, jobject thisObj) {
   printf("Hello World!\n");
   init();
   parse("all fruit");
   return;
}