#ifndef __CARET_COMPACT_3D_LOOKUP_H__
#define __CARET_COMPACT_3D_LOOKUP_H__

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

#include "CaretCompactLookup.h"

namespace caret
{
    
    template <typename T>
    class CaretCompact3DLookup
    {
        CaretCompactLookup<CaretCompactLookup<CaretCompactLookup<T> > > m_lookup;//the whole point of this class is to deal with this ugliness
    public:
        ///creates the element if it didn't exist, and returns a reference to it
        T& at(const int64_t& index1, const int64_t& index2, const int64_t& index3);
        ///creates the element if it didn't exist, and returns a reference to it
        T& at(const int64_t index[3]) { return at(index[0], index[1], index[2]); }
        ///add or overwrite an element in the lookup
        void insert(const int64_t& index1, const int64_t& index2, const int64_t& index3, const T& value)
        { at(index1, index2, index3) = value; }
        ///add or overwrite an element in the lookup
        void insert(const int64_t index[3], const T& value)
        { at(index) = value; }
        ///returns a pointer to the desired element, or NULL if no such element is found
        T* find(const int64_t& index1, const int64_t& index2, const int64_t& index3);
        ///returns a pointer to the desired element, or NULL if no such element is found
        T* find(const int64_t index[3]) { return find(index[0], index[1], index[2]); }
        ///returns a pointer to the desired element, or NULL if no such element is found
        const T* find(const int64_t& index1, const int64_t& index2, const int64_t& index3) const;
        ///returns a pointer to the desired element, or NULL if no such element is found
        const T* find(const int64_t index[3]) const { return find(index[0], index[1], index[2]); }
    };
    
    template<typename T>
    T& CaretCompact3DLookup<T>::at(const int64_t& index1, const int64_t& index2, const int64_t& index3)
    {
        return m_lookup[index3][index2][index1];//a lot of complexity is hidden in those operator[]s
    }

    template<typename T>
    T* CaretCompact3DLookup<T>::find(const int64_t& index1, const int64_t& index2, const int64_t& index3)
    {
        typename CaretCompactLookup<CaretCompactLookup<CaretCompactLookup<T> > >::iterator iter1 = m_lookup.find(index3);//oh the humanity
        if (iter1 == m_lookup.end()) return NULL;
        typename CaretCompactLookup<CaretCompactLookup<T> >::iterator iter2 = iter1->find(index2);
        if (iter2 == iter1->end()) return NULL;
        typename CaretCompactLookup<T>::iterator iter3 = iter2->find(index1);
        if (iter3 == iter2->end()) return NULL;
        return &(*iter3);
    }

    template <typename T>
    const T* CaretCompact3DLookup<T>::find(const int64_t& index1, const int64_t& index2, const int64_t& index3) const
    {
        typename CaretCompactLookup<CaretCompactLookup<CaretCompactLookup<T> > >::const_iterator iter1 = m_lookup.find(index3);//oh the humanity
        if (iter1 == m_lookup.end()) return NULL;
        typename CaretCompactLookup<CaretCompactLookup<T> >::const_iterator iter2 = iter1->find(index2);
        if (iter2 == iter1->end()) return NULL;
        typename CaretCompactLookup<T>::const_iterator iter3 = iter2->find(index1);
        if (iter3 == iter2->end()) return NULL;
        return &(*iter3);
    }

}

#endif //__CARET_COMPACT_3D_LOOKUP_H__
