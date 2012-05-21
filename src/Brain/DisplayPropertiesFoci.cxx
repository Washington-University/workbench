
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
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_pasteOntoSurface[i] = false;
        m_displayStatus[i] = false;
        m_contralateralDisplayStatus[i] = false;
        m_fociSize[i] = 4.0;
        m_coloringType[i] = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
        m_drawingType[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
    }
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
 * @param displayGroup
 *     Display group.
 */
bool 
DisplayPropertiesFoci::isDisplayed(const DisplayGroupEnum::Enum displayGroup) const
{
    CaretAssertArrayIndex(m_displayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_displayStatus[displayGroup];
}

/**
 * Set the display status for foci.
 * @param displayGroup
 *     Display group.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_displayStatus[displayGroup] = displayStatus;
}

/**
 * @return  Contralateral display status of foci.
 * @param displayGroup
 *     Display group.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFoci::isContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup) const
{
    CaretAssertArrayIndex(m_contralateralDisplayStatus, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_contralateralDisplayStatus[displayGroup];
}

/**
 * Set the contralateral display status for foci.
 * @param displayGroup
 *     Display group.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
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
DisplayPropertiesFoci::getDisplayGroupForTab(const int32_t browserTabIndex) const
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
DisplayPropertiesFoci::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The foci size.
 * @param displayGroup
 *     Display group.
 */
float 
DisplayPropertiesFoci::getFociSize(const DisplayGroupEnum::Enum displayGroup) const
{
    CaretAssertArrayIndex(m_fociSize, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_fociSize[displayGroup];
}

/**
 * Set the foci size to the given value.
 * @param displayGroup
 *     Display group.
 * @param fociSize
 *     New value for foci size.
 */
void 
DisplayPropertiesFoci::setFociSize(const DisplayGroupEnum::Enum displayGroup,
                                   const float fociSize)
{
    CaretAssertArrayIndex(m_fociSize, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_fociSize[displayGroup] = fociSize;
}

/**
 * @return The coloring type.
 * @param displayGroup
 *     Display group.
 */
FociColoringTypeEnum::Enum 
DisplayPropertiesFoci::getColoringType(const DisplayGroupEnum::Enum displayGroup) const
{
    CaretAssertArrayIndex(m_coloringType, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    return m_coloringType[displayGroup];
}

/**
 * Set the coloring type.
 * @param displayGroup
 *     Display group.
 * @param coloringType
 *    New value for coloring type.
 */
void 
DisplayPropertiesFoci::setColoringType(const DisplayGroupEnum::Enum displayGroup,
                                       const FociColoringTypeEnum::Enum coloringType)
{
    CaretAssertArrayIndex(m_coloringType, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_coloringType[displayGroup] = coloringType;
}

/**
 * @param displayGroup
 *     Display group.
 * @return The drawing type.
 */
FociDrawingTypeEnum::Enum 
DisplayPropertiesFoci::getDrawingType(const DisplayGroupEnum::Enum displayGroup) const
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
DisplayPropertiesFoci::setDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                      const FociDrawingTypeEnum::Enum drawingType)
{
    CaretAssertArrayIndex(m_drawingType, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_drawingType[displayGroup] = drawingType;
}

/**
 * Set paste onto surface so the foci are placed directly on the surface.
 * @param displayGroup
 *     Display group.
 * @param enabled
 *   True if pasting foci onto surface is enabled.
 */
void 
DisplayPropertiesFoci::setPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup,
                                           const bool enabled)
{
    CaretAssertArrayIndex(m_pasteOntoSurface, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    m_pasteOntoSurface[displayGroup] = enabled; 
}

/**
 * @param displayGroup
 *     Display group.
 * @return True if foci are pasted onto surface.
 */
bool 
DisplayPropertiesFoci::isPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup) const
{
   CaretAssertArrayIndex(m_pasteOntoSurface, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
   return m_pasteOntoSurface[displayGroup];
}


