/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
#include <QMutexLocker>
#include <QMutex>

namespace caret {
    
    class CaretMutex : public QMutex
    {
    public:
        
#if QT_VERSION >= 0x060000
        /*
         * Default constructor needed for Qt6.
         * If Recursion was used in any code, it needs replacement with QRecursiveMutex
         */
        CaretMutex() : QMutex() { }
#else
        /* Recursion mode removed in Qt 6; QRecursiveMutex class was added*/
        CaretMutex(RecursionMode mode = NonRecursive) : QMutex(mode) { }
#endif
        CaretMutex(const CaretMutex&) : QMutex() { };//allow copy, assign, but make them do nothing other than default construct
        CaretMutex& operator=(const CaretMutex&) { return *this; };
    };
   
#if QT_VERSION >= 0x060000
    class CaretMutexLocker : public QMutexLocker<QMutex>
#else
    class CaretMutexLocker : public QMutexLocker
#endif
    {
    public:
        CaretMutexLocker(CaretMutex* theMutex) : QMutexLocker(theMutex) { }
    };

}

#endif

#endif //__CARET_MUTEX_H__
