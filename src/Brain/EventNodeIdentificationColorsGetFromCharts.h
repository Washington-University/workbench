#ifndef __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_H__
#define __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_H__

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

#include <map>

#include "Event.h"
#include "StructureEnum.h"

namespace caret {

    class EventNodeIdentificationColorsGetFromCharts : public Event {
        
    public:
        EventNodeIdentificationColorsGetFromCharts(const StructureEnum::Enum structure,
                                                   const int32_t tabIndex,
                                                   const std::vector<int32_t>& nodeIndices);
        
        virtual ~EventNodeIdentificationColorsGetFromCharts();
        
        void addNode(const int32_t nodeIndex,
                     const float rgba[4]);


        void applyChartColorToNode(const int32_t nodeIndex,
                                   uint8_t rgba[4]);
        
        AString getStructureName() const;
        
        int32_t getTabIndex() const;
        
        std::vector<int32_t> getNodeIndices() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        EventNodeIdentificationColorsGetFromCharts(const EventNodeIdentificationColorsGetFromCharts&);

        EventNodeIdentificationColorsGetFromCharts& operator=(const EventNodeIdentificationColorsGetFromCharts&);
        
        // ADD_NEW_MEMBERS_HERE
        
        AString m_structureName;

        int32_t m_tabIndex;
        
        std::vector<int32_t> m_nodeIndices;
        
        struct RgbColor {
            uint8_t rgb[3];
        };
        
        std::map<int32_t, RgbColor> m_nodeRgbColor;
    };
    
#ifdef __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_DECLARE__

} // namespace
#endif  //__EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_H__
