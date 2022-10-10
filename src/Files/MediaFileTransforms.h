#ifndef __MEDIA_FILE_TRANSFORMS_H__
#define __MEDIA_FILE_TRANSFORMS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include <QRectF>

#include "CaretObject.h"
#include "Matrix4x4.h"


namespace caret {

    class CziNonLinearTransform;
    class PixelIndex;
    class PixelLogicalIndex;
    class Vector3D;
    
    class MediaFileTransforms : public CaretObject {
        
    public:
        class Inputs {
        public:
            Inputs() { }
            
            Inputs(const QRectF& logicalBoundsRect,
                   const Matrix4x4& pixelIndexToPlaneMatrix,
                   const bool pixelIndexToPlaneMatrixValidFlag,
                   const Matrix4x4& planeToMillimetersMatrix,
                   const bool planeToMillimetersMatrixValidFlag,
                   std::shared_ptr<CziNonLinearTransform>& toStereotaxicNonLinearTransform,
                   std::shared_ptr<CziNonLinearTransform>& fromStereotaxicNonLinearTransform);
            
            bool computeMillimetersToPlaneMatrix(const Matrix4x4& planeToMillimetersMatrix,
                                                 Matrix4x4& millimetersToPlaneMatrixOut) const;
            
            QRectF m_logicalBoundsRect;
            
            bool m_logicalBoundsRectValidFlag = false;
            
            Matrix4x4 m_millimetersToPlaneMatrix;
            
            bool m_millimetersToPlaneMatrixValidFlag = false;
            
            Matrix4x4 m_pixelIndexToPlaneMatrix;
            
            bool m_pixelIndexToPlaneMatrixValidFlag = false;
            
            Matrix4x4 m_planeToMillimetersMatrix;
            
            bool m_planeToMillimetersMatrixValidFlag = false;
            
            Matrix4x4 m_planeToPixelIndexMatrix;
            
            bool m_planeToPixelIndexMatrixValidFlag = false;
            
            std::shared_ptr<CziNonLinearTransform> m_toStereotaxicNonLinearTransform;
            
            std::shared_ptr<CziNonLinearTransform> m_fromStereotaxicNonLinearTransform;
            
            Matrix4x4 m_invertedPlaneToMillimetersMatrix;
            
            bool m_invertedPlaneToMillimetersMatrixValidFlag = false;

            float m_planeZ = 0.0;
        };
        
        MediaFileTransforms();
        
        MediaFileTransforms(const Inputs& inputs);
        
        virtual ~MediaFileTransforms();
        
        MediaFileTransforms(const MediaFileTransforms&);

        MediaFileTransforms& operator=(const MediaFileTransforms&);
        
        virtual PixelIndex logicalPixelIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToPlaneXYZ(const float logicalX,
                                                 const float logicalY,
                                                 Vector3D& planeXyzOut) const;
        
        virtual bool logicalPixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogialIndex,
                                                       Vector3D& stereotaxicXyzOut) const;
        
        virtual bool logicalPixelIndexToStereotaxicXYZ(const float logicalX,
                                                       const float logicalY,
                                                       Vector3D& stereotaxicXyzOut) const;
        
        virtual PixelLogicalIndex pixelIndexToLogicalPixelIndex(const PixelIndex& pixelIndex) const;
        
        virtual bool pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                          Vector3D& planeXyzOut) const;
        
        virtual bool pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndex,
                                                Vector3D& xyzOut) const;
        
        virtual bool planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                                 PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool planeXyzToPixelIndex(const Vector3D& planeXyz,
                                          PixelIndex& pixelIndexOut) const;
        
        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicNoNonLinearXyzOut,
                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicXyzOut) const;
        
        virtual bool stereotaxicXyzToLogicalPixelIndex(const Vector3D& xyz,
                                                       PixelLogicalIndex& pixelLogicalIndexOut) const;
        
        virtual bool stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                                PixelIndex& pixelIndexOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const;
        
        virtual bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeNoNonLinearXyzOut,
                                              Vector3D& planeXyzOut) const;
        
        Matrix4x4 getPlaneToMillimetersMatrix() const;
        
        virtual bool isPlaneToMillimetersMatrixValid() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        Inputs m_inputs;
        
        void testTransforms();
        
        void copyHelper(const MediaFileTransforms& mft);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MEDIA_FILE_TRANSFORMS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_TRANSFORMS_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_TRANSFORMS_H__
