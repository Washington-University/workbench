#ifndef __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
#define __MODEL_DISPLAY_CONTROLLER_VOLUME_H__

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

namespace caret {

    class Brain;
    class SceneClassAssistant;
    class OverlaySetArray;
    class VolumeMappableInterface;
    
    /// Controls the display of a volumes.
    class ModelVolume : public Model, public EventListenerInterface {
        
    public:        
        ModelVolume(Brain* brain);
        
        virtual ~ModelVolume();
        
        VolumeMappableInterface* getUnderlayVolumeFile(const int32_t windowTabNumber) const;
        
        void updateController(const int32_t windowTabNumber);
        
        void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        virtual AString toDescriptiveString() const;
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
    private:
        ModelVolume(const ModelVolume&);
        
        ModelVolume& operator=(const ModelVolume&);
        
    private:
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        VolumeMappableInterface* m_lastVolumeFile;

        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_overlaySetArray;
        
        SceneClassAssistant* m_sceneAssistant;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_VOLUME_H__
