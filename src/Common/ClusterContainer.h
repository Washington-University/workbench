#ifndef __CLUSTER_CONTAINER_H__
#define __CLUSTER_CONTAINER_H__

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

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "CaretObject.h"
#include "Cluster.h"


namespace caret {

    class ClusterContainer : public CaretObject {
        
    public:
        ClusterContainer();
        
        virtual ~ClusterContainer();
        
        ClusterContainer(const ClusterContainer&) = delete;

        ClusterContainer& operator=(const ClusterContainer&) = delete;
        
        void addCluster(Cluster* cluster);
        
        const std::vector<const Cluster*>& getClustersSortedByKey() const;
        
        const std::vector<const Cluster*>& getClustersSortedByName() const;
        
        std::vector<const Cluster*> getClustersWithKey(const int32_t key) const;
        
        std::vector<const Cluster*> getClustersWithName(const AString& name) const;
        
        std::vector<int32_t> getAllClusterKeys() const;
        
        void addKeyThatIsNotInAnyCluster(const int32_t key);
        
        std::set<int32_t> getKeysThatAreNotInAnyClusters() const;
        
        void clear();

        AString getClustersInFormattedString() const;
        
        std::unique_ptr<ClusterContainer> mergeDisjointRightLeftClusters() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void clearSortedContainers();
        
        void createMultimapClustersSortedByKey() const;
        
        void createMultimapClustersSortedByName() const;
        
        /**
         * THE container for the clusters.  Will delete the clusters at some time.
         */
        std::vector<std::unique_ptr<Cluster>> m_clusters;
        
        /**
         * Lazily initialized multimap with pointers to clusters sorted by name.
         * Multimap since more than one cluster may have the same name.
         * Used to quickly access clusters by name
         */
        typedef std::multimap<AString, const Cluster*> MultimapSortedByName;
        typedef MultimapSortedByName::const_iterator MultimapSortedByNameIterator;
        mutable MultimapSortedByName m_mapWithClustersSortedByName;
        
        /*
         * Lazily initialized pointers to all clusters sorted by Key
         */
        mutable std::vector<const Cluster*> m_vectorClustersSortedByKey;
        
        /*
         * Lazily initialized pointers to all clusters sorted by name
         */
        mutable std::vector<const Cluster*> m_vectorClustersSortedByName;
        
        /**
         * Lazily initialized multimap with pointers to clusters sorted by key.
         * Multimap since more than one cluster may have the same key.
         * Used to quickly access clusters by key
         */
        typedef std::multimap<int32_t, const Cluster*> MultimapSortedByKey;
        typedef MultimapSortedByKey::const_iterator MultimapSortedByKeyIterator;
        mutable MultimapSortedByKey m_mapWithClustersSortedByKey;
        
        /**
         * Lazily initilaized keys sorted
         */
        mutable std::vector<int32_t> m_keysSorted;
        
        /**
         * Keys that do not map to any cluster
         */
        std::set<int32_t> m_keysThatAreNotInAnyClusters;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLUSTER_CONTAINER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLUSTER_CONTAINER_DECLARE__

} // namespace
#endif  //__CLUSTER_CONTAINER_H__
