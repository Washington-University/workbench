
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

#define __SURFACE_MONTAGE_VIEWPORT_DECLARE__
#include "SurfaceMontageViewport.h"
#undef __SURFACE_MONTAGE_VIEWPORT_DECLARE__

#include "CaretAssert.h"
#include "Surface.h"
#include "StructureEnum.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageViewport 
 * \brief Viewports in a surface montage
 */

/**
 * Constructor.
 *
 * @param surface
 *   Surface in the montage.
 * @param surfaceForSettingOrthographicProjection
 *   Surface to use for setting the orthographic projection
 *   If different that getSurface() is allows scaling surfaces in a montage view to match another surface.
 *   Flat Dentate Hippocampus surface use the Hippocampus Surface for orthographic projection
 *   so that they are drawn in the same width
 * @param projectionViewType
 *   Projection view type.
 */
SurfaceMontageViewport::SurfaceMontageViewport(Surface* surface,
                                               Surface* surfaceForSettingOrthographicProjection,
                                               const ProjectionViewTypeEnum::Enum projectionViewType)
: CaretObject()
{
    CaretAssert(surface);
    CaretAssert(surfaceForSettingOrthographicProjection);
    
    m_surface = surface;
    m_surfaceForSettingOrthographicProjection = surfaceForSettingOrthographicProjection;
    m_structure = surface->getStructure();
    m_projectionViewType = projectionViewType;
    m_viewport[0] = -1;
    m_viewport[1] = -1;
    m_viewport[2] = -1;
    m_viewport[3] = -1;
    
    m_row    = -1;
    m_column = -1;
}


/**
 * Destructor.
 */
SurfaceMontageViewport::~SurfaceMontageViewport()
{
    
}

/**
 * @return true if the coordinates are inside the viewport, else false.
 * @param x
 *   X-coordinate
 * @param y
 *   Y-coordinate
 */
bool
SurfaceMontageViewport::isInside(const int32_t x,
                                 const int32_t y) const
{
    if (x < m_viewport[0]) return false;
    if (x > (m_viewport[0] + m_viewport[2])) return false;
    if (y < m_viewport[1]) return false;
    if (y > (m_viewport[1] + m_viewport[3])) return false;
    
    return true;
}

/**
 * Set the row and column.
 *
 * @param row
 *    New value for row.
 * @param column
 *    New value for column.
 */
void
SurfaceMontageViewport::setRowAndColumn(const int32_t row,
                                        const int32_t column)
{
    m_row    = row;
    m_column = column;
}

/**
 * Get the viewport.
 * 
 * @param viewportOut
 *    Output containing viewport.
 */
void
SurfaceMontageViewport::getViewport(int32_t viewportOut[4]) const
{
    CaretAssert((m_viewport[0] >= 0)
                && (m_viewport[1] >= 0)
                && (m_viewport[2] > 0)
                && (m_viewport[3] > 1));
    
    viewportOut[0] = m_viewport[0];
    viewportOut[1] = m_viewport[1];
    viewportOut[2] = m_viewport[2];
    viewportOut[3] = m_viewport[3];
}

/**
 * Set the viewport for this item.
 * @param viewport
 *     Values for viewport.
 */
void
SurfaceMontageViewport::setViewport(const int32_t viewport[4])
{
    m_viewport[0] = viewport[0];
    m_viewport[1] = viewport[1];
    m_viewport[2] = viewport[2];
    m_viewport[3] = viewport[3];
}

/**
 * @return Row of this item (0 is top)
 */
int32_t
SurfaceMontageViewport::getRow() const
{
    CaretAssert(m_row >= 0);
    return m_row;
}

/**
 * @return Column of this item (0 is left)
 */
int32_t
SurfaceMontageViewport::getColumn() const
{
    CaretAssert(m_column >= 0);
    return m_column;
}

/**
 * @return X-coordinate in viewport.
 */
int32_t
SurfaceMontageViewport::getX() const
{
    CaretAssert( m_viewport[0] >= 0);
    return m_viewport[0];
}

/**
 * @return Y-Coordinate in viewport.
 */
int32_t
SurfaceMontageViewport::getY() const
{
    CaretAssert(m_viewport[1] >= 0);
    return m_viewport[1];
}

/**
 * @return Width of viewport.
 */
int32_t
SurfaceMontageViewport::getWidth() const
{
    CaretAssert(m_viewport[2] > 0);
    return m_viewport[2];
}

/**
 * @return Height of viewport.
 */
int32_t
SurfaceMontageViewport::getHeight() const
{
    CaretAssert(m_viewport[3] > 0);
    return m_viewport[3];
}

/**
 * Find the number of rows and columns for the given montage viewports.
 *
 * @param montageViewports
 *     The montage viewports.
 * @param numberOfRowsOut
 *     Output number of rows.
 * @param numberOfColumnsOut
 *     Output number of columns.
 */
void
SurfaceMontageViewport::getNumberOfRowsAndColumns(const std::vector<SurfaceMontageViewport*>& montageViewports,
                                                  int32_t& numberOfRowsOut,
                                                  int32_t& numberOfColumnsOut)
{
    std::vector<const SurfaceMontageViewport*> vps(montageViewports.begin(),
                                                   montageViewports.end());
    getNumberOfRowsAndColumns(vps,
                              numberOfRowsOut,
                              numberOfColumnsOut);
}

/**
 * Find the number of rows and columns for the given montage viewports.
 *
 * @param montageViewports
 *     The montage viewports.
 * @param numberOfRowsOut
 *     Output number of rows.
 * @param numberOfColumnsOut
 *     Output number of columns.
 */
void
SurfaceMontageViewport::getNumberOfRowsAndColumns(const std::vector<const SurfaceMontageViewport*>& montageViewports,
                                                  int32_t& numberOfRowsOut,
                                                  int32_t& numberOfColumnsOut)
{
    numberOfRowsOut    = 0;
    numberOfColumnsOut = 0;
    
    if (montageViewports.empty()) {
        return;
    }
    
    for (std::vector<const SurfaceMontageViewport*>::const_iterator iter = montageViewports.begin();
         iter != montageViewports.end();
         iter++) {
        const SurfaceMontageViewport* svp = *iter;
        const int32_t row    = svp->getRow();
        const int32_t column = svp->getColumn();

        if (row > numberOfRowsOut) {
            numberOfRowsOut = row;
        }
        if (column > numberOfColumnsOut) {
            numberOfColumnsOut = column;
        }
    }
    
    /*
     * Add one since row and column in viewports are indices that range 0 to 1
     */
    numberOfRowsOut++;
    numberOfColumnsOut++;
}

