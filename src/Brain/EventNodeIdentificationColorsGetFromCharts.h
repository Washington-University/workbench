#ifndef __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_H__
#define __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
