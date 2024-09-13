
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __BRAINORDINATE_CLUSTER_DECLARE__
#include "BrainordinateCluster.h"
#undef __BRAINORDINATE_CLUSTER_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BrainordinateCluster 
 * \brief Data for a brainordinate cluster
 * \ingroup Common
 */

/**
 * Constructor.
 * @param name
 *    Name of cluster
 * @param key
 *    Index of cluster (such as a label index)
 * @param centerOfGravityXYZ
 *    Center of gravity of the cluster
 * @param numberOfBrainordinates
 *    Number of brainordinates in the cluster
 */
BrainordinateCluster::BrainordinateCluster(const AString& name,
                                           const int32_t key,
                                           const Vector3D& centerOfGravityXYZ,
                                           const int64_t numberOfBrainordinates)
: CaretObject(),
m_name(name),
m_key(key),
m_centerOfGravityXYZ(centerOfGravityXYZ),
m_numberOfBrainordinates(numberOfBrainordinates)
{
    
}

/**
 * Destructor.
 */
BrainordinateCluster::~BrainordinateCluster()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainordinateCluster::BrainordinateCluster(const BrainordinateCluster& obj)
: CaretObject(obj)
{
    this->copyHelperBrainordinateCluster(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BrainordinateCluster&
BrainordinateCluster::operator=(const BrainordinateCluster& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBrainordinateCluster(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BrainordinateCluster::copyHelperBrainordinateCluster(const BrainordinateCluster& obj)
{
    m_name                   = obj.m_name;
    m_key                    = obj.m_key;
    m_centerOfGravityXYZ     = obj.m_centerOfGravityXYZ;
    m_numberOfBrainordinates = obj.m_numberOfBrainordinates;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
BrainordinateCluster::operator<(const BrainordinateCluster& obj) const
{
    if (m_name < obj.m_name) {
        return true;
    }
    else if (m_name > obj.m_name) {
        return false;
    }
    
    if (m_key < obj.m_key) {
        return true;
    }
    else if (m_key > obj.m_key) {
        return false;
    }
    
    if (m_numberOfBrainordinates < obj.m_numberOfBrainordinates) {
        return true;
    }
    else if (m_numberOfBrainordinates > obj.m_numberOfBrainordinates) {
        return false;
    }
    
    return (m_centerOfGravityXYZ < obj.m_centerOfGravityXYZ);
}

/**
 * @return Name of cluster
 */
AString
BrainordinateCluster::getName() const
{
    return m_name;
}

/**
 * @return Key for cluster
 */
int32_t
BrainordinateCluster::getKey() const
{
    return m_key;
}

/**
 * @return Center of gravity for cluster
 */
Vector3D
BrainordinateCluster::getCenterOfGravityXYZ() const
{
    return m_centerOfGravityXYZ;
}

/**
 * @return Number of brainordinates in cluster
 */
int64_t
BrainordinateCluster::getNumberOfBrainordinates() const
{
    return m_numberOfBrainordinates;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainordinateCluster::toString() const
{
    AString text;
    text.append("Name=" + m_name);
    text.append(" Key=" + AString::number(m_key));
    text.append(" COG=" + m_centerOfGravityXYZ.toString());
    text.append(" Number of Brainordinates=" + AString::number(m_numberOfBrainordinates));
    return text;
}

