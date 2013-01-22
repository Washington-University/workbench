#ifndef __OVERLAY_SET__H_
#define __OVERLAY_SET__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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
#include "Overlay.h"
#include "SceneableInterface.h"

namespace caret {

    class BrainStructure;
    class BrowserTabContent;
    class LabelFile;
    class Model;
    class SceneClassAssistant;
    class Surface;
    class VolumeFile;
    
    class OverlaySet : public CaretObject, public SceneableInterface {
        
    public:
        OverlaySet(BrainStructure* brainStructure);
        
        OverlaySet(ModelVolume* modelDisplayControllerVolume);
        
        OverlaySet(ModelWholeBrain* modelDisplayControllerWholeBrain);
        
        OverlaySet(ModelYokingGroup* modelDisplayControllerYokingGroup);
        
        OverlaySet(ModelSurfaceMontage* modelDisplayControllerSurfaceMontage);
        
        virtual ~OverlaySet();
        
        void copyOverlaySet(const OverlaySet* overlaySet);
        
        Overlay* getPrimaryOverlay();
        
        Overlay* getUnderlay();
        
        VolumeFile* getUnderlayVolume();
        
        Overlay* getOverlay(const int32_t overlayNumber);
        
        const Overlay* getOverlay(const int32_t overlayNumber) const;
        
        void addDisplayedOverlay();
        
        void setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays);
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        void insertOverlayAbove(const int32_t overlayIndex);
        
        void insertOverlayBelow(const int32_t overlayIndex);
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);
        
        VolumeFile* setUnderlayToVolume();
        
        void initializeOverlays();
        
        void getLabelFilesForSurface(const Surface* surface,
                                     std::vector<LabelFile*>& labelFilesOut,
                                     std::vector<int32_t>& labelMapIndicesOut);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    public:
        virtual AString toString() const;
        
    private:
        void initializeOverlaySet(Model* modelDisplayController,
                                  BrainStructure* brainStructure);

        OverlaySet(const OverlaySet&);
        
        OverlaySet& operator=(const OverlaySet&);
        
        Overlay* m_overlays[BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        Model* m_modelDisplayController;
        
        BrainStructure* m_brainStructure;
        
        int32_t m_numberOfDisplayedOverlays;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __OVERLAY_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_SET_DECLARE__

} // namespace
#endif  //__OVERLAY_SET__H_
