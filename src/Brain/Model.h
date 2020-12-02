#ifndef __MODEL_DISPLAY_CONTROLLER_H__
#define __MODEL_DISPLAY_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "ModelTypeEnum.h"
#include "SceneableInterface.h"

namespace caret {
    class Brain;
    class ChartTwoOverlaySet;
    class MediaOverlaySet;
    class OverlaySet;
    class PlainTextStringBuilder;
    
    /// Base class for a model
    class Model : public CaretObject, public SceneableInterface {
        
    protected:
        Model(const ModelTypeEnum::Enum modelType,
                               Brain* brain);
        
        virtual ~Model();
        
    private:        
        Model(const Model& o);
        Model& operator=(const Model& o);
        
        void initializeMembersModel();
        
    public:
        virtual void initializeOverlays() = 0;
        
        Brain* getBrain();
        
        ModelTypeEnum::Enum getModelType() const;
        
        virtual AString getNameForGUI(const bool includeStructureFlag) const = 0;
        
        virtual AString getNameForBrowserTab() const = 0;
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const;
        
        virtual OverlaySet* getOverlaySet(const int tabIndex) = 0;
        
        virtual const OverlaySet* getOverlaySet(const int tabIndex) const = 0;
        
        virtual ChartTwoOverlaySet* getChartTwoOverlaySet(const int tabIndex);
        
        virtual const ChartTwoOverlaySet* getChartTwoOverlaySet(const int tabIndex) const;
        
        virtual MediaOverlaySet* getMediaOverlaySet(const int tabIndex);
        
        virtual const MediaOverlaySet* getMediaOverlaySet(const int tabIndex) const;
        
        virtual void initializeSelectedSurfaces();

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        bool getOldSceneTransformation(const int tabIndex,
                                       float translationOut[3],
                                       float& scalingOut,
                                       float rotationMatrixOut[4][4]) const;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
        bool isRestoredFromScene() const;
        
        void setRestoredFromScene(const bool restoredStatus);
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                 SceneClass* sceneClass) = 0;
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                      const SceneClass* sceneClass) = 0;
        
        /** Brain which contains the model */
        Brain* m_brain;
        
    private:
        /**
         * Transformations in older scene files when transforms were stored
         * in each of the models for every tab.  
         */
        class OldSceneTransformation {
        public:
            float m_translation[3];
            float m_scaling;
            float m_rotationMatrix[4][4];
            bool m_translationValid;
            bool m_scalingValid;
            bool m_rotationValid;
        };
        
        ModelTypeEnum::Enum m_modelType;
        
        std::vector<OldSceneTransformation> m_oldSceneTransformations;
        
        bool m_restoredFromSceneFlag = false;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_H__
