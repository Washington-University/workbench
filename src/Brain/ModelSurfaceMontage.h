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

namespace caret {

    class SurfaceSelectionModel;
    
    /// Controls the display of a surface montage
    class ModelSurfaceMontage : public Model, public EventListenerInterface  {
        
    public:
        ModelSurfaceMontage(Brain* brain);
        
        virtual ~ModelSurfaceMontage();
        
        void initializeSurfaces();
        
        virtual void resetView(const int32_t windowTabNumber);
        
        virtual void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        SurfaceSelectionModel* getLeftSurfaceSelectionModel(const int tabIndex);
        
        SurfaceSelectionModel* getLeftSecondSurfaceSelectionModel(const int tabIndex);
        
        SurfaceSelectionModel* getRightSurfaceSelectionModel(const int tabIndex);
        
        SurfaceSelectionModel* getRightSecondSurfaceSelectionModel(const int tabIndex);
        
        Surface* getSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber);
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void setDefaultScalingToFitWindow();
        
        bool isLeftEnabled(const int tabIndex) const;
        
        void setLeftEnabled(const int tabIndex,
                                    const bool enabled);
        
        bool isRightEnabled(const int tabIndex) const;
        
        void setRightEnabled(const int tabIndex,
                            const bool enabled);
        
        bool isFirstSurfaceEnabled(const int tabIndex) const;
        
        void setFirstSurfaceEnabled(const int tabIndex,
                                         const bool enabled);
        
        bool isSecondSurfaceEnabled(const int tabIndex) const;
        
        void setSecondSurfaceEnabled(const int tabIndex,
                                    const bool enabled);
        
        virtual void copyTransformationsAndViews(const Model& controller,
                                                 const int32_t windowTabNumberSource,
                                                 const int32_t windowTabNumberTarget);
        
        void setMontageViewports(const int32_t tabIndex,
                                 const std::vector<SurfaceMontageViewport>& montageViewports);
        
        void getMontageViewports(const int32_t tabIndex,
                                 std::vector<SurfaceMontageViewport>& montageViewportsOut);
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
    private:
        ModelSurfaceMontage(const ModelSurfaceMontage&);
        
        ModelSurfaceMontage& operator=(const ModelSurfaceMontage&);
        
        SurfaceSelectionModel* m_leftSurfaceSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceSelectionModel* m_leftSecondSurfaceSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceSelectionModel* m_rightSurfaceSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SurfaceSelectionModel* m_rightSecondSurfaceSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_leftEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_rightEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_firstSurfaceEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_secondSurfaceEnabled[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        std::vector<SurfaceMontageViewport> m_montageViewports[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Overlays sets for this model and for each tab */
        OverlaySet* m_overlaySet[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };

} // namespace

#endif // __MODEL_SURFACE_MONTAGE_H__
