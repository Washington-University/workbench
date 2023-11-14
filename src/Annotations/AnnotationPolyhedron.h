#ifndef __ANNOTATION_POLYHEDRON_H__
#define __ANNOTATION_POLYHEDRON_H__

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

#include "AnnotationFontAttributesInterface.h"
#include "AnnotationMultiPairedCoordinateShape.h"
#include "Plane.h"

namespace caret {

    class AnnotationFontAttributes;
    class Plane;
    
    class AnnotationPolyhedron : public AnnotationMultiPairedCoordinateShape,
           public AnnotationFontAttributesInterface {
        
    public:
        class Edge {
        public:
            Edge(const Vector3D& v1,
                 const Vector3D& v2)
            : m_v1(v1), m_v2(v2) { }
            
            const Vector3D m_v1;
            
            const Vector3D m_v2;
        };
        
        class Triangle {
        public:
            Triangle(const Vector3D& v1,
                     const Vector3D& v2,
                     const Vector3D& v3)
            : m_v1(v1), m_v2(v2), m_v3(v3) { }
            
            const Vector3D m_v1;
            
            const Vector3D m_v2;

            const Vector3D m_v3;
        };
        
        AnnotationPolyhedron(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationPolyhedron();
        
        AnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        AnnotationPolyhedron& operator=(const AnnotationPolyhedron& obj);

        virtual AnnotationPolyhedron* castToPolyhedron() override;
        
        virtual const AnnotationPolyhedron* castToPolyhedron() const override;
        
        bool resetRangeToPlanes(const Plane& planeOne,
                                const Plane& planeTwo,
                                AString& errorMessageOut);

        Plane getPlaneOne() const;
        
        Plane getPlaneTwo() const;
               
        void setPlanes(const Plane& planeOne,
                       const Plane& planeTwo);
        
        AnnotationFontAttributes* getFontAttributes();
        
        const AnnotationFontAttributes* getFontAttributes() const;
        
        void getEdgesAndTriangles(std::vector<Edge>& edgesOut,
                                  std::vector<Triangle>& trianglesOut) const;
        
        static float slicesToMillimeters(const float sliceThickness,
                                         const float numberOfSlices);
        
        static float millimetersToSlices(const float sliceThickness,
                                         const float millimeters);
        
        virtual AnnotationTextFontNameEnum::Enum getFont() const override;
        
        virtual void setFont(const AnnotationTextFontNameEnum::Enum font) override;
        
        virtual float getFontPercentViewportSize() const override;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight) override;
        
        virtual CaretColorEnum::Enum getTextColor() const override;
        
        virtual void setTextColor(const CaretColorEnum::Enum color) override;
        
        virtual void getTextColorRGBA(float rgbaOut[4]) const override;
        
        virtual void getTextColorRGBA(uint8_t rgbaOut[4]) const override;
        
        virtual void getCustomTextColor(float rgbaOut[4]) const override;
        
        virtual void getCustomTextColor(uint8_t rgbaOut[4]) const override;
        
        virtual void setCustomTextColor(const float rgba[4]) override;
        
        virtual void setCustomTextColor(const uint8_t rgba[4]) override;
        
        virtual bool isBoldStyleEnabled() const override;
        
        virtual void setBoldStyleEnabled(const bool enabled) override;
        
        virtual bool isItalicStyleEnabled() const override;
        
        virtual void setItalicStyleEnabled(const bool enabled) override;
        
        virtual bool isUnderlineStyleEnabled() const override;
        
        virtual void setUnderlineStyleEnabled(const bool enabled) override;
        
        bool isFontTooSmallWhenLastDrawn() const override;
        
        void setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const override;
               
        virtual bool isModified() const override;
       
        virtual void clearModified() override;
               
        AString getPolyhedronInformationHtml() const;
             
        AString getMetadataInformationHtml() const;
               
        float computePolyhedronVolume(AString& errorMessageOut) const;
               
        bool computePolyhedronVolume(float& volumeOut,
                                     float& endOneAreaOut,
                                     float& endTwoAreaOut,
                                     float& endToEndDistanceOut,
                                     AString& warningMessageOut,
                                     AString& errorMessageOut) const;
               
       // ADD_NEW_METHODS_HERE
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void setFromFileReading(const Plane& planeOne,
                                const Plane& planeTwo);
        
        void copyHelperAnnotationPolyhedron(const AnnotationPolyhedron& obj);
        
        void initializeMembersAnnotationPolyhedron();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Plane m_planeOne;
        
        Plane m_planeTwo;
               
        std::unique_ptr<AnnotationFontAttributes> m_fontAttributes;
        
        // ADD_NEW_MEMBERS_HERE

        /* Needs to call setPlane() and setDepth() */
        friend class AnnotationFileXmlReader;
    };
    
#ifdef __ANNOTATION_POLYHEDRON_DECLARE__
#endif // __ANNOTATION_POLYHEDRON_DECLARE__

} // namespace
#endif  //__ANNOTATION_POLYHEDRON_H__
