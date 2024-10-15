
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __TAB_DRAWING_INFO_DECLARE__
#include "TabDrawingInfo.h"
#undef __TAB_DRAWING_INFO_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::TabDrawingInfo 
 * \brief Info for drawing a tab
 * \ingroup FilesBase
 *
 * By placing this info in a an object, items can be added without having to modify many function parameters
 */

/**
 * Constructor.
 * @param mapFile
 *    The map file being drawn
 * @parm mapIndex
 *    Map in file that is being drawn
 * @param displayGroup
 *    The display group selected
 * @param labelViewMode
 *    The label viewing mode
 * @param tabIndex
 *    Index of tab being drawn
 */
TabDrawingInfo::TabDrawingInfo(CaretMappableDataFile* mapFile,
                               const int32_t mapIndex,
                               const DisplayGroupEnum::Enum displayGroup,
                               const LabelViewModeEnum::Enum labelViewMode,
                               const int32_t tabIndex)
: CaretObject(),
m_mapFile(mapFile),
m_mapIndex(mapIndex),
m_displayGroup(displayGroup),
m_labelViewMode(labelViewMode),
m_tabIndex(tabIndex)
{
    CaretAssert(mapFile);
    CaretAssert(mapIndex >= 0);
    CaretAssert((tabIndex >= 0)
                && (tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
}

/**
 * Destructor.
 */
TabDrawingInfo::~TabDrawingInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TabDrawingInfo::TabDrawingInfo(const TabDrawingInfo& obj)
: CaretObject(obj)
{
    this->copyHelperTabDrawingInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TabDrawingInfo&
TabDrawingInfo::operator=(const TabDrawingInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTabDrawingInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TabDrawingInfo::copyHelperTabDrawingInfo(const TabDrawingInfo& obj)
{
    m_mapFile       = obj.m_mapFile;
    m_mapIndex      = obj.m_mapIndex;
    m_displayGroup  = obj.m_displayGroup;
    m_labelViewMode = obj.m_labelViewMode;
    m_tabIndex      = obj.m_tabIndex;
}

/**
 * @return Map file being drawn
 */
CaretMappableDataFile*
TabDrawingInfo::getMapFile() const
{
    return m_mapFile;
}

/**
 * @return Map index being drawn
 */
int32_t
TabDrawingInfo::getMapIndex() const
{
    return m_mapIndex;
}

/**
 * @return The display group
 */
DisplayGroupEnum::Enum
TabDrawingInfo::getDisplayGroup() const
{
    return m_displayGroup;
}

/**
 * @return The label view mode
 */
LabelViewModeEnum::Enum
TabDrawingInfo::getLabelViewMode() const
{
    return m_labelViewMode;
}

/**
 * @return The tab index
 */
int32_t
TabDrawingInfo::getTabIndex() const
{
    return m_tabIndex;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TabDrawingInfo::toString() const
{
    return "TabDrawingInfo";
}

