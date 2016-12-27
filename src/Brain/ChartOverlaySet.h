#ifndef __CHART_OVERLAY_SET_H__
#define __CHART_OVERLAY_SET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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
#include "ChartDataTypeEnum.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class ChartOverlay;
    class PlainTextStringBuilder;
    class SceneClassAssistant;

    class ChartOverlaySet : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        ChartOverlaySet(const ChartDataTypeEnum::Enum chartDataType,
                        const AString& name,
                        const int32_t tabIndex);
        
        virtual ~ChartOverlaySet();
        
        void copyOverlaySet(const ChartOverlaySet* overlaySet);
        
        ChartOverlay* getPrimaryOverlay();
        
        ChartOverlay* getUnderlay();
        
        ChartOverlay* getOverlay(const int32_t overlayNumber);
        
        const ChartOverlay* getOverlay(const int32_t overlayNumber) const;
        
        void addDisplayedOverlay();
        
        void setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays);
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        void insertOverlayAbove(const int32_t overlayIndex);
        
        void insertOverlayBelow(const int32_t overlayIndex);
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);
        
        void initializeOverlays();
        
        void resetOverlayYokingToOff();

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        ChartOverlaySet(const ChartOverlaySet&);

        ChartOverlaySet& operator=(const ChartOverlaySet&);
        
        SceneClassAssistant* m_sceneAssistant;

        ChartOverlay* m_overlays[BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        const ChartDataTypeEnum::Enum m_chartDataType;
        
        const AString m_name;
        
        const int32_t m_tabIndex;
        
        int32_t m_numberOfDisplayedOverlays;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_OVERLAY_SET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_OVERLAY_SET_DECLARE__

} // namespace
#endif  //__CHART_OVERLAY_SET_H__
