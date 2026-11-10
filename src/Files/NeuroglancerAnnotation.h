#ifndef __NEUROGLANCER_ANNOTATION_H__
#define __NEUROGLANCER_ANNOTATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include "RgbaUnsignedByte.h"
#include "Vector3D.h"

namespace caret {

    class Matrix4x4;
    class NeuroglancerAnnotationFile;
    class NeuroglancerPropertyInfo;
    class RgbaUnsignedByte;
    
    class NeuroglancerAnnotation {
        
    public:
        NeuroglancerAnnotation(const uint64_t& uniqueID,
                               const Vector3D& ijk,
                               const std::vector<int32_t>& propertyValues);
        
        virtual ~NeuroglancerAnnotation();
        
        NeuroglancerAnnotation(const NeuroglancerAnnotation&) = delete;

        NeuroglancerAnnotation& operator=(const NeuroglancerAnnotation&) = delete;

        const Vector3D& getIJK() const;
        
        const Vector3D& getXYZ() const;
        
        void updateXYZ(const Matrix4x4& ijkToXyzTransformationMatrix);
        
        uint64_t getUniqueID() const;
        
        AString getUniqueIdAsString() const;
        
        int32_t getNumberOfProperties() const;
        
        int32_t getPropertyAsInt(const int32_t propertyIndex) const;
        
        uint32_t getPropertyAsUnsignedInt(const int32_t propertyIndex) const;
        
        float getPropertyAsFloat(const int32_t propertyIndex) const;
        
        int32_t getPropertyAsLabelIndex(const int32_t propertyIndex) const;
        
        RgbaUnsignedByte getPropertyAsRGBA(const int32_t propertyIndex) const;
        
        const uint8_t* getRgba() const;

        void setRgbaFromPropertyAtIndex(const int32_t propertyIndex);
        
        void getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                   const NeuroglancerAnnotationFile* neuroAnnFile,
                                   const bool toolTipFlag) const;
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool status);
        
        void updateDisplayedStatus(const std::vector<const NeuroglancerPropertyInfo*>& allPropertyInfos);
        
        // ADD_NEW_METHODS_HERE

    private:
        void setUniqueID(const uint64_t& uniqueID);
        
        uint64_t m_uniqueID = -1;
        
        const Vector3D m_ijk;
        
        Vector3D m_xyz;
        
        const std::vector<int32_t> m_propertyValues;
        
        RgbaUnsignedByte m_rgba;
        
        bool m_displayedFlag = true;
        
        friend class NeuroglancerAnnotationFileImporter;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __NEUROGLANCER_ANNOTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_ANNOTATION_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_H__
