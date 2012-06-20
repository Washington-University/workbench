#ifndef __DISPLAY_PROPERTIES_VOLUME__H_
#define __DISPLAY_PROPERTIES_VOLUME__H_

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


#include "CaretColorEnum.h"
#include "DisplayProperties.h"

namespace caret {

    class Surface;
    class VolumeSurfaceOutlineModel;
    
    class DisplayPropertiesVolume : public DisplayProperties {
        
    public:
        DisplayPropertiesVolume(Brain* brain);
        
        virtual ~DisplayPropertiesVolume();
        
        void reset();
        
        void update();
        
        static const int32_t MAXIMUM_NUMBER_OF_SURFACE_OUTLINES;
        
        VolumeSurfaceOutlineModel* getSurfaceOutlineSelection(const int32_t indx);
        
        const VolumeSurfaceOutlineModel* getSurfaceOutlineSelection(const int32_t indx) const;
        
        void selectSurfacesAfterSpecFileLoaded(const bool searchForTabs);
        
        virtual SceneClass* saveToScene(const SceneAttributes& sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes& sceneAttributes,
                                      const SceneClass& sceneClass);
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
    private:
        DisplayPropertiesVolume(const DisplayPropertiesVolume&);

        DisplayPropertiesVolume& operator=(const DisplayPropertiesVolume&);
        
    private:
        void addSurfaceOutline(Surface* surface,
                               const float thickness,
                               const int32_t browserTabIndex,
                               const CaretColorEnum::Enum color,
                               int32_t& outlineIndex);
        
        std::vector<VolumeSurfaceOutlineModel*> volumeSurfaceOutlineModels;
    };
    
#ifdef __DISPLAY_PROPERTIES_VOLUME_DECLARE__
    const int32_t DisplayPropertiesVolume::MAXIMUM_NUMBER_OF_SURFACE_OUTLINES = 10;
#endif // __DISPLAY_PROPERTIES_VOLUME_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_VOLUME__H_
