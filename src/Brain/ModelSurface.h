#ifndef __MODEL_SURFACE_H__
#define __MODEL_SURFACE_H__

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


#include "EventListenerInterface.h"
#include "Model.h"

namespace caret {

    class Surface;
    
    /// Controls the display of a surface.
    class ModelSurface : public Model, public EventListenerInterface  {
        
    public:
        ModelSurface(Brain* brain,
                                      Surface* surface);
        
        virtual ~ModelSurface();
        
        virtual void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        virtual void getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
    private:
        ModelSurface(const ModelSurface&);
        
        ModelSurface& operator=(const ModelSurface&);
        
    private:
        void initializeMembersModelSurface();
        
    public:
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        //void setDefaultScalingToFitWindow();

    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
        
    private:
        /**Surface that uses this model */
        Surface* m_surface;
        
    };

} // namespace

#endif // __MODEL_SURFACE_H__
