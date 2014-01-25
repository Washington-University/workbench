/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#ifndef __CARET_MUTEX_H__
#define __CARET_MUTEX_H__

#include "CaretOMP.h"

//omp mutexes are faster than QMutex, especially without contention
#ifdef CARET_OMP

namespace caret {
    
    class CaretMutex
    {
        omp_lock_t m_lock;
    public:
        CaretMutex(const CaretMutex&) { omp_init_lock(&m_lock); };//allow copy, assign, but make them do nothing other than default construct
        CaretMutex& operator=(const CaretMutex&) { return *this; };
        CaretMutex() { omp_init_lock(&m_lock); }
        ~CaretMutex() { omp_destroy_lock(&m_lock); }
        friend class CaretMutexLocker;
    };
    
    class CaretMutexLocker
    {
        CaretMutex* m_mutex;
        CaretMutexLocker();//disallow default construction, assign
        CaretMutexLocker& operator=(const CaretMutexLocker& rhs);
    public:
        CaretMutexLocker(CaretMutex* mutex) { m_mutex = mutex; omp_set_lock(&(m_mutex->m_lock)); }
        ~CaretMutexLocker() { omp_unset_lock(&(m_mutex->m_lock)); }
    };
    
}

#else
//if we don't have openmp, fall back to QMutex
#include <QMutex>

namespace caret {
    
    class CaretMutex : public QMutex
    {
    public:
        CaretMutex(RecursionMode mode = NonRecursive) : QMutex(mode) { }
        CaretMutex(const CaretMutex&) : QMutex() { };//allow copy, assign, but make them do nothing other than default construct
        CaretMutex& operator=(const CaretMutex&) { return *this; };
    };
   
    class CaretMutexLocker : public QMutexLocker
    {
    public:
        CaretMutexLocker(CaretMutex* theMutex) : QMutexLocker(theMutex) { }
    };

}

#endif

#endif //__CARET_MUTEX_H__
