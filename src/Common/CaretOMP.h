#ifndef __CARET_OMP_H__
#define __CARET_OMP_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

///this include file is for anything specific to how we use openmp in caret, and also for convenience
///for instance, you don't need #ifdef guards around including this file, unlike omp.h

#ifdef _OPENMP
#include "omp.h"

///another define to use in guards, might be easier to remember
#define CARET_OMP

#endif //_OPENMP

///some defines for #pragma omp, in case we want to add default options (for workarounds or global options)
///if we never need to use these for workarounds or global options, so be it
///NOTE: neither "#pragma" nor "omp" can be put into a define and have it work correctly, so they can't be made that much more friendly
///defined regardless so the preprocessor and compiler aren't any more confused if _OPENMP isn't defined (will complain about ignoring pragma if used without guards)
///DO NOT add scheduling to this list, scheduling choice needs to be able to be changed
#define CARET_PAR_OPTIONS
#define CARET_FOR_OPTIONS
#define CARET_SINGLE_OPTIONS

///and defines to combine them with the pragmas they are intended for
///use them as "#pragma omp CARET_PARFOR [other options]"
#define CARET_PAR parallel CARET_PAR_OPTIONS
#define CARET_FOR for CARET_FOR_OPTIONS
#define CARET_PARFOR parallel for CARET_PAR_OPTIONS CARET_FOR_OPTIONS
#define CARET_SINGLE single CARET_SINGLE_OPTIONS

#endif //__CARET_OMP_H__
