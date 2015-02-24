
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __SELECTION_ITEM_VOXEL_IDENTIFICATION_SYMBOL_DECLARE__
#include "SelectionItemVoxelIdentificationSymbol.h"
#undef __SELECTION_ITEM_VOXEL_IDENTIFICATION_SYMBOL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemVoxelIdentificationSymbol 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Brain
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
SelectionItemVoxelIdentificationSymbol::SelectionItemVoxelIdentificationSymbol()
: SelectionItem(SelectionItemDataTypeEnum::VOXEL_IDENTIFICATION_SYMBOL)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}

/**
 * Destructor.
 */
SelectionItemVoxelIdentificationSymbol::~SelectionItemVoxelIdentificationSymbol()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemVoxelIdentificationSymbol::SelectionItemVoxelIdentificationSymbol(const SelectionItemVoxelIdentificationSymbol& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemVoxelIdentificationSymbol(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemVoxelIdentificationSymbol&
SelectionItemVoxelIdentificationSymbol::operator=(const SelectionItemVoxelIdentificationSymbol& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemVoxelIdentificationSymbol(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemVoxelIdentificationSymbol::copyHelperSelectionItemVoxelIdentificationSymbol(const SelectionItemVoxelIdentificationSymbol& obj)
{
    m_voxelXYZ[0] = obj.m_voxelXYZ[0];
    m_voxelXYZ[1] = obj.m_voxelXYZ[1];
    m_voxelXYZ[2] = obj.m_voxelXYZ[2];
    m_voxelValid  = obj.m_voxelValid;
}

/**
 * Get the coordinates of the voxel identification symbol.
 *
 * @param xyzOut
 *    Coordinates of the voxel identification symbol.
 */
void
SelectionItemVoxelIdentificationSymbol::getVoxelXYZ(float xyzOut[3]) const
{
    xyzOut[0] = m_voxelXYZ[0];
    xyzOut[1] = m_voxelXYZ[1];
    xyzOut[2] = m_voxelXYZ[2];
}

/**
 * Set the coordinates of the voxel identification symbol.
 *
 * @param xyzOut
 *    Coordinates of the voxel identification symbol.
 */
void
SelectionItemVoxelIdentificationSymbol::setVoxelXYZ(const float xyz[3])
{
    m_voxelXYZ[0] = xyz[0];
    m_voxelXYZ[1] = xyz[1];
    m_voxelXYZ[2] = xyz[2];
    m_voxelValid  = true;
}

/**
 * Reset this selection item.
 */
void
SelectionItemVoxelIdentificationSymbol::reset()
{
    SelectionItem::reset();
    
    resetPrivate();
}

/**
 * Reset this items data.
 */
void
SelectionItemVoxelIdentificationSymbol::resetPrivate()
{
    m_voxelValid  = false;
    m_voxelXYZ[0] = 0.0;
    m_voxelXYZ[1] = 0.0;
    m_voxelXYZ[2] = 0.0;
}

/**
 * @return Is this selected item valid?
 */
bool
SelectionItemVoxelIdentificationSymbol::isValid() const
{
    return m_voxelValid;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SelectionItemVoxelIdentificationSymbol::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Voxel XYZ: " + AString::fromNumbers(m_voxelXYZ, 3, ", ") + "\n"
             + "Valid: " + AString::fromBool(m_voxelValid) + "\n");
    
    
    return text;
}

