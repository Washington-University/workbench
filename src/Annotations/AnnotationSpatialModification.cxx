
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

#define __ANNOTATION_SPATIAL_MODIFICATION_DECLARE__
#include "AnnotationSpatialModification.h"
#undef __ANNOTATION_SPATIAL_MODIFICATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationSpatialModification 
 * \brief Contains information for spatially modifying an annotation.
 * \ingroup Annotations
 */

/**
 * Apply a move or resize operation received from the GUI.
 *
 * @param handleSelected
 *     Annotatoion handle that is being processed by the user.
 * @param viewportWidth
 *     Width of viewport
 * @param viewportHeight
 *     Height of viewport
 * @param mousePressX
 *     Mouse pressed X-coordinate.
 * @param mousePressY
 *     Mouse pressed Y-coordinate.
 * @param mouseX
 *     Mouse X-coordinate.
 * @param mouseY
 *     Mouse Y-coordinate.
 * @param mouseDX
 *     Change in mouse X-coordinate.
 * @param mouseDY
 *     Change in mouse Y-coordinate.
 * @param polyLineCoordinateIndex
 *     Index of poly line coordinate
 * @param startOfDraggingFlag
 *     True when user starts to drag mouse.
 */
AnnotationSpatialModification::AnnotationSpatialModification(const AnnotationSizingHandleTypeEnum::Enum sizingHandleType,
                                                             const float viewportWidth,
                                                             const float viewportHeight,
                                                             const float mousePressX,
                                                             const float mousePressY,
                                                             const float mouseX,
                                                             const float mouseY,
                                                             const float mouseDX,
                                                             const float mouseDY,
                                                             const int32_t polyLineCoordinateIndex,
                                                             const bool  startOfDraggingFlag)
: CaretObject(),
m_sizingHandleType(sizingHandleType),
m_viewportWidth(viewportWidth),
m_viewportHeight(viewportHeight),
m_mousePressX(mousePressX),
m_mousePressY(mousePressY),
m_mouseX(mouseX),
m_mouseY(mouseY),
m_mouseDX(mouseDX),
m_mouseDY(mouseDY),
m_polyLineCoordinateIndex(polyLineCoordinateIndex),
m_startOfDraggingFlag(startOfDraggingFlag)
{
    m_chartCoordAtMouseXY.m_chartXYZValid  = false;
    m_chartCoordAtPreviousMouseXY.m_chartXYZValid = false;
    m_stereotaxicCoordinateAtMouseXY.m_stereotaxicValid  = false;
    m_surfaceCoordinateAtMouseXY.m_surfaceNodeValid = false;
    m_histologyCoordAtMouseXY.m_histologyXYZValid = false;
    m_histologyCoordAtPreviousMouseXY.m_histologyXYZValid = false;
    m_mediaCoordAtMouseXY.m_mediaXYZValid = false;
    m_mediaCoordAtPreviousMouseXY.m_mediaXYZValid = false;
}

/**
 * Destructor.
 */
AnnotationSpatialModification::~AnnotationSpatialModification()
{
}

/**
 * Set the surface coordinate at mouse X/Y.
 *
 * @param structure
 *     The surface structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in the surface.
 * @param surfaceNodeIndex
 *     Surface node index.
 */
void
AnnotationSpatialModification::setSurfaceCoordinateAtMouseXY(const StructureEnum::Enum structure,
                                                             const int32_t surfaceNumberOfNodes,
                                                             const int32_t surfaceNodeIndex)
{
    m_surfaceCoordinateAtMouseXY.m_surfaceStructure     = structure;
    m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes = surfaceNumberOfNodes;
    m_surfaceCoordinateAtMouseXY.m_surfaceNodeIndex     = surfaceNodeIndex;
    m_surfaceCoordinateAtMouseXY.m_surfaceNodeValid     = false;
    if ((m_surfaceCoordinateAtMouseXY.m_surfaceStructure != StructureEnum::INVALID)
        && (m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes > 0)
        && (m_surfaceCoordinateAtMouseXY.m_surfaceNodeIndex >= 0)
        && (m_surfaceCoordinateAtMouseXY.m_surfaceNodeIndex < m_surfaceCoordinateAtMouseXY.m_surfaceNumberOfNodes)) {
        m_surfaceCoordinateAtMouseXY.m_surfaceNodeValid = true;
    }
}

/**
 * Set the stereotaxic coordinate at mouse X/Y
 *
 * @param stereotaxicX
 *     stereotaxic X-coordinate.
 * @param stereotaxicY
 *     stereotaxic Y-coordinate.
 * @param stereotaxicZ
 *     stereotaxic Z-coordinate.
 */
void
AnnotationSpatialModification::setStereotaxicCoordinateAtMouseXY(const float stereotaxicX,
                                                                 const float stereotaxicY,
                                                                 const float stereotaxicZ)
{
    m_stereotaxicCoordinateAtMouseXY.m_stereotaxicXYZ[0] = stereotaxicX;
    m_stereotaxicCoordinateAtMouseXY.m_stereotaxicXYZ[1] = stereotaxicY;
    m_stereotaxicCoordinateAtMouseXY.m_stereotaxicXYZ[2] = stereotaxicZ;
    m_stereotaxicCoordinateAtMouseXY.m_stereotaxicValid  = true;
}

/**
 * Set the chart coordinate at mouse X/Y
 *
 * @param chartX
 *     chart X-coordinate.
 * @param chartY
 *     chart Y-coordinate.
 * @param chartZ
 *     chart Z-coordinate.
 */
void
AnnotationSpatialModification::setChartCoordinateAtMouseXY(const float chartX,
                                                                 const float chartY,
                                                                 const float chartZ)
{
    m_chartCoordAtMouseXY.m_chartXYZ[0] = chartX;
    m_chartCoordAtMouseXY.m_chartXYZ[1] = chartY;
    m_chartCoordAtMouseXY.m_chartXYZ[2] = chartZ;
    m_chartCoordAtMouseXY.m_chartXYZValid  = true;
}

/**
 * Set the chart coordinate at previous mouse X/Y
 *
 * @param chartX
 *     chart X-coordinate.
 * @param chartY
 *     chart Y-coordinate.
 * @param chartZ
 *     chart Z-coordinate.
 */
void
AnnotationSpatialModification::setChartCoordinateAtPreviousMouseXY(const float chartX,
                                                                   const float chartY,
                                                                   const float chartZ)
{
    m_chartCoordAtPreviousMouseXY.m_chartXYZ[0] = chartX;
    m_chartCoordAtPreviousMouseXY.m_chartXYZ[1] = chartY;
    m_chartCoordAtPreviousMouseXY.m_chartXYZ[2] = chartZ;
    m_chartCoordAtPreviousMouseXY.m_chartXYZValid  = true;
}

/**
 * Set the media coordinate at mouse X/Y
 *
 * @param mediaX
 *     media X-coordinate.
 * @param mediaY
 *     media Y-coordinate.
 * @param mediaZ
 *     media Z-coordinate.
 */
void
AnnotationSpatialModification::setMediaCoordinateAtMouseXY(const float mediaX,
                                                           const float mediaY,
                                                           const float mediaZ)
{
    m_mediaCoordAtMouseXY.m_mediaXYZ[0] = mediaX;
    m_mediaCoordAtMouseXY.m_mediaXYZ[1] = mediaY;
    m_mediaCoordAtMouseXY.m_mediaXYZ[2] = mediaZ;
    m_mediaCoordAtMouseXY.m_mediaXYZValid  = true;
}

/**
 * Set the media previous coordinate at mouse X/Y
 *
 * @param mediaX
 *     media X-coordinate.
 * @param mediaY
 *     media Y-coordinate.
 * @param mediaZ
 *     media Z-coordinate.
 */
void
AnnotationSpatialModification::setMediaCoordinateAtPreviousMouseXY(const float mediaX,
                                                                   const float mediaY,
                                                                   const float mediaZ)
{
    m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[0] = mediaX;
    m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[1] = mediaY;
    m_mediaCoordAtPreviousMouseXY.m_mediaXYZ[2] = mediaZ;
    m_mediaCoordAtPreviousMouseXY.m_mediaXYZValid  = true;
}


/**
 * Set the histology coordinate at mouse X/Y
 *
 * @param histologyX
 *     histology X-coordinate.
 * @param histologyY
 *     histology Y-coordinate.
 * @param histologyZ
 *     histology Z-coordinate.
 */
void
AnnotationSpatialModification::setHistologyCoordinateAtMouseXY(const float histologyX,
                                                               const float histologyY,
                                                               const float histologyZ)
{
    m_histologyCoordAtMouseXY.m_histologyXYZ[0] = histologyX;
    m_histologyCoordAtMouseXY.m_histologyXYZ[1] = histologyY;
    m_histologyCoordAtMouseXY.m_histologyXYZ[2] = histologyZ;
    m_histologyCoordAtMouseXY.m_histologyXYZValid  = true;
}

/**
 * Set the previous histology coordinate at mouse X/Y
 *
 * @param histologyX
 *     histology X-coordinate.
 * @param histologyY
 *     histology Y-coordinate.
 * @param histologyZ
 *     histology Z-coordinate.
 */
void
AnnotationSpatialModification::setHistologyCoordinateAtPreviousMouseXY(const float histologyX,
                                                                       const float histologyY,
                                                                       const float histologyZ)
{
    m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[0] = histologyX;
    m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[1] = histologyY;
    m_histologyCoordAtPreviousMouseXY.m_histologyXYZ[2] = histologyZ;
    m_histologyCoordAtPreviousMouseXY.m_histologyXYZValid  = true;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationSpatialModification::toString() const
{
    return "AnnotationSpatialModification";
}

