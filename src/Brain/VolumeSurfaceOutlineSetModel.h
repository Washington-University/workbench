#ifndef __VOLUME_SURFACE_OUTLINE_SET_MODEL__H_
#define __VOLUME_SURFACE_OUTLINE_SET_MODEL__H_

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

#include <utility>

#include "BrainConstants.h"
#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class Brain;
    class Surface;
    class SceneAttributes;
    class SceneClassAssistant;
    class VolumeSurfaceOutlineModel;
    
    class VolumeSurfaceOutlineSetModel : public CaretObject, public SceneableInterface {
        
    public:
        VolumeSurfaceOutlineSetModel();
        
        virtual ~VolumeSurfaceOutlineSetModel();
        
        void copyVolumeSurfaceOutlineSetModel(VolumeSurfaceOutlineSetModel* setModel);
        
        int32_t getNumberOfDislayedVolumeSurfaceOutlines() const;
        
        void setNumberOfDisplayedVolumeSurfaceOutlines(const int32_t numberDisplayed);
        
        VolumeSurfaceOutlineModel* getVolumeSurfaceOutlineModel(const int32_t indx);
        
        const VolumeSurfaceOutlineModel* getVolumeSurfaceOutlineModel(const int32_t indx) const;
        
        void selectSurfacesAfterSpecFileLoaded(Brain* brain,
                                               const bool searchForTabs);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        VolumeSurfaceOutlineSetModel(const VolumeSurfaceOutlineSetModel&);

        VolumeSurfaceOutlineSetModel& operator=(const VolumeSurfaceOutlineSetModel&);
        
        void addSurfaceOutline(Surface* surface,
                               const int32_t browserTabIndex,
                               const CaretColorEnum::Enum color,
                               int32_t& outlineIndex);
        
        std::pair<Surface*,Surface*> getHippoInnerOuterSurfaces(Brain* brain,
                                                                const StructureEnum::Enum structure);

        VolumeSurfaceOutlineModel* m_outlineModels[BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES];
        
        int32_t m_numberOfDisplayedVolumeSurfaceOutlines;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_SET_MODEL__H_
