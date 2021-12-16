#ifndef __DISPLAY_PROPERTIES_CZI_IMAGES__H_
#define __DISPLAY_PROPERTIES_CZI_IMAGES__H_

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

#include <array>

#include "BrainConstants.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "DisplayProperties.h"

namespace caret {

    class Surface;
    
    class DisplayPropertiesCziImages : public DisplayProperties {
        
    public:
        DisplayPropertiesCziImages();
        
        virtual ~DisplayPropertiesCziImages();
        
        void reset();
        
        void update();
        
        virtual void copyDisplayProperties(const int32_t sourceTabIndex,
                                           const int32_t targetTabIndex);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        DisplayPropertiesCziImages(const DisplayPropertiesCziImages&);

        DisplayPropertiesCziImages& operator=(const DisplayPropertiesCziImages&);
        
    };
    
#ifdef __DISPLAY_PROPERTIES_CZI_IMAGES_DECLARE__
#endif // __DISPLAY_PROPERTIES_CZI_IMAGES_DECLARE__

} // namespace
#endif  //__DISPLAY_PROPERTIES_CZI_IMAGES__H_
