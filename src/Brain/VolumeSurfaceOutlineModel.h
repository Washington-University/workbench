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

#include <map>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModelCacheKey.h"

namespace caret {

    class GraphicsPrimitive;
    class HistologySlice;
    class Surface;
    class SceneAttributes;
    class SceneClassAssistant;
    class SurfaceSelectionModel;
    class VolumeMappableInterface;
    class VolumeSurfaceOutlineModelCacheValue;
    
    class VolumeSurfaceOutlineModel : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        VolumeSurfaceOutlineModel();
        
        virtual ~VolumeSurfaceOutlineModel();
        
        void copyVolumeSurfaceOutlineModel(VolumeSurfaceOutlineModel* modelToCopy);
        
        virtual void receiveEvent(Event* event) override;
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);
        
        float getThicknessPercentageViewportHeight() const;
        
        void setThicknessPercentageViewportHeight(const float thickness);
        
        float getThicknessPixelsObsolete() const;
        
        void setThicknessPixelsObsolete(const float thickness);
        
        float getSlicePlaneDepth() const;
        
        void setSlicePlaneDepth(const float depth);
        
        float getUserOutlineSlicePlaneDepthSeparation() const;
        
        void setUserOutlineSlicePlaneDepthSeparation(const float depthSeparation);
        
        SurfaceSelectionModel* getSurfaceSelectionModel();
        
        const Surface* getSurface() const;
        
        Surface* getSurface();
        
        VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel();
        
        const VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel() const;
        
        void setOutlineCachePrimitives(const HistologySlice*          histologySlice,
                                       const VolumeMappableInterface* underlayVolume,
                                       const VolumeSurfaceOutlineModelCacheKey& key,
                                       const std::vector<GraphicsPrimitive*>& primitives);
        
        bool getOutlineCachePrimitives(const HistologySlice*          histologySlice,
                                       const VolumeMappableInterface* underlayVolume,
                                       const VolumeSurfaceOutlineModelCacheKey& key,
                                       std::vector<GraphicsPrimitive*>& primitivesOut);
        
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
        void clearOutlineCache();
        
        bool m_displayed;
        
        float m_thicknessPixelsObsolete;
        
        float m_thicknessPercentageViewportHeight;
        
        float m_slicePlaneDepth = 0.0;
        
        float m_userOutlineSlicePlaneDepthSeparation = 0.0;
        
        SurfaceSelectionModel* m_surfaceSelectionModel;
        
        VolumeSurfaceOutlineColorOrTabModel* m_colorOrTabModel;
        
        SceneClassAssistant* m_sceneAssistant;
        
        class OutlineCacheInfo {
        public:
            OutlineCacheInfo();
            
            ~OutlineCacheInfo();
            
            void clear();
            
            bool isValid(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                         const HistologySlice*          histologySlice,
                         const VolumeMappableInterface* underlayVolume);
            
            void update(VolumeSurfaceOutlineModel* surfaceOutlineModel,
                        const HistologySlice*      histologySlice,
                        const VolumeMappableInterface* underlayVolume);
            
            /** The histology slice */
            const HistologySlice* m_histologySlice;
            
            /** The underlay volume */
            const VolumeMappableInterface* m_underlayVolume;
            
            /** Thickness when first outline is added to outline cache */
            float m_thicknessPercentageViewportHeight = -1.0;
            
            /** slice plane depth when first added to cache */
            float m_slicePlaneDepth = 0.0;
            
            /** User can override outline depth separation */
            float m_userOutlineSlicePlaneDepthSeparation = 0.0;
            
            /** Surface when first outline is added to outline cache */
            Surface* m_surface = NULL;
            
            /** Color Or Tab selection item when first outline is added to cache */
            std::unique_ptr<VolumeSurfaceOutlineColorOrTabModel::Item> m_colorItem;
        };
        
        /** info about outline cache that tracks validity of cache */
        OutlineCacheInfo m_outlineCacheInfo;
        
        /** Cache for volume surface outlines */
        std::map<VolumeSurfaceOutlineModelCacheKey, VolumeSurfaceOutlineModelCacheValue*> m_outlineCache;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__
    const float VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE = 2.0f;
    const float VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT = 0.4f;
#endif // __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_MODEL__H_
