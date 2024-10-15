#ifndef __CARET_POINTER_H__
#define __CARET_POINTER_H__

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

#include "CaretMutex.h"
#include "CaretAssert.h"

//NOTE: AFAIK, shared_ptr and raw pointers don't get along (can't pass to an old ownership-taking object without changing it to use shared_ptr)
//      so, these smart pointers have .releasePointer() which stops any smart pointer from deleting it (via an extra variable alongside the refcount)

namespace caret
{
    //implement copy semantics with a forward-declared pointer type, as long as copy, destructor, =, etc are implemented in the cxx
    //use -> and * for access to members, but =, destruct, copy construct behaves like it was a non-pointer member
    template<typename T>
    class CaretForwardHelper
    {
        mutable T* m_pointer;
    public:
        //can be done with only forward declaration
        CaretForwardHelper() { m_pointer = NULL; }
        
        //below need to have the real header for the type
        CaretForwardHelper(const CaretForwardHelper& rhs) { m_pointer = new T(*rhs); } //rhs template can't replace default copy
        //copy from another forward declare of arbitrary type, as long as it is compatible
        template<typename R>
        CaretForwardHelper(const CaretForwardHelper<R>& rhs) { m_pointer = new T(*rhs); }
        //copy from some arbitrary other type
        template<typename R>
        CaretForwardHelper(const R& rhs) { m_pointer = new T(rhs); }
        //implement dereference as the way to instantiate m_pointer, useful for implementing the rest of the logic
        const T& operator*() const
        {
            if (m_pointer == NULL)
            {
                m_pointer = new T();
            }
            return *m_pointer;
        }
        T& operator*()
        {
            if (m_pointer == NULL)
            {
                m_pointer = new T();
            }
            return *m_pointer;
        }
        //default assignment implementation isn't what we want, fix it
        T& operator=(const CaretForwardHelper& rhs) { return **this = *rhs; } //similar to copy, rhs template can't replace default
        template<typename R>
        T& operator=(const CaretForwardHelper<R>& rhs) { return **this = *rhs; } //could check for rhs.m_pointer == NULL, but this is cleaner
        template<typename R>
        T& operator=(const R& rhs) { return **this = rhs; }
        //arrow, conversion
        const T *const& operator->() const { return &**this; } //deref this to get template, deref again to call custom operator, then address-of to get real pointer
        T * operator->() { return &**this; } //we do want const on the template to return a pointer to const
        operator const T& () const { return **this; }
        operator T& () { return **this; }
        ~CaretForwardHelper() { if (m_pointer != NULL) delete m_pointer; }
    };
    

    namespace _caret_pointer_impl
    {//namespace to hide things that shouldn't be used outside the header

        struct CaretPointerShare
        {//can't be member type because member types of templates on different types are incompatible
            int64_t m_refCount;
            bool m_doNotDelete;
            CaretPointerShare()
            {
                m_refCount = 1;//NOTE: don't initialize to 0, this way we don't have to change it every time we make one
                m_doNotDelete = false;
            }
        };

        struct CaretPointerSyncShare
        {//same, but with mutex
            int64_t m_refCount;
            CaretMutex m_mutex;//protects m_refCount, m_doNotDelete
            bool m_doNotDelete;
            CaretPointerSyncShare()
            {
                m_refCount = 1;
                m_doNotDelete = false;
            }
        };

        template <typename T>
        class CaretPointerCommon
        {//provides only identical functionality between the four types - having a pointer member, and having ==, !=, a getPointer() method, and decay to pointer
        protected:
            T* m_pointer;
            CaretPointerCommon() { }//prevent standalone use, initialize with the share in derived classes
        public:
            template <typename T2>
            bool operator==(const T2* right) const { return m_pointer == right; }
            template <typename T2>
            bool operator!=(const T2* right) const { return !(*this == right); }
            template <typename T2>
            bool operator==(const CaretPointerCommon<T2>& right) const { return m_pointer == right.m_pointer; }
            template <typename T2>
            bool operator!=(const CaretPointerCommon<T2>& right) const { return !(*this == right); }
            operator T *const&() const { return m_pointer; }//never allow modifying the pointer, and also work when object is const
            template <typename T2> friend class CaretPointerCommon;//because for const compatibility, we need to access a different template's members
        };

        template <typename T>
        class CaretPointerBase : public CaretPointerCommon<T>
        {//provides common functionality between just the 2 pointer types
        protected:
            using CaretPointerCommon<T>::m_pointer;
            CaretPointerBase() { }//prevent standalone use
        public:
            T*& getPointer() { return m_pointer; }
            T *const& getPointer() const { return m_pointer; }
            T& operator*() const
            {
                CaretAssert(m_pointer != NULL);
                return *(m_pointer);
            }
            T *const& operator->() const
            {
                CaretAssert(m_pointer != NULL);
                return m_pointer;
            }
        };

        template <typename T>
        class CaretArrayBase : public CaretPointerCommon<T>
        {//provides common functionality between just the 2 array types
        protected:
            using CaretPointerCommon<T>::m_pointer;
            int64_t m_size;
            CaretArrayBase() { }//prevent standalone use, initialize size with share in derived classes
        public:
            T *const& getArray() const { return m_pointer; }
            template <typename I>
            T& operator[](const I& index)
            {
                CaretAssert(m_pointer != NULL);
                CaretAssert(index >= 0 && (int64_t)index < m_size);
                return m_pointer[index];
            }
            template <typename I>
            const T& operator[](const I& index) const
            {
                CaretAssert(m_pointer != NULL);
                CaretAssert(index >= 0 && (int64_t)index < m_size);
                return m_pointer[index];
            }
            const int64_t& size() const { return m_size; }
        };

    }

    template <typename T>
    class CaretPointerNonsync : public _caret_pointer_impl::CaretPointerBase<T>
    {
        using _caret_pointer_impl::CaretPointerCommon<T>::m_pointer;
        _caret_pointer_impl::CaretPointerShare* m_share;
    public:
        CaretPointerNonsync();
        ~CaretPointerNonsync();
        CaretPointerNonsync(const CaretPointerNonsync& right);//because a templated function apparently can't override default copy
        template <typename T2>
        CaretPointerNonsync(const CaretPointerNonsync<T2>& right);
        explicit CaretPointerNonsync(T* right);
        CaretPointerNonsync& operator=(const CaretPointerNonsync& right);//or default =
        template <typename T2>
        CaretPointerNonsync& operator=(const CaretPointerNonsync<T2>& right);
        void grabNew(T* right);//substitute for operator= to bare pointer
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances that are identical to this one will delete it (setting is per-pointer-capture-event, not per-object)
        T*const& releasePointer();
        template <typename T2> friend class CaretPointerNonsync;//because for const compatibility, we need to access a different template's members
    };

    template <typename T>
    class CaretPointer : public _caret_pointer_impl::CaretPointerBase<T>
    {
        using _caret_pointer_impl::CaretPointerCommon<T>::m_pointer;
        _caret_pointer_impl::CaretPointerSyncShare* m_share;
        mutable CaretMutex m_mutex;//protects members from modification while reading, or from reading while modifying
    public:
        CaretPointer();
        ~CaretPointer();
        CaretPointer(const CaretPointer& right);
        template <typename T2>
        CaretPointer(const CaretPointer<T2>& right);
        explicit CaretPointer(T* right);
        CaretPointer& operator=(const CaretPointer& right);
        template <typename T2>
        CaretPointer& operator=(const CaretPointer<T2>& right);
        void grabNew(T* right);
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances that are identical to this one will delete it (setting is per-pointer-capture-event, not per-object)
        T*const& releasePointer();
        template <typename T2> friend class CaretPointer;
    };

    //separate array because delete and delete[] are different, and use indexing on one, and dereference/arrow on the other
    template <typename T>
    class CaretArrayNonsync : public _caret_pointer_impl::CaretArrayBase<T>
    {
        using _caret_pointer_impl::CaretPointerCommon<T>::m_pointer;
        using _caret_pointer_impl::CaretArrayBase<T>::m_size;
        _caret_pointer_impl::CaretPointerShare* m_share;//same share because it doesn't contain any specific information about what it is counting
    public:
        CaretArrayNonsync();
        ~CaretArrayNonsync();
        CaretArrayNonsync(const CaretArrayNonsync& right);
        template <typename T2>
        CaretArrayNonsync(const CaretArrayNonsync<T2>& right);
        CaretArrayNonsync(int64_t size);//for simpler construction
        CaretArrayNonsync(int64_t size, const T& initializer);//plus initialization
        CaretArrayNonsync& operator=(const CaretArrayNonsync& right);
        template <typename T2>
        CaretArrayNonsync& operator=(const CaretArrayNonsync<T2>& right);
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T*const& releasePointer();
        template <typename T2> friend class CaretArrayNonsync;
    };

    template <typename T>
    class CaretArray : public _caret_pointer_impl::CaretArrayBase<T>
    {
        using _caret_pointer_impl::CaretPointerCommon<T>::m_pointer;
        using _caret_pointer_impl::CaretArrayBase<T>::m_size;
        _caret_pointer_impl::CaretPointerSyncShare* m_share;//same share because it doesn't contain any specific information about what it is counting
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
        int64_t getReferenceCount() const;
        ///breaks the hold on the pointer that is currently held by this, NO instances will delete it (setting is per-pointer, not per-instance)
        T*const& releasePointer();
        template <typename T2> friend class CaretArray;
    };

    //NOTE:begin pointer functions
    template <typename T>
    CaretPointerNonsync<T>::CaretPointerNonsync()
    {
        m_share = NULL;
        m_pointer = NULL;
    }

    template <typename T>
    CaretPointerNonsync<T>::CaretPointerNonsync(const CaretPointerNonsync<T>& right) : _caret_pointer_impl::CaretPointerBase<T>()
    {
        m_share = right.m_share;
        m_pointer = right.m_pointer;
        if (m_share != NULL) ++(m_share->m_refCount);
    }

    template <typename T> template <typename T2>
    CaretPointerNonsync<T>::CaretPointerNonsync(const CaretPointerNonsync<T2>& right) : _caret_pointer_impl::CaretPointerBase<T>()
    {
        m_share = right.m_share;
        m_pointer = right.m_pointer;
        if (m_share != NULL) ++(m_share->m_refCount);
    }

    template <typename T>
    CaretPointerNonsync<T>::CaretPointerNonsync(T* right)
    {
        if (right == NULL)
        {
            m_share = NULL;
            m_pointer = NULL;
        } else {
            try
            {
                m_share = new _caret_pointer_impl::CaretPointerShare();//starts refcount at 1
            } catch (...) {//don't leak the passed memory when exceptions happen
                delete right;
                throw;
            }
            m_pointer = right;
        }
    }

    template <typename T>
    CaretPointerNonsync<T>& CaretPointerNonsync<T>::operator=(const CaretPointerNonsync<T>& right)
    {
        if (this == &right) return *this;//short circuit self assignment
        CaretPointerNonsync<T> temp(right);//copy construct from it, takes care type checking
        _caret_pointer_impl::CaretPointerShare* tempShare = temp.m_share;//swap the members
        T* tempPointer = temp.m_pointer;
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;
        return *this;//temp destructor takes care of the rest
    }

    template <typename T> template <typename T2>
    CaretPointerNonsync<T>& CaretPointerNonsync<T>::operator=(const CaretPointerNonsync<T2>& right)
    {//self asignment won't hit this operator=
        CaretPointerNonsync<T> temp(right);//copy construct from it, takes care of type checking
        _caret_pointer_impl::CaretPointerShare* tempShare = temp.m_share;//swap the members
        T* tempPointer = temp.m_pointer;
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;
        return *this;//temp destructor takes care of the rest
    }

    template <typename T>
    void CaretPointerNonsync<T>::grabNew(T* right)
    {
        if (right == NULL && m_pointer == NULL) return;//short circuit a case that doesn't need to do anything
        CaretPointerNonsync<T> temp(right);//construct from the pointer
        _caret_pointer_impl::CaretPointerShare* tempShare = temp.m_share;//swap the members
        T* tempPointer = temp.m_pointer;
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        m_share = tempShare;
        m_pointer = tempPointer;//destructor of temp takes care of the rest
    }
    
    template <typename T>
    CaretPointerNonsync<T>::~CaretPointerNonsync()
    {
        if (m_share == NULL) return;
        --(m_share->m_refCount);
        if (m_share->m_refCount == 0)
        {
            if (!m_share->m_doNotDelete) delete m_pointer;
            delete m_share;
        }
    }

    template <typename T>
    int64_t CaretPointerNonsync<T>::getReferenceCount() const
    {
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T*const& CaretPointerNonsync<T>::releasePointer()
    {
        if (m_share != NULL)
        {
            m_share->m_doNotDelete = true;
        }
        return m_pointer;
    }
    
    //NOTE:begin sync pointer functions
    template <typename T>
    CaretPointer<T>::CaretPointer()
    {
        m_share = NULL;
        m_pointer = NULL;
    }

    template <typename T>
    CaretPointer<T>::CaretPointer(const CaretPointer<T>& right) : _caret_pointer_impl::CaretPointerBase<T>()
    {//don't need to lock self during constructor
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

    template <typename T> template <typename T2>
    CaretPointer<T>::CaretPointer(const CaretPointer<T2>& right) : _caret_pointer_impl::CaretPointerBase<T>()
    {//don't need to lock self during constructor
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

    template <typename T>
    CaretPointer<T>::CaretPointer(T* right)
    {//don't need to lock self during constructor
        if (right == NULL)
        {
            m_share = NULL;
            m_pointer = NULL;
        } else {
            try
            {
                m_share = new _caret_pointer_impl::CaretPointerSyncShare();//starts refcount at 1
            } catch (...) {
                delete right;
                throw;
            }
            m_pointer = right;
        }
    }

    template <typename T>
    CaretPointer<T>& CaretPointer<T>::operator=(const CaretPointer<T>& right)
    {
        if (this == &right) return *this;//short circuit self assignment
        CaretPointer<T> temp(right);//copy construct from it, takes care of locking and type checking
        _caret_pointer_impl::CaretPointerSyncShare* tempShare = temp.m_share;//prepare to swap the members
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
    {//self asignment won't hit this operator=
        CaretPointer<T> temp(right);//copy construct from it, takes care of locking and type checking
        _caret_pointer_impl::CaretPointerSyncShare* tempShare = temp.m_share;//prepare to swap the members
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
        if (right == NULL && m_pointer == NULL) return;//short circuit a case that doesn't need any mutexes
        CaretPointer<T> temp(right);//construct from the pointer
        _caret_pointer_impl::CaretPointerSyncShare* tempShare = temp.m_share;//prepare to swap the members
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
            --(m_share->m_refCount);
            if (m_share->m_refCount == 0)
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
    T*const& CaretPointer<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//lock to keep m_share and m_pointer coherent until after return - must return the pointer that was released
        if (m_share != NULL)
        {
            m_share->m_doNotDelete = true;
        }
        return m_pointer;
    }

    //NOTE:begin array functions
    template <typename T>
    CaretArrayNonsync<T>::CaretArrayNonsync()
    {
        m_share = NULL;
        m_pointer = NULL;
        m_size = 0;
    }

    template <typename T>
    CaretArrayNonsync<T>::CaretArrayNonsync(const CaretArrayNonsync<T>& right) : _caret_pointer_impl::CaretArrayBase<T>()
    {
        m_share = right.m_share;
        m_pointer = right.m_pointer;
        m_size = right.m_size;
        if (m_share != NULL) ++(m_share->m_refCount);
    }

    template <typename T> template <typename T2>
    CaretArrayNonsync<T>::CaretArrayNonsync(const CaretArrayNonsync<T2>& right) : _caret_pointer_impl::CaretArrayBase<T>()
    {
        m_share = right.m_share;
        m_pointer = right.m_pointer;
        m_size = right.m_size;
        if (m_share != NULL) ++(m_share->m_refCount);
    }

    template <typename T>
    CaretArrayNonsync<T>::CaretArrayNonsync(int64_t size)
    {
        if (size > 0)
        {
            m_share = new _caret_pointer_impl::CaretPointerShare();
            try
            {
                m_pointer = new T[size];
            } catch (...) {//don't leak share objects if we can't allocate the memory
                delete m_share;
                m_share = NULL;//also keep state consistent
                throw;
            }
            m_size = size;
        } else {
            m_share = NULL;
            m_pointer = NULL;
            m_size = 0;
        }
    }

    template <typename T>
    CaretArrayNonsync<T>::CaretArrayNonsync(int64_t size, const T& initializer)
    {
        if (size > 0)
        {
            m_share = new _caret_pointer_impl::CaretPointerShare();
            try
            {
                m_pointer = new T[size];
            } catch (...) {
                delete m_share;
                m_share = NULL;
                throw;
            }
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
    CaretArrayNonsync<T>& CaretArrayNonsync<T>::operator=(const CaretArrayNonsync<T>& right)
    {
        CaretArrayNonsync<T> temp(right);
        _caret_pointer_impl::CaretPointerShare* tempShare = temp.m_share;//swap the shares and fill members
        T* tempPointer = temp.m_pointer;
        int64_t tempSize = temp.m_size;
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        temp.m_size = m_size;
        m_share = tempShare;
        m_pointer = tempPointer;
        m_size = tempSize;
        return *this;//destructor of temp cleans up
    }

    template <typename T> template <typename T2>
    CaretArrayNonsync<T>& CaretArrayNonsync<T>::operator=(const CaretArrayNonsync<T2>& right)
    {
        CaretArrayNonsync<T> temp(right);
        _caret_pointer_impl::CaretPointerShare* tempShare = temp.m_share;//swap the shares and fill members
        T* tempPointer = temp.m_pointer;
        int64_t tempSize = temp.m_size;
        temp.m_share = m_share;
        temp.m_pointer = m_pointer;
        temp.m_size = m_size;
        m_share = tempShare;
        m_pointer = tempPointer;
        m_size = tempSize;
        return *this;//destructor of temp cleans up
    }

    template <typename T>
    CaretArrayNonsync<T>::~CaretArrayNonsync()
    {
        if (m_share == NULL) return;
        --(m_share->m_refCount);
        if (m_share->m_refCount == 0)
        {
            if (!m_share->m_doNotDelete) delete[] m_pointer;
            delete m_share;
        }
    }

    template <typename T>
    int64_t CaretArrayNonsync<T>::getReferenceCount() const
    {
        if (m_share == NULL)
        {
            return 0;
        }
        return m_share->m_refCount;
    }

    template <typename T>
    T*const& CaretArrayNonsync<T>::releasePointer()
    {
        if (m_share != NULL)
        {
            m_share->m_doNotDelete = true;
        }
        return m_pointer;
    }

    //NOTE:begin sync array functions
    template <typename T>
    CaretArray<T>::CaretArray()
    {
        m_share = NULL;
        m_pointer = NULL;
        m_size = 0;
    }

    template <typename T>
    CaretArray<T>::CaretArray(const CaretArray<T>& right) : _caret_pointer_impl::CaretArrayBase<T>()
    {//don't need to lock self during constructor
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

    template <typename T> template <typename T2>
    CaretArray<T>::CaretArray(const CaretArray<T2>& right) : _caret_pointer_impl::CaretArrayBase<T>()
    {//don't need to lock self during constructor
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
            this->m_pointer = right.m_pointer;
            m_size = right.m_size;
        }
    }

    template <typename T>
    CaretArray<T>::CaretArray(int64_t size)
    {
        if (size > 0)
        {
            m_share = new _caret_pointer_impl::CaretPointerSyncShare();
            try
            {
                m_pointer = new T[size];
            } catch (...) {
                delete m_share;
                m_share = NULL;
                throw;
            }
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
            m_share = new _caret_pointer_impl::CaretPointerSyncShare();
            try
            {
                m_pointer = new T[size];
            } catch (...) {
                delete m_share;
                m_share = NULL;
                throw;
            }
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
    CaretArray<T>& CaretArray<T>::operator=(const CaretArray<T>& right)
    {
        CaretArray<T> temp(right);//copy construct from it, takes care of locking
        _caret_pointer_impl::CaretPointerSyncShare* tempShare = temp.m_share;//prepare to swap the shares and fill members
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
        _caret_pointer_impl::CaretPointerSyncShare* tempShare = temp.m_share;//prepare to swap the shares and fill members
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
            --(m_share->m_refCount);
            if (m_share->m_refCount == 0)
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
    T*const& CaretArray<T>::releasePointer()
    {
        CaretMutexLocker locked(&m_mutex);//lock because m_pointer and m_share need to remain coherent
        if (m_share != NULL)
        {
            m_share->m_doNotDelete = true;
        }
        return m_pointer;
    }

}

#endif //__CARET_POINTER_H__
