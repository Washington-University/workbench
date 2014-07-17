#ifndef __BORDER_TRACING_HELPER_H__
#define __BORDER_TRACING_HELPER_H__

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
#include "CaretPointer.h"
#include "StructureEnum.h"
#include "TopologyHelper.h"

#include <vector>

namespace caret {
    
    class Border;
    class SurfaceFile;
    
    class BorderTracingHelper
    {
        int m_numNodes;
        StructureEnum::Enum m_structure;
        CaretPointer<TopologyHelper> m_topoHelp;
        BorderTracingHelper();//no default
        BorderTracingHelper(const BorderTracingHelper&);//no copy
        BorderTracingHelper& operator=(const BorderTracingHelper&);//no assign
        std::vector<CaretPointer<Border> > tracePrivate(std::vector<int>& marked, const float& placement);
    public:
        BorderTracingHelper(const SurfaceFile* surfIn);
        template<typename T, typename Test>
        std::vector<CaretPointer<Border> > traceData(T* data, const Test& myTester, const float& placement = 0.33f);
        
        //some useful selection objects
        class LabelSelect
        {
            int32_t m_select;
            LabelSelect();
        public:
            LabelSelect(const int32_t& value) : m_select(value) { }
            bool operator() (const int32_t& toTest) const { return toTest == m_select; }
        };
        
        template<typename T>
        class LessThan
        {
            T m_threshold;
            bool m_inclusive;
            LessThan();
        public:
            LessThan(const T& value, const bool& inclusive) : m_threshold(value), m_inclusive(inclusive) { }
            bool operator() (const T& toTest) const
            {
                if (m_inclusive)
                {
                    return toTest <= m_threshold;
                } else {
                    return toTest < m_threshold;
                }
            }
        };
        
        template<typename T>
        class GreaterThan
        {
            T m_threshold;
            bool m_inclusive;
            GreaterThan();
        public:
            GreaterThan(const T& value, const bool& inclusive) : m_threshold(value), m_inclusive(inclusive) { }
            bool operator() (const T& toTest) const
            {
                if (m_inclusive)
                {
                    return toTest >= m_threshold;
                } else {
                    return toTest > m_threshold;
                }
            }
        };
    };
    
    template<typename T, typename Test>
    std::vector<CaretPointer<Border> > BorderTracingHelper::traceData(T* data, const Test& myTester, const float& placement)
    {
        CaretAssert(placement >= 0.0f && placement <= 1.0f);
        std::vector<int> marked(m_numNodes);
        for (int i = 0; i < m_numNodes; ++i)
        {
            marked[i] = (myTester(data[i]) ? 1 : 0);
        }
        return tracePrivate(marked, placement);
    }
}//namespace

#endif  //__BORDER_TRACING_HELPER_H__
