
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ANNOTATION_COORDINATE_INFORMATION_DECLARE__
#include "AnnotationCoordinateInformation.h"
#undef __ANNOTATION_COORDINATE_INFORMATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationCoordinateInformation 
 * \brief Contains annotation coordinate types valid for a window coordinate
 * \ingroup GuiQt
 *
 * Annotations support several different coordinate systems.  This class
 * contains validity for each these different coordinate systems.
 */

/**
 * Constructor.
 */
AnnotationCoordinateInformation::AnnotationCoordinateInformation()
{
    reset();
}

/**
 * Destructor.
 */
AnnotationCoordinateInformation::~AnnotationCoordinateInformation()
{
}

/**
 * Reset all coordinate information to invalid.
 */
void
AnnotationCoordinateInformation::reset() {
    m_modelXYZValid    = false;
    m_surfaceNodeValid = false;
    m_surfaceStructure = StructureEnum::INVALID;
    m_surfaceNumberOfNodes = 0;
    m_surfaceNodeIndex = -1;
    m_surfaceNodeOffset = 0.0;
    m_surfaceNodeVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    m_tabIndex         = -1;
    m_tabWidth         = 0;
    m_tabHeight        = 0;
    m_windowIndex      = -1;
    m_windowWidth      = 0;
    m_windowHeight     = 0;
    m_modelXYZ[0]  = 0.0;
    m_modelXYZ[1]  = 0.0;
    m_modelXYZ[2]  = 0.0;
    m_tabXYZ[0]    = 0.0;
    m_tabXYZ[1]    = 0.0;
    m_tabXYZ[2]    = 0.0;
    m_windowXYZ[0] = 0.0;
    m_windowXYZ[1] = 0.0;
    m_windowXYZ[2] = 0.0;
}
