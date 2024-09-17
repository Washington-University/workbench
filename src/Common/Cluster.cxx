
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
using namespace caret;


    
/**
 * \class caret::Cluster 
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
Cluster::Cluster(const AString& name,
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
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Cluster::copyHelperCluster(const Cluster& obj)
{
    m_name                   = obj.m_name;
    m_key                    = obj.m_key;
    m_centerOfGravityXYZ     = obj.m_centerOfGravityXYZ;
    m_numberOfBrainordinates = obj.m_numberOfBrainordinates;
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
    return m_centerOfGravityXYZ;
}

/**
 * @return Number of brainordinates in cluster
 */
int64_t
Cluster::getNumberOfBrainordinates() const
{
    return m_numberOfBrainordinates;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Cluster::toString() const
{
    AString text;
    text.append("Name=" + m_name);
    text.append(" Key=" + AString::number(m_key));
    text.append(" COG=" + m_centerOfGravityXYZ.toString());
    text.append(" Number of Brainordinates=" + AString::number(m_numberOfBrainordinates));
    return text;
}

