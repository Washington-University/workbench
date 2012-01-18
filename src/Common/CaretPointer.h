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
            T* m_pointer;//allows grab and release to be atomic including securing the correct pointer value (avoid assignment races)
            CaretPointerShare()
            {
                m_refCount = 1;//NOTE: don't initialize to 0, so we don't have to change it every time we make one
                m_doNotDelete = false;
                m_pointer = NULL;
            }
        };
        void fillMembers();
        T* m_pointer;//because less indirection should be more efficient
        CaretPointerShare* m_share;
        mutable CaretMutex m_mutex;//protects members from modification while reading, or from reading while modifying
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
            T* m_pointer;//allows grab and release to be atomic including securing the correct pointer value (avoid assignment races)
            int64_t m_size;
            CaretArrayShare()
            {
                m_refCount = 1;//NOTE: don't initialize to 0, so we don't have to change it every time we make one
                m_doNotDelete = false;
                m_pointer = NULL;
                m_size = 0;
            }
        };
        void fillMembers();
        mutable int64_t m_size;
        T* m_pointer;
        CaretArrayShare* m_share;
        mutable CaretMutex m_mutex;//protects members from modification while reading, or from reading while modifying
    public:
        CaretArray();
        ~CaretArray();
        CaretArray(const CaretArray<T>& right);
        CaretArray(int64_t size);//for simpler construction
        CaretArray(int64_t size, const T& initializer);//plus initialization
        CaretArray& operator=(const CaretArray<T>& right);
        bool operator==(const T* right) const;
        bool operator!=(const T* right) const;
        template <typename I>
        T& operator[](const I& index);
        template <typename I>
        const T& operator[](const I& index) const;
        operator T*();
        T* getArray();
        const int64_t& size() const;
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T* releasePointer();
    };

    //NOTE:begin pointer functions
    template <typename T>
    CaretPointer<T>::CaretPointer()
    {
        m_share = NULL;
        fillMembers();
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(const CaretPointer& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
            }
        }
        fillMembers();
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(T* right)
    {//don't need to lock self during constructor
        if (right == NULL)
        {
            m_share = NULL;
        } else {
            m_share = new CaretPointerShare();
            m_share->m_pointer = right;
        }
        fillMembers();
    }

    template <typename T>
    void CaretPointer<T>::fillMembers()
    {
        if (m_share == NULL)//the purpose of this is so that things that don't involve reference counts don't need to go through two pointers rather than just one
        {
            m_pointer = NULL;
        } else {
            m_pointer = m_share->m_pointer;
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
    T& CaretPointer<T>::operator*()
    {
        CaretAssert(m_pointer != NULL);
        return *(m_pointer);
    }

    template <typename T>
    CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer& right)
    {
        CaretPointer<T> temp(right);//copy construct from it, takes care of locking
        CaretPointerShare* tempShare = temp.m_share;//swap the shares and fill members
        temp.m_share = m_share;//don't need to lock temp before modifying it, it is local, also don't need to fill in its members, it is only going to call the destructor
        CaretMutexLocker locked(&m_mutex);//lock myself before modifying
        m_share = tempShare;
        fillMembers();
        return *this;
    }

    template <typename T>
    CaretPointer<T>::~CaretPointer()
    {//access during destructor is programmer error, don't lock self
        if (m_share == NULL) return;
        bool deleteShare = false;
        {
            CaretMutexLocker locked(&(m_share->m_mutex));//do lock the refcount, though
            if (--(m_share->m_refCount) == 0)
            {
                deleteShare = true;
                if (!m_share->m_doNotDelete) delete m_share->m_pointer;
            }
        }//left refcount unlock before deleting the object that contains it
        if (deleteShare)
        {
            delete m_share;
        }
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
        CaretMutexLocker locked(&m_mutex);//lock self so that the if expression doesn't change
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretPointer<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//and again
        if (m_share == NULL) return NULL;
        m_share->m_doNotDelete = true;
        return m_pointer;
    }

    //NOTE:begin array functions
    template <typename T>
    CaretArray<T>::CaretArray()
    {
        m_share = NULL;
        fillMembers();
    }

    template <typename T>
    CaretArray<T>::CaretArray(const CaretArray& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
            }
        }
        fillMembers();
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size)
    {
        if (size > 0)
        {
            m_share = new CaretArrayShare();
            m_share->m_pointer = new T[size];
            m_share->m_size = size;
        } else {
            m_share = NULL;
        }
        fillMembers();
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size, const T& initializer)
    {
        if (size > 0)
        {
            m_share = new CaretArrayShare();
            m_share->m_pointer = new T[size];
            m_share->m_size = size;
            T* end = m_pointer + size, *iter = m_pointer;
            do
            {
                *iter = initializer;//somewhat optimized, since this code will probably get used many places
                ++iter;
            } while (iter != end);
        } else {
            m_share = NULL;
        }
        fillMembers();
    }

    template <typename T>
    void CaretArray<T>::fillMembers()
    {
        if (m_share == NULL)//the purpose of this is so that things that don't involve reference counts don't need to go through two pointers rather than just one
        {
            m_pointer = NULL;
            m_size = 0;
        } else {
            m_pointer = m_share->m_pointer;
            m_size = m_share->m_size;
        }
    }

    template <typename T>
    T* CaretArray<T>::getArray()
    {
        return m_pointer;
    }

    template <typename T>
    const int64_t& CaretArray<T>::size() const
    {
        return m_size;
    }

    template <typename T>
    CaretArray<T>::operator T*()
    {
        return m_pointer;
    }

    template <typename T> template <typename I>
    T& CaretArray<T>::operator[](const I& index)
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && index < (I)m_size);
        return m_pointer[index];
    }

    template <typename T> template <typename I>
    const T& CaretArray<T>::operator[](const I& index) const
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && index < (I)m_size);
        return m_pointer[index];
    }

    template <typename T>
    CaretArray<T>& CaretArray<T>::operator=(const CaretArray& right)
    {
        CaretArray<T> temp(right);//copy construct from it, takes care of locking
        CaretArrayShare* tempShare = temp.m_share;//swap the shares and fill members
        temp.m_share = m_share;//don't need to lock temp before modifying it, it is local
        CaretMutexLocker locked(&m_mutex);//lock myself before modifying
        m_share = tempShare;
        fillMembers();
        return *this;
    }

    template <typename T>
    CaretArray<T>::~CaretArray()
    {//access during destructor is programmer error, don't lock self
        if (m_share == NULL) return;
        bool deleteShare = false;
        {
            CaretMutexLocker locked(&(m_share->m_mutex));//do lock the refcount, though
            if (--(m_share->m_refCount) == 0)
            {
                deleteShare = true;
                if (!m_share->m_doNotDelete) delete[] m_share->m_pointer;
            }
        }//left refcount unlock before deleting the object that contains it
        if (deleteShare)
        {
            delete m_share;
        }
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
        CaretMutexLocker locked(&m_mutex);//lock self so that the if expression doesn't change
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretArray<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//and again
        if (m_share == NULL) return NULL;
        m_share->m_doNotDelete = true;
        return m_pointer;
    }

}

#endif //__CARET_POINTER_H__
