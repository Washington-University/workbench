
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

#define __CONTROL_POINT3_D_DECLARE__
#include "ControlPoint3D.h"
#undef __CONTROL_POINT3_D_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

using namespace caret;


    
/**
 * \class caret::ControlPoint3D 
 * \brief 3D control point with source and target coordinates.
 * \ingroup Common
 */

/**
 * Constructor.
 *
 * @param sourceXYZ
 *     The source coordinate.
 * @param targetXYZ
 *     The target coordinate.
 */
ControlPoint3D::ControlPoint3D(const float sourceXYZ[3],
                               const float targetXYZ[3])
: CaretObjectTracksModification(),
m_sourceX(sourceXYZ[0]),
m_sourceY(sourceXYZ[1]),
m_sourceZ(sourceXYZ[2]),
m_targetX(targetXYZ[0]),
m_targetY(targetXYZ[1]),
m_targetZ(targetXYZ[2])
{
    setModified();
}

/**
 * Constructor.
 *
 * @param sourceX
 *     The source X-coordinate.
 * @param sourceY
 *     The source Y-coordinate.
 * @param sourceZ
 *     The source Z-coordinate.
 * @param targetX
 *     The target X-coordinate.
 * @param targetY
 *     The target Y-coordinate.
 * @param targetZ
 *     The target Z-coordinate.
 */
ControlPoint3D::ControlPoint3D(const float sourceX,
                               const float sourceY,
                               const float sourceZ,
                               const float targetX,
                               const float targetY,
                               const float targetZ)
: CaretObjectTracksModification(),
m_sourceX(sourceX),
m_sourceY(sourceY),
m_sourceZ(sourceZ),
m_targetX(targetX),
m_targetY(targetY),
m_targetZ(targetZ)
{
    setModified();
}


/**
 * Destructor.
 */
ControlPoint3D::~ControlPoint3D()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ControlPoint3D::ControlPoint3D(const ControlPoint3D& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperControlPoint3D(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ControlPoint3D&
ControlPoint3D::operator=(const ControlPoint3D& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperControlPoint3D(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ControlPoint3D::copyHelperControlPoint3D(const ControlPoint3D& obj)
{
    m_sourceX = obj.m_sourceX;
    m_sourceY = obj.m_sourceY;
    m_sourceZ = obj.m_sourceZ;
    
    m_targetX = obj.m_targetX;
    m_targetY = obj.m_targetY;
    m_targetZ = obj.m_targetZ;

    setModified();
}

/**
 * Get the source coordinate.
 *
 * @param pt
 *     Output with source coordinate.
 */
void
ControlPoint3D::getSourceXYZ(double pt[3]) const
{
    pt[0] = m_sourceX;
    pt[1] = m_sourceY;
    pt[2] = m_sourceZ;
}

/**
 * Get the target coordinate.
 *
 * @param pt
 *     Output with target coordinate.
 */
void
ControlPoint3D::getTargetXYZ(double pt[3]) const
{
    pt[0] = m_targetX;
    pt[1] = m_targetY;
    pt[2] = m_targetZ;
}

/**
 * Get the source coordinate.
 *
 * @param pt
 *     Output with source coordinate.
 */
void
ControlPoint3D::getSourceXYZ(float pt[3]) const
{
    pt[0] = m_sourceX;
    pt[1] = m_sourceY;
    pt[2] = m_sourceZ;
}

/**
 * Get the target coordinate.
 *
 * @param pt
 *     Output with target coordinate.
 */
void
ControlPoint3D::getTargetXYZ(float pt[3]) const
{
    pt[0] = m_targetX;
    pt[1] = m_targetY;
    pt[2] = m_targetZ;
}

/** 
 * @return The source X-coordinate
 */
float
ControlPoint3D::getSourceX() const
{
    return m_sourceX;
}

/**
 * @return The source Y-coordinate
 */
float ControlPoint3D::getSourceY() const
{
    return m_sourceY;
}

/** 
 * @return The source Z-coordinate 
 */
float ControlPoint3D::getSourceZ() const
{
    return m_sourceZ;
}

/** 
 * @return The target X-coordinate 
 */
float ControlPoint3D::getTargetX() const
{
    return m_targetX;
}

/**
 * @return The target Y-coordinate
 */
float ControlPoint3D::getTargetY() const
{
    return m_targetY;
}

/** @return 
 * The target Z-coordinate
 */
float ControlPoint3D::getTargetZ() const
{
    return m_targetZ;
}

/**
 * @return String containing control point coordinates.
 */
AString
ControlPoint3D::toString() const
{
    const AString s("Source: ("
                    + AString::number(m_sourceX)
                    + ", " + AString::number(m_sourceY)
                    + ", " + AString::number(m_sourceZ)
                    + ")  Target: ("
                    + AString::number(m_targetX)
                    + ", " + AString::number(m_targetY)
                    + ", " + AString::number(m_targetZ)
                    + ")");
    return s;
}

/**
 * Get the normal vector for the source coordinates of the first three
 * control points.  If there are less than three control points,
 * a unit vector is returned.
 *
 * @param controlPoints
 *     The control points.
 * @param sourceNormalVectorOut
 *     Output unit normal vector for source points.
 */
void
ControlPoint3D::getSourceNormalVector(const std::vector<ControlPoint3D>& controlPoints,
                                      float sourceNormalVectorOut[3])
{
    if (controlPoints.size() < 3) {
        sourceNormalVectorOut[0] = 0.0;
        sourceNormalVectorOut[1] = 0.0;
        sourceNormalVectorOut[2] = 1.0;
        CaretLogSevere("Cannot compute normal vector for fewer than three control points.");
    }
    
    CaretAssertVectorIndex(controlPoints, 2);
    
    float s1[3];
    controlPoints[0].getSourceXYZ(s1);
    float s2[3];
    controlPoints[1].getSourceXYZ(s2);
    float s3[3];
    controlPoints[2].getSourceXYZ(s3);
    MathFunctions::normalVector(s1, s2, s3, sourceNormalVectorOut);
}


