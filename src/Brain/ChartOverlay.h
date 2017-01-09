#ifndef __CHART_OVERLAY_H__
#define __CHART_OVERLAY_H__

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


#include "CaretObject.h"

#include "ChartTwoCompoundDataType.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationColorBar;
    class CaretMappableDataFile;
    class PlainTextStringBuilder;
    class SceneClassAssistant;

    class ChartOverlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        ChartOverlay(const ChartTwoDataTypeEnum::Enum chartDataType,
                     const int32_t overlayIndex);
        
        virtual ~ChartOverlay();
        
        void setChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType);
        
        AString getName() const;
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        void copyData(const ChartOverlay* overlay);
        
        void swapData(ChartOverlay* overlay);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        AnnotationColorBar* getColorBar();
        
        const AnnotationColorBar* getColorBar() const;
        
        void getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                              CaretMappableDataFile* &selectedMapFileOut,
                              int32_t& selectedMapIndexOut);
        
        void getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                              int32_t& selectedMapIndexOut);
        
        void setSelectionData(CaretMappableDataFile* selectedMapFile,
                              const int32_t selectedMapIndex);
        
        bool isAllMapsSelected() const;
        
        void setAllMapsSelected(const bool status);
        
        virtual void receiveEvent(Event* event);

        // ADD_NEW_METHODS_HERE
        
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
        ChartOverlay(const ChartOverlay&);

        ChartOverlay& operator=(const ChartOverlay&);
        
        /** Enumerated Type of charts allowed in this overlay */
        const ChartTwoDataTypeEnum::Enum m_chartDataType;
        
        /** Index of this overlay (DO NOT COPY)*/
        const int32_t m_overlayIndex;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        /** Current 'compound chart type' of charts allowed in this overlay */
        ChartTwoCompoundDataType m_chartCompoundDataType;
        
        /** Name of overlay (DO NOT COPY)*/
        AString m_name;
        
        /** enabled status */
        mutable bool m_enabled = true;
        
        /** map yoking group */
        MapYokingGroupEnum::Enum m_mapYokingGroup;

        /** The color bar displayed in the graphics window */
        std::unique_ptr<AnnotationColorBar> m_colorBar;
        
        /** selected mappable file */
        CaretMappableDataFile* m_selectedMapFile = NULL;
        
        /** selected map index */
        int32_t m_selectedMapIndex = -1;
        
        bool m_allMapsSelectedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_OVERLAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_OVERLAY_DECLARE__

} // namespace
#endif  //__CHART_OVERLAY_H__
