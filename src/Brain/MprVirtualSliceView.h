#ifndef __MPR_VIRTUAL_SLICE_VIEW_H__
#define __MPR_VIRTUAL_SLICE_VIEW_H__

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
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {
    class SceneClassAssistant;
    class VolumeMappableInterface;

    class MprVirtualSliceView : public CaretObject, public SceneableInterface {
        
    public:
        enum ViewType {
            /*
             * Volume does not get transformed;
             * Camera position is transformed
             */
            ROTATE_CAMERA,
            /*
             * Volume is transformed;
             * Camera is not transformed
             */
            ROTATE_VOLUME
        };
        
        static ViewType getViewType();
        
        MprVirtualSliceView();
        
        MprVirtualSliceView(const Vector3D& volumeCenterXYZ,
                     const Vector3D& selectedSlicesXYZ,
                     const float sliceWidthHeight,
                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                     const Matrix4x4& rotationMatrix);

        virtual ~MprVirtualSliceView();
        
        MprVirtualSliceView(const MprVirtualSliceView& obj);

        MprVirtualSliceView& operator=(const MprVirtualSliceView& obj);

        Vector3D getCameraXYZ() const;
        
        Vector3D getCameraLookAtXYZ() const;
        
        Vector3D getCameraUpVector() const;

        bool getTriangleFanCoordinates(const VolumeMappableInterface* volume,
                                       std::vector<Vector3D>& stereotaxicXyzOut,
                                       std::vector<Vector3D>& primtiveVertexXyzOut,
                                       std::vector<Vector3D>& primitiveTextureStrOut) const;

        bool getTrianglesCoordinates(const VolumeMappableInterface* volume,
                                     std::vector<Vector3D>& stereotaxicXyzOut,
                                     std::vector<Vector3D>& primtiveVertexXyzOut,
                                     std::vector<Vector3D>& primitiveTextureStrOut) const;
        
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
        void copyHelperMprVirtualSliceView(const MprVirtualSliceView& obj);

        std::vector<Vector3D> createVirtualSliceTriangleFan(const VolumeMappableInterface* volume) const;
        
        std::vector<Vector3D> createVirtualSliceTriangles(const VolumeMappableInterface* volume) const;
        
        std::vector<Vector3D> mapTextureCoordinates(const VolumeMappableInterface* volume,
                                                    const std::vector<Vector3D>& vertexXyz) const;
        
        std::vector<Vector3D> mapBackToStereotaxicCoordinates(const std::vector<Vector3D>& intersectionXyz) const;
        
        //std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        Vector3D m_volumeCenterXYZ;
        
        Vector3D m_selectedSlicesXYZ;

        float m_sliceWidthHeight;

        VolumeSliceViewPlaneEnum::Enum m_sliceViewPlane;
        
        Matrix4x4 m_rotationMatrix;
        
        Matrix4x4 m_transformationMatrix;
        
        Vector3D m_cameraXYZ;
        
        Vector3D m_cameraLookAtXYZ;
        
        Vector3D m_cameraUpVector;
        
        Vector3D m_planeRightVector;
        
        Vector3D m_planeUpVector;
        
        Plane m_virtualSlicePlane;
        
        static const ViewType s_viewType;
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MPR_VIRTUAL_SLICE_VIEW_DECLARE__
    const MprVirtualSliceView::ViewType MprVirtualSliceView::s_viewType = MprVirtualSliceView::ViewType::ROTATE_CAMERA;
#endif // __MPR_VIRTUAL_SLICE_VIEW_DECLARE__

} // namespace
#endif  //__MPR_VIRTUAL_SLICE_VIEW_H__
