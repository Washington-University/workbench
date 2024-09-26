
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

#define __CLUSTER_DECLARE__
#include "Cluster.h"
#undef __CLUSTER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;

/**
 * \class caret::Cluster 
 * \brief Data for a brainordinate cluster
 * \ingroup Common
 */

/**
 * @return string name for the given type
 * @param type
 *    The type
 */
AString 
Cluster::locationTypeToName(const LocationType type)
{
    AString name("invalid");
    switch (type) {
        case LocationType::UNKNOWN:
            name = "Unknown";
            break;
        case LocationType::CENTRAL:
            name = "Central";
            break;
        case LocationType::LEFT:
            name = "Left";
            break;
        case LocationType::RIGHT:
            name = "Right";
            break;
    }
    return name;
}

/**
 * Default constructor creates an invalid cluster.
 */
Cluster::Cluster()
: CaretObject(),
m_location(LocationType::UNKNOWN),
m_name(""),
m_key(-1)
{
    
}

/**
 * Constructor for a cluster with type NONE
 * @param name
 *    Name of cluster
 * @param key
 *    Index of cluster (such as a label index)
 * @param coordinatesXYZ
 *    Coordinates in the cluster.
 */
Cluster::Cluster(const AString& name,
                 const int32_t key,
                 const std::vector<Vector3D>& coordinatesXYZ)
: CaretObject(),
m_location(LocationType::UNKNOWN),
m_name(name),
m_key(key),
m_coordinateXYZ(coordinatesXYZ)
{
    
}

/**
 * Destructor.
 */
Cluster::~Cluster()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
Cluster::Cluster(const Cluster& obj)
: CaretObject(obj)
{
    this->copyHelperCluster(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
Cluster&
Cluster::operator=(const Cluster& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCluster(obj);
    }
    return *this;    
}

/**
 * @return True if this instance is valid (valid is one or more brainordinates)
 */
bool
Cluster::isValid() const
{
    return ( ! m_coordinateXYZ.empty());
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Cluster::copyHelperCluster(const Cluster& obj)
{
    m_location               = obj.m_location;
    m_name                   = obj.m_name;
    m_key                    = obj.m_key;
    m_coordinateXYZ          = obj.m_coordinateXYZ;
    m_centerOfGravityXYZ     = obj.m_centerOfGravityXYZ;
}

/**
 * @return Name of cluster
 */
AString
Cluster::getName() const
{
    return m_name;
}

/**
 * @return Location of the cluster
 */
Cluster::LocationType
Cluster::getLocationType() const
{
    computeCenterOfGravityAndLocation();
    return m_location;
}

/**
 * @return Name of location type of the cluster
 */
AString
Cluster::getLocationTypeName() const
{
    return Cluster::locationTypeToName(m_location);
}

/**
 * @return Key for cluster
 */
int32_t
Cluster::getKey() const
{
    return m_key;
}

/**
 * @return Center of gravity for cluster
 */
Vector3D
Cluster::getCenterOfGravityXYZ() const
{
    computeCenterOfGravityAndLocation();
    return m_centerOfGravityXYZ;
}

/**
 * @return Number of brainordinates in cluster
 */
int64_t
Cluster::getNumberOfBrainordinates() const
{
    return m_coordinateXYZ.size();
}

/**
 * Add a coordinate to this cluster.
 * @param coordinateXYZ
 *    Coordinate added to this cluster
 */
void
Cluster::addCoordinate(const Vector3D& coordinateXYZ)
{
    m_coordinateXYZ.push_back(coordinateXYZ);
    invalidateCenterOfGravityAndLocation();
}

/**
 * @return Number of coordinates in this cluster
 */
const Vector3D&
Cluster::getCoordinate(const int32_t index) const
{
    CaretAssertVectorIndex(m_coordinateXYZ, index);
    return m_coordinateXYZ[index];
}

/**
 * Merge the coordinates from the given cluster with this cluster's coordinates
 */
void
Cluster::mergeCoordinates(const Cluster& cluster)
{
    m_coordinateXYZ.insert(m_coordinateXYZ.end(),
                           cluster.m_coordinateXYZ.begin(), cluster.m_coordinateXYZ.end());
}


/**
 * Invalidate center-of-gravity and location of cluster.
 */
void
Cluster::invalidateCenterOfGravityAndLocation()
{
    m_centerOfGravityAndLocationValidFlag = false;
}

/**
 * Compute center-of-gravity and location of cluster.
 */
void
Cluster::computeCenterOfGravityAndLocation() const
{
    if (m_centerOfGravityAndLocationValidFlag) {
        return;
    }
    
    m_centerOfGravityXYZ.fill(0.0);
    m_location = LocationType::UNKNOWN;
    const float numCoords(m_coordinateXYZ.size());
    if (numCoords < 1.0) {
        return;
    }
    
    float numLeft(0.0);
    float numRight(0.0);
    float numZero(0.0);
    
    Vector3D sumXYZ;
    for (const Vector3D& xyz : m_coordinateXYZ) {
        sumXYZ += xyz;
        
        /*
         * At 0.0 is neither left nor right
         */
        if (xyz[0] > 0.0) {
            numRight += 1.0;
        }
        else if (xyz[0] < 0.0){
            numLeft += 1.0;
        }
        else {
            numZero += 1.0;
        }
    }
    
    CaretAssert(numCoords >= 1.0);
    m_centerOfGravityXYZ = (sumXYZ / numCoords);
    
    /*
     * Estimate if cluster is left, right, central
     * using percentage of nodes with negative and
     * postive X-coordinate
     */
    const float numLeftRight(numLeft + numRight);
    if (numLeftRight >= 1.0) {
        const float leftPercent((numLeft / numLeftRight) * 100.0);
        const float rightPercent((numRight / numLeftRight) * 100.0);
        
        const float centralPercentMinimim(40.0);
        const float centralPercentMaximum(60.0);
        if ((leftPercent >= centralPercentMinimim)
            && (leftPercent <= centralPercentMaximum)
            && (rightPercent >= centralPercentMinimim)
            && (rightPercent <= centralPercentMaximum)) {
            m_location = LocationType::CENTRAL;
        }
        else if (leftPercent > rightPercent) {
            m_location = LocationType::LEFT;
        }
        else {
            m_location = LocationType::RIGHT;
        }

        if (m_location == LocationType::UNKNOWN) {
            const AString msg("Cluster \""
                              + getName()
                              + "\" " + AString::number(m_coordinateXYZ.size()) + " brainordinates"
                              " COG: " + m_centerOfGravityXYZ.toString()
                              + " location is UNKNOWN "
                              + " Percent Left=" + AString::number(leftPercent)
                              + " Percent Right=" + AString::number(rightPercent));
            CaretLogWarning(msg);
        }
    }
    else if (numZero >= 1.0) {
        /*
         * Cluster has all coordinates at x==0.0
         */
        m_location = LocationType::CENTRAL;
    }
    
    m_centerOfGravityAndLocationValidFlag = true;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Cluster::toString() const
{
    computeCenterOfGravityAndLocation(); /* ensure cog and type have been calculated */
    
    AString text;
    text.append("Name=" + m_name);
    text.append(" LocationType=" + getLocationTypeName());
    text.append(" Key=" + AString::number(m_key));
    text.append(" COG=" + m_centerOfGravityXYZ.toString());
    text.append(" Number of Brainordinates=" + AString::number(getNumberOfBrainordinates()));
    return text;
}

