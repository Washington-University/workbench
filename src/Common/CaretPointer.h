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

    struct CaretPointerShare
    {//can't be member type because member types of templates on different types are incompatible
        int64_t m_refCount;
        CaretMutex m_mutex;//protects m_refCount
        bool m_doNotDelete;
        CaretPointerShare()
        {
            m_refCount = 1;//NOTE: don't initialize to 0, so we don't have to change it every time we make one
            m_doNotDelete = false;
        }
    };
    
    template <typename T>
    class CaretPointer
    {
        T* m_pointer;//to allow different templated CaretPointers to share the refcount, the share object can't contain the pointer
        CaretPointerShare* m_share;
        mutable CaretMutex m_mutex;//protects members from modification while reading, or from reading while modifying
    public:
        CaretPointer();
        ~CaretPointer();
        CaretPointer(const CaretPointer& right);//because a templated function apparently can't override default copy
        template <typename T2>
        CaretPointer(const CaretPointer<T2>& right);
        explicit CaretPointer(T* right);
        CaretPointer& operator=(const CaretPointer& right);//or default =
        template <typename T2>
        CaretPointer& operator=(const CaretPointer<T2>& right);
        void grabNew(T* right);//substitute for operator= to bare pointer
        template <typename T2>
        bool operator==(const T2* right) const;
        template <typename T2>
        bool operator!=(const T2* right) const;
        template <typename T2>
        bool operator==(const CaretPointer<T2>& right) const;
        template <typename T2>
        bool operator!=(const CaretPointer<T2>& right) const;
        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;
        T* getPointer();
        operator T*();
        operator const T*() const;
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T* releasePointer();
        template <typename T2> friend class CaretPointer;//because for const compatibility, we need to access a different template's members
    };

    //separate these because delete and delete[] are slightly different, and only allow [] on one, and * on the other
    template <typename T>
    class CaretArray
    {
        int64_t m_size;
        T* m_pointer;
        CaretPointerShare* m_share;//same share because it doesn't contain any specific information about what it is counting
        mutable CaretMutex m_mutex;//protects members from modification while reading, or from reading while modifying
    public:
        CaretArray();
        ~CaretArray();
        CaretArray(const CaretArray& right);
        template <typename T2>
        CaretArray(const CaretArray<T2>& right);
        CaretArray(int64_t size);//for simpler construction
        CaretArray(int64_t size, const T& initializer);//plus initialization
        CaretArray& operator=(const CaretArray& right);
        template <typename T2>
        CaretArray& operator=(const CaretArray<T2>& right);
        template <typename T2>
        bool operator==(const T2* right) const;
        template <typename T2>
        bool operator!=(const T2* right) const;
        template <typename T2>
        bool operator==(const CaretArray<T2>& right) const;
        template <typename T2>
        bool operator!=(const CaretArray<T2>& right) const;
        template <typename I>
        T& operator[](const I& index);
        template <typename I>
        const T& operator[](const I& index) const;
        operator T*();
        operator const T*() const;
        T* getArray();
        const T* getArray() const;
        const int64_t& size() const;
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T* releasePointer();
        template <typename T2> friend class CaretArray;
    };

    //NOTE:begin pointer functions
    template <typename T>
    CaretPointer<T>::CaretPointer()
    {
        m_share = NULL;
        m_pointer = NULL;
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(const CaretPointer<T>& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
                m_pointer = NULL;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
                m_pointer = right.m_pointer;
            }
        }
    }

    template <typename T> template <typename T2>
    CaretPointer<T>::CaretPointer(const CaretPointer<T2>& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
                m_pointer = NULL;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
                m_pointer = right.m_pointer;
            }
        }
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(T* right)
    {//don't need to lock self during constructor
        if (right == NULL)
        {
            m_share = NULL;
            m_pointer = NULL;
        } else {
            m_share = new CaretPointerShare();//starts refcount at 1
            m_pointer = right;
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
    CaretPointer<T>::operator const T*() const
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
    const T& CaretPointer<T>::operator*() const
    {
        CaretAssert(m_pointer != NULL);
        return *(m_pointer);
    }

    template <typename T>
    CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer<T>& right)
    {
        CaretPointer<T> temp(right);//copy construct from it, takes care of locking and type checking
        CaretPointerShare* tempShare = temp.m_share;//prepare to swap the members
        T* tempPointer = temp.m_pointer;
        CaretMutexLocker locked(&m_mutex);//lock myself before using internal state
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;
        return *this;//temp destructor takes care of the rest
    }

    template <typename T> template <typename T2>
    CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer<T2>& right)
    {
        CaretPointer<T> temp(right);//copy construct from it, takes care of locking and type checking
        CaretPointerShare* tempShare = temp.m_share;//prepare to swap the members
        T* tempPointer = temp.m_pointer;
        CaretMutexLocker locked(&m_mutex);//lock myself before using internal state
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;
        return *this;//temp destructor takes care of the rest
    }

    template <typename T>
    void CaretPointer<T>::grabNew(T* right)
    {
        CaretPointer<T> temp(right);//construct from the pointer
        CaretPointerShare* tempShare = temp.m_share;//prepare to swap the members
        T* tempPointer = temp.m_pointer;
        CaretMutexLocker locked(&m_mutex);//lock myself before using internal state
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;//destructor of temp takes care of the rest
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
                if (!m_share->m_doNotDelete) delete m_pointer;
            }
        }//unlock refcount mutex before deleting the object that contains it, otherwise Very Bad Things
        if (deleteShare)
        {
            delete m_share;
        }
    }

    template <typename T>
    T* CaretPointer<T>::operator->()
    {
        CaretAssert(m_pointer != NULL);
        return m_pointer;
    }

    template <typename T>
    const T* CaretPointer<T>::operator->() const
    {
        CaretAssert(m_pointer != NULL);
        return m_pointer;
    }

    template <typename T> template <typename T2>
    bool CaretPointer<T>::operator==(const T2* right) const
    {
        return (m_pointer == right);
    }

    template <typename T> template <typename T2>
    bool CaretPointer<T>::operator!=(const T2* right) const
    {
        return !(*this == right);
    }

    template <typename T> template <typename T2>
    bool CaretPointer<T>::operator==(const CaretPointer<T2>& right) const
    {
        return (m_pointer == right.m_pointer);
    }

    template <typename T> template <typename T2>
    bool CaretPointer<T>::operator!=(const CaretPointer<T2>& right) const
    {
        return !(*this == right);
    }

    template <typename T>
    int64_t CaretPointer<T>::getReferenceCount() const
    {
        CaretMutexLocker locked(&m_mutex);//lock so that m_share can't be deleted in the middle
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretPointer<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//lock because m_share and m_pointer need to stay coherent
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
    CaretArray<T>::CaretArray(const CaretArray<T>& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
                m_pointer = NULL;
                m_size = 0;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
                m_pointer = right.m_pointer;
                m_size = right.m_size;
            }
        }
    }

    template <typename T> template <typename T2>
    CaretArray<T>::CaretArray(const CaretArray<T2>& right)
    {//don't need to lock self during constructor
        {
            CaretMutexLocker locked(&(right.m_mutex));//don't let right modify its share until our reference is counted
            if (right.m_share == NULL)//guarantees it won't be deleted, because right has a counted reference
            {
                m_share = NULL;
                m_pointer = NULL;
                m_size = 0;
            } else {
                CaretMutexLocker locked2(&(right.m_share->m_mutex));
                ++(right.m_share->m_refCount);
                m_share = right.m_share;//now our reference is counted and we have the share, we can unlock everything
                m_pointer = right.m_pointer;
                m_size = right.m_size;
            }
        }
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size)
    {
        if (size > 0)
        {
            m_share = new CaretPointerShare();
            m_pointer = new T[size];
            m_size = size;
        } else {
            m_share = NULL;
            m_pointer = NULL;
            m_size = 0;
        }
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size, const T& initializer)
    {
        if (size > 0)
        {
            m_share = new CaretPointerShare();
            m_pointer = new T[size];
            m_size = size;
            T* end = m_pointer + size, *iter = m_pointer;
            do
            {
                *iter = initializer;//somewhat optimized, since this code will probably get used many places
                ++iter;
            } while (iter != end);
        } else {
            m_share = NULL;
            m_pointer = NULL;
            m_size = 0;
        }
    }

    template <typename T>
    T* CaretArray<T>::getArray()
    {
        return m_pointer;
    }

    template <typename T>
    const T* CaretArray<T>::getArray() const
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

    template <typename T>
    CaretArray<T>::operator const T*() const
    {
        return m_pointer;
    }

    template <typename T> template <typename I>
    T& CaretArray<T>::operator[](const I& index)
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && (int64_t)index < m_size);
        return m_pointer[index];
    }

    template <typename T> template <typename I>
    const T& CaretArray<T>::operator[](const I& index) const
    {
        CaretAssert(m_pointer != NULL);
        CaretAssert(index >= 0 && (int64_t)index < m_size);
        return m_pointer[index];
    }

    template <typename T>
    CaretArray<T>& CaretArray<T>::operator=(const CaretArray<T>& right)
    {
        CaretArray<T> temp(right);//copy construct from it, takes care of locking
        CaretPointerShare* tempShare = temp.m_share;//prepare to swap the shares and fill members
        T* tempPointer = temp.m_pointer;
        int64_t tempSize = temp.m_size;
        CaretMutexLocker locked(&m_mutex);//lock myself before using internal state
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        temp.m_size = m_size;
        m_share = tempShare;
        m_pointer = tempPointer;
        m_size = tempSize;
        return *this;//destructor of temp cleans up
    }

    template <typename T> template <typename T2>
    CaretArray<T>& CaretArray<T>::operator=(const CaretArray<T2>& right)
    {
        CaretArray<T> temp(right);//copy construct from it, takes care of locking
        CaretPointerShare* tempShare = temp.m_share;//prepare to swap the shares and fill members
        T* tempPointer = temp.m_pointer;
        int64_t tempSize = temp.m_size;
        CaretMutexLocker locked(&m_mutex);//lock myself before using internal state
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        temp.m_size = m_size;
        m_share = tempShare;
        m_pointer = tempPointer;
        m_size = tempSize;
        return *this;//destructor of temp cleans up
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
                if (!m_share->m_doNotDelete) delete[] m_pointer;
            }
        }//left refcount unlock before deleting the object that contains it
        if (deleteShare)
        {
            delete m_share;
        }
    }

    template <typename T> template <typename T2>
    bool CaretArray<T>::operator==(const T2* right) const
    {
        return (m_pointer == right);
    }

    template <typename T> template <typename T2>
    bool CaretArray<T>::operator!=(const T2* right) const
    {
        return !(*this == right);
    }

    template <typename T> template <typename T2>
    bool CaretArray<T>::operator==(const CaretArray<T2>& right) const
    {
        return (m_pointer == right.m_pointer);
    }

    template <typename T> template <typename T2>
    bool CaretArray<T>::operator!=(const CaretArray<T2>& right) const
    {
        return !(*this == right);
    }

    template <typename T>
    int64_t CaretArray<T>::getReferenceCount() const
    {
        CaretMutexLocker locked(&m_mutex);//lock to keep m_share from being deleted
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T* CaretArray<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//lock because m_pointer and m_share need to remain coherent
        if (m_share == NULL) return NULL;
        m_share->m_doNotDelete = true;
        return m_pointer;
    }

}

#endif //__CARET_POINTER_H__
