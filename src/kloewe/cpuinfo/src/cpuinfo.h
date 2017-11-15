/*----------------------------------------------------------------------------
  File    : cpuinfo.h
  Contents: processor information queries
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/
#ifndef CPUINFO_H
#define CPUINFO_H

/*----------------------------------------------------------------------------
  Functions
----------------------------------------------------------------------------*/
extern int physcnt       (void); /* # physical processors */
extern int corecnt       (void); /* # processor cores */
extern int proccnt       (void); /* # logical processors */

extern int hasMMX        (void);
extern int hasSSE        (void);
extern int hasSSE2       (void);
extern int hasSSE3       (void);
extern int hasSSSE3      (void);
extern int hasSSE41      (void);
extern int hasSSE42      (void);
extern int hasPOPCNT     (void);
extern int hasAVX        (void);
extern int hasAVX2       (void);
extern int hasFMA3       (void);
extern int hasAVX512f    (void);
extern int hasAVX512cd   (void);
extern int hasAVX512bw   (void);
extern int hasAVX512dq   (void);
extern int hasAVX512vl   (void);

#endif  /* #ifndef CPUINFO_H */
