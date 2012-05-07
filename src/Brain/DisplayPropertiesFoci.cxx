
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

#define __DISPLAY_PROPERTIES_FOCI_DECLARE__
#include "DisplayPropertiesFoci.h"
#undef __DISPLAY_PROPERTIES_FOCI_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesFoci 
 * \brief Contains display properties for foci.
 */

/**
 * Constructor.
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesFoci::DisplayPropertiesFoci(Brain* brain)
: DisplayProperties(brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayStatus[i] = false;
        m_contralateralDisplayStatus[i] = false;
        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
        m_pasteOntoSurface[i] = false;
    }
    
    m_fociSize = 4.0;
    m_coloringType = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
    m_drawingType = FociDrawingTypeEnum::DRAW_AS_SQUARES;
}

/**
 * Destructor.
 */
DisplayPropertiesFoci::~DisplayPropertiesFoci()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesFoci::reset()
{
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        m_displayStatus[i] = true;
//        m_contralateralDisplayStatus[i] = false;
//        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
//        m_pasteOntoSurface[i] = false;
//    }
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesFoci::update()
{
    
}

/**
 * @return  Display status of foci.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFoci::isDisplayed(const int32_t browserTabIndex) const
{
    return m_displayStatus[browserTabIndex];
}

/**
 * Set the display status for foci.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setDisplayed(const int32_t browserTabIndex,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayStatus[browserTabIndex] = displayStatus;
}

/**
 * @return  Contralateral display status of foci.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFoci::isContralateralDisplayed(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_contralateralDisplayStatus[browserTabIndex];
}

/**
 * Set the contralateral display status for foci.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setContralateralDisplayed(const int32_t browserTabIndex,
                                                    const bool contralateralDisplayStatus)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_contralateralDisplayStatus[browserTabIndex] = contralateralDisplayStatus;
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesFoci::getDisplayGroup(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(m_displayGroup, 
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
DisplayPropertiesFoci::setDisplayGroup(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The foci size.
 */
float 
DisplayPropertiesFoci::getFociSize() const
{
    return m_fociSize;
}

/**
 * Set the foci size to the given value.
 * @param fociSize
 *     New value for foci size.
 */
void 
DisplayPropertiesFoci::setFociSize(const float fociSize)
{
    m_fociSize = fociSize;
}

/**
 * @return The coloring type.
 */
FociColoringTypeEnum::Enum 
DisplayPropertiesFoci::getColoringType() const
{
    return m_coloringType;
}

/**
 * Set the coloring type.
 * @param coloringType
 *    New value for coloring type.
 */
void 
DisplayPropertiesFoci::setColoringType(const FociColoringTypeEnum::Enum coloringType)
{
    m_coloringType = coloringType;
}

/**
 * @return The drawing type.
 */
FociDrawingTypeEnum::Enum 
DisplayPropertiesFoci::getDrawingType() const
{
    return m_drawingType;
}

/**
 * Set the drawing type to the given value.
 * @param drawingType
 *     New value for drawing type.
 */
void 
DisplayPropertiesFoci::setDrawingType(const FociDrawingTypeEnum::Enum drawingType)
{
    m_drawingType = drawingType;
}

/**
 * Set paste onto surface so the foci are placed directly on the surface.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param enabled
 *   True if pasting foci onto surface is enabled.
 */
void 
DisplayPropertiesFoci::setPasteOntoSurface(const int32_t browserTabIndex,
                                           const bool enabled)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_pasteOntoSurface[browserTabIndex] = enabled; 
}

/**
 * @param browserTabIndex
 *    Index of browser tab.
 * @return True if foci are pasted onto surface.
 */
bool 
DisplayPropertiesFoci::isPasteOntoSurface(const int32_t browserTabIndex) const
{
    return m_pasteOntoSurface[browserTabIndex];
}


