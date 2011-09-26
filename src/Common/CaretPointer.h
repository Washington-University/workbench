
#ifndef __CARET_POINTER_H__
#define __CARET_POINTER_H__

/*LICENSE_START*/ 
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

#include "CaretMutex.h"
#include "CaretAssert.h"

namespace caret {
   
   template <typename T>
   class CaretPointer
   {
      struct CaretPointerRef
      {
         T* m_pointer;
         uint64_t m_refCount;
         CaretMutex m_mutex;//protects m_refCount
      };
      CaretPointerRef* m_pointerRef;
      void release();
      void grab(CaretPointerRef* toGrab);
   public:
      CaretPointer();
      ~CaretPointer();
      CaretPointer(const CaretPointer<T>& right);
      CaretPointer(T* right);
      CaretPointer& operator=(const CaretPointer<T>& right);
      T& operator*();
      T* getPointer();//use at your own peril
   };

   template <typename T>
   CaretPointer<T>::CaretPointer()
   {
      m_pointerRef = NULL;
   }
   
   template <typename T>
   CaretPointer<T>::CaretPointer(const CaretPointer& right)
   {
      grab(right.m_pointerRef);
   }
   
   template <typename T>
   CaretPointer<T>::CaretPointer(T* right)
   {
      m_pointerRef = NULL;
      if (right == NULL) return;
      CaretPointerRef* temp = new CaretPointerRef();
      CaretAssert(temp != NULL);
      temp->m_pointer = right;
      temp->m_refCount = 0;
      grab(temp);//to keep the code path the same
   }


   template <typename T>
   T* CaretPointer<T>::getPointer()
   {
      if (m_pointerRef == NULL) return NULL;
      return m_pointerRef->m_pointer;
   }

   template <typename T>
   void CaretPointer<T>::grab(CaretPointerRef* toGrab)
   {
      if (toGrab == NULL) return;
      m_pointerRef = toGrab;
      CaretAssert(m_pointerRef->m_pointer != NULL);
      CaretMutexLocker(&(m_pointerRef->m_mutex));
      ++(m_pointerRef->m_refCount);
   }

   template <typename T>
   T& CaretPointer<T>::operator*()
   {
      CaretAssert(m_pointerRef != NULL);
      return *(m_pointerRef->m_pointer);
   }

   template <typename T>
   CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer& right)
   {
      if (this == &right) return *this;
      release();
      grab(right.m_pointerRef);
      return *this;
   }

   template <typename T>
   void CaretPointer<T>::release()
   {
      if (m_pointerRef == NULL) return;
      CaretAssert(m_pointerRef->m_pointer != NULL);
      bool shouldDelete = false;
      {
         CaretMutexLocker(&(m_pointerRef->m_mutex));
         if (--(m_pointerRef->m_refCount) == 0)
         {
            shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it
            delete m_pointerRef->m_pointer;
            m_pointerRef->m_pointer = NULL;
         }
      }
      if (shouldDelete)
      {
         delete m_pointerRef;
         m_pointerRef = NULL;
      }
   }
   
   template <typename T>
   CaretPointer<T>::~CaretPointer()
   {
      release();
   }

}

#endif //__CARET_POINTER_H__
