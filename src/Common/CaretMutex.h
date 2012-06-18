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

#include <QMutex>

namespace caret {

   class CaretMutex : public QMutex
   {
   public:
       CaretMutex(RecursionMode mode = NonRecursive);
   };
   
   class CaretMutexLocker : public QMutexLocker
   {
   public:
      CaretMutexLocker(CaretMutex* theMutex);
   };
   
   //cheap object designed to enforce correct use of functions that may not be called by multiple threads in certain ways, for instance if each thread must have its own instance of an object
   //if something tries to lock the mutex while it is locked, a CaretException is thrown
   class CaretThrowMutex
   {
       bool m_inUse;
       CaretThrowMutex(const CaretThrowMutex& rhs);//prevent copy, assignment
       CaretThrowMutex& operator=(const CaretThrowMutex& rhs);
   public:
       CaretThrowMutex();
       friend class CaretThrowMutexLocker;
   };
   
   class CaretThrowMutexLocker
   {
       CaretThrowMutex* m_Mutex;
       CaretThrowMutexLocker();//prevent default construction, copy, assign
       CaretThrowMutexLocker(const CaretThrowMutexLocker& rhs);
       CaretThrowMutexLocker& operator=(const CaretThrowMutexLocker& rhs);
   public:
       CaretThrowMutexLocker(CaretThrowMutex* theMutex, const char* message);//set the message to include in the exception
       ~CaretThrowMutexLocker();
   };

}

#endif //__CARET_MUTEX_H__
