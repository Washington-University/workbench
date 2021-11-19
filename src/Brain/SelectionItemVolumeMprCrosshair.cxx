
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

#define __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR_DECLARE__
#include "SelectionItemVolumeMprCrosshair.h"
#undef __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class SelectionItemVolumeMprCrosshair
 * \brief Selected voxel.
 *
 * Information about an selected voxel.
 */


/**
 * Constructor.
 */
SelectionItemVolumeMprCrosshair::SelectionItemVolumeMprCrosshair()
: SelectionItem(SelectionItemDataTypeEnum::VOLUME_MPR_CROSSHAIR)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}

/**
 * Constructor for child classes.
 *
 * @param itemDataType
 *     The selection item data type for child class.
 */
SelectionItemVolumeMprCrosshair::SelectionItemVolumeMprCrosshair(const SelectionItemDataTypeEnum::Enum itemDataType)
: SelectionItem(itemDataType)
{
    /*
     * Note: reset() is virtual so cannot call from constructor.
     */
    resetPrivate();
}


/**
 * Destructor.
 */
SelectionItemVolumeMprCrosshair::~SelectionItemVolumeMprCrosshair()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemVolumeMprCrosshair::SelectionItemVolumeMprCrosshair(const SelectionItemVolumeMprCrosshair& obj)
: SelectionItem(obj)
{
    copyHelperSelectionItemVolumeMprCrosshair(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
SelectionItemVolumeMprCrosshair&
SelectionItemVolumeMprCrosshair::operator=(const SelectionItemVolumeMprCrosshair& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        copyHelperSelectionItemVolumeMprCrosshair(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItemVolumeMprCrosshair::copyHelperSelectionItemVolumeMprCrosshair(const SelectionItemVolumeMprCrosshair& idItem)
{
    m_axis        = idItem.m_axis;
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemVolumeMprCrosshair::reset()
{
    SelectionItem::reset();
    
    resetPrivate();
}

/**
 * Reset this selection item.
 */
void
SelectionItemVolumeMprCrosshair::resetPrivate()
{
    m_axis        = Axis::INVALID;
}

/**
 * @return Axis selected
 */
SelectionItemVolumeMprCrosshair::Axis
SelectionItemVolumeMprCrosshair::getAxis() const
{
    return m_axis;
}

/**
 * Set the axis identification.
 *
 * @param brain
 *    Brain containing the volume.
 * @param axis
 *    Axis selected
 * @param screenDepth
 *    The screen depth.
 */
void
SelectionItemVolumeMprCrosshair::setIdentification(Brain* brain,
                                                   const Axis axis,
                                                   const double screenDepth)
{
    setBrain(brain);
    m_axis        = axis;
    setScreenDepth(screenDepth);
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemVolumeMprCrosshair::isValid() const
{
    return (m_axis != Axis::INVALID);
}

bool
SelectionItemVolumeMprCrosshair::isRotateAxisSelected() const
{
    bool rotateFlag(false);
    switch (m_axis) {
        case Axis::INVALID:
            break;
        case Axis::ANTERIOR_ROTATE:
        case Axis::INFERIOR_ROTATE:
        case Axis::LEFT_ROTATE:
        case Axis::POSTERIOR_ROTATE:
        case Axis::RIGHT_ROTATE:
        case Axis::SUPERIOR_ROTATE:
        case Axis::RING_ROTATE:
            rotateFlag = true;
            break;
        case Axis::ANTERIOR_SLICE:
        case Axis::INFERIOR_SLICE:
        case Axis::LEFT_SLICE:
        case Axis::POSTERIOR_SLICE:
        case Axis::RIGHT_SLICE:
        case Axis::SUPERIOR_SLICE:
            break;
    }
    
    return rotateFlag;
}

/**
 * @return True if a slice axis is selected
 */
bool
SelectionItemVolumeMprCrosshair::isSliceAxisSelected() const
{
    bool sliceFlag(false);
    switch (m_axis) {
        case Axis::INVALID:
            break;
        case Axis::ANTERIOR_ROTATE:
        case Axis::INFERIOR_ROTATE:
        case Axis::LEFT_ROTATE:
        case Axis::POSTERIOR_ROTATE:
        case Axis::RIGHT_ROTATE:
        case Axis::SUPERIOR_ROTATE:
        case Axis::RING_ROTATE:
            break;
        case Axis::ANTERIOR_SLICE:
        case Axis::INFERIOR_SLICE:
        case Axis::LEFT_SLICE:
        case Axis::POSTERIOR_SLICE:
        case Axis::RIGHT_SLICE:
        case Axis::SUPERIOR_SLICE:
            sliceFlag = true;
            break;
    }

    return sliceFlag;
}


/**
 * @return Text name of axis.
 * @param axis
 *    Name of axis.
 */
AString
SelectionItemVolumeMprCrosshair::axisToName(const Axis axis)
{
    AString name("INVALID");
    
    switch (axis) {
        case Axis::ANTERIOR_ROTATE:
            name = "Anterior Rotate";
            break;
        case Axis::ANTERIOR_SLICE:
            name = "Anterior Slice";
            break;
        case Axis::INFERIOR_ROTATE:
            name = "Inferior Rotate";
            break;
        case Axis::INFERIOR_SLICE:
            name = "Inferior Slice";
            break;
        case Axis::INVALID:
            name = "Invalid";
            break;
        case Axis::LEFT_ROTATE:
            name = "Left Rotate";
            break;
        case Axis::LEFT_SLICE:
            name = "Left Slice";
            break;
        case Axis::POSTERIOR_ROTATE:
            name = "Posterior Rotate";
            break;
        case Axis::POSTERIOR_SLICE:
            name = "Posterior Slice";
            break;
        case Axis::RIGHT_ROTATE:
            name = "Right Rotate";
            break;
        case Axis::RIGHT_SLICE:
            name = "Right Slice";
            break;
        case Axis::RING_ROTATE:
            name = "Ring Rotate";
            break;
        case Axis::SUPERIOR_ROTATE:
            name = "Superior Rotate";
            break;
        case Axis::SUPERIOR_SLICE:
            name = "Superior Slice";
            break;
    }

    return name;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemVolumeMprCrosshair::toString() const
{
    AString text = SelectionItem::toString();
    AString name = "INVALID";
    
    
    return text;
}




