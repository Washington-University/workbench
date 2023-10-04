
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE_DECLARE__
#include "DrawingViewportContentVolumeSlice.h"
#undef __DRAWING_VIEWPORT_CONTENT_VOLUME_SLICE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DrawingViewportContentVolumeSlice
 * \brief Info about a volue slice viewport
 * \ingroup Brain
 */

/**
 * Default constructor of invalid instance.
 */
DrawingViewportContentVolumeSlice::DrawingViewportContentVolumeSlice()
: DrawingViewportContentVolumeSlice(-1, -1, -1, -1, VolumeSliceViewPlaneEnum::ALL, Plane(), Vector3D())
{
}

/**
 * Constructor.
 * @param numberOfRows
 *    Number or rows
 * @param numberOfColumns
 *    Number of columns
 * @param rowIndex
 *    Index of row
 * @param columnIndex
 *    Index of column
 * @param slicePlane
 *    The slice plane
 * @param plane
 *    The plane equation
 * @param sliceXYZ
 *    XYZ of the slice
 */
DrawingViewportContentVolumeSlice::DrawingViewportContentVolumeSlice(const int32_t numberOfRows,
                                                                     const int32_t numberOfColumns,
                                                                     const int32_t rowIndex,
                                                                     const int32_t columnIndex,
                                                                     const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                                     const Plane& plane,
                                                                     const Vector3D& sliceXYZ)
: CaretObject(),
m_numberOfRows(numberOfRows),
m_numberOfColumns(numberOfColumns),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex),
m_slicePlane(slicePlane),
m_plane(plane),
m_sliceXYZ(sliceXYZ)
{
    
}

/**
 * Destructor.
 */
DrawingViewportContentVolumeSlice::~DrawingViewportContentVolumeSlice()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DrawingViewportContentVolumeSlice::DrawingViewportContentVolumeSlice(const DrawingViewportContentVolumeSlice& obj)
: CaretObject(obj)
{
    this->copyHelperDrawingViewportContentVolumeSlice(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DrawingViewportContentVolumeSlice&
DrawingViewportContentVolumeSlice::operator=(const DrawingViewportContentVolumeSlice& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDrawingViewportContentVolumeSlice(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DrawingViewportContentVolumeSlice::copyHelperDrawingViewportContentVolumeSlice(const DrawingViewportContentVolumeSlice& obj)
{
    m_numberOfRows    = obj.m_numberOfRows;
    m_numberOfColumns = obj.m_numberOfColumns;
    m_rowIndex        = obj.m_rowIndex;
    m_columnIndex     = obj.m_columnIndex;
    m_slicePlane      = obj.m_slicePlane;
    m_plane           = obj.m_plane;
    m_sliceXYZ        = obj.m_sliceXYZ;
}

/**
 * @return Number of rows
 */
int32_t
DrawingViewportContentVolumeSlice::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return Number of columns
 */
int32_t
DrawingViewportContentVolumeSlice::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * @return Row index
 */
int32_t
DrawingViewportContentVolumeSlice::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return Column index
 */
int32_t
DrawingViewportContentVolumeSlice::getColumnIndex() const
{
    return m_columnIndex;
}

/**
 * @return The slice view plane
 */
VolumeSliceViewPlaneEnum::Enum
DrawingViewportContentVolumeSlice::getVolumeSliceViewPlane() const
{
    return m_slicePlane;
}

/**
 * @return The plane
 */
Plane
DrawingViewportContentVolumeSlice::getPlane() const
{
    return m_plane;
}

/**
 * @return The slice XYZ
 */
Vector3D
DrawingViewportContentVolumeSlice::getSliceXYZ() const
{
    return m_sliceXYZ;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DrawingViewportContentVolumeSlice::toString() const
{
    return "DrawingViewportContentVolumeSlice";
}

