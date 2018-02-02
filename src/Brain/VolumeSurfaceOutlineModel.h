#ifndef __VOLUME_SURFACE_OUTLINE_MODEL__H_
#define __VOLUME_SURFACE_OUTLINE_MODEL__H_

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

#include "CaretObject.h"
#include "SceneableInterface.h"

namespace caret {

    class Surface;
    class SceneAttributes;
    class SceneClassAssistant;
    class SurfaceSelectionModel;
    class VolumeSurfaceOutlineColorOrTabModel;
    
    class VolumeSurfaceOutlineModel : public CaretObject, public SceneableInterface {
        
    public:
        VolumeSurfaceOutlineModel();
        
        virtual ~VolumeSurfaceOutlineModel();
        
        void copyVolumeSurfaceOutlineModel(VolumeSurfaceOutlineModel* modelToCopy);
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);
        
        float getThicknessPercentageViewportHeight() const;
        
        void setThicknessPercentageViewportHeight(const float thickness);
        
        float getThicknessPixelsObsolete() const;
        
        void setThicknessPixelsObsolete(const float thickness);
        
        SurfaceSelectionModel* getSurfaceSelectionModel();
        
        const Surface* getSurface() const;
        
        Surface* getSurface();
        
        VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel();
        
        const VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        static const float DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE;
        static const float DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT;
        
    private:
        VolumeSurfaceOutlineModel(const VolumeSurfaceOutlineModel&);

        VolumeSurfaceOutlineModel& operator=(const VolumeSurfaceOutlineModel&);
        
    public:
        virtual AString toString() const;
        
    private:
        bool m_displayed;
        
        float m_thicknessPixelsObsolete;
        
        float m_thicknessPercentageViewportHeight;
        
        SurfaceSelectionModel* m_surfaceSelectionModel;
        
        VolumeSurfaceOutlineColorOrTabModel* m_colorOrTabModel;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__
    const float VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE = 2.0;
    const float VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT = 1.0;
#endif // __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_MODEL__H_
