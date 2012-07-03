#ifndef __MODEL_DISPLAY_CONTROLLER_H__
#define __MODEL_DISPLAY_CONTROLLER_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "BrainConstants.h"
#include "CaretObject.h"
#include "ModelTypeEnum.h"
#include "Matrix4x4.h"
#include "SceneableInterface.h"

namespace caret {
    class Brain;
    class UserView;
    class OverlaySet;
    
    /// Base class for controlling a model
    class Model : public CaretObject, public SceneableInterface {
        
    protected:
        enum YokingAllowedType {
            YOKING_ALLOWED_YES,
            YOKING_ALLOWED_NO
        };
        
        enum RotationAllowedType {
            ROTATION_ALLOWED_YES,
            ROTATION_ALLOWED_NO
        };
        
        Model(const ModelTypeEnum::Enum controllerType,
                               const YokingAllowedType allowsYokingStatus,
                               const RotationAllowedType allowsRotationStatus,
                               Brain* brain);
        
        virtual ~Model();
        
    private:        
        Model(const Model& o);
        Model& operator=(const Model& o);
        
        void initializeMembersModel();
        
    public:
        /**
         * Index for getting the viewing transformation.
         */
        enum ViewingTransformIndex {
            /** For normal viewing modes (and surface montage left) */
            VIEWING_TRANSFORM_NORMAL = 0,
            /** For right surface lateral/medial yoked to left surface */
            VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED = 1,
            /** Left surface opposing view in surface montage */
            VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE = 2,
            /** Right surface view in surface montage */
            VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT = 3,
            /** Right surface opposing view in surface montage */
            VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE = 4,
            /** Number of rotation matrices */
            VIEWING_TRANSFORM_COUNT = 5
        };
        
        virtual void initializeOverlays() = 0;
        
        Brain* getBrain();
        
        ModelTypeEnum::Enum getControllerType() const;
        
        virtual AString getNameForGUI(const bool includeStructureFlag) const = 0;
        
        virtual AString getNameForBrowserTab() const = 0;
        
        bool isRotationAllowed() const;
        
        bool isYokeable() const;
        
        bool isYokingModel() const;
        
        virtual void copyTransformationsAndViews(const Model& controller,
                                 const int32_t windowTabNumberSource,
                                 const int32_t windowTabNumberTarget);
        
        virtual Matrix4x4* getViewingRotationMatrix(const int32_t windowTabNumber,
                                            const ViewingTransformIndex viewingTransformIndex);
        
        virtual const Matrix4x4* getViewingRotationMatrix(const int32_t windowTabNumber,
                                                  const ViewingTransformIndex viewingTransformIndex) const;
        
        virtual const float* getTranslation(const int32_t windowTabNumber,
                                    const ViewingTransformIndex viewingTransformIndex) const;
        
        virtual void setTranslation(const int32_t windowTabNumber,
                            const float t[3]);
        
        virtual void setTranslation(const int32_t windowTabNumber,
                            const float tx,
                            const float ty,
                            const float tz);
        
        virtual void setTranslation(const int32_t windowTabNumber,
                            const ViewingTransformIndex viewingTransformIndex,
                            const float tx,
                            const float ty,
                            const float tz);
        
        virtual float getScaling(const int32_t windowTabNumber) const;
        
        virtual void setScaling(const int32_t windowTabNumber,
                        const float s);
        
        virtual void resetView(const int32_t windowTabNumber);
        
        virtual void rightView(const int32_t windowTabNumber);
        
        virtual void leftView(const int32_t windowTabNumber);
        
        virtual void anteriorView(const int32_t windowTabNumber);
        
        virtual void posteriorView(const int32_t windowTabNumber);
        
        virtual void dorsalView(const int32_t windowTabNumber);
        
        virtual void ventralView(const int32_t windowTabNumber);
                
        void getTransformationsInUserView(const int32_t windowTabNumber,
                                          UserView& userView) const;
        
        void setTransformationsFromUserView(const int32_t windowTabNumber,
                                            const UserView& userView);
        
        virtual AString toString() const;
        
        virtual AString toDescriptiveString() const;
        
        virtual OverlaySet* getOverlaySet(const int tabIndex) = 0;
        
        virtual const OverlaySet* getOverlaySet(const int tabIndex) const = 0;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        void resetViewPrivate(const int windowTabNumber);
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                 SceneClass* sceneClass) = 0;
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                      const SceneClass* sceneClass) = 0;
        
        /** Brain which contains the controller */
        Brain* m_brain;
        
        float m_defaultModelScaling;
        
        /** 
         * Rotation matrix.
         */
        Matrix4x4 m_viewingRotationMatrix[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS][VIEWING_TRANSFORM_COUNT];
        
        /**translation. */
        float m_translation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS][VIEWING_TRANSFORM_COUNT][3];
        
        /**scaling. */
        float m_scaling[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    private:
        ModelTypeEnum::Enum m_modelType;
        
        YokingAllowedType m_allowsYokingStatus;
        
        RotationAllowedType m_allowsRotationStatus;
        
        bool m_isYokingController;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_H__
