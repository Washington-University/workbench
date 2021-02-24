#ifndef __MULTI_DIM_ITERATOR_H__
#define __MULTI_DIM_ITERATOR_H__

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

#include "CaretAssert.h"
#include "CaretException.h"

#include "stdint.h"
#include <vector>

namespace caret
{
    
    template<typename T>
    class MultiDimIterator
    {
        std::vector<T> m_dims, m_pos;
        bool m_atEnd, m_beforeBegin;
        void gotoBegin();
        void gotoLast();
    public:
        explicit MultiDimIterator(const std::vector<T>& dimensions);
        void operator++();
        void operator++(int);
        void operator--();
        void operator--(int);
        const std::vector<T>& operator*() const { return m_pos; }
        bool atEnd() const { return m_atEnd; }
    };
    
    template<typename T>
    MultiDimIterator<T>::MultiDimIterator(const std::vector<T>& dimensions)
    {
        m_dims = dimensions;
        gotoBegin();
    }
    
    template<typename T>
    void MultiDimIterator<T>::gotoBegin()
    {
        m_pos = std::vector<T>(m_dims.size(), 0);
        m_atEnd = false;
        m_beforeBegin = false;
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            if (m_dims[i] < 1)
            {
                m_atEnd = true;
                break;
            }
        }
    }
    
    template<typename T>
    void MultiDimIterator<T>::gotoLast()
    {
        m_pos = std::vector<T>(m_dims.size());
        m_atEnd = false;
        m_beforeBegin = false;
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            m_pos[i] = m_dims[i] - 1;
            if (m_dims[i] < 1)
            {
                m_atEnd = true;
            }
        }
    }

    template<typename T>
    void MultiDimIterator<T>::operator++()
    {
        if (atEnd())//error
        {
            CaretAssert(false);
            throw CaretException("tried to increment MultiDimIterator that is out of range");
        }
        if (m_beforeBegin)
        {
            gotoBegin();
            return;
        }
        if (m_dims.size() == 0)
        {
            m_atEnd = true;//special case: no dimensions works the same as 1 dimension of length 1
            m_beforeBegin = false;
            return;
        }
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            ++m_pos[i];
            if (m_pos[i] < m_dims[i]) return;
            m_pos[i] = 0;
        }
        m_atEnd = true;//if we didn't return already, all of them wrapped, so we are at the end
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator++(int)
    {
        ++(*this);
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator--()
    {
        if (m_beforeBegin)//error
        {
            CaretAssert(false);
            throw CaretException("tried to decrement MultiDimIterator that is out of range");
        }
        if (atEnd())
        {
            gotoLast();
            return;
        }
        if (m_dims.size() == 0)
        {
            m_atEnd = false;//special case: no dimensions works the same as 1 dimension of length 1
            m_beforeBegin = true;
            return;
        }
        size_t numDims = m_dims.size();
        for (size_t i = 0; i < numDims; ++i)
        {
            if (m_pos[i] > 0)
            {
                --m_pos[i];
                return;
            } else {
                m_pos[i] = m_dims[i] - 1;
            }
        }
        m_beforeBegin = true;//if we didn't return already, all of them wrapped, so we are before the beginning
    }
    
    template<typename T>
    void MultiDimIterator<T>::operator--(int)
    {
        --(*this);
    }
    
}

#endif //__MULTI_DIM_ITERATOR_H__
