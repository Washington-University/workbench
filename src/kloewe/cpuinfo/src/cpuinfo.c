/*----------------------------------------------------------------------------
  File    : cpuinfo.c
  Contents: processor information queries
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifdef _WIN32                       /* if Microsoft Windows system */
#  include <windows.h>
#else
#  include <unistd.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdint.h>
#  ifdef __APPLE__                  /* if Apple Mac OS system */
#    include <sys/sysctl.h>
#    include <sys/types.h>
#  elif defined __linux__           /* if Linux system */
#    ifdef HAVE_HWLOC
#      include <hwloc.h>            /* needed for corecntHwloc() */
#    endif
#  endif  /* #ifdef __APPLE__ .. #elif defined __linux__ */
#endif  /* #ifdef _WIN32 .. #else .. */

#include "cpuinfo.h"

/*----------------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------------*/
#ifdef NDEBUG
#define DBGMSG(...)  ((void)0)
#else
#line __LINE__ "cpuinfo.c"
#define DBGMSG(...)  do { fprintf(stderr, "%s:%d:%s()\n", \
                          __FILE__, __LINE__, __func__); \
                          fprintf(stderr, __VA_ARGS__); } while(0)
#endif

/*----------------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------------*/
typedef struct {                    /* --- processor ids --- */
  int phys;                         /* physical id */
  int core;                         /* core     id */
} PROCIDS;                          /* (processor ids) */

/*----------------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------------*/
static int cpuinfo[4];              /* cpu information (cpuid) */
static int peax = -1;               /* previous eax */
static int pecx = -1;               /* previous ecx */
static int nphys  = 0;              /* # processors/packages/sockets */
static int ncores = 0;              /* # processor cores */
static int nprocs = 0;              /* # logical processors */

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
#ifdef _WIN32                       /* if Microsoft Windows system */
#define cpuid   __cpuidex           /* map existing function */
#else                               /* if Linux/Unix system */

static void cpuid (int32_t info[4], int32_t eax, int32_t ecx)
{                                   /* --- get CPU information */
  __asm__ __volatile__ ("cpuid" :
                        "=a" (info[0]),
                        "=b" (info[1]),
                        "=c" (info[2]),
                        "=d" (info[3])
                        : "a" (eax), "c" (ecx));
  peax = eax;
  pecx = ecx;
}  /* cpuid() */

#endif  /* #ifdef _WIN32 .. #else .. */
/*----------------------------------------------------------------------------
References (cpuid):
  en.wikipedia.org/wiki/CPUID
  stackoverflow.com/a/7495023
  msdn.microsoft.com/en-us/library/vstudio/hskdteyh%28v=vs.100%29.aspx
----------------------------------------------------------------------------*/
#if defined __linux__ && defined HAVE_HWLOC

int corecntHwloc (void)
{                                   /* --- number of processor cores */
  int depth, cnt = -1;
  hwloc_topology_t topology;        /* init and load topology */
  hwloc_topology_init(&topology);
  hwloc_topology_load(topology);
  depth = hwloc_get_type_depth(topology, HWLOC_OBJ_CORE);
                                    /* get the number of cores */
  if (depth != HWLOC_TYPE_DEPTH_UNKNOWN)
    cnt = (int)hwloc_get_nbobjs_by_depth(topology, (unsigned)depth);
  hwloc_topology_destroy(topology); /* destroy topology object */
  return cnt;                       /* return the number of cores */
}  /* corecntHwloc() */

#endif  /* #if defined __linux__ && defined HAVE_HWLOC */
/*----------------------------------------------------------------------------
Additional info and references (corecntHwloc):
  This function depends on the Portable Hardware Locality (hwloc)
  software package.
  stackoverflow.com/a/12486105
  open-mpi.org/projects/hwloc
----------------------------------------------------------------------------*/
#ifdef __linux__                    /* if Linux system */

static int cmpids (const void *p, const void *q)
{                                   /* --- compare processor ids */
  const PROCIDS *a = (PROCIDS*)p;   /* type the given pointers */
  const PROCIDS *b = (PROCIDS*)q;   /* (PROCIDS array elements) */
  if (a->phys < b->phys) return -1;
  if (a->phys > b->phys) return +1;
  if (a->core < b->core) return -1;
  if (a->core > b->core) return +1;
  return 0;                         /* return sign of difference */
}  /* cmpids() */

/*--------------------------------------------------------------------------*/

static int enumerate (void)
{                                   /* --- enumerate topology */
  FILE    *fp;                      /* file for /proc/cpuinfo */
  PROCIDS *pids;                    /* processor ids (physical & core) */
  int n;                            /* # log. processors found in cpuinfo */
  int i, p, c, np, nc;              /* loop variables */

  /* determine number of logical processors (reference) */
  #ifdef _SC_NPROCESSORS_ONLN       /* if glibc's sysconf is available */
  nprocs = (int)sysconf(_SC_NPROCESSORS_ONLN);
  #else
  nprocs = -1;
  #endif
  if (nprocs < 0) {                 /* abort if nprocs can't be determined */
    nphys  = -1;
    ncores = -1;
    nprocs = -1;
    return -1;
  }

  /* collect physical id and core id of each logical processor */
  pids = calloc((size_t)nprocs, sizeof(PROCIDS));
  if (!pids) return -1;             /* create array for processor ids */
  fp = fopen("/proc/cpuinfo", "r");
  if (!fp) { free(pids); return -1; }
  for (c = p = n = 0; (c != EOF) && (n < nprocs); ) {
    if      (fscanf(fp, "physical id : %d", &i) > 0) {
      pids[n].phys = i; p |= 1; }
    else if (fscanf(fp, "core id : %d",     &i) > 0) {
      pids[n].core = i; p |= 2; }
    if (p >= 3) { p = 0; n += 1; }
    while (((c = fgetc(fp)) != EOF) && (c != '\n'));
  }
  fclose(fp);
  if (n != nprocs) {                /* abort if the number of log. procs */
    nphys  = -1;                    /* found differs from the reference */
    ncores = -1;
    nprocs = -1;
    free(pids);
    return -1;
  }

  /* sort the array of processor ids and determine the number of cores */
  qsort(pids, (size_t)nprocs, sizeof(PROCIDS), cmpids);
  p = pids[0].phys; c = pids[0].core;
  nphys = ncores = 1;
  for (i = np = nc = 0; i < n; i++) {
    if (pids[i].phys == p) {   np += 1;
      if (pids[i].core == c) { nc += 1; continue; }
      DBGMSG("phys %d, core %2d: %d logical processor(s)\n", p, c, nc);
      c = pids[i].core; nc = 1;
      ncores += 1; continue;
    }
    DBGMSG("phys %d, core %d: %d logical processor(s)\n", p, c, nc);
    DBGMSG("physical id %d: %d logical processor(s)\n", p, np);
    p = pids[i].phys; np = 1;       /* count number of core ids */
    c = pids[i].core; nc = 1;       /* per physical id and number */
    ncores += 1; nphys += 1;        /* of different physical ids */
  }
  DBGMSG("phys %d, core %d: %d logical processor(s)\n", p, c, nc);
  DBGMSG("physical id %d: %d logical processor(s)\n", p, np);
  DBGMSG("number of physical processors: %d\n", nphys);
  DBGMSG("number of cores: %d\n", ncores);
  free(pids);
  if (nprocs % ncores) {            /* check results for consistency */
    nphys  = -1;
    ncores = -1;
    nprocs = -1;
    return -1; }
  else
    return 0;
}  /* enumerate() */

/*--------------------------------------------------------------------------*/

int physcnt (void)
{                                   /* --- number of physical processors */
  if (!nphys)
    if (enumerate()) { return -1; }
  return nphys;
}  /* physcnt() */

/*--------------------------------------------------------------------------*/

int corecnt (void)
{                                   /* --- number of processor cores */
  if (!ncores)
    if (enumerate()) { return -1; }
  return ncores;
}  /* corecnt() */

/*--------------------------------------------------------------------------*/

int proccnt (void)
{                                   /* --- number of logical processors */
  if (!nprocs)
    if (enumerate()) { return -1; }
  return nprocs;
}  /* proccnt() */

/*--------------------------------------------------------------------------*/
#elif defined _WIN32                /* if Microsoft Windows system */

int physcnt (void)
{                                   /* --- number of physical processors */
  return -1;                        /* not yet implemented for Windows */
}  /* physcnt() */

/*--------------------------------------------------------------------------*/

int corecnt (void)
{                                   /* --- number of processor cores */
  return -1;                        /* not yet implemented for Windows */
}  /* corecnt() */

/*--------------------------------------------------------------------------*/

int proccnt (void)
{                                   /* --- number of logical processors */
  SYSTEM_INFO sysinfo;              /* system information structure */
  GetSystemInfo(&sysinfo);          /* get system information */
  return sysinfo.dwNumberOfProcessors;
}  /* proccnt() */

/*--------------------------------------------------------------------------*/
#elif defined __APPLE__             /* if Apple Mac OS system */

int physcnt (void)
{                                   /* --- number of physical processors */
  int nphys;
  size_t len = sizeof(nphys);
  if (sysctlbyname("hw.packages", &nphys, &len, NULL, (size_t)0))
    return -1;
  return nphys;
}  /* physcnt() */

/*--------------------------------------------------------------------------*/

int corecnt (void)
{                                   /* --- number of processor cores */
  int ncores;
  size_t len = sizeof(ncores);
  if (sysctlbyname("hw.physicalcpu", &ncores, &len, NULL, (size_t)0))
    return -1;
  return ncores;
}  /* corecnt() */

/*--------------------------------------------------------------------------*/

int proccnt (void)
{                                   /* --- number of logical processors */
  int nproc;
  size_t len = sizeof(nproc);
  if (sysctlbyname("hw.logicalcpu", &nproc, &len, NULL, (size_t)0))
    return -1;
  return nproc;
}  /* proccnt() */

#endif  /* #ifdef __linux__ .. #elif def. _WIN32 .. #elif def. __APPLE__ .. */
/*----------------------------------------------------------------------------
References (proccnt, Windows version):
  Info on SYSTEM_INFO structure:
  msdn.microsoft.com/en-us/library/windows/desktop/ms724958%28v=vs.85%29.aspx
Additional info and references (proccnt, Linux version):
  This function depends on glibc's sysconf extension.
  glibc also provides a function to access _SC_NPROCESSORS_ONLN
  in <sys/sysinfo.h> : get_nprocs which could be use instead of 
  sysconf(_SC_NPROCESSORS_ONLN).
  According to a comment by Joey Adams at stackoverflow.com/q/2693948
  sysconf(_SC_NPROCESSORS_ONLN) relies on /proc/stat if that exist and on
  /proc/cpuinfo if it doesn't.
  gnu.org/software/libc/manual/html_node/Processor-Resources.html
----------------------------------------------------------------------------*/

int proccntmax (void)
{                                   /* --- max. number of logical processors
                                     *     per physical processor */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] >> 16) & 0xff; /* EBX[23:16] */
}  /* proccntmax() */

/*----------------------------------------------------------------------------
References (proccntmax):
  "CPUID.1:EBX[23:16] represents the maximum number of addressable IDs
  (initial APIC ID) that can be assigned to logical processors in a
  physical package. The value may not be the same as the number of
  logical processors that are present in the hardware of a physical package."
  software.intel.com/en-us/articles/
    intel-64-architecture-processor-topology-enumeration
  software.intel.com/sites/default/files/63/1a/
    Kuo_CpuTopology_rc1.rh1.final.pdf
----------------------------------------------------------------------------*/

int hasMMX (void)
{                                   /* --- check for MMX instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[3] & (1 << 23)) != 0;
}  /* hasMMX() */

/*--------------------------------------------------------------------------*/

int hasSSE (void)
{                                   /* --- check for SSE instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[3] & (1 << 25)) != 0; /* EDX 25 */
}  /* hasSSE() */

/*--------------------------------------------------------------------------*/

int hasSSE2 (void)
{                                   /* --- check for SSE2 instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[3] & (1 << 26)) != 0; /* EDX 26 */
}  /* hasSSE2() */

/*--------------------------------------------------------------------------*/

int hasSSE3 (void)
{                                   /* --- check for SSE3 instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 <<  0)) != 0; /* ECX 0 */
}  /* hasSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSSE3 (void)
{                                   /* --- check for SSSE3 instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 <<  9)) != 0; /* ECX 9 */
}  /* hasSSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSE41 (void)
{                                   /* --- check for SSE4.1 instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 << 19)) != 0; /* ECX 19 */
}  /* hasSSE41() */

/*--------------------------------------------------------------------------*/

int hasSSE42 (void)
{                                   /* --- check for SSE4.2 instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 << 20)) != 0; /* ECX 20 */
}  /* hasSSE42() */

/*--------------------------------------------------------------------------*/

int hasPOPCNT (void)
{                                   /* --- check for popcnt instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 << 23)) != 0; /* ECX 23 */
}  /* hasPOPCNT() */

/*--------------------------------------------------------------------------*/

int hasAVX (void)
{                                   /* --- check for AVX instructions */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 << 28)) != 0; /* ECX 28 */
}  /* hasAVX() */

/*--------------------------------------------------------------------------*/

int hasAVX2 (void)
{                                   /* --- check for AVX2 instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 <<  5)) != 0; /* EBX 5 */
}  /* hasAVX2() */

/*--------------------------------------------------------------------------*/

int hasFMA3 (void)
{                                   /* --- check for FMA3 */
  int eax = 1;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[2] & (1 << 12)) != 0; /* ECX 12 */
}  /* hasFMA3() */

/*--------------------------------------------------------------------------*/

int hasAVX512f (void)
{                                   /* --- check for AVX512f instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 << 16)) != 0; /* EBX 16 */
}  /* hasAVX512f() */

/*--------------------------------------------------------------------------*/

int hasAVX512cd (void)
{                                   /* --- check for AVX512cd instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 << 28)) != 0; /* EBX 28 */
}  /* hasAVX512cd() */

/*--------------------------------------------------------------------------*/

int hasAVX512bw (void)
{                                   /* --- check for AVX512bw instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 << 30)) != 0; /* EBX 30 */
}  /* hasAVX512bw() */

/*--------------------------------------------------------------------------*/

int hasAVX512dq (void)
{                                   /* --- check for AVX512dq instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 << 17)) != 0; /* EBX 17 */
}  /* hasAVX512dq() */

/*--------------------------------------------------------------------------*/

int hasAVX512vl (void)
{                                   /* --- check for AVX512vl instructions */
  int eax = 7;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  return (cpuinfo[1] & (1 << 31)) != 0; /* EBX 31 */
}  /* hasAVX512vl() */

/*--------------------------------------------------------------------------*/

void getVendorID (char *buf)
{                                   /* --- get vendor id */
  /* the string is going to be exactly 12 characters long, allocate
     the buffer outside this function accordingly */
  int eax = 0;
  int ecx = 0;
  if ((eax != peax) || (ecx != pecx))
    cpuid(cpuinfo, eax, ecx);
  ((unsigned *)buf)[0] = (unsigned)cpuinfo[1]; /* EBX */
  ((unsigned *)buf)[1] = (unsigned)cpuinfo[3]; /* EDX */
  ((unsigned *)buf)[2] = (unsigned)cpuinfo[2]; /* ECX */
}  /* getVendorID() */

/*----------------------------------------------------------------------------
References (getVendorID):
  stackoverflow.com/a/3082553
----------------------------------------------------------------------------*/
#ifdef CPUINFO_MAIN

int main (int argc, char* argv[])
{
  char vendor[12];
  getVendorID(vendor);
  printf("Vendor              %s\n", vendor);
  printf("Physical processors %d\n", physcnt());
  printf("Processor cores     %d\n", corecnt());
  printf("Logical processors  %d\n", proccnt());
  printf("MMX                 %d\n", hasMMX());
  printf("SSE                 %d\n", hasSSE());
  printf("SSE2                %d\n", hasSSE2());
  printf("SSE3                %d\n", hasSSE3());
  printf("SSSE3               %d\n", hasSSSE3());
  printf("SSE41               %d\n", hasSSE41());
  printf("SSE42               %d\n", hasSSE42());
  printf("POPCNT              %d\n", hasPOPCNT());
  printf("AVX                 %d\n", hasAVX());
  printf("AVX2                %d\n", hasAVX2());
  printf("FMA3                %d\n", hasFMA3());
  printf("AVX512f             %d\n", hasAVX512f());
  printf("AVX512cd            %d\n", hasAVX512cd());
  printf("AVX512bw            %d\n", hasAVX512bw());
  printf("AVX512dq            %d\n", hasAVX512dq());
  printf("AVX512vl            %d\n", hasAVX512vl());

/*
   physcnt    -> number of physical processors/packages/sockets
   corecnt    -> number of processor cores
   proccnt    -> number of logical processors
   proccntmax -> max. number of logical processors per core
   See the glossary at:
     software.intel.com/en-us/articles/
       intel-64-architecture-processor-topology-enumeration
*/

}  /* main() */

#endif
