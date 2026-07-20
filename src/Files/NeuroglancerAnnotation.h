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



#include <memory>

#include "CaretColor.h"
#include "CaretObject.h"
#include "NeuroglancerAnnotationTypeEnum.h"
#include "Vector3D.h"


namespace caret {
    
    class NeuroglancerAnnotation : public CaretObject {
        
    public:
        NeuroglancerAnnotation(const NeuroglancerAnnotationTypeEnum::Enum annotationType,
                               const AString& fileName,
                               const std::vector<Vector3D>& xyz,
                               const CaretColor& color);
        
        virtual ~NeuroglancerAnnotation();
        
        NeuroglancerAnnotation(const NeuroglancerAnnotation& obj);
        
        NeuroglancerAnnotation& operator=(const NeuroglancerAnnotation& obj);
        
        NeuroglancerAnnotationTypeEnum::Enum getType() const;
        
        AString getFileName() const;
        
        int32_t getNumberOfXYZ() const;
        
        const Vector3D& getXYZ(const int32_t index) const;
        
        const CaretColor& getColor() const;
        
        float getSize() const;

        AString getTypeName() const;
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        void copyHelperNeuroglancerAnnotation(const NeuroglancerAnnotation& obj);
        
        NeuroglancerAnnotationTypeEnum::Enum m_annotationType = NeuroglancerAnnotationTypeEnum::INVALID;
        
        AString m_fileName;
        
        std::vector<Vector3D> m_xyz;
        
        CaretColor m_color;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef ____NEUROGLANCER_ANNOTATION__DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // ____NEUROGLANCER_ANNOTATION__DECLARE__
    
} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_H__

