#ifndef __CARET_HEAP__
#define __CARET_HEAP__
#include "CaretAssertion.h"

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

#include <vector>
#include "stdint.h"
#include "CaretAssert.h"

namespace caret
{
    
    template <typename T, typename K, typename C>
    class CaretHeapBase
    {
    public:
        struct DataStruct
        {
            K m_key;
            T m_data;
            int64_t m_index;
            DataStruct(const K& key, const T& data) : m_key(key), m_data(data) { }
        };
    protected:
        CaretHeapBase() { }//this is not a usable class by itself - use CaretMinHeap or CaretMaxHeap
        std::vector<DataStruct> m_datastore;
        std::vector<int64_t> m_heap;
        std::vector<int64_t> m_unusedStore;
        ///primitive used to make the code a bit nicer, modifies the heap and the field in the data to point to it
        ///DOES NOT GUARANTEE CONSISTENT HEAP STATE
        void put(const int64_t& dataIndex, const int64_t& heapLoc);
        //uses curiously recurring template pattern to use child class's comparison without virtual - can't pass parameters to a typedef, so this is the most convenient way for usage
        inline bool compare(const K& left, const K& right) { return C::mycompare(left, right); }
        void heapify_up(const int64_t& start);
        void heapify_down(const int64_t& start);
    public:
        ///this heap is special, save the return value of push() and you can modify the key/data later (constant time for data, log(heapsize) time to change key)
        int64_t push(const K& key, const T& data);
        
        ///look at the data of the top element
        T& top(K* key = NULL);
        
        ///remove and return the top element
        T pop(K* key = NULL);
        
        ///modify the key, and reheapify
        void changekey(const int64_t& dataIndex, K key);
        
        ///modify or just access the data - could be an operator[] but that would be kind of confusing
        T& data(const int64_t& dataIndex);
        
        ///preallocate for efficiency, if you know about how big it will be
        void reserve(int64_t expectedSize);
        
        ///retrieve the key value
        const K& getKey(const int64_t& dataIndex) const;
    };
    
    template <typename T, typename K>
    class CaretMinHeap : public CaretHeapBase<T, K, CaretMinHeap<T, K> >
    {
    public:
        static inline bool mycompare(const K& left, const K& right)
        {
            return left < right;//as long as vector op[] optimizes out in release, this should be only two lookups
        }
    };
    
    template <typename T, typename K>
    class CaretMaxHeap : public CaretHeapBase<T, K, CaretMinHeap<T, K> >
    {
    public:
        static inline bool mycompare(const K& left, const K& right)
        {
            return right < left;
        }
    };
    
    template <typename T, typename K, typename C>
    void CaretHeapBase<T, K, C>::changekey(const int64_t& dataIndex, K key)
    {
        CaretAssertVectorIndex(m_datastore, dataIndex);
        K oldkey = m_datastore[dataIndex].m_key;
        m_datastore[dataIndex].m_key = key;
        if (compare(oldkey, key))
        {
            heapify_down(m_datastore[dataIndex].m_index);
        } else {
            heapify_up(m_datastore[dataIndex].m_index);
        }
    }
    
    template <typename T, typename K, typename C>
    T& CaretHeapBase<T, K, C>::data(const int64_t& dataIndex)
    {
        CaretAssertVectorIndex(m_datastore, dataIndex);
        return m_datastore[dataIndex].m_data;
    }
    
    template <typename T, typename K, typename C>
    void CaretHeapBase<T, K, C>::heapify_down(const int64_t& start)
    {
        if (m_heap.size() == 0) return;
        CaretAssertVectorIndex(m_heap, start);
        int64_t cur = start, nextInd = (start << 1) + 1, mySize = (int64_t)m_heap.size();
        int64_t temp = m_heap[start];//save current data index, don't swap it around until we stop
        while (nextInd < mySize)
        {
            if (nextInd + 1 < mySize && compare(m_datastore[m_heap[nextInd + 1]].m_key, m_datastore[m_heap[nextInd]].m_key))
            {
                ++nextInd;
            }
            if (compare(m_datastore[m_heap[nextInd]].m_key, m_datastore[temp].m_key))
            {
                put(m_heap[nextInd], cur);//move the best child up
                cur = nextInd;//advance current
                nextInd = (cur << 1) + 1;
            } else {
                break;
            }
        }
        if (cur != start) put(temp, cur);//stopped, now we put it and finish, but only if we moved something
    }
    
    template <typename T, typename K, typename C>
    void CaretHeapBase<T, K, C>::heapify_up(const int64_t& start)
    {
        if (m_heap.size() == 0) return;
        CaretAssertVectorIndex(m_heap, start);
        int64_t cur = start, nextInd = (start - 1) >> 1;
        int64_t temp = m_heap[start];
        while (cur > 0)
        {
            if (compare(m_datastore[temp].m_key, m_datastore[m_heap[nextInd]].m_key))
            {
                put(m_heap[nextInd], cur);
                cur = nextInd;
                nextInd = (cur - 1) >> 1;
            } else {
                break;
            }
        }
        if (cur != start) put(temp, cur);
    }
    
    template <typename T, typename K, typename C>
    T CaretHeapBase<T, K, C>::pop(K* key)
    {
        CaretAssert(m_heap.size() > 0);
        T ret = m_datastore[m_heap[0]].m_data;
        if (key != NULL) *key = m_datastore[m_heap[0]].m_key;
        m_unusedStore.push_back(m_heap[0]);//should this try garbage collection?  currently, the T data will remain until overwritten...would require another level of indirection to fix
        put(m_heap[m_heap.size() - 1], 0);
        m_heap.pop_back();
        heapify_down(0);
        return ret;
    }

    template <typename T, typename K, typename C>
    int64_t CaretHeapBase<T, K, C>::push(const K& key, const T& data)
    {
        //std::cout << "pushing " << key << std::endl;
        int64_t dataLoc;
        if (m_unusedStore.size() > 0)
        {
            dataLoc = m_unusedStore[m_unusedStore.size() - 1];
            m_datastore[dataLoc].m_key = key;
            m_datastore[dataLoc].m_data = data;
            m_unusedStore.pop_back();
        } else {
            dataLoc = m_datastore.size();
            m_datastore.push_back(DataStruct(key, data));
        }
        m_heap.push_back(dataLoc);
        put(dataLoc, m_heap.size() - 1);
        heapify_up(m_heap.size() - 1);
        return dataLoc;
    }

    template <typename T, typename K, typename C>
    void CaretHeapBase<T, K, C>::put(const int64_t& dataIndex, const int64_t& heapLoc)
    {
        CaretAssertVectorIndex(m_datastore, dataIndex);
        CaretAssertVectorIndex(m_heap, heapLoc);
        m_datastore[dataIndex].m_index = heapLoc;
        m_heap[heapLoc] = dataIndex;
    }

    template <typename T, typename K, typename C>
    void CaretHeapBase<T, K, C>::reserve(int64_t expectedSize)
    {
        CaretAssert(expectedSize > 0);
        m_heap.reserve(expectedSize);
        m_datastore.reserve(expectedSize);
        m_unusedStore.reserve(expectedSize);//expect them to eventually pop() everything
    }
    
    template <typename T, typename K, typename C>
    T& CaretHeapBase<T, K, C>::top(K* key)
    {
        CaretAssert(m_heap.size() > 0);
        if (key != NULL) *key = m_datastore[m_heap[0]].m_key;
        return m_datastore[m_heap[0]].m_data;
    }

}

#endif //__CARET_HEAP__
