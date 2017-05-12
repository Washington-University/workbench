#ifndef __ANNOTATION_IMAGE_H__
#define __ANNOTATION_IMAGE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "AnnotationTwoDimensionalShape.h"
#include "CaretPointer.h"
#include "DrawnWithOpenGLTextureInfo.h"
#include "DrawnWithOpenGLTextureInterface.h"


namespace caret {

    class GraphicsPrimitiveV3fT3F;
    
    class AnnotationImage : public AnnotationTwoDimensionalShape, public DrawnWithOpenGLTextureInterface {
        
    public:
        AnnotationImage(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationImage();
        
        AnnotationImage(const AnnotationImage& obj);
        
        AnnotationImage& operator=(const AnnotationImage& obj);
        
        virtual bool isLineWidthSupported() const;
        
        virtual bool isBackgroundColorSupported() const;
        
        virtual bool isFixedAspectRatio() const;
        
        virtual float getFixedAspectRatio() const;

        void setImageBytesRGBA(const uint8_t* imageBytesRGBA,
                               const int32_t imageWidth,
                               const int32_t imageHeight);
        
        int32_t getImageWidth() const;
        
        int32_t getImageHeight() const;
        
        const uint8_t* getImageBytesRGBA() const;

        virtual DrawnWithOpenGLTextureInfo* getDrawWithOpenGLTextureInfo();
        
        virtual const DrawnWithOpenGLTextureInfo* getDrawWithOpenGLTextureInfo() const;
        
        GraphicsPrimitiveV3fT3F* getGraphicsPrimitive() const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationImage(const AnnotationImage& obj);
        
        void initializeMembersAnnotationImage();
        
        SceneClassAssistant* m_sceneAssistant;

        std::vector<uint8_t> m_imageBytesRGBA;
        
        int32_t m_imageWidth;
        
        int32_t m_imageHeight;
        
        CaretPointer<DrawnWithOpenGLTextureInfo> m_drawnWithOpenGLTextureInfo;
        
        mutable std::unique_ptr<GraphicsPrimitiveV3fT3F> m_graphicsPrimitive;
                
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_IMAGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_IMAGE_DECLARE__

} // namespace
#endif  //__ANNOTATION_IMAGE_H__
