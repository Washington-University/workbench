
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

#define __DRAWING_VIEWPORT_CONTENT_SURFACE_DECLARE__
#include "DrawingViewportContentSurface.h"
#undef __DRAWING_VIEWPORT_CONTENT_SURFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DrawingViewportContentSurface
 * \brief Info about a volue slice viewport
 * \ingroup Brain
 */

/**
 * Default constructor of invalid instance.
 */
DrawingViewportContentSurface::DrawingViewportContentSurface()
: DrawingViewportContentSurface(-1, -1, -1, -1)
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
DrawingViewportContentSurface::DrawingViewportContentSurface(const int32_t numberOfRows,
                                                             const int32_t numberOfColumns,
                                                             const int32_t rowIndex,
                                                             const int32_t columnIndex)
: CaretObject(),
m_numberOfRows(numberOfRows),
m_numberOfColumns(numberOfColumns),
m_rowIndex(rowIndex),
m_columnIndex(columnIndex)
{
    
}

/**
 * Destructor.
 */
DrawingViewportContentSurface::~DrawingViewportContentSurface()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DrawingViewportContentSurface::DrawingViewportContentSurface(const DrawingViewportContentSurface& obj)
: CaretObject(obj)
{
    this->copyHelperDrawingViewportContentSurface(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
DrawingViewportContentSurface&
DrawingViewportContentSurface::operator=(const DrawingViewportContentSurface& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDrawingViewportContentSurface(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
DrawingViewportContentSurface::copyHelperDrawingViewportContentSurface(const DrawingViewportContentSurface& obj)
{
    m_numberOfRows    = obj.m_numberOfRows;
    m_numberOfColumns = obj.m_numberOfColumns;
    m_rowIndex        = obj.m_rowIndex;
    m_columnIndex     = obj.m_columnIndex;
}

/**
 * @return Number of rows
 */
int32_t
DrawingViewportContentSurface::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * @return Number of columns
 */
int32_t
DrawingViewportContentSurface::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * @return Row index
 */
int32_t
DrawingViewportContentSurface::getRowIndex() const
{
    return m_rowIndex;
}

/**
 * @return Column index
 */
int32_t
DrawingViewportContentSurface::getColumnIndex() const
{
    return m_numberOfColumns;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DrawingViewportContentSurface::toString() const
{
    return "DrawingViewportContentSurface";
}

