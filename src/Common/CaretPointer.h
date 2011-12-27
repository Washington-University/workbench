
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

//memory managing pointers and standard pointers DO NOT get along (someone passes you a pointer, you assign it to a CaretPointer,
//    goes out of scope and deletes it on return, they try to use it after your function returns and it doesn't exist)

namespace caret {

    template <typename T>
    class CaretPointer
    {
        struct CaretPointerShare
        {
            int64_t m_refCount;
            CaretMutex m_mutex;//protects m_refCount
            bool m_doNotDelete;
            CaretPointerShare()
            {
                m_refCount = 0;
                m_doNotDelete = false;
            }
        };
        T* m_pointer;
        CaretPointerShare* m_share;
        void release();
        void grab(CaretPointerShare* toGrab);
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
        operator T*();
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T* releasePointer();
    };

    //separate these because delete and delete[] are slightly different, and only allow [] on one, and * on the other
    template <typename T>
    class CaretArray
    {
        struct CaretArrayShare
        {
            int64_t m_refCount;
            CaretMutex m_mutex;//protects m_refCount
            bool m_doNotDelete;
            CaretArrayShare()
            {
                m_refCount = 0;
                m_doNotDelete = false;
            }
        };
        int64_t m_size;
        T* m_pointer;
        CaretArrayShare* m_share;
        void release();
        void grab(CaretArrayShare* toGrab);
    public:
        CaretArray();
        ~CaretArray();
        CaretArray(const CaretArray<T>& right);
        CaretArray(int64_t size);//for simpler construction
        CaretArray& operator=(const CaretArray<T>& right);
        bool operator==(const T* right) const;
        bool operator!=(const T* right) const;
        T& operator[](const int64_t& index);
        const T& operator[](const int64_t& index) const;
        T* getArray();
        int64_t size() const;
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T* releasePointer();
    };

    template <typename T>
    class CaretReferenceCount
    {//instead of deleting, merely counts the number of references, so that an object can track what is or is not in use elsewhere - not useful because ones that automatically delete are easier to deal with?
        struct CaretRefCountShare
        {
            int64_t m_refCount;
            CaretMutex m_mutex;//protects m_refCount
            CaretRefCountShare()
            {
                m_refCount = 0;
            }
        };
        T* m_pointer;
        CaretRefCountShare* m_share;
        void release();
        void grab(CaretRefCountShare* toGrab);
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
        operator T*();
        int64_t getReferenceCount() const;
    };

    //NOTE:begin pointer functions
    template <typename T>
    CaretPointer<T>::CaretPointer()
    {
        m_share = NULL;
        m_pointer = NULL;
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(const CaretPointer& right)
    {
        grab(right.m_share);
        m_pointer = right.m_pointer;
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(T* right)
    {
        if (right == NULL)
        {
            m_share = NULL;
            m_pointer = NULL;
        } else {
            CaretPointerShare* temp = new CaretPointerShare();
            m_pointer = right;
            grab(temp);//to keep the code path the same
        }
    }


    template <typename T>
    T* CaretPointer<T>::getPointer()
    {
        return m_pointer;
    }

    template <typename T>
    CaretPointer<T>::operator T*()
    {
        return m_pointer;
    }

    template <typename T>
    void CaretPointer<T>::grab(CaretPointerShare* toGrab)
    {
        if (toGrab == NULL)
        {
            m_share = NULL;
        } else {
            CaretMutexLocker(&(toGrab->m_mutex));
            m_share = toGrab;
            ++(m_share->m_refCount);
        }
    }

    template <typename T>
    T& CaretPointer<T>::operator*()
    {
        CaretAssert(m_pointer != NULL);
        return *(m_pointer);
    }

    template <typename T>
    CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer& right)
    {
        if (this == &right) return *this;
        release();
        grab(right.m_share);
        m_pointer = right.m_pointer;
        return *this;
    }

    template <typename T>
    void CaretPointer<T>::release()
    {
        if (m_share == NULL) return;
        bool shouldDelete = false;
        {
            CaretMutexLocker(&(m_share->m_mutex));
            if (--(m_share->m_refCount) == 0)
            {
                shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
                if (!m_share->m_doNotDelete) delete m_pointer;
            }
        }
        if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
        {
            delete m_share;
        }
        m_pointer = NULL;
        m_share = NULL;
    }

    template <typename T>
    CaretPointer<T>::~CaretPointer()
    {
        release();
    }

    template <typename T>
    T* CaretPointer<T>::operator->()
    {
        return m_pointer;
    }

    template <typename T>
    bool CaretPointer<T>::operator==(const T* right) const
    {
        return (m_pointer == right);
    }

    template <typename T>
    bool CaretPointer<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }

    template <typename T>
    int64_t CaretPointer<T>::getReferenceCount() const
    {
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretPointer<T>::releasePointer()
    {
        if (m_share == NULL) return NULL;
        m_share->m_doNotDelete = true;
        return m_pointer;
    }

    //NOTE:begin array functions
    template <typename T>
    CaretArray<T>::CaretArray()
    {
        m_share = NULL;
        m_pointer = NULL;
        m_size = 0;
    }

    template <typename T>
    CaretArray<T>::CaretArray(const CaretArray& right)
    {
        grab(right.m_share);
        m_pointer = right.m_pointer;
        m_size = right.m_size;
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size)
    {
        if (size > 0)
        {
            CaretArrayShare* temp = new CaretArrayShare();
            m_pointer = new T[size];
            m_size = size;
            grab(temp);
        } else {
            m_pointer = NULL;
            m_share = NULL;
            m_size = 0;
        }
    }

    template <typename T>
    T* CaretArray<T>::getArray()
    {
        return m_pointer;
    }

    template <typename T>
    int64_t CaretArray<T>::size() const
    {
        return m_size;
    }

    template <typename T>
    void CaretArray<T>::grab(CaretArrayShare* toGrab)
    {
        if (toGrab == NULL)
        {
            m_share = NULL;
        } else {
            CaretMutexLocker(&(toGrab->m_mutex));
            m_share = toGrab;
            ++(m_share->m_refCount);
        }
    }

    template <typename T>
    T& CaretArray<T>::operator[](const int64_t& index)
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && index < m_size);
        return m_pointer[index];
    }

    template <typename T>
    const T& CaretArray<T>::operator[](const int64_t& index) const
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && index < m_size);
        return m_pointer[index];
    }

    template <typename T>
    CaretArray<T>& CaretArray<T>::operator=(const CaretArray& right)
    {
        if (this == &right) return *this;
        release();
        grab(right.m_share);
        m_pointer = right.m_pointer;
        m_size = right.m_size;
        return *this;
    }

    template <typename T>
    void CaretArray<T>::release()
    {
        if (m_share == NULL) return;
        bool shouldDelete = false;
        {
            CaretMutexLocker(&(m_share->m_mutex));
            if (--(m_share->m_refCount) == 0)
            {
                shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
                if (!m_share->m_doNotDelete) delete[] m_pointer;//the only real difference in how the objects work internally
            }
        }
        if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
        {
            delete m_share;
        }
        m_pointer = NULL;
        m_share = NULL;
        m_size = 0;
    }

    template <typename T>
    CaretArray<T>::~CaretArray()
    {
        release();
    }

    template <typename T>
    bool CaretArray<T>::operator==(const T* right) const
    {
        return (m_pointer == right);
    }
    
    template <typename T>
    bool CaretArray<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }

    template <typename T>
    int64_t CaretArray<T>::getReferenceCount() const
    {
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretArray<T>::releasePointer()
    {
        if (m_share == NULL) return NULL;
        m_share->m_doNotDelete = true;
        return m_pointer;
    }


    //NOTE:begin reference functions
    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount()
    {
        m_share = NULL;
        m_pointer = NULL;
    }

    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount(const CaretReferenceCount& right)
    {
        grab(right.m_share);
        m_pointer = right.m_pointer;
    }

    template <typename T>
    CaretReferenceCount<T>::CaretReferenceCount(T* right)
    {
        if (right == NULL)
        {
            m_share = NULL;
            m_pointer = NULL;
        } else {
            CaretRefCountShare* temp = new CaretRefCountShare();
            m_pointer = right;
            grab(temp);//to keep the code path the same
        }
    }


    template <typename T>
    T* CaretReferenceCount<T>::getPointer()
    {
        return m_pointer;
    }
    
    template <typename T>
    CaretReferenceCount<T>::operator T*()
    {
        return m_pointer;
    }

    template <typename T>
    void CaretReferenceCount<T>::grab(CaretRefCountShare* toGrab)
    {
        if (toGrab == NULL)
        {
            m_share = NULL;
        } else {
            CaretMutexLocker(&(toGrab->m_mutex));
            m_share = toGrab;
            ++(m_share->m_refCount);
        }
    }

    template <typename T>
    T& CaretReferenceCount<T>::operator*()
    {
        CaretAssert(m_pointer != NULL);
        return *(m_pointer);
    }

    template <typename T>
    CaretReferenceCount<T>& CaretReferenceCount<T>::operator=(const CaretReferenceCount& right)
    {
        if (this == &right) return *this;
        release();
        grab(right.m_share);
        m_pointer = right.m_pointer;
        return *this;
    }

    template <typename T>
    void CaretReferenceCount<T>::release()
    {
        if (m_share == NULL) return;
        bool shouldDelete = false;
        {
            CaretMutexLocker(&(m_share->m_mutex));
            if (--(m_share->m_refCount) == 0)
            {
                shouldDelete = true;//because we need to unlock the mutex before deleting the object containing it, or bad things will happen
            }
        }
        if (shouldDelete)//uses a LOCAL variable set inside the critical section to prevent more than one thread from evaluating true here
        {
            delete m_share;
        }
        m_pointer = NULL;//drop the reference, DO NOT delete, because this is NOT a memory management type
        m_share = NULL;
    }

    template <typename T>
    CaretReferenceCount<T>::~CaretReferenceCount()
    {
        release();
    }

    template <typename T>
    T* CaretReferenceCount<T>::operator->()
    {
        return m_pointer;
    }
    
    template <typename T>
    bool CaretReferenceCount<T>::operator==(const T* right) const
    {
        return (m_pointer == right);
    }

    template <typename T>
    bool CaretReferenceCount<T>::operator!=(const T* right) const
    {
        return !(*this == right);
    }

    template <typename T>
    int64_t CaretReferenceCount<T>::getReferenceCount() const
    {
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

}

#endif //__CARET_POINTER_H__
