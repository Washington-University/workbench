#ifndef __CLIPPING_PLANE_GROUP_H__
#define __CLIPPING_PLANE_GROUP_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretObject.h"
#include "ClippingPlanePanningModeEnum.h"
#include "Matrix4x4.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"


namespace caret {
    class Plane;
    class SceneClassAssistant;

    class ClippingPlaneGroup : public CaretObject, public SceneableInterface {
        
    public:
        ClippingPlaneGroup(const int32_t tabIndex);
        
        virtual ~ClippingPlaneGroup();
        
        ClippingPlaneGroup(const ClippingPlaneGroup& obj);

        ClippingPlaneGroup& operator=(const ClippingPlaneGroup& obj);
        
        void resetTransformation();
        
        void resetToDefaultValues();
        
        bool isEnabled() const;
        
        void setEnabled(const bool status);
        
        void setEnabledAndEnablePlanes(const bool status);
        
        std::vector<const Plane*> getActiveClippingPlanesForStructure(const StructureEnum::Enum structure) const;
        
        void setPanningMode(const ClippingPlanePanningModeEnum::Enum panningMode);
        
        ClippingPlanePanningModeEnum::Enum getPanningMode() const;
        
        void getTranslation(float translation[3]) const;
        
        void setTranslation(const float translation[3]);
        
        void getTranslationForStructure(const StructureEnum::Enum structure,
                                        float translation[3]) const;
        
        Matrix4x4 getRotationMatrixForStructure(const StructureEnum::Enum structure) const;
        
        void setRotationMatrix(const Matrix4x4& rotationMatrix);
        
        void getRotationAngles(float rotationAngles[3]) const;
        
        void setRotationAngles(const float rotationAngles[3]);
        
        void setRotation(const float rotation[4][4]);
        
        void getThickness(float thickness[3]) const;
        
        void setThickness(const float thickness[3]);
        
        bool isXAxisSelected() const;
        
        bool isYAxisSelected() const;
        
        bool isZAxisSelected() const;
        
        void setXAxisSelected(const bool xAxisSelected);
        
        void setYAxisSelected(const bool yAxisSelected);
        
        void setZAxisSelected(const bool zAxisSelected);
        
        bool isDisplayClippingBoxSelected() const;
        
        void setDisplayClippingBoxSelected(const bool selected);
        
        bool isSurfaceSelected() const;
        
        void setSurfaceSelected(const bool selected);
        
        bool isVolumeSelected() const;
        
        void setVolumeSelected(const bool selected);
        
        bool isFeaturesSelected() const;
        
        bool isFeaturesAndAnyAxisSelected() const;
        
        void setFeaturesSelected(const bool selected);
        
        bool isCoordinateInsideClippingPlanesForStructure(const StructureEnum::Enum structure,
                                                          const float xyz[3]) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
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
        enum PlaneIdentifier {
            PLANE_MINIMUM_X,
            PLANE_MAXIMUM_X,
            PLANE_MINIMUM_Y,
            PLANE_MAXIMUM_Y,
            PLANE_MINIMUM_Z,
            PLANE_MAXIMUM_Z
        };
        
        void copyHelperClippingPlaneGroup(const ClippingPlaneGroup& obj);

        Plane* createClippingPlane(const PlaneIdentifier planeIdentifier,
                                  const StructureEnum::Enum structure) const;
        
        void updateActiveClippingPlainEquations() const;
        
        void invalidateActiveClippingPlainEquations();
        
        const int32_t m_tabIndex;
        
        /**
         * For all everthing EXCEPT right structures
         */
        mutable std::vector<Plane*> m_activeClippingPlanes;
        
        /**
         * Mirror flipped for RIGHT structure
         */
        mutable std::vector<Plane*> m_rightStructureActiveClippingPlanes;
        
        mutable bool m_activeClippingPlanesValid;
        
        SceneClassAssistant* m_sceneAssistant;

        float getXCoordinateForStructure(const StructureEnum::Enum structure) const;
        
        ClippingPlanePanningModeEnum::Enum m_panningMode = ClippingPlanePanningModeEnum::PAN_XYZ;
        
        mutable float m_translation[3];
        
        Matrix4x4 m_rotationMatrix;
        
        float m_thickness[3];
        
        bool m_enabledStatus = false;
        
        bool m_xAxisSelectionStatus;
        
        bool m_yAxisSelectionStatus;
        
        bool m_zAxisSelectionStatus;
        
        bool m_surfaceSelectionStatus;
        
        bool m_volumeSelectionStatus;
        
        bool m_featuresSelectionStatus;
        
        bool m_displayClippingBoxStatus;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLIPPING_PLANE_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLIPPING_PLANE_GROUP_DECLARE__

} // namespace
#endif  //__CLIPPING_PLANE_GROUP_H__
