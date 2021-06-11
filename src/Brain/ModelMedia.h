#ifndef __MODEL_MEDIA_H__
#define __MODEL_MEDIA_H__

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
#include <memory>

#include "BrainConstants.h"
#include "CaretDataFileSelectionModel.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "Model.h"

namespace caret {

    class BrowserTabContent;
    class CaretDataFileSelectionModel;
    class GraphicsRegionSelectionBox;
    class MediaOverlaySet;
    class MediaOverlaySetArray;
    class OverlaySetArray;

    /// Controls the display of a chart.
    class ModelMedia : public Model, public EventListenerInterface  {
        
    public:
        ModelMedia(Brain* brain);
        
        virtual ~ModelMedia();
        
        virtual void initializeOverlays() override;
        
        void updateModel();
        
        virtual AString getNameForGUI(const bool includeStructureFlag) const override;
        
        virtual AString getNameForBrowserTab() const override;
                
        virtual OverlaySet* getOverlaySet(const int tabIndex) override;
        
        virtual const OverlaySet* getOverlaySet(const int tabIndex) const override;

        virtual MediaOverlaySet* getMediaOverlaySet(const int tabIndex) override;
        
        virtual const MediaOverlaySet* getMediaOverlaySet(const int tabIndex) const override;
        
        CaretDataFileSelectionModel* getFileSelectionModel(const int32_t tabIndex);
        
        const CaretDataFileSelectionModel* getFileSelectionModel(const int32_t tabIndex) const;
        
        bool isHighResolutionSelectionEnabled(const int32_t tabIndex);
        
        void setHighResolutionSelectionEnabled(const int32_t tabIndex,
                                               const bool enabled);
        
        bool createHighResolutionImageFromRegion(const GraphicsRegionSelectionBox* selectionBox,
                                                 BrowserTabContent* browserTab,
                                                 AString& errorMessageOut);
        
        virtual void receiveEvent(Event* event) override;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex) override;
        
        void reset();
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass) override;
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass) override;
        
    private:
        ModelMedia(const ModelMedia&);
        
        ModelMedia& operator=(const ModelMedia&);
        
        void initializeMedia();
        
        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_layerOverlaySetArray;
        
        /** Overlays sets for this model and for each tab */
        MediaOverlaySetArray* m_mediaOverlaySetArray;
        
        std::unique_ptr<CaretDataFileSelectionModel> m_fileSelectionModels[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** enables high-resolution selection for a tab;  NOT saved to scenes */
        std::array<bool, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS> m_highResolutionSelectedEnabled;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
    };

} // namespace

#endif // __MODEL_MEDIA_H__
