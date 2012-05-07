
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
        this->displayStatus[i] = false;
        this->contralateralDisplayStatus[i] = false;
        this->displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
    }
    
    m_lineWidth  = 1.0;
    m_pointSize = 2.0;
    m_drawingType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
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
DisplayPropertiesBorders::isDisplayed(const int32_t browserTabIndex) const
{
    return this->displayStatus[browserTabIndex];
}

/**
 * Set the display status for borders.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setDisplayed(const int32_t browserTabIndex,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    this->displayStatus[browserTabIndex] = displayStatus;
}

/**
 * @return  Contralateral display status of borders.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesBorders::isContralateralDisplayed(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return this->contralateralDisplayStatus[browserTabIndex];
}

/**
 * Set the contralateral display status for borders.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setContralateralDisplayed(const int32_t browserTabIndex,
                                                    const bool contralateralDisplayStatus)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    this->contralateralDisplayStatus[browserTabIndex] = contralateralDisplayStatus;
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesBorders::getDisplayGroup(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return this->displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void 
DisplayPropertiesBorders::setDisplayGroup(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    this->displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The point size.
 */
float 
DisplayPropertiesBorders::getPointSize() const
{
    return m_pointSize;
}

/**
 * Set the point size to the given value.
 * @param pointSize
 *     New value for point size.
 */
void 
DisplayPropertiesBorders::setPointSize(const float pointSize)
{
    m_pointSize = pointSize;
}

/**
 * @return The line width.
 */
float 
DisplayPropertiesBorders::getLineWidth() const
{
    return m_lineWidth;
}

/**
 * Set the line width to the given value.
 * @param lineWidth
 *     New value for line width.
 */
void 
DisplayPropertiesBorders::setLineWidth(const float lineWidth)
{
    m_lineWidth = lineWidth;
}

/**
 * @return The drawing type.
 */
BorderDrawingTypeEnum::Enum 
DisplayPropertiesBorders::getDrawingType() const
{
    return m_drawingType;
}

/**
 * Set the drawing type to the given value.
 * @param drawingType
 *     New value for drawing type.
 */
void 
DisplayPropertiesBorders::setDrawingType(const BorderDrawingTypeEnum::Enum drawingType)
{
    m_drawingType = drawingType;
}


