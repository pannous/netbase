#ifdef jni
#include <jni.h>
#include <stdio.h>
#include "../netbase.hpp"
#include "../init.hpp"
#include "../console.hpp"
#include "NetbaseJNI.h"

// JUST USE JNA!!!!!!!!!!!

JNIEXPORT void JNICALL Java_NetbaseJNI_query(JNIEnv *env, jobject thisObj) {
   printf("NetbaseJNI\n");
   init();
   parse("all fruit");
   return;
}
#endif
