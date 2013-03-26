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
#include "SceneableInterface.h"

namespace caret {
    class Brain;
    class OverlaySet;
    
    /// Base class for controlling a model
    class Model : public CaretObject, public SceneableInterface {
        
    protected:
        enum YokingAllowedType {
            YOKING_ALLOWED_YES,
            YOKING_ALLOWED_NO
        };
        
        Model(const ModelTypeEnum::Enum controllerType,
                               const YokingAllowedType allowsYokingStatus,
                               Brain* brain);
        
        virtual ~Model();
        
    private:        
        Model(const Model& o);
        Model& operator=(const Model& o);
        
        void initializeMembersModel();
        
    public:
        virtual void initializeOverlays() = 0;
        
        Brain* getBrain();
        
        ModelTypeEnum::Enum getControllerType() const;
        
        virtual AString getNameForGUI(const bool includeStructureFlag) const = 0;
        
        virtual AString getNameForBrowserTab() const = 0;
        
        bool isYokeable() const;
        
        bool isYokingModel() const;
        
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
        
    private:
        ModelTypeEnum::Enum m_modelType;
        
        YokingAllowedType m_allowsYokingStatus;
        
        bool m_isYokingController;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_H__
