
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

#define __SELECTION_ITEM_IMAGE_CONTROL_POINT_DECLARE__
#include "SelectionItemImageControlPoint.h"
#undef __SELECTION_ITEM_IMAGE_CONTROL_POINT_DECLARE__

#include "ControlPointFile.h"
#include "ImageFile.h"

using namespace caret;


    
/**
 * \class SelectionItemImageControlPoint 
 * \brief Contains information about the selected control point.
 */
/**
 * Constructor.
 */
SelectionItemImageControlPoint::SelectionItemImageControlPoint()
: SelectionItem(SelectionItemDataTypeEnum::IMAGE_CONTROL_POINT)
{
    m_imageFile        = NULL;
    m_controlPointFile = NULL;
    m_controlPoint     = NULL;
    m_controlPointIndexInFile = -1;
}

/**
 * Destructor.
 */
SelectionItemImageControlPoint::~SelectionItemImageControlPoint()
{
}

/**
 * Reset this selection item.
 */
void
SelectionItemImageControlPoint::reset()
{
    SelectionItem::reset();
    
    m_imageFile        = NULL;
    m_controlPointFile = NULL;
    m_controlPoint     = NULL;
    m_controlPointIndexInFile = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemImageControlPoint::isValid() const
{
    if (m_imageFile == NULL) {
        return false;
    }
    if (m_controlPointFile == NULL) {
        return false;
    }
    if (m_controlPoint == NULL) {
        return false;
    }
    if ((m_controlPointIndexInFile < 0)
        || (m_controlPointIndexInFile >= m_controlPointFile->getNumberOfControlPoints())) {
        return false;
    }
    
    return true;
}

/**
 * @return Image File that was selected (NULL if not valid).
 */
const ImageFile*
SelectionItemImageControlPoint::getImageFile() const
{
    return m_imageFile;
}

/**
 * @return Image File that was selected (NULL if not valid).
 */
ImageFile*
SelectionItemImageControlPoint::getImageFile()
{
    return m_imageFile;
}

/**
 * Set the image file that was selected.
 *
 * @param imageFile
 *    Pointer to selected image file (NULL if not valid).
 */
void 
SelectionItemImageControlPoint::setImageFile(ImageFile* imageFile)
{
    m_imageFile = imageFile;
}

/**
 * @return Control point that was selected (NULL if not valid).
 */
const ControlPointFile*
SelectionItemImageControlPoint::getControlPointFile() const
{
    return m_controlPointFile;
}

/**
 * @return Control point that was selected (NULL if not valid).
 */
ControlPointFile*
SelectionItemImageControlPoint::getControlPointFile()
{
    return m_controlPointFile;
}

/**
 * Set the control point file that was selected.
 *
 * @param controlPointFile
 *    Pointer to selected control point file (NULL if not valid).
 */
void
SelectionItemImageControlPoint::setControlPointFile(ControlPointFile* controlPointFile)
{
    m_controlPointFile = controlPointFile;
}

/**
 * @return Index of the control point in the control point file (negative if invalid)
 */
int32_t
SelectionItemImageControlPoint::getControlPointIndexInFile() const
{
    return m_controlPointIndexInFile;
}

/**
 * Set the index of the control point in the file.
 *
 * @param controlPointIndexInFile
 *     Index of the control point in the file.
 */
void
SelectionItemImageControlPoint::setControlPointIndexInFile(const int32_t controlPointIndexInFile)
{
    m_controlPointIndexInFile = controlPointIndexInFile;
}

/**
 * @return Pointer to the control point (NULL if invalid)
 */
ControlPoint3D*
SelectionItemImageControlPoint::getControlPoint()
{
    return m_controlPoint;
}

/**
 * @return Pointer to the control point (NULL if invalid)
 */
const ControlPoint3D*
SelectionItemImageControlPoint::getControlPoint() const
{
    return m_controlPoint;
}

/**
 * Set the control point.
 *
 * @param controlPoint
 *     Pointer to the control point.
 */
void
SelectionItemImageControlPoint::setControlPoint(ControlPoint3D* controlPoint)
{
    m_controlPoint = controlPoint;
}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemImageControlPoint::toString() const
{
    AString text = SelectionItem::toString();
    text += ("ImageFile: " + ((m_imageFile != NULL) ? m_imageFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("ControlPointFile: " + ((m_imageFile != NULL) ? m_controlPointFile->getFileNameNoPath() : "INVALID") + "\n");
    text += ("Index in File: " + AString::number(m_controlPointIndexInFile) + "\n");
    return text;
}
