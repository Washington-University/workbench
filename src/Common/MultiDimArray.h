#ifndef __MULTI_DIM_ARRAY_H__
#define __MULTI_DIM_ARRAY_H__

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

#include "stdint.h"
#include <vector>

namespace caret
{
    
    template<typename T>
    class MultiDimArray
    {
        std::vector<int64_t> m_dims, m_skip;//always use int64_t for indexes internally
        std::vector<T> m_data;
        template<typename I>
        int64_t index(const int& fullDims, const std::vector<I>& indexSelect) const;//assume we never need over 2 billion dimensions
    public:
        const std::vector<int64_t>& getDimensions() const { return m_dims; }
        template<typename I>
        void resize(const std::vector<I>& dims);//destructive resize
        template<typename I>
        T& at(const std::vector<I>& pos);
        template<typename I>
        const T& at(const std::vector<I>& pos) const;
        template<typename I>
        T* get(const int& fullDims, const std::vector<I>& indexSelect);//subarray reference selection
        template<typename I>
        const T* get(const int& fullDims, const std::vector<I>& indexSelect) const;
    };
    
    template<typename T>
    template<typename I>
    void MultiDimArray<T>::resize(const std::vector<I>& dims)
    {
        m_dims = std::vector<int64_t>(dims.begin(), dims.end());
        m_skip.resize(m_dims.size());
        if (dims.size() == 0)
        {
            m_data.clear();
            return;
        }
        int64_t numElems = 1;
        for (int i = 0; i < (int)m_dims.size(); ++i)
        {
            CaretAssert(m_dims[i] > 0);
            m_skip[i] = numElems;
            numElems *= m_dims[i];
        }
        m_data.resize(numElems);
    }
    
    template<typename T>
    template<typename I>
    int64_t MultiDimArray<T>::index(const int& fullDims, const std::vector<I>& indexSelect) const
    {
        CaretAssert(fullDims + indexSelect.size() == m_dims.size());
        int64_t ret = 0;
        for (int i = fullDims; i < (int)m_dims.size(); ++i)
        {
            CaretAssert(indexSelect[i - fullDims] >= 0 && indexSelect[i - fullDims] < m_dims[i]);
            ret += m_skip[i] * indexSelect[i - fullDims];
        }
        return ret;
    }
    
    template<typename T>
    template<typename I>
    T& MultiDimArray<T>::at(const std::vector<I>& pos)
    {
        return m_data[index(0, pos)];
    }
    
    template<typename T>
    template<typename I>
    const T& MultiDimArray<T>::at(const std::vector<I>& pos) const
    {
        return m_data[index(0, pos)];
    }
    
    template<typename T>
    template<typename I>
    T* MultiDimArray<T>::get(const int& fullDims, const std::vector<I>& indexSelect)
    {
        return m_data.data() + index(fullDims, indexSelect);
    }
    
    template<typename T>
    template<typename I>
    const T* MultiDimArray<T>::get(const int& fullDims, const std::vector<I>& indexSelect) const
    {
        return m_data.data() + index(fullDims, indexSelect);
    }
}

#endif //__MULTI_DIM_ARRAY_H__
