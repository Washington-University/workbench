#ifndef __HISTOLOGY_SLICE_H__
#define __HISTOLOGY_SLICE_H__

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
#include <vector>

#include "BoundingBox.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneableInterface.h"


namespace caret {
    class CziNonLinearTransform;
    class DataFileContentInformation;
    class HistologySliceImage;
    class MediaFile;
    class SceneClassAssistant;

    class HistologySlice : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        HistologySlice(const int32_t sliceIndex,
                       const int32_t sliceNumber,
                       const AString& MRIToHistWarpFileName,
                       const AString& histToMRIWarpFileName,
                       const Matrix4x4& planeToMillimetersMatrix,
                       const bool planeToMillimetersMatrixValidFlag);
        
        virtual ~HistologySlice();
        
        HistologySlice(const HistologySlice& obj);

        HistologySlice& operator=(const HistologySlice& obj);
        
        int32_t getSliceIndex() const;
        
        int32_t getSliceNumber() const;
        
        void addHistologySliceImage(HistologySliceImage* histologySliceImage);
        
        int32_t getNumberOfHistologySliceImages() const;
        
        HistologySliceImage* getHistologySliceImage(const int32_t index);

        const HistologySliceImage* getHistologySliceImage(const int32_t index) const;
         
        MediaFile* findMediaFileWithName(const AString& mediaFileName) const;

//        Matrix4x4 getPlaneToMillimetersMatrix() const;
        
//        bool isPlaneToMillimetersMatrixValid() const;

        virtual BoundingBox getStereotaxicXyzBoundingBox() const;
        
        virtual BoundingBox getPlaneXyzBoundingBox() const;
        
        bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                      Vector3D& stereotaxicXyzOut) const;
        
        bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                      Vector3D& planeXyzOut) const;
        
        bool planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                      Vector3D& stereotaxicNoNonLinearXyzOut,
                                      Vector3D& stereotaxicWithNonLinearXyzOut) const;
        
        bool stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                      Vector3D& planeNoNonLinearXyzOut,
                                      Vector3D& planeWithNonLinearXyzOut) const;
        
        const Plane& getStereotaxicPlane() const;
        
        const Plane& getPlaneXyzPlane() const;
        
        virtual std::vector<AString> getChildDataFilePathNames() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
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
        void copyHelperHistologySlice(const HistologySlice& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<CziNonLinearTransform> m_toStereotaxicNonLinearTransform;
        
        std::unique_ptr<CziNonLinearTransform> m_fromStereotaxicNonLinearTransform;
        
        int32_t m_sliceIndex  = -1;
        
        int32_t m_sliceNumber = -1;
        
        AString m_MRIToHistWarpFileName;
        
        AString m_histToMRIWarpFileName;
        
        Matrix4x4 m_planeToMillimetersMatrix;
        
        bool m_planeToMillimetersMatrixValidFlag = false;
        
        mutable BoundingBox m_stereotaxicXyzBoundingBox;
        
        mutable bool m_stereotaxicXyzBoundingBoxValidFlag = false;
        
        mutable BoundingBox m_planeXyzBoundingBox;
        
        mutable bool m_planeXyzBoundingBoxValidFlag = false;
        
        Matrix4x4 m_millimetersToPlaneMatrix;

        bool m_millimetersToPlaneMatrixValidFlag = false;
        
        std::vector<std::unique_ptr<HistologySliceImage>> m_histologySliceImages;

        mutable Plane m_stereotaxicPlane;
        
        mutable bool m_stereotaxicPlaneValidFlag = false;

        mutable Plane m_planeXyzPlane;
        
        mutable bool m_planeXyzPlaneValidFlag = false;
        
        static constexpr bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SLICE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOLOGY_SLICE_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SLICE_H__
