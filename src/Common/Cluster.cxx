
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
Cluster::typeToTypeName(const Type type)
{
    AString name("invalid");
    switch (type) {
        case Type::NONE:
            name = "None";
            break;
        case Type::ALL:
            name = "All";
            break;
        case Type::LEFT:
            name = "Left";
            break;
        case Type::RIGHT:
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
m_type(Type::NONE),
m_name(""),
m_mergedName(""),
m_key(-1),
m_numberOfBrainordinates(0)
{
    
}

/**
 * Constructor for a cluster with type NONE
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
m_type(Type::NONE),
m_name(name),
m_mergedName(""),
m_key(key),
m_centerOfGravityXYZ(centerOfGravityXYZ),
m_numberOfBrainordinates(numberOfBrainordinates)
{
    
}

/**
 * Constructor for a merged cluster
 * @param type
 *    Type of the cluster
 * @param name
 *    Name of cluster
 * @param key
 *    Index of cluster (such as a label index)
 * @param centerOfGravityXYZ
 *    Center of gravity of the cluster
 * @param numberOfBrainordinates
 *    Number of brainordinates in the cluster
 */
Cluster::Cluster(const Type type,
                 const AString& name,
                 const int32_t key,
                 const Vector3D& centerOfGravityXYZ,
                 const int64_t numberOfBrainordinates)
: CaretObject(),
m_type(type),
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
 * @return True if this instance is valid (valid is one or more brainordinates)
 */
bool
Cluster::isValid() const
{
    return (m_numberOfBrainordinates > 0);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
Cluster::copyHelperCluster(const Cluster& obj)
{
    m_type                   = obj.m_type;
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
 * @return Type of the cluster
 */
Cluster::Type
Cluster::getType() const
{
    return m_type;
}

/**
 * @return Name of type of the cluster
 */
AString
Cluster::getTypeName() const
{
    return Cluster::typeToTypeName(m_type);
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
    text.append(" Type=" + typeToTypeName(m_type));
    text.append(" Key=" + AString::number(m_key));
    text.append(" COG=" + m_centerOfGravityXYZ.toString());
    text.append(" Number of Brainordinates=" + AString::number(m_numberOfBrainordinates));
    return text;
}

/**
 * Merge the given clusters by sign of X-coordinate from COG into
 * All, Left, and Right clusters.
 * @param clusters
 *    Clusters to merge.
 * @return
 *   Vector containing, in order, all left, right, merged clusters (not all may be present)
 */
std::vector<Cluster>
Cluster::mergeClustersByCogSignOfX(const std::vector<const Cluster*>& clusters)
{
    std::vector<Cluster> clustersOut;
    
    const int32_t numClusters(clusters.size());
    if (numClusters == 0) {
        return clustersOut;
    }

    Vector3D allCog, leftCog, rightCog;
    float    allCount(0.0), leftCount(0.0), rightCount(0.0);
    for (const auto* c : clusters) {
        const Vector3D cog(c->getCenterOfGravityXYZ());
        const float count(c->getNumberOfBrainordinates());
        if (cog[0] > 0.0) {
            rightCog += (cog * count);
            rightCount += count;
        }
        else {
            leftCog += (cog * count);
            leftCount += count;
        }
    }
    
    CaretAssertVectorIndex(clusters, 0);
    const AString name(clusters[0]->getName());
    const int32_t key(clusters[0]->getKey());
    
    if (leftCount >= 1.0) {
        allCog   += leftCog;
        allCount += leftCount;
        clustersOut.emplace_back(Type::LEFT,
                                 name,
                                 key,
                                 (leftCog / leftCount),
                                 static_cast<int64_t>(leftCount));
    }
    
    if (rightCount >= 1.0) {
        allCog   += rightCog;
        allCount += rightCount;
        clustersOut.emplace_back(Type::RIGHT,
                                 name,
                                 key,
                                 (rightCog / rightCount),
                                 static_cast<int64_t>(rightCount));
    }
    
    if (allCount>= 1.0) {
        clustersOut.emplace(clustersOut.begin(),
                            Type::ALL,
                            name,
                            key,
                            (allCog / allCount),
                            static_cast<int64_t>(allCount));
    }
    
    return clustersOut;
}

/**
 * Merge the given clusters for the given merge type
 * @param name
 *    Name for all output clusters
 * @param key
 *    Key for output clusters
 * @param clusters
 *    Clusters to merge.
 * @return
 *   Vector containing, in order, all left, right, merged clusters (not all may be present)
 */
std::vector<Cluster>
Cluster::mergeClustersByClusterType(const AString& name,
                                    const int32_t key,
                                    const std::vector<const Cluster*>& clusters,
                                    const bool allowNoneTypeFlag)
{
    std::vector<Cluster> clustersOut;
    
    
    const int32_t numClusters(clusters.size());
    if (numClusters == 0) {
        return clustersOut;
    }
    
    Vector3D noneCog, allCog, leftCog, rightCog;
    float    noneCount(0), allCount(0.0), leftCount(0.0), rightCount(0.0);
    
    /*
     * Temporary until a better classification for cluster bilateral can
     * be added.  If there is one cluster that is bilateral such as
     * the superior colliculus.
     */
    const bool allOnlyFlag(true);
    
    for (const auto* c : clusters) {
        const Vector3D cog(c->getCenterOfGravityXYZ());
        const float count(c->getNumberOfBrainordinates());
        if (allOnlyFlag) {
            /*
             * ALL includes left and right so don't use ALL
             */
            if (c->getType() != Type::ALL) {
                allCog   += (cog * count);
                allCount += count;
            }
        }
        else {
            switch (c->getType()) {
                case Type::NONE:
                    noneCog   += (cog * count);
                    noneCount += count;
                    break;
                case Type::ALL:
                    //                allCog   += (cog * count);
                    //                allCount += count;
                    break;
                case Type::LEFT:
                    allCog   += (cog * count);
                    allCount += count;
                    
                    leftCog   += (cog * count);
                    leftCount += count;
                    break;
                case Type::RIGHT:
                    allCog   += (cog * count);
                    allCount += count;
                    
                    rightCog   += (cog * count);
                    rightCount += count;
                    break;
            }
        }
    }

    if (noneCount >= 1.0) {
        if (allowNoneTypeFlag) {
            clustersOut.emplace_back(Type::NONE,
                                     name,
                                     key,
                                     (noneCog / noneCount),
                                     static_cast<int64_t>(noneCount));
        }
    }
    
    if (allCount >= 1.0) {
        clustersOut.emplace_back(Type::ALL,
                                 name,
                                 key,
                                 (allCog / allCount),
                                 static_cast<int64_t>(allCount));
    }
    
    if (leftCount >= 1.0) {
        clustersOut.emplace_back(Type::LEFT,
                                 name,
                                 key,
                                 (leftCog / leftCount),
                                 static_cast<int64_t>(leftCount));
    }
    
    if (rightCount >= 1.0) {
        clustersOut.emplace_back(Type::RIGHT,
                                 name,
                                 key,
                                 (rightCog / rightCount),
                                 static_cast<int64_t>(rightCount));
    }
    
    return clustersOut;
}
