#ifndef __ANNOTATION_COORDINATE_H__
#define __ANNOTATION_COORDINATE_H__

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

#include "AnnotationAttributesDefaultTypeEnum.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationSurfaceOffsetVectorTypeEnum.h"
#include "CaretObjectTracksModification.h"
#include "HistologySpaceKey.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "Vector3D.h"

namespace caret {
    class SceneClassAssistant;

    class AnnotationCoordinate : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        static float getSurfaceTextOffsetPolarAngleDefaultValue();
        
        static float getSurfaceTextOffsetScreenDepthDefaultValue();

        AnnotationCoordinate(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationCoordinate();
        
        AnnotationCoordinate(const AnnotationCoordinate& obj);

        AnnotationCoordinate& operator=(const AnnotationCoordinate& obj);
        
        bool equalXYZ(const Vector3D& xyz) const;
        
        const Vector3D getXYZ() const;
        
        void getXYZ(float xyzOut[3]) const;
        
        void setXYZ(const float xyz[3]);
        
        void setXYZ(const double xyz[3]);
        
        void setXYZ(const float x,
                    const float y,
                    const float z);
        
        void addToXYZ(const float dx,
                      const float dy,
                      const float dz);
        
        void setXYZFromViewportXYZ(const float viewportWidth,
                                   const float viewportHeight,
                                   const float viewportX,
                                   const float viewportY);
        
        void getViewportXY(const float viewportWidth,
                           const float viewportHeight,
                           float& viewportXOut,
                           float& viewportYOut) const;
        
        StructureEnum::Enum getSurfaceSpaceStructure() const;
        
        int32_t getSurfaceSpaceNumberOfNodes() const;
        
        int32_t getSurfaceSpaceNodeIndex() const;
        
        float getSurfaceSpaceOffsetLength() const;
        
        AnnotationSurfaceOffsetVectorTypeEnum::Enum getSurfaceSpaceOffsetVectorType() const;
        
        float getSurfaceTextOffsetPolarAngle() const;
        
        float getSurfaceTextOffsetScreenDepth() const;
        
        void setSurfaceSpaceStructure(const StructureEnum::Enum structure);
        
        void setSurfaceSpaceNumberOfNodes(const int32_t numberOfNodes);
        
        void setSurfaceSpaceNodeIndex(const int32_t nodeIndex);
        
        void setSurfaceSpaceOffsetLength(const float offsetLength);
        
        void setSurfaceSpaceOffsetVectorType(const AnnotationSurfaceOffsetVectorTypeEnum::Enum offsetVectorType);
        
        void setSurfaceTextOffsetPolarAngle(const float angle);

        void setSurfaceTextOffsetScreenDepth(const float depth);
        
        void getSurfaceSpace(StructureEnum::Enum& structureOut,
                             int32_t& surfaceNumberOfNodesOut,
                             int32_t& surfaceNodeIndexOut) const;
        
        void getSurfaceSpace(StructureEnum::Enum& structureOut,
                             int32_t& surfaceNumberOfNodesOut,
                             int32_t& surfaceNodeIndexOut,
                             float& surfaceOffsetLengthOut,
                             AnnotationSurfaceOffsetVectorTypeEnum::Enum& surfaceOffsetVectorTypeOut) const;
        
        void setSurfaceSpace(const StructureEnum::Enum structure,
                             const int32_t surfaceNumberOfNodes,
                             const int32_t surfaceNodeIndex);
        
        void setSurfaceSpace(const StructureEnum::Enum structure,
                             const int32_t surfaceNumberOfNodes,
                             const int32_t surfaceNodeIndex,
                             const float surfaceOffsetLength,
                             const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType);
        
        void setMediaFileNameAndPixelSpace(const AString& mediaFileName,
                                           const float xyz[3]);
        
        const HistologySpaceKey& getHistologySpaceKey() const;
        
        void setHistologySpaceKey(const HistologySpaceKey& histlogySpaceKey);
        
        void getHistologySpace(HistologySpaceKey& histologySpaceKeyOut,
                               float xyzOut[3]) const;
        
        void setHistologySpace(const HistologySpaceKey& histologySpaceKey,
                               const float xyz[3]);
        
        float getSurfaceOffsetLength() const;
        
        StructureEnum::Enum getSurfaceStructure() const;
        
        AnnotationSurfaceOffsetVectorTypeEnum::Enum getSurfaceOffsetVectorType() const;
        
        AString getMediaFileName() const;
        
        void setMediaFileName(const AString& mediaFileName);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual AString toStringForCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum space) const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
        static float getDefaultSurfaceOffsetLength();

        static void setUserDefautlSurfaceOffsetVectorType(const AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorType);
        
        static void setUserDefaultSurfaceOffsetLength(const float surfaceOffsetLength);
          
        
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotationCoordinate(const AnnotationCoordinate& obj);

        void initializeAnnotationCoordinateMembers();
        
        const AnnotationAttributesDefaultTypeEnum::Enum m_attributeDefaultType;

        SceneClassAssistant* m_sceneAssistant;

        float m_xyz[3];
        
        int32_t m_surfaceSpaceNodeIndex;
        
        int32_t m_surfaceSpaceNumberOfNodes;
        
        StructureEnum::Enum m_surfaceSpaceStructure;
        
        float m_surfaceOffsetLength;
        
        float m_surfaceTextOffsetPolarAngle = 90.0;
        
        float m_surfaceTextOffsetScreenDepth = 0.0;
        
        AnnotationSurfaceOffsetVectorTypeEnum::Enum m_surfaceOffsetVectorType;
        
        AString m_mediaFileName;
        
        HistologySpaceKey m_histologySpaceKey;
        
//        AString m_histologySlicesFileName;
//
//        int32_t m_histologySliceIndex = 0;

        static float s_userDefaultSurfaceOffsetLength;
        
        static AnnotationSurfaceOffsetVectorTypeEnum::Enum s_userDefaultSurfaceOffsetVectorType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COORDINATE_DECLARE__
    float AnnotationCoordinate::s_userDefaultSurfaceOffsetLength = 1.0f;
    AnnotationSurfaceOffsetVectorTypeEnum::Enum AnnotationCoordinate::s_userDefaultSurfaceOffsetVectorType = AnnotationSurfaceOffsetVectorTypeEnum::TANGENT;
#endif // __ANNOTATION_COORDINATE_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATE_H__
