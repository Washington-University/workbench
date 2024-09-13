#ifndef __BRAINORDINATE_CLUSTER_H__
#define __BRAINORDINATE_CLUSTER_H__

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

#include "CaretObject.h"
#include "Vector3D.h"

namespace caret {

    class BrainordinateCluster : public CaretObject {
        
    public:
        BrainordinateCluster(const AString& name,
                             const int32_t key,
                             const Vector3D& centerOfGravityXYZ,
                             const int64_t numberOfBrainordinates);
        
        virtual ~BrainordinateCluster();
        
        BrainordinateCluster(const BrainordinateCluster& obj);

        BrainordinateCluster& operator=(const BrainordinateCluster& obj);
        
        bool operator<(const BrainordinateCluster& obj) const;
        
        AString getName() const;
        
        int32_t getKey() const;
        
        Vector3D getCenterOfGravityXYZ() const;
        
        int64_t getNumberOfBrainordinates() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperBrainordinateCluster(const BrainordinateCluster& obj);

        AString m_name;
        
        int32_t m_key;
        
        Vector3D m_centerOfGravityXYZ;
        
        int64_t m_numberOfBrainordinates;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAINORDINATE_CLUSTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAINORDINATE_CLUSTER_DECLARE__

} // namespace
#endif  //__BRAINORDINATE_CLUSTER_H__
