
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

//NOTE: I do NOT intend to ever support operator=(T*), because it would be WAY too easy to change a pointer to a CaretPointer,
//    miss one of the places where it gets assigned to by a pointer, and have runtime rather than compilation breakage
//memory managing pointers and standard pointers DO NOT get along (someone passes you a pointer, you assign it to a CaretPointer,
//    goes out of scope and deletes it on return, they try to use it after your function returns and it doesn't exist)

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
      bool operator==(const T* right) const;
      bool operator!=(const T* right) const;
      T& operator*();
      T* operator->();
      T* getPointer();
      uint64_t getReferenceCount() const;
   };

   //separate these because delete and delete[] are slightly different, and only allow [] on one, and * on the other
   template <typename T>
   class CaretArray
   {
      struct CaretArrayRef
      {
         T* m_pointer;
         int64_t m_size;
         uint64_t m_refCount;
         CaretMutex m_mutex;//protects m_refCount
      };
      CaretArrayRef* m_pointerRef;
      void release();
      void grab(CaretArrayRef* toGrab);
   public:
      CaretArray();
      ~CaretArray();
      CaretArray(const CaretArray<T>& right);
      CaretArray(T* right, int64_t size);
      CaretArray(int64_t size);//for simpler construction
      CaretArray& operator=(const CaretArray<T>& right);
      bool operator==(const T* right) const;
      bool operator!=(const T* right) const;
      T& operator[](const uint64_t& index);
      T* getArray();
      int64_t size() const;
      uint64_t getReferenceCount() const;
   };

   template <typename T>
   class CaretReferenceCount
   {//instead of deleting, merely counts the number of references, so that an object can track what is or is not in use elsewhere - not useful because ones that automatically delete are easier to deal with?
       struct CaretRefCountRef
       {
           T* m_pointer;
           uint64_t m_refCount;
           CaretMutex m_mutex;//protects m_refCount
       };
       CaretRefCountRef* m_pointerRef;
       void release();
       void grab(CaretRefCountRef* toGrab);
   public:
       CaretReferenceCount();
       ~CaretReferenceCount();
       CaretReferenceCount(const CaretReferenceCount<T>& right);
       CaretReferenceCount(T* right);
       CaretReferenceCount& operator=(const CaretReferenceCount<T>& right);
       bool operator==(const T* right) const;
       bool operator!=(const T* right) const;
       T& operator*();
       T* operator->();
       T* getPointer();
       uint64_t getReferenceCount() const;
   };
   
   //NOTE:begin pointer functions
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
            shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
            delete m_pointerRef->m_pointer;
            m_pointerRef->m_pointer = NULL;
         }
      }
      if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
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
   
   template <typename T>
   T* CaretPointer<T>::operator->()
   {
      if (m_pointerRef == NULL) return NULL;
      return m_pointerRef->m_pointer;
   }

    template <typename T>
    bool CaretPointer<T>::operator==(const T* right) const
    {
        if (m_pointerRef == NULL)
        {
            return right == NULL;
        }
        return (m_pointerRef->m_pointer == right);
    }

    template <typename T>
    bool CaretPointer<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }
    
    template <typename T>
    uint64_t CaretPointer<T>::getReferenceCount() const
    {
        if (m_pointerRef == NULL)
        {
            return 0;
        }
        return m_pointerRef->m_refCount;
    }

   //NOTE:begin array functions
   template <typename T>
   CaretArray<T>::CaretArray()
   {
      m_pointerRef = NULL;
   }
   
   template <typename T>
   CaretArray<T>::CaretArray(const CaretArray& right)
   {
      grab(right.m_pointerRef);
   }
   
    template <typename T>
    CaretArray<T>::CaretArray(int64_t size)
    {
        CaretAssert(size > 0);
        m_pointerRef = NULL;
        CaretArrayRef* temp = new CaretArrayRef();
        temp->m_pointer = new T[size];
        temp->m_refCount = 0;
        temp->m_size = size;
        grab(temp);
    }
   
   template <typename T>
   CaretArray<T>::CaretArray(T* right, int64_t size)
   {
      m_pointerRef = NULL;
      if (right == NULL) return;
      CaretArrayRef* temp = new CaretArrayRef();
      temp->m_pointer = right;
      temp->m_refCount = 0;
      temp->m_size = size;
      grab(temp);//to keep the code path the same
   }


   template <typename T>
   T* CaretArray<T>::getArray()
   {
      if (m_pointerRef == NULL) return NULL;
      return m_pointerRef->m_pointer;
   }

   template <typename T>
   int64_t CaretArray<T>::size() const
   {
      if (m_pointerRef == NULL) return 0;
      return m_pointerRef->m_size;
   }

   template <typename T>
   void CaretArray<T>::grab(CaretArrayRef* toGrab)
   {
      if (toGrab == NULL) return;
      m_pointerRef = toGrab;
      CaretAssert(m_pointerRef->m_pointer != NULL);
      CaretMutexLocker(&(m_pointerRef->m_mutex));
      ++(m_pointerRef->m_refCount);
   }

   template <typename T>
   T& CaretArray<T>::operator[](const uint64_t& index)
   {
      CaretAssert(m_pointerRef != NULL);
      return m_pointerRef->m_pointer[index];
   }

   template <typename T>
   CaretArray<T>& CaretArray<T>::operator=(const CaretArray& right)
   {
      if (this == &right) return *this;
      release();
      grab(right.m_pointerRef);
      return *this;
   }

   template <typename T>
   void CaretArray<T>::release()
   {
      if (m_pointerRef == NULL) return;
      CaretAssert(m_pointerRef->m_pointer != NULL);
      bool shouldDelete = false;
      {
         CaretMutexLocker(&(m_pointerRef->m_mutex));
         if (--(m_pointerRef->m_refCount) == 0)
         {
            shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
            delete[] m_pointerRef->m_pointer;//the only real difference in how the objects work internally
            m_pointerRef->m_pointer = NULL;
         }
      }
      if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
      {
         delete m_pointerRef;
         m_pointerRef = NULL;
      }
   }
   
   template <typename T>
   CaretArray<T>::~CaretArray()
   {
      release();
   }

    template <typename T>
    bool CaretArray<T>::operator==(const T* right) const
    {
        if (m_pointerRef == NULL)
        {
            return right == NULL;
        }
        return (m_pointerRef->m_pointer == right);
    }
    
    template <typename T>
    bool CaretArray<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }

    template <typename T>
    uint64_t CaretArray<T>::getReferenceCount() const
    {
        if (m_pointerRef == NULL)
        {
            return 0;
        }
        return m_pointerRef->m_refCount;
    }

    //NOTE:begin reference functions
    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount()
    {
        m_pointerRef = NULL;
    }

    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount(const CaretReferenceCount& right)
    {
        grab(right.m_pointerRef);
    }

    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount(T* right)
    {
        m_pointerRef = NULL;
        if (right == NULL) return;
        CaretRefCountRef* temp = new CaretRefCountRef();
        temp->m_pointer = right;
        temp->m_refCount = 0;
        grab(temp);//to keep the code path the same
    }


    template <typename T>
    T* CaretReferenceCount<T>::getPointer()
    {
        if (m_pointerRef == NULL) return NULL;
        return m_pointerRef->m_pointer;
    }

    template <typename T>
    void CaretReferenceCount<T>::grab(CaretRefCountRef* toGrab)
    {
        if (toGrab == NULL) return;
        m_pointerRef = toGrab;
        CaretAssert(m_pointerRef->m_pointer != NULL);
        CaretMutexLocker(&(m_pointerRef->m_mutex));
        ++(m_pointerRef->m_refCount);
    }

    template <typename T>
    T& CaretReferenceCount<T>::operator*()
    {
        CaretAssert(m_pointerRef != NULL);
        return *(m_pointerRef->m_pointer);
    }

    template <typename T>
    CaretReferenceCount<T>& CaretReferenceCount<T>::operator=(const CaretReferenceCount& right)
    {
        if (this == &right) return *this;
        release();
        grab(right.m_pointerRef);
        return *this;
    }

    template <typename T>
    void CaretReferenceCount<T>::release()
    {
        if (m_pointerRef == NULL) return;
        CaretAssert(m_pointerRef->m_pointer != NULL);
        bool shouldDelete = false;
        {
            CaretMutexLocker(&(m_pointerRef->m_mutex));
            if (--(m_pointerRef->m_refCount) == 0)
            {
                shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
                m_pointerRef->m_pointer = NULL;//drop the reference, DO NOT delete, because this is NOT a memory management type
            }
        }
        if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
        {
            delete m_pointerRef;
            m_pointerRef = NULL;
        }
    }

    template <typename T>
    CaretReferenceCount<T>::~CaretReferenceCount()
    {
        release();
    }

    template <typename T>
    T* CaretReferenceCount<T>::operator->()
    {
        if (m_pointerRef == NULL) return NULL;
        return m_pointerRef->m_pointer;
    }
    
    template <typename T>
    bool CaretReferenceCount<T>::operator==(const T* right) const
    {
        if (m_pointerRef == NULL)
        {
            return right == NULL;
        }
        return (m_pointerRef->m_pointer == right);
    }

    template <typename T>
    bool CaretReferenceCount<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }

    template <typename T>
    uint64_t CaretReferenceCount<T>::getReferenceCount() const
    {
        if (m_pointerRef == NULL)
        {
            return 0;
        }
        return m_pointerRef->m_refCount;
    }

}

#endif //__CARET_POINTER_H__
