
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#define __DISPLAY_PROPERTIES_BORDERS_DECLARE__
#include "DisplayPropertiesBorders.h"
#undef __DISPLAY_PROPERTIES_BORDERS_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesBorders 
 * \brief Contains display properties for borders.
 */

/**
 * Constructor.
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesBorders::DisplayPropertiesBorders(Brain* brain)
: DisplayProperties(brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatus[i] = false;
        m_contralateralDisplayStatus[i] = false;
        m_lineWidth[i]  = 1.0;
        m_pointSize[i] = 2.0;
        m_drawingType[i] = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    }
}

/**
 * Destructor.
 */
DisplayPropertiesBorders::~DisplayPropertiesBorders()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesBorders::reset()
{
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        this->displayStatus[i] = false;
//        this->contralateralDisplayStatus[i] = false;
//        this->displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
//    }
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesBorders::update()
{
    
}

/**
 * @return  Display status of borders.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesBorders::isDisplayed(const DisplayGroupEnum::Enum  displayGroup) const
{
    CaretAssertArrayIndex(m_displayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_displayStatus[displayGroup];
}

/**
 * Set the display status for borders for the given display group.
 * @param displayGroup
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_displayStatus[displayGroup] = displayStatus;
}

/**
 * @return  Contralateral display status of borders.
 * @param displayGroup
 *     Display group.
 */
bool 
DisplayPropertiesBorders::isContralateralDisplayed(const DisplayGroupEnum::Enum  displayGroup) const
{
    CaretAssertArrayIndex(m_contralateralDisplayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_contralateralDisplayStatus[displayGroup];
}

/**
 * Set the contralateral display status for borders.
 * @param displayGroup
 *     Display group.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setContralateralDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                                    const bool contralateralDisplayStatus)
{
    CaretAssertArrayIndex(m_contralateralDisplayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_contralateralDisplayStatus[displayGroup] = contralateralDisplayStatus;
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesBorders::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void 
DisplayPropertiesBorders::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The point size.
 * @param displayGroup
 *     Display group.
 */
float 
DisplayPropertiesBorders::getPointSize(const DisplayGroupEnum::Enum  displayGroup) const
{
    CaretAssertArrayIndex(m_pointSize, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_pointSize[displayGroup];
}

/**
 * Set the point size to the given value.
 * @param displayGroup
 *     Display group.
 * @param pointSize
 *     New value for point size.
 */
void 
DisplayPropertiesBorders::setPointSize(const DisplayGroupEnum::Enum  displayGroup,
                                       const float pointSize)
{
    CaretAssertArrayIndex(m_pointSize, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_pointSize[displayGroup] = pointSize;
}

/**
 * @return The line width.
 * @param displayGroup
 *     Display group.
 */
float 
DisplayPropertiesBorders::getLineWidth(const DisplayGroupEnum::Enum  displayGroup) const
{
    CaretAssertArrayIndex(m_lineWidth, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_lineWidth[displayGroup];
}

/**
 * Set the line width to the given value.
 * @param displayGroup
 *     Display group.
 * @param lineWidth
 *     New value for line width.
 */
void 
DisplayPropertiesBorders::setLineWidth(const DisplayGroupEnum::Enum  displayGroup,
                                       const float lineWidth)
{
    CaretAssertArrayIndex(m_lineWidth, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_lineWidth[displayGroup] = lineWidth;
}

/**
 * @return The drawing type.
 * @param displayGroup
 *     Display group.
 */
BorderDrawingTypeEnum::Enum 
DisplayPropertiesBorders::getDrawingType(const DisplayGroupEnum::Enum  displayGroup) const
{
    CaretAssertArrayIndex(m_drawingType, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_drawingType[displayGroup];
}

/**
 * Set the drawing type to the given value.
 * @param displayGroup
 *     Display group.
 * @param drawingType
 *     New value for drawing type.
 */
void 
DisplayPropertiesBorders::setDrawingType(const DisplayGroupEnum::Enum  displayGroup,
                                         const BorderDrawingTypeEnum::Enum drawingType)
{
    CaretAssertArrayIndex(m_drawingType, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_drawingType[displayGroup] = drawingType;
}


