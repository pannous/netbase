/* 
javah HelloJNI
vi HelloJNI.c
javac HelloJNI.java
java HelloJNI

Since there is no official java API to create a shared memory segment, you need to resort to a helper library/DDL and JNI to use shared memory to have two Java processes talk to each other.

*/

public class HelloJNI {
   static {
	// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH;.
      System.load("/Users/me/netbase/netbase"); // hello.dll (Windows) or libhello.so (Unixes) PATHS OK HERE
      // System.loadLibrary("netbase"); // hello.dll (Windows) or libhello.so (Unixes) NO PATHS HERE
   }
   // A native method that receives nothing and returns void
   private native void sayHello();
 
   public static void main(String[] args) {
      new HelloJNI().sayHello();  // invoke the native method
   }
}