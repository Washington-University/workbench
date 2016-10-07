/*----------------------------------------------------------------------------
  File    : real-is-double.inc
  Contents: check the definition of REAL
  Author  : Kristian Loewe, Christian Borgelt
----------------------------------------------------------------------------*/

#ifdef REAL
#  define float  1                // to check the definition of REAL
#  define double 2
#  if   REAL == float             // if single precision data
#    undef  REAL_IS_DOUBLE
#    define REAL_IS_DOUBLE  0     // clear indicator for double
#  elif REAL == double            // if double precision data
#    undef  REAL_IS_DOUBLE
#    define REAL_IS_DOUBLE  1     // set   indicator for double
#  else
#    error "REAL must be either 'float' or 'double'"
#  endif
#  undef float                    // delete definitions
#  undef double                   // used for type checking
#endif
