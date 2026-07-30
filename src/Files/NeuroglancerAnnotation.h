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

#include <QColor>

#include "NeuroglancerAnnotationBase.h"
#include "NeuroglancerAnnotationTypeEnum.h"
#include "Vector3D.h"


namespace caret {
    class CaretDataFileSelectionModel;
    class NeuroglancerAnnotationPropertyValue;
    class NeuroglancerAnnotationsFile;
    
    class NeuroglancerAnnotation : public NeuroglancerAnnotationBase {

    public:
        NeuroglancerAnnotation(const NeuroglancerAnnotationTypeEnum::Enum annotationType,
                               const AString& fileName,
                               const std::vector<Vector3D>& ijk,
                               const QColor& color,
                               const float symbolSize,
                               const std::vector<const NeuroglancerAnnotationPropertyValue*>& propertyValues);
        
        virtual ~NeuroglancerAnnotation();
        
        NeuroglancerAnnotation(const NeuroglancerAnnotation& obj) = delete;
        
        NeuroglancerAnnotation& operator=(const NeuroglancerAnnotation& obj) = delete;
        
        NeuroglancerAnnotationTypeEnum::Enum getType() const;
        
        bool isDisplayed() const;
        
        AString getFileName() const;
        
        int32_t getNumberOfIJK() const;
        
        const Vector3D& getIJK(const int32_t index) const;
        
        const QColor& getColor() const;
        
        float getSymbolSize() const;

        AString getTypeName() const;
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const override;
        
        void getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                   const NeuroglancerAnnotationsFile* neuroglancerAnnotationFile,
                                   const int32_t annotationIndex,
                                   const bool toolTipFlag) const;
        
    private:
        void copyHelperNeuroglancerAnnotation(const NeuroglancerAnnotation& obj);
        
        NeuroglancerAnnotationTypeEnum::Enum m_annotationType = NeuroglancerAnnotationTypeEnum::INVALID;
        
        AString m_fileName;
        
        std::vector<Vector3D> m_ijk;
        
        QColor m_color;
        
        float m_symbolSize;
        
        std::vector<const NeuroglancerAnnotationPropertyValue*> m_propertyValues;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef ____NEUROGLANCER_ANNOTATION__DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // ____NEUROGLANCER_ANNOTATION__DECLARE__
    
} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_H__

