#ifndef __VOLUME_MPR_VIRTUAL_SLICE_VIEW_H__
#define __VOLUME_MPR_VIRTUAL_SLICE_VIEW_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "CaretObject.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneableInterface.h"
#include "Vector3D.h"
#include "VolumeMprOrientationModeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {
    class SceneClassAssistant;
    class VolumeMappableInterface;

    class VolumeMprVirtualSliceView : public CaretObject, public SceneableInterface {
        
    public:
        enum class ViewType {
            /*
             * Creates slices with MPR rotation applied
             * Look at, camara, etc are not used
             */
            ALL_VIEW_SLICES,
            /*
             * Cameras are fixed, volume coordinates are transformed
             */
            VOLUME_VIEW_FIXED_CAMERA
        };
        
        static ViewType getViewTypeForVolumeSliceView();
        
        static ViewType getViewTypeForAllView();
        
        VolumeMprVirtualSliceView();
        
        VolumeMprVirtualSliceView(const ViewType viewType,
                                  const Vector3D& volumeCenterXYZ,
                                  const Vector3D& selectedSlicesXYZ,
                                  const float sliceWidthHeight,
                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                  const VolumeMprOrientationModeEnum::Enum& mprOrientationMode,
                                  const Matrix4x4& rotationMatrix);

        virtual ~VolumeMprVirtualSliceView();
        
        VolumeMprVirtualSliceView(const VolumeMprVirtualSliceView& obj);

        VolumeMprVirtualSliceView& operator=(const VolumeMprVirtualSliceView& obj);

        ViewType getViewType() const;
        
        Vector3D getCameraXYZ() const;
        
        Vector3D getCameraLookAtXYZ() const;
        
        Vector3D getCameraUpVector() const;

        Plane getVirtualPlane() const;
        
        Plane getOriginalUtransformedPlane() const;
        
        Vector3D getPlaneRightVector() const;
        
        Vector3D getPlaneUpVector() const;
        
        Plane getMontageIncreasingDirectionPlane() const;
        
        Vector3D getVolumeCenterXYZ() const;
        
        Vector3D getPreLookAtTranslation() const;
        
        Vector3D getPostLookAtTranslation() const;
        
        Matrix4x4 getTransformationMatrix() const;
        
        bool getTrianglesCoordinates(const VolumeMappableInterface* volume,
                                     std::vector<Vector3D>& stereotaxicXyzOut,
                                     std::vector<Vector3D>& textureStrOut,
                                     Plane& layersDrawingPlaneOut) const;
        
        void getAxisLabels(AString& leftScreenLabelTextOut,
                           AString& rightScreenLabelTextOut,
                           AString& bottomScreenLabelTextOut,
                           AString& topScreenLabelTextOut) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        class VectorAndLabel {
        public:
            VectorAndLabel(const AString& label,
                           const float x,
                           const float y,
                           const float z)
            : m_label(label),
            m_vector(x, y, z)
            
            {
                
            }
            
            AString m_label;
            Vector3D m_vector;
        };
        
        void copyHelperVolumeMprVirtualSliceView(const VolumeMprVirtualSliceView& obj);

        void initializeModeAllViewSlices();
        
        void initializeModeVolumeViewFixedCamera();
        
        std::vector<Vector3D> createVirtualSliceTriangleFan(const VolumeMappableInterface* volume) const;
        
        std::vector<Vector3D> createVirtualSliceTriangles(const VolumeMappableInterface* volume) const;
                
        std::vector<Vector3D> mapTextureCoordinates(const VolumeMappableInterface* volume,
                                                    const std::vector<Vector3D>& vertexXyz) const;
        
        void computeVirtualSlicePlane();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        ViewType m_viewType;
        
        Vector3D m_volumeCenterXYZ;
        
        Vector3D m_selectedSlicesXYZ;

        float m_sliceWidthHeight;

        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
        
        VolumeMprOrientationModeEnum::Enum m_mprOrientationMode;

        Matrix4x4 m_rotationMatrix;
        
        Matrix4x4 m_transformationMatrix;
        
        Vector3D m_cameraXYZ;
        
        Vector3D m_cameraLookAtXYZ;
        
        Vector3D m_cameraUpVector;
        
        Vector3D m_planeRightVector;
        
        Vector3D m_planeUpVector;
        
        Vector3D m_preLookAtTranslation;
        
        Vector3D m_postLookAtTranslation;
        
        /* Plane prior to transformation */
        Plane m_originalPlane;
        
        /* Plane after transformation that points to user */
        Plane m_virtualPlane;
        
        Plane m_montageVirutalSliceIncreasingDirectionPlane;

        bool m_radiologicalOrientationFlag = false;
        
        bool m_neurologicalOrientationFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_MPR_VIRTUAL_SLICE_VIEW_DECLARE__
#endif // __VOLUME_MPR_VIRTUAL_SLICE_VIEW_DECLARE__

} // namespace
#endif  //__VOLUME_MPR_VIRTUAL_SLICE_VIEW_H__
