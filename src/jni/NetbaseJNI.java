/*
javah NetbaseJNI
// vi NetbaseJNI.c
../../compile.sh
javac NetbaseJNI.java && java NetbaseJNI
// WORKS !!!!

Since there is no official java API to create a shared memory segment, you need to resort to a helper library/DDL and JNI to use shared memory to have two Java processes talk to each other.

*/

public class NetbaseJNI {
   static {
      System.load("/root/netbase/netbase");
//      System.load("../../netbase");
//	   System.load("netbase");
		// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH;.
      // System.load("netbase"); // Expecting an absolute path of the library: netbase
		// netbase.dll (Windows) or netbase.so (Unixes) PATHS OK HERE

      	// System.loadLibrary("netbase"); // hello.dll (Windows) or libhello.so (Unixes) NO PATHS HERE
   }
   // A native method that receives nothing and returns void
   private native void query();

   public static void main(String[] args) {
      new NetbaseJNI().query();  // invoke the native method
   }
}