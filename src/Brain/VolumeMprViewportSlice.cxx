
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

#define __VOLUME_MPR_VIEWPORT_SLICE_DECLARE__
#include "VolumeMprViewportSlice.h"
#undef __VOLUME_MPR_VIEWPORT_SLICE_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"

using namespace caret;


    
/**
 * \class caret::VolumeMprViewportSlice
 * \brief Information about slice mapped to/from viewport
 * \ingroup Brain
 */

/**
 * Constructor.
 */
VolumeMprViewportSlice::VolumeMprViewportSlice(const GraphicsViewport& viewport,
                                   const Plane& plane)
: CaretObject(),
m_viewport(viewport),
m_plane(plane)
{
    /*
     * Transform selected coordinates to viewport X/Y
     */
    EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(transformEvent.getPointer());
    
    m_bottomLeftXYZ  = mapToSlicePlane(transformEvent,
                                       m_viewport.getBottomLeft());
    m_bottomRightXYZ = mapToSlicePlane(transformEvent,
                                       m_viewport.getBottomRight());
    m_topRightXYZ    = mapToSlicePlane(transformEvent,
                                       m_viewport.getTopRight());
    m_topLeftXYZ     = mapToSlicePlane(transformEvent,
                                       m_viewport.getTopLeft());

    m_triangleStripXYZ.push_back(m_topLeftXYZ);
    m_triangleStripXYZ.push_back(m_bottomLeftXYZ);
    m_triangleStripXYZ.push_back(m_topRightXYZ);
    m_triangleStripXYZ.push_back(m_bottomRightXYZ);
}

/**
 * Destructor.
 */
VolumeMprViewportSlice::~VolumeMprViewportSlice()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
VolumeMprViewportSlice::VolumeMprViewportSlice(const VolumeMprViewportSlice& obj)
: CaretObject(obj)
{
    this->copyHelperVolumeMprViewportSlice(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
VolumeMprViewportSlice&
VolumeMprViewportSlice::operator=(const VolumeMprViewportSlice& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperVolumeMprViewportSlice(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
VolumeMprViewportSlice::copyHelperVolumeMprViewportSlice(const VolumeMprViewportSlice& obj)
{
    m_viewport = obj.m_viewport;
    m_plane    = obj.m_plane;
    
    m_bottomLeftXYZ  = obj.m_bottomLeftXYZ;
    m_bottomRightXYZ = obj.m_bottomRightXYZ;
    m_topRightXYZ    = obj.m_topRightXYZ;
    m_topLeftXYZ     = obj.m_topLeftXYZ;
    
    m_triangleStripXYZ = obj.m_triangleStripXYZ;
}

Vector3D
VolumeMprViewportSlice::mapWindowXyzToSliceXYZ(const Vector3D& windowXYZ) const
{
    /*
     * Transforms model <=> window
     * Instance used in constructor is not member so we must
     * create an instance here
     */
    EventOpenGLObjectToWindowTransform transformEvent(EventOpenGLObjectToWindowTransform::SpaceType::MODEL);
    EventManager::get()->sendEvent(transformEvent.getPointer());
    
    Vector3D slicePlaneXYZ  = mapToSlicePlane(transformEvent,
                                              windowXYZ);
    return slicePlaneXYZ;
}

/**
 * @return True if the window X, Y are inside the viewport for this slice
 * @param windowX
 *    The window X
 * @param windowX
 *    The window Y
 */
bool
VolumeMprViewportSlice::containsWindowXY(const int32_t windowX,
                                   const int32_t windowY) const
{
    return m_viewport.containsWindowXY(windowX, windowY);
}

/**
 * Map the window coordinate to a model coordinate and then project it to the slice plane
 * @param transformEvent
 *    Model to window transform
 * @param windowXYZ
 *    The window coordinate
 * @return
 *    Coordinate projected to the slice plane
 */
Vector3D
VolumeMprViewportSlice::mapToSlicePlane(const EventOpenGLObjectToWindowTransform& transformEvent,
                                  const Vector3D windowXYZ) const
{
    const Vector3D modelXYZ(transformEvent.inverseTransformPoint(windowXYZ));
    const Vector3D slicePlaneXYZ(m_plane.projectPointToPlane(modelXYZ));
    return slicePlaneXYZ;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeMprViewportSlice::toString() const
{
    return "VolumeMprViewportSlice";
}

/*
 * @return Vertices ordered for triangle strip
 */
std::vector<Vector3D>
VolumeMprViewportSlice::getTriangleStripXYZ() const
{
    return m_triangleStripXYZ;
}
