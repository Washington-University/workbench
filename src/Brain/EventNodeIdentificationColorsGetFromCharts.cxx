
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

#define __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_DECLARE__
#include "EventNodeIdentificationColorsGetFromCharts.h"
#undef __EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventNodeIdentificationColorsGetFromCharts 
 * \brief Get colors for node identification systems displayed in charts
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param structure
 *    Structure for matching to nodes in charts.
 * @param tabIndex
 *    Index of tab
 * @param nodeIndices
 *    Indices of nodes for which chart colors are requested.
 */
EventNodeIdentificationColorsGetFromCharts::EventNodeIdentificationColorsGetFromCharts(const StructureEnum::Enum structure,
                                                                                       const int32_t tabIndex,
                                                                                       const std::vector<int32_t>& nodeIndices)
: Event(EventTypeEnum::EVENT_NODE_IDENTIFICATION_COLORS_GET_FROM_CHARTS)
{
    m_structureName = StructureEnum::toName(structure);
    m_tabIndex      = tabIndex;
    m_nodeIndices   = nodeIndices;
}

/**
 * Destructor.
 */
EventNodeIdentificationColorsGetFromCharts::~EventNodeIdentificationColorsGetFromCharts()
{
}

/**
 * Add a node and its color (this method is called by charts).
 *
 * @param nodeIndex
 *     Index of the node.
 * @param rgb
 *     RGB coloring for the node.
 */
void
EventNodeIdentificationColorsGetFromCharts::addNode(const int32_t nodeIndex,
                                                    const float rgb[3])
{
    if (m_nodeRgbColor.find(nodeIndex) != m_nodeRgbColor.end()) {
        return;
    }
    
    RgbColor rgbColor;
    rgbColor.rgb[0] = rgb[0] * 255.0;
    rgbColor.rgb[1] = rgb[1] * 255.0;
    rgbColor.rgb[2] = rgb[2] * 255.0;
    m_nodeRgbColor.insert(std::make_pair<int32_t, RgbColor>(nodeIndex,
                                                            rgbColor));
}

/**
 * Apply chart color to node.  If there is chart coloring for the node
 * with the given index, it is applied.  Otherwise, no action is taken.
 * This method is called by drawing code.
 *
 * @param nodeIndex
 *     Index of the node.
 * @param rgb
 *     Receives coloring for the node from the chart coloring (if available).
 */
void
EventNodeIdentificationColorsGetFromCharts::applyChartColorToNode(const int32_t nodeIndex,
                                                                  uint8_t rgb[3])
{
    std::map<int32_t, RgbColor>::const_iterator iter = m_nodeRgbColor.find(nodeIndex);
    if (iter != m_nodeRgbColor.end()) {
        RgbColor rgbColor = iter->second;
        rgb[0] = rgbColor.rgb[0];
        rgb[1] = rgbColor.rgb[1];
        rgb[2] = rgbColor.rgb[2];
    }
}

/**
 * @return Name of the surface node's structure.  This method is called by charts.
 */
AString
EventNodeIdentificationColorsGetFromCharts::getStructureName() const
{
    return m_structureName;
}

/**
 * @return Index of tab where notification symbols are displayed.
 */
int32_t
EventNodeIdentificationColorsGetFromCharts::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * @return Indices of nodes for which chart colors are requested.
 */
std::vector<int32_t>
EventNodeIdentificationColorsGetFromCharts::getNodeIndices() const
{
    return m_nodeIndices;
}



