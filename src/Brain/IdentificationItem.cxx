
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __IDENTIFICATION_ITEM_DECLARE__
#include "IdentificationItem.h"
#undef __IDENTIFICATION_ITEM_DECLARE__
#include <limits>
using namespace caret;

#include "IdentificationItemDataTypeEnum.h"

/**
 * \class IdentificationItem
 * \brief Abstract class for identified items.
 *
 * Abstract class for identified items.
 */

/**
 * Constructor.
 */
IdentificationItem::IdentificationItem(const IdentificationItemDataTypeEnum::Enum itemDataType)
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
IdentificationItem::~IdentificationItem()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
IdentificationItem::IdentificationItem(const IdentificationItem& obj)
: CaretObject(obj)
{
    copyHelperIdentificationItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
IdentificationItem&
IdentificationItem::operator=(const IdentificationItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        copyHelperIdentificationItem(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param ff
 *    Object that is copied.
 */
void
IdentificationItem::copyHelperIdentificationItem(const IdentificationItem& idItem)
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
IdentificationItem::reset()
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
IdentificationItem::toString() const
{
    AString text = "";
    text += ("Type: " + IdentificationItemDataTypeEnum::toGuiName(m_itemDataType) + "\n");
    text += ("Depth: " + AString::number(m_screenDepth) + "\n");
    text += ("Model XYZ:  " + AString::fromNumbers(m_modelXYZ, 3, ", ") + "\n");
    text += ("Screen XYZ: " + AString::fromNumbers(m_screenXYZ, 3, ", ") + "\n");
    return text;
}

/**
 * @return The type of identified item.
 */
IdentificationItemDataTypeEnum::Enum 
IdentificationItem::getItemDataType() const
{
    return m_itemDataType;
}

/**
 * @return Data type enabled for selection.
 */
bool 
IdentificationItem::isEnabledForSelection() const
{
    return m_enabledForSelection;
}

/**
 * Set the data type enabled for selection.
 * @param enabled
 *    New value for selection enabled status.
 */
void 
IdentificationItem::setEnabledForSelection(const bool enabled)
{
    m_enabledForSelection = enabled;
}

/**
 * @return Brain in which identification item resides.
 */
Brain* 
IdentificationItem::getBrain()
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
IdentificationItem::setBrain(Brain* brain)
{
    m_brain = brain;
}

/**
 * @return Screen depth of item.
 */
double 
IdentificationItem::getScreenDepth() const
{
    return m_screenDepth;
}

/**
 * Set the screen depth of the item.
 * @param screenDepth
 *    New value for screen depth.
 */
void 
IdentificationItem::setScreenDepth(const double screenDepth)
{
    m_screenDepth = screenDepth;
}

/**
 * Get the screen XYZ of the identified item.
 * @param screenXYZ
 *    XYZ out.
 */
void 
IdentificationItem::getScreenXYZ(double screenXYZ[3]) const
{
    screenXYZ[0] = m_screenXYZ[0];
    screenXYZ[1] = m_screenXYZ[1];
    screenXYZ[2] = m_screenXYZ[2];
}

/**
 * Set the screen XYZ of the identified item.
 * @param screenXYZ
 *    new XYZ.
 */
void 
IdentificationItem::setScreenXYZ(const double screenXYZ[3])
{
    m_screenXYZ[0] = screenXYZ[0];
    m_screenXYZ[1] = screenXYZ[1];
    m_screenXYZ[2] = screenXYZ[2];
}

/**
 * Get the model XYZ of the identified item.
 * @param modelXYZ
 *    XYZ out.
 */
void 
IdentificationItem::getModelXYZ(double modelXYZ[3]) const
{
    modelXYZ[0] = m_modelXYZ[0];
    modelXYZ[1] = m_modelXYZ[1];
    modelXYZ[2] = m_modelXYZ[2];
}

/**
 * Set the model XYZ of the identified item.
 * @param modelXYZ
 *    new XYZ.
 */
void 
IdentificationItem::setModelXYZ(const double modelXYZ[3])
{
    m_modelXYZ[0] = modelXYZ[0];
    m_modelXYZ[1] = modelXYZ[1];
    m_modelXYZ[2] = modelXYZ[2];
}

/**
 * Is the other screen depth closer to the viewer than the currently 
 * identified item?  So, if true is returned, then replace the
 * current identification item
 *
 * (1) If there is no identified item, true is immediately returned.
 * (2) If there is an identified item and the other screen depth is closer
 * to the viewer, true is returned.
 * (3) false is returned.
 *
 * @param otherScreenDepth
 *    Screen depth for testing.
 * @return result of test.
 */
bool 
IdentificationItem::isOtherScreenDepthCloserToViewer(const double otherScreenDepth) const
{
    if (isValid() == false) {
        return true;
    }
    
    if (otherScreenDepth < m_screenDepth) {
        return true;
    }

    return false;
}


