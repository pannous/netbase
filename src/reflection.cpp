/*
 * File:   reflection.hpp
 * Author: me
 *
 * Created on October 12, 2013, 2:26 PM
 */




#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#include <mach/task.h>
#include <mach/task_info.h>
     #include <sys/types.h>
     #include <sys/sysctl.h>
#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
#include <fcntl.h>
#include <procfs.h>
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#include <stdio.h>
#include <sys/sysinfo.h>
#endif
#else
#error "Cannot define getPeakRSS( ) or getCurrentRSS( ) for an unknown OS."
#endif

//
//size_t getTotalSystemMemory()
//{
//    long pages = sysconf(_SC_PHYS_PAGES);
//    long page_size = sysconf(_SC_PAGE_SIZE);
//    return pages * page_size;
//}

size_t getFreeSystemMemory(){
	#if defined(__linux__)
//  struct sysinfo info;
//  sysinfo(&info);
//  printf( "Free ram: %d\n", info.freeram );
//  return info.freeram;
	#else
//http://stackoverflow.com/questions/12523704/mac-os-x-equivalent-header-file-for-sysinfo-h-in-linux
//	https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/sysctl.3.html
           int mib[4];
           size_t len;
           struct kinfo_proc kp;

           /* Fill out the first three components of the mib */
//           len = 4; sysctlnametomib("kern.proc.pid", mib, &len);
           mib[0] = CTL_KERN;
           mib[1] = HW_USERMEM ;//"hw.logicalcpu";// KERN_MAXPROC;
//                   mib[3] = i;// other process pid
                   len = sizeof(kp);
                   int res=sysctl(mib, 2, &kp, &len, NULL, 0);
				   res=sysctlbyname("hw.logicalcpu", &kp, &len, NULL, 0);
				   return res;//kp.kp_eproc.;
//				   return kp.kp_proc.p_cpticks;
//                           perror("sysctl");
//                   else if (len > 0)
//                           printkproc(&kp);
	#endif
}

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t getPeakRSS( )
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.PeakWorkingSetSize;

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    /* AIX and Solaris ------------------------------------------ */
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* BSD, Linux, and OSX -------------------------------------- */
    struct rusage rusage;
    getrusage( RUSAGE_SELF, &rusage );
#if defined(__APPLE__) && defined(__MACH__)
    return (size_t)rusage.ru_maxrss;
#else
    return (size_t)(rusage.ru_maxrss * 1024L);
#endif

#else
    /* Unknown OS ----------------------------------------------- */
    return (size_t)0L;          /* Unsupported. */
#endif
}





/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 */
size_t getCurrentRSS( )
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.WorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    /* OSX ------------------------------------------------------ */
    struct mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
    if ( task_info( mach_task_self( ), MACH_TASK_BASIC_INFO,
        (task_info_t)&info, &infoCount ) != KERN_SUCCESS )
        return (size_t)0L;      /* Can't access? */
    return (size_t)info.resident_size;

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    /* Linux ---------------------------------------------------- */
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);

#else
    /* AIX, BSD, Solaris, and Unknown OS ------------------------ */
    return (size_t)0L;          /* Unsupported. */
#endif
}

