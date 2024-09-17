
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

#define __CLUSTER_CONTAINER_DECLARE__
#include "ClusterContainer.h"
#undef __CLUSTER_CONTAINER_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "StringTableModel.h"

using namespace caret;


    
/**
 * \class caret::ClusterContainer 
 * \brief Container for clusters
 * \ingroup Common
 */

/**
 * Constructor.
 */
ClusterContainer::ClusterContainer()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
ClusterContainer::~ClusterContainer()
{
}

/**
 * Add a cluster.
 * @param cluster
 *   Cluster to add.  Takes ownership of cluster and will eventually delete it.
 */
void
ClusterContainer::addCluster(Cluster* cluster)
{
    m_clusters.emplace_back(cluster);
    
    clearSortedContainers();
}

/**
 * Clear the clusters in this container.
 */
void
ClusterContainer::clear()
{
    clearSortedContainers();
    m_clusters.clear();
}

/**
 * Clear the sorted containers.
 */
void 
ClusterContainer::clearSortedContainers()
{
    m_mapWithClustersSortedByKey.clear();
    m_mapWithClustersSortedByName.clear();
    m_vectorClustersSortedByKey.clear();
    m_vectorClustersSortedByName.clear();
}

/**
 * @return ALL clusters sorted by key
 */
const std::vector<const Cluster*>&
ClusterContainer::getClustersSortedByKey() const
{
    createMultimapClustersSortedByKey();
    
    if (m_vectorClustersSortedByKey.empty()) {
        if ( ! m_mapWithClustersSortedByKey.empty()) {
            for (const auto& keyCluster : m_mapWithClustersSortedByKey) {
                m_vectorClustersSortedByKey.push_back(keyCluster.second);
            }
        }
    }
    return m_vectorClustersSortedByKey;
}

/**
 * Lazily initialize the multimap that sorts clusters by key
 */
void
ClusterContainer::createMultimapClustersSortedByKey() const
{
    if (m_mapWithClustersSortedByKey.empty()) {
        if ( ! m_clusters.empty()) {
            for (const auto& c : m_clusters) {
                m_mapWithClustersSortedByKey.insert(std::make_pair(c->m_key, c.get()));
            }
        }
    }
}

/**
 * @return ALL clusters sorted by name
 */
const std::vector<const Cluster*>&
ClusterContainer::getClustersSortedByName() const
{
    createMultimapClustersSortedByName();
    
    if (m_vectorClustersSortedByName.empty()) {
        if ( ! m_mapWithClustersSortedByName.empty()) {
            for (const auto& nameCluster : m_mapWithClustersSortedByName) {
                m_vectorClustersSortedByName.push_back(nameCluster.second);
            }
        }
    }
    return m_vectorClustersSortedByName;
}

/**
 * Lazily initialize the multimap that sorts clusters by name
 */
void
ClusterContainer::createMultimapClustersSortedByName() const
{
    if (m_mapWithClustersSortedByName.empty()) {
        if ( ! m_clusters.empty()) {
            for (const auto& c : m_clusters) {
                m_mapWithClustersSortedByName.insert(std::make_pair(c->m_name, c.get()));
            }
        }
    }
}

/**
 * @return All clusters with the given key
 * @param key
 *    Key for requested clusters
 */
std::vector<const Cluster*>
ClusterContainer::getClustersWithKey(const int32_t key) const
{
    createMultimapClustersSortedByKey();
    
    std::pair<MultimapSortedByKeyIterator, MultimapSortedByKeyIterator> matches = m_mapWithClustersSortedByKey.equal_range(key);
    std::vector<const Cluster*> clustersOut;
    for (MultimapSortedByKeyIterator iter = matches.first;
         iter != matches.second;
         iter++) {
        clustersOut.push_back(iter->second);
    }
    return clustersOut;
}

/**
 * @return All clusters with the given name
 * @param name
 *    Name for requested clusters
 */
std::vector<const Cluster*>
ClusterContainer::getClustersWithName(const AString& name) const
{
    createMultimapClustersSortedByName();
    
    std::pair<MultimapSortedByNameIterator, MultimapSortedByNameIterator> matches = m_mapWithClustersSortedByName.equal_range(name);
    std::vector<const Cluster*> clustersOut;
    for (MultimapSortedByNameIterator iter = matches.first;
         iter != matches.second;
         iter++) {
        clustersOut.push_back(iter->second);
    }
    return clustersOut;
}

/**
 * Add a key that does not map to any cluster
 */
void
ClusterContainer::addKeyThatIsNotInAnyCluster(const int32_t key)
{
    m_keysThatAreNotInAnyClusters.insert(key);
}

/**
 * @return Keys that do not map to any clusters (do not map to brainordinates)
 */
std::set<int32_t>
ClusterContainer::getKeysThatAreNotInAnyClusters() const
{
    return m_keysThatAreNotInAnyClusters;
}


/**
 * @return The clusters in a formatted text string
 */
AString
ClusterContainer::getClustersInFormattedString() const
{
    const std::vector<const Cluster*> clusters(getClustersSortedByName());
    
    const int32_t numClusters(clusters.size());
    if (numClusters <= 0) {
        return "No clusters were found.";
    }
    
    StringTableModel stm(numClusters + 1, 6);
    stm.setColumnAlignment(0, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 0, "Key");
    stm.setColumnAlignment(1, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 1, "Count");
    stm.setColumnAlignment(2, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 2, "X");
    stm.setColumnAlignment(3, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 3, "Y");
    stm.setColumnAlignment(4, StringTableModel::ALIGN_RIGHT);
    stm.setElement(0, 4, "Z");
    stm.setColumnAlignment(5, StringTableModel::ALIGN_LEFT);
    stm.setElement(0, 5, "Cluster Name");
    for (int32_t i = 0; i < numClusters; i++) {
        const Cluster* c = clusters[i];
        CaretAssert(c);
        const int32_t row(i + 1);
        stm.setElement(row, 0, c->getKey());
        stm.setElement(row, 1, c->getNumberOfBrainordinates());
        const Vector3D& cog = c->getCenterOfGravityXYZ();
        stm.setElement(row, 2, cog[0]);
        stm.setElement(row, 3, cog[1]);
        stm.setElement(row, 4, cog[2]);
        stm.setElement(row, 5, c->getName());
    }
    
    return stm.getInString();
}

