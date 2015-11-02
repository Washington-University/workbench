
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

#define __PALETTE_COLOR_BAR_DRAWING_INFORMATION_DECLARE__
#include "PaletteColorBarDrawingInformation.h"
#undef __PALETTE_COLOR_BAR_DRAWING_INFORMATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::PaletteColorBarDrawingInformation 
 * \brief Contains data for drawing a palette colorbar.
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param annotationColorBar
 *    The annotation color bar.
 * @param statistics
 *    Map/File statistics.
 * @param paletteColorMapping
 *    Map/File palette color mapping.
 */
PaletteColorBarDrawingInformation::PaletteColorBarDrawingInformation(const AnnotationColorBar*  annotationColorBar,
                                                                     const FastStatistics*      statistics,
                                                                     const PaletteColorMapping* paletteColorMapping)
: CaretObject(),
m_annotationColorBar(annotationColorBar),
m_statistics(statistics),
m_paletteColorMapping(paletteColorMapping)
{
    
}

/**
 * Destructor.
 */
PaletteColorBarDrawingInformation::~PaletteColorBarDrawingInformation()
{
}

/**
 * @return Is the information valid?
 */
bool
PaletteColorBarDrawingInformation::isValid() const
{
    if ((m_annotationColorBar != NULL)
        && (m_paletteColorMapping != NULL)
        && (m_statistics != NULL)) {
        return true;
    }
    
    return false;
}


/**
 * @return The annotation color bar.
 */
const AnnotationColorBar*
PaletteColorBarDrawingInformation::getAnnotationColorBar() const
{
    return m_annotationColorBar;
}

/**
 * @return The map/file statistics.
 */
const FastStatistics*
PaletteColorBarDrawingInformation::getStatistics() const
{
    return m_statistics;
}

/**
 * @return The map/file palette color mapping.
 */
const PaletteColorMapping*
PaletteColorBarDrawingInformation::getPaletteColorMapping() const
{
    return m_paletteColorMapping;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PaletteColorBarDrawingInformation::toString() const
{
    return "PaletteColorBarDrawingInformation";
}

