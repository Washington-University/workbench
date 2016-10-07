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
#define DBGMSG(...)  fprintf(stderr, __VA_ARGS__)
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
static int cpuinfo[5];              /* cpu information */

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
#ifdef _WIN32                       /* if Microsoft Windows system */
#define cpuid   __cpuid             /* map existing function */
#else                               /* if Linux/Unix system */

static void cpuid (int32_t info[4], int32_t type)
{                                   /* --- get CPU information */
  __asm__ __volatile__ ("cpuid" :
                        "=a" (info[0]),
                        "=b" (info[1]),
                        "=c" (info[2]),
                        "=d" (info[3])
                        : "a" (type), "c" (0)); // : "a" (type));
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

int corecnt (void)
{                                   /* --- number of processor cores */
  FILE    *fp;                      /* file for /proc/cpuinfo */
  PROCIDS *pids;                    /* processor ids (physical & core) */
  int n;                            /* # log. processors found in cpuinfo */
  int i, p, c, np, nc;              /* loop variables */
  int nphys  = 0;                   /* number of physical processors */
  int ncores = 0;                   /* number of physical cores */
  int nprocs = proccnt();           /* # log. processors (reference) */

  if (nprocs < 0) return -1;        /* abort if nprocs can't be determined */

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
  if (n != nprocs) { free(pids); return -1; }

  /* sort the array of processor ids and determine the number of cores */
  qsort(pids, (size_t)nprocs, sizeof(PROCIDS), cmpids);
  p = pids[0].phys; c = pids[0].core;
  nphys = ncores = 1;
  for (i = np = nc = 0; i < n; i++) {
    if (pids[i].phys == p) {   np += 1;
      if (pids[i].core == c) { nc += 1; continue; }
      DBGMSG("phys %d, core %d: %d logical processor(s)\n", p, c, nc);
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
  if ((ncores != nprocs) && (ncores != nprocs/2))
    return -1;                      /* check result for consistency */
  return ncores;                    /* return the number of cores */
}  /* corecnt() */

#elif defined _WIN32                /* if Microsoft Windows system */

int corecnt (void)
{                                   /* --- number of processor cores */
  return -1;                        /* not yet implemented for Windows */
}  /* corecnt() */

#elif defined __APPLE__             /* if Apple Mac OS system */

int corecnt (void)
{                                   /* --- number of processor cores */
  int ncores;
  size_t len = sizeof(ncores);
  if (sysctlbyname("hw.physicalcpu", &ncores, &len, NULL, (size_t)0))
    return -1;
  return ncores;
}  /* corecnt() */

#endif  /* #ifdef __linux__ .. #elif def. _WIN32 .. #elif def. __APPLE__ .. */
/*--------------------------------------------------------------------------*/
#ifdef __linux__                    /* if Linux system */
#ifdef _SC_NPROCESSORS_ONLN         /* if glibc's sysconf is available */

int proccnt (void)
{                                   /* --- number of logical processors */
  return (int)sysconf(_SC_NPROCESSORS_ONLN);
}  /* proccnt() */

#endif  /* #ifdef _SC_NPROCESSORS_ONLN */
#elif defined _WIN32                /* if Microsoft Windows system */

int proccnt (void)
{                                   /* --- number of logical processors */
  SYSTEM_INFO sysinfo;              /* system information structure */
  GetSystemInfo(&sysinfo);          /* get system information */
  return sysinfo.dwNumberOfProcessors;
}  /* proccnt() */

#elif defined __APPLE__             /* if Apple Mac OS system */

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
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
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
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 23)) != 0;
}  /* hasMMX() */

/*--------------------------------------------------------------------------*/

int hasSSE (void)
{                                   /* --- check for SSE instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 25)) != 0;
}  /* hasSSE() */

/*--------------------------------------------------------------------------*/

int hasSSE2 (void)
{                                   /* --- check for SSE2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[3] & (1 << 26)) != 0;
}  /* hasSSE2() */

/*--------------------------------------------------------------------------*/

int hasSSE3 (void)
{                                   /* --- check for SSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  0)) != 0;
}  /* hasSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSSE3 (void)
{                                   /* --- check for SSSE3 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 <<  9)) != 0;
}  /* hasSSSE3() */

/*--------------------------------------------------------------------------*/

int hasSSE41 (void)
{                                   /* --- check for SSE4.1 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 19)) != 0;
}  /* hasSSE41() */

/*--------------------------------------------------------------------------*/

int hasSSE42 (void)
{                                   /* --- check for SSE4.2 instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 20)) != 0;
}  /* hasSSE42() */

/*--------------------------------------------------------------------------*/

int hasPOPCNT (void)
{                                   /* --- check for popcnt instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 23)) != 0;
}  /* hasPOPCNT() */

/*--------------------------------------------------------------------------*/

int hasAVX (void)
{                                   /* --- check for AVX instructions */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 28)) != 0;
}  /* hasAVX() */

/*--------------------------------------------------------------------------*/

int hasFMA3 (void)
{                                   /* --- check for FMA3 */
  if (!cpuinfo[4]) { cpuid(cpuinfo, 1); cpuinfo[4] = -1; }
  return (cpuinfo[2] & (1 << 12)) != 0;
}  /* hasFMA3() */

/*--------------------------------------------------------------------------*/

void getVendorID (char *buf)
{                                   /* --- get vendor id */
  /* the string is going to be exactly 12 characters long, allocate
     the buffer outside this function accordingly */
  int regs[4];
  cpuid(regs, 0);
  ((unsigned *)buf)[0] = (unsigned)regs[1]; // EBX
  ((unsigned *)buf)[1] = (unsigned)regs[3]; // EDX
  ((unsigned *)buf)[2] = (unsigned)regs[2]; // ECX
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
  printf("Vendor             %s\n", vendor);
  #ifndef _WIN32
  printf("Processor cores    %d\n", corecnt());
  #endif
  printf("Logical processors %d\n", proccnt());
  printf("MMX                %d\n", hasMMX());
  printf("SSE                %d\n", hasSSE());
  printf("SSE2               %d\n", hasSSE2());
  printf("SSE3               %d\n", hasSSE3());
  printf("SSSE3              %d\n", hasSSSE3());
  printf("SSE41              %d\n", hasSSE41());
  printf("SSE42              %d\n", hasSSE42());
  printf("POPCNT             %d\n", hasPOPCNT());
  printf("AVX                %d\n", hasAVX());
  printf("FMA3               %d\n", hasFMA3());

/* corecnt    -> number of processor cores
   proccnt    -> number of logical processors
   proccntmax -> max. number of logical processors per core
   See the glossary at:
     software.intel.com/en-us/articles/
       intel-64-architecture-processor-topology-enumeration */

}  /* main() */

#endif
