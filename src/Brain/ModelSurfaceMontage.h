#ifndef __MODEL_SURFACE_MONTAGE_H__
#define __MODEL_SURFACE_MONTAGE_H__

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


#include "EventListenerInterface.h"
#include "Model.h"
#include "StructureEnum.h"
#include "SurfaceMontageViewport.h"
#include "SurfaceMontageConfigurationTypeEnum.h"

namespace caret {

    class SurfaceMontageConfigurationAbstract;
    class SurfaceMontageConfigurationCerebellar;
    class SurfaceMontageConfigurationCerebral;
    class SurfaceMontageConfigurationFlatMaps;
    
    /// Controls the display of a surface montage
    class ModelSurfaceMontage : public Model, public EventListenerInterface  {
        
    public:
        ModelSurfaceMontage(Brain* brain);
        
        virtual ~ModelSurfaceMontage();
        
        virtual void initializeSelectedSurfaces();
        
        virtual void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        Surface* getSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber);
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void getSurfaceMontageViewportsForDrawing(const int32_t tabIndex,
                                                  std::vector<SurfaceMontageViewport*>& surfaceMontageViewports);
        
        void getSurfaceMontageViewportsForTransformation(const int32_t tabIndex,
                                                         std::vector<const SurfaceMontageViewport*>& surfaceMontageViewports) const;
        
        SurfaceMontageConfigurationTypeEnum::Enum getSelectedConfigurationType(const int32_t tabIndex) const;
        
        void setSelectedConfigurationType(const int32_t tabIndex,
                                          const SurfaceMontageConfigurationTypeEnum::Enum configurationType);
        
        SurfaceMontageConfigurationAbstract* getSelectedConfiguration(const int32_t tabIndex);
        
        const SurfaceMontageConfigurationAbstract* getSelectedConfiguration(const int32_t tabIndex) const;
        
        SurfaceMontageConfigurationCerebellar * getCerebellarConfiguration(const int32_t tabIndex);
        
        const SurfaceMontageConfigurationCerebellar* getCerebellarConfiguration(const int32_t tabIndex) const;
        
        SurfaceMontageConfigurationCerebral * getCerebralConfiguration(const int32_t tabIndex);
        
        const SurfaceMontageConfigurationCerebral* getCerebralConfiguration(const int32_t tabIndex) const;
        
        SurfaceMontageConfigurationFlatMaps * getFlatMapsConfiguration(const int32_t tabIndex);
        
        const SurfaceMontageConfigurationFlatMaps* getFlatMapsConfiguration(const int32_t tabIndex) const;
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
    private:
        ModelSurfaceMontage(const ModelSurfaceMontage&);
        
        ModelSurfaceMontage& operator=(const ModelSurfaceMontage&);
        
        void restoreFromSceneVersionTwoAndEarlier(const SceneAttributes* sceneAttributes,
                                               const SceneClass* sceneClass,
                                                  const int32_t montageVersion);
        
        SurfaceMontageConfigurationCerebellar* m_cerebellarConfiguration[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceMontageConfigurationCerebral* m_cerebralConfiguration[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceMontageConfigurationFlatMaps* m_flatMapsConfiguration[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceMontageConfigurationTypeEnum::Enum m_selectedConfigurationType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };

} // namespace

#endif // __MODEL_SURFACE_MONTAGE_H__
