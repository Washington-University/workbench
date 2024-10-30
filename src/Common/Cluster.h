#ifndef __CLUSTER_H__
#define __CLUSTER_H__

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
#include <memory>
#include <vector>

#include "AString.h"
#include "Vector3D.h"

namespace caret {

    class Cluster {
        
    public:
        /**
         * Location type of the cluster
         */
        enum class LocationType {
            /** unknown - default */
            UNKNOWN,
            /** neither left nor right */
            CENTRAL,
            /** left side accumulation */
            LEFT,
            /** right side accumulation */
            RIGHT
        };
        
        static AString locationTypeToName(const LocationType type);
        
        Cluster();
        
        Cluster(const AString& name,
                const int32_t key,
                const std::vector<Vector3D>& coordinatesXYZ);
        
        virtual ~Cluster();
        
        Cluster(const Cluster& obj);

        Cluster& operator=(const Cluster& obj);
        
        bool isValid() const;
        
        AString getName() const;
        
        LocationType getLocationType() const;
        
        AString getLocationTypeName() const;
        
        int32_t getKey() const;
        
        Vector3D getCenterOfGravityXYZ() const;
        
        int64_t getNumberOfBrainordinates() const;

        void addCoordinate(const Vector3D& coordinateXYZ);
        
        const Vector3D& getCoordinate(const int32_t index) const;
        
        void mergeCoordinates(const Cluster& cluster);
        
        std::vector<Cluster*> splitClusterIntoRightAndLeft() const;
        
        bool isSplitClusterFlag() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperCluster(const Cluster& obj);

        void computeCenterOfGravityAndLocation() const;
        
        void invalidateCenterOfGravityAndLocation();
        
        mutable LocationType m_location = LocationType::UNKNOWN;
        
        AString m_name = "";
        
        int32_t m_key = -1;
        
        std::vector<Vector3D> m_coordinateXYZ;
        
        mutable Vector3D m_centerOfGravityXYZ;
        
        mutable bool m_centerOfGravityAndLocationValidFlag = false;
        
        /**
         * True if this cluster was split into other clusters (typically Central is
         * split into left and right).
         */
        mutable bool m_splitClusterFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ClusterContainer;
    };
    
#ifdef __CLUSTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLUSTER_DECLARE__

} // namespace
#endif  //__CLUSTER_H__
