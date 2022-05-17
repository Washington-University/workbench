#ifndef __MODEL_WHOLE_BRAIN_H__
#define __MODEL_WHOLE_BRAIN_H__

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

#include <set>

#include "BrainConstants.h"
#include "EventListenerInterface.h"
#include "Model.h"
#include "StructureEnum.h"
#include "SurfaceTypeEnum.h"

namespace caret {

    class Brain;
    class OverlaySetArray;
    class Surface;
    class SceneClassAssistant;
    class VolumeMappableInterface;
    
    /// Controls the display of a whole brain.
    class ModelWholeBrain : public Model, public EventListenerInterface {
        
    public:
        ModelWholeBrain(Brain* brain);
        
        virtual ~ModelWholeBrain();
        
        VolumeMappableInterface* getUnderlayVolumeFile(const int32_t windowTabNumber) const;
        
        void getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum>& surfaceTypesOut);
        
        SurfaceTypeEnum::Enum getSelectedSurfaceType(const int32_t windowTabNumber);
        
        void setSelectedSurfaceType(const int32_t windowTabNumber,
                                    const SurfaceTypeEnum::Enum surfaceType);
        
        std::vector<Surface*> getSelectedSurfaces(const int32_t windowTabNumber);
        
        Surface* getSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber);
        
        void setSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber,
                                    Surface* surface);
        
        void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
                                            
        virtual void initializeSelectedSurfaces();
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
        void updateModel();
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
    private:
        ModelWholeBrain(const ModelWholeBrain&);
        
        ModelWholeBrain& operator=(const ModelWholeBrain&);
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Type of surface for display */
        mutable SurfaceTypeEnum::Enum m_selectedSurfaceType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Selected surface for structure/surface-type */
        std::map<std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum>, Surface*> m_selectedSurface[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Available surface types */
        std::set<SurfaceTypeEnum::Enum> m_availableSurfaceTypes;
        
        bool m_leftEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_rightEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_cerebellumEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_leftRightSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        float m_cerebellumSeparation[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Surface types that have an anatomical appearance. */
        static std::vector<SurfaceTypeEnum::Enum> s_anatomicalSurfaceTypes;
        
        VolumeMappableInterface* m_lastVolumeFile;

        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_overlaySetArray;
        
        SceneClassAssistant* m_sceneAssistant;
        
    };

#ifdef __MODEL_WHOLE_BRAIN_DEFINE__
    std::vector<SurfaceTypeEnum::Enum> ModelWholeBrain::s_anatomicalSurfaceTypes;
#endif // __MODEL_WHOLE_BRAIN_DEFINE__
    
} // namespace


#endif // __MODEL_WHOLE_BRAIN_H__
