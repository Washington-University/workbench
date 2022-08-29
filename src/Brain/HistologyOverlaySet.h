#ifndef __HISTOLOGY_OVERLAY_SET__H_
#define __HISTOLOGY_OVERLAY_SET__H_

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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

namespace caret {

    class BrowserTabContent;
    class HistologyOverlay;
    class HistologySlicesFile;
    class SceneClassAssistant;
    class PlainTextStringBuilder;
    
    class HistologyOverlaySet : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        HistologyOverlaySet(const AString& name,
                            const int32_t tabIndex);
        
        virtual ~HistologyOverlaySet();
        
        virtual void receiveEvent(Event* event);
        
        void copyHistologyOverlaySet(const HistologyOverlaySet* overlaySet);
        
        HistologyOverlay* getPrimaryOverlay();
        
        HistologyOverlay* getUnderlay();
        
        HistologyOverlay* getBottomMostEnabledOverlay();
        
        HistologyOverlay* getOverlay(const int32_t overlayNumber);
        
        const HistologyOverlay* getOverlay(const int32_t overlayNumber) const;
        
        void addDisplayedOverlay();
        
        void setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays);
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        void insertOverlayAbove(const int32_t overlayIndex);
        
        void insertOverlayBelow(const int32_t overlayIndex);
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);
        
        void initializeOverlays();
        
        HistologySlicesFile* getBottomMostHistologySlicesFile();
        
        void getSelectedIndicesForFile(const HistologySlicesFile* HistologySlicesFile,
                                       const bool isLimitToEnabledOverlays,
                                       std::vector<int32_t>& selectedIndicesOut) const;
        
        void resetOverlayYokingToOff();
        
        std::vector<HistologySlicesFile*> getDisplayedHistologySlicesFiles() const;
        
        void getDisplayedHistologySlicesFileAndOverlayIndices(std::vector<HistologySlicesFile*>& histologySlicesFilesOut,
                                                              std::vector<int32_t>& overlayIndicesOut) const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    public:
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
    private:
        
        HistologyOverlaySet(const HistologyOverlaySet&);
        
        HistologyOverlaySet& operator=(const HistologyOverlaySet&);
        
        HistologyOverlay* m_overlays[BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        AString m_name;
        
        int32_t m_tabIndex;
        
        /** Surface structures of data files displayed in this overlay */
        int32_t m_numberOfDisplayedOverlays;
        
        SceneClassAssistant* m_sceneAssistant;

    };
    
#ifdef __HISTOLOGY_OVERLAY_SET_DECLARE__
#endif // __HISTOLOGY_OVERLAY_SET_DECLARE__

} // namespace
#endif  //__HISTOLOGY_OVERLAY_SET__H_
