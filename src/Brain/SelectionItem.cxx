
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

#define __SELECTION_ITEM_DECLARE__
#include "SelectionItem.h"
#undef __SELECTION_ITEM_DECLARE__
#include <limits>
using namespace caret;

#include "SelectionItemDataTypeEnum.h"

/**
 * \class SelectionItem
 * \brief Abstract class for selected items.
 *
 * Abstract class for selected items.
 */

/**
 * Constructor.
 */
SelectionItem::SelectionItem(const SelectionItemDataTypeEnum::Enum itemDataType)
: CaretObject()
{
    m_itemDataType = itemDataType;
    m_enabledForSelection = true;
    m_brain = NULL;
    m_screenDepth = 0.0;
    m_screenXYZ[0] = 0.0;
    m_screenXYZ[1] = 0.0;
    m_screenXYZ[2] = std::numeric_limits<double>::max();
    m_modelXYZ[0] = 0.0;
    m_modelXYZ[1] = 0.0;
    m_modelXYZ[2] = 0.0;
}

/**
 * Destructor.
 */
SelectionItem::~SelectionItem()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItem::SelectionItem(const SelectionItem& obj)
: CaretObject(obj)
{
    copyHelperSelectionItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
SelectionItem&
SelectionItem::operator=(const SelectionItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        copyHelperSelectionItem(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItem::copyHelperSelectionItem(const SelectionItem& idItem)
{
    m_brain = idItem.m_brain;
    m_enabledForSelection = idItem.m_enabledForSelection;
    m_screenDepth  = idItem.m_screenDepth;
    m_screenXYZ[0] = idItem.m_screenXYZ[0];
    m_screenXYZ[1] = idItem.m_screenXYZ[1];
    m_screenXYZ[2] = idItem.m_screenXYZ[2];
    m_modelXYZ[0]  = idItem.m_modelXYZ[0];
    m_modelXYZ[1]  = idItem.m_modelXYZ[1];
    m_modelXYZ[2]  = idItem.m_modelXYZ[2];
}

/**
 * Reset this selection item.  Deriving
 * classes should override this method to
 * reset its selection data and also call
 * the method in this class.
 */
void 
SelectionItem::reset()
{
    m_brain = NULL;
    m_screenDepth = 0.0;
    m_screenXYZ[0] = 0.0;
    m_screenXYZ[1] = 0.0;
    m_screenXYZ[2] = std::numeric_limits<double>::max();
    m_modelXYZ[0] = 0.0;
    m_modelXYZ[1] = 0.0;
    m_modelXYZ[2] = 0.0;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItem::toString() const
{
    AString text = "";
    text += ("Type: " + SelectionItemDataTypeEnum::toGuiName(m_itemDataType) + "\n");
    text += ("Depth: " + AString::number(m_screenDepth) + "\n");
    text += ("Model XYZ:  " + AString::fromNumbers(m_modelXYZ, 3, ", ") + "\n");
    text += ("Screen XYZ: " + AString::fromNumbers(m_screenXYZ, 3, ", ") + "\n");
    return text;
}

/**
 * @return The type of selected item.
 */
SelectionItemDataTypeEnum::Enum 
SelectionItem::getItemDataType() const
{
    return m_itemDataType;
}

/**
 * @return Data type enabled for selection.
 */
bool 
SelectionItem::isEnabledForSelection() const
{
    return m_enabledForSelection;
}

/**
 * Set the data type enabled for selection.
 * @param enabled
 *    New value for selection enabled status.
 */
void 
SelectionItem::setEnabledForSelection(const bool enabled)
{
    m_enabledForSelection = enabled;
}

/**
 * @return Brain in which identification item resides.
 */
Brain* 
SelectionItem::getBrain()
{
    return m_brain;
}

/**
 * Set the brain.
 *
 * param brain
 *    Brain in which identification item resides.
 */
void 
SelectionItem::setBrain(Brain* brain)
{
    m_brain = brain;
}

/**
 * @return Screen depth of item.
 */
double 
SelectionItem::getScreenDepth() const
{
    return m_screenDepth;
}

/**
 * Set the screen depth of the item.
 * @param screenDepth
 *    New value for screen depth.
 */
void 
SelectionItem::setScreenDepth(const double screenDepth)
{
    m_screenDepth = screenDepth;
}

/**
 * Get the screen XYZ of the selected item.
 * @param screenXYZ
 *    XYZ out.
 */
void 
SelectionItem::getScreenXYZ(double screenXYZ[3]) const
{
    screenXYZ[0] = m_screenXYZ[0];
    screenXYZ[1] = m_screenXYZ[1];
    screenXYZ[2] = m_screenXYZ[2];
}

/**
 * Set the screen XYZ of the selected item.
 * @param screenXYZ
 *    new XYZ.
 */
void 
SelectionItem::setScreenXYZ(const double screenXYZ[3])
{
    m_screenXYZ[0] = screenXYZ[0];
    m_screenXYZ[1] = screenXYZ[1];
    m_screenXYZ[2] = screenXYZ[2];
}

/**
 * Set the screen XYZ of the selected item.
 * @param screenXYZ
 *    new XYZ.
 */
void
SelectionItem::setScreenXYZ(const float screenXYZ[3])
{
    m_screenXYZ[0] = screenXYZ[0];
    m_screenXYZ[1] = screenXYZ[1];
    m_screenXYZ[2] = screenXYZ[2];
}

/**
 * Get the model XYZ of the selected item.
 * @param modelXYZ
 *    XYZ out.
 */
void 
SelectionItem::getModelXYZ(double modelXYZ[3]) const
{
    modelXYZ[0] = m_modelXYZ[0];
    modelXYZ[1] = m_modelXYZ[1];
    modelXYZ[2] = m_modelXYZ[2];
}

/**
 * Set the model XYZ of the selected item.
 * @param modelXYZ
 *    new XYZ.
 */
void 
SelectionItem::setModelXYZ(const double modelXYZ[3])
{
    m_modelXYZ[0] = modelXYZ[0];
    m_modelXYZ[1] = modelXYZ[1];
    m_modelXYZ[2] = modelXYZ[2];
}

/**
 * Set the model XYZ of the selected item.
 * @param modelXYZ
 *    new XYZ.
 */
void
SelectionItem::setModelXYZ(const float modelXYZ[3])
{
    m_modelXYZ[0] = modelXYZ[0];
    m_modelXYZ[1] = modelXYZ[1];
    m_modelXYZ[2] = modelXYZ[2];
}

/**
 * Is the other screen depth closer to the viewer than the currently 
 * selected item?  So, if true is returned, then replace the
 * current identification item
 *
 * (1) If there is no selected item, true is immediately returned.
 * (2) If there is an selected item and the other screen depth is closer
 * to the viewer, true is returned.
 * (3) false is returned.
 *
 * @param otherScreenDepth
 *    Screen depth for testing.
 * @return result of test.
 */
bool 
SelectionItem::isOtherScreenDepthCloserToViewer(const double otherScreenDepth) const
{
    if (isValid() == false) {
        return true;
    }
    
    /*
     * Multiple annotations that are on top of each other may be drawn at the same
     * depth so using "<=" should select the one that appears to be "on top" to the user.
     */
    if ((m_itemDataType == SelectionItemDataTypeEnum::ANNOTATION)
        || (m_itemDataType == SelectionItemDataTypeEnum::CHART_TWO_LINE_LAYER)) {
        if (otherScreenDepth <= m_screenDepth) {
            return true;
        }
    }
    else {
        if (otherScreenDepth < m_screenDepth) {
            return true;
        }
    }

    return false;
}


