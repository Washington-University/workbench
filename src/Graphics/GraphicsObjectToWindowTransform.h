#ifndef __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_H__
#define __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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


#include <array>
#include <memory>

#include "CaretObject.h"
#include "Matrix4x4.h"


namespace caret {

    class GraphicsObjectToWindowTransform : public CaretObject {
        
    public:
        /**
         * Space in which data is drawn
         */
        enum class SpaceType {
            /** not valid */
            INVALID,
            /** Transformation data that is being drawn in model space (except volume)*/
            MODEL,
            /** Transformation data that is being drawn in VOLUME  model space*/
            VOLUME_SLICE_MODEL,
            /** Transformation data that is being drawn in window space*/
            WINDOW
        };
        
        GraphicsObjectToWindowTransform();
        
        GraphicsObjectToWindowTransform(const GraphicsObjectToWindowTransform&);
        
        GraphicsObjectToWindowTransform& operator=(const GraphicsObjectToWindowTransform&);
        
        virtual ~GraphicsObjectToWindowTransform();

        Matrix4x4 getMatrix() const;
        
        Matrix4x4 getModelviewMatrix() const;
        
        bool isValid() const;
        
        void setValid(const bool validFlag);
        
        void modelViewTransformPoint(const float xyz[3],
                                     float xyzOut[3]) const;
        
        bool modelViewInverseTransformPoint(const float xyz[3],
                                            float xyzOut[3]) const;
        
        bool inverseTransformPoint(const float windowXYZ[3],
                                   float objectXYZOut[3]) const;
        
        bool inverseTransformPoint(const float windowX,
                                   const float windowY,
                                   const float windowZ,
                                   float objectXYZOut[3]) const;

        bool transformPoint(const float objectXYZ[3],
                            float windowXYZOut[3]) const;

        std::unique_ptr<GraphicsObjectToWindowTransform> cloneWithNewModelViewMatrix(const Matrix4x4& modelviewMatrix) const;
        
        void setup(const SpaceType spaceType,
                   const std::array<double, 16>& modelviewMatrixArray,
                   const std::array<double, 16>& projectionMatrixArray,
                   const std::array<int32_t, 4>& viewport,
                   const std::array<double, 2>& depthRange,
                   const std::array<float, 4>& orthoLRBT,
                   const double centerToEyeDistance);
        
        void replaceModelviewMatrix(const std::array<double, 16>& modelviewMatrixArray);
        
        std::array<int32_t, 4> getViewport() const;
        
        std::array<float, 4> getOrthoLRBT() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelper(const GraphicsObjectToWindowTransform& transform);
        
        SpaceType m_spaceType = SpaceType::INVALID;
        
        Matrix4x4 m_transformMatrix;
        
        mutable std::unique_ptr<Matrix4x4> m_inverseTransformMatrix;
        
        std::array<double, 16> m_projectionMatrixArray;
        
        Matrix4x4 m_modelviewMatrix;
        
        std::array<int32_t, 4> m_viewport;
        
        std::array<double, 2> m_depthRange;
        
        double m_centerToEyeDistance;
        
        std::array<float, 4> m_orthoLRBT;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__

} // namespace
#endif  //__GRAPHICS_OBJECT_TO_WINDOW_TRANSFORM_H__
