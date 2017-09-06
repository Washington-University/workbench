#ifndef __CHART_TWO_LINE_SERIES_HISTORY_H__
#define __CHART_TWO_LINE_SERIES_HISTORY_H__

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

#include <deque>

#include "BrainConstants.h"
#include "CaretColorEnum.h"
#include "CaretObjectTracksModification.h"

#include "SceneableInterface.h"


namespace caret {
    class BoundingBox;
    class ChartTwoDataCartesian;
    class SceneClassAssistant;

    class ChartTwoLineSeriesHistory : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ChartTwoLineSeriesHistory();
        
        virtual ~ChartTwoLineSeriesHistory();
        
        ChartTwoLineSeriesHistory(const ChartTwoLineSeriesHistory& obj);

        ChartTwoLineSeriesHistory& operator=(const ChartTwoLineSeriesHistory& obj);

        bool isLoadingEnabled() const;
        
        void setLoadingEnabled(const bool enabled);
        
        CaretColorEnum::Enum getDefaultColor() const;
        
        void setDefaultColor(const CaretColorEnum::Enum defaultColor);
        
        float getDefaultLineWidth() const;
        
        void setDefaultLineWidth(const float defaultLineWidth);
        
        int32_t getDisplayCount() const;
        
        void setDisplayCount(const int32_t count);
        
        int32_t getHistoryCount() const;

        void addHistoryItem(ChartTwoDataCartesian* historyItem);
        
        ChartTwoDataCartesian* getHistoryItem(const int32_t index);
        
        const ChartTwoDataCartesian* getHistoryItem(const int32_t index) const;
        
        void removeHistoryItem(const int32_t index);
        
        void clearHistory();
        
        bool getBounds(BoundingBox& boundingBoxOut) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const override;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;

        virtual bool isModified() const override;
        
        virtual void clearModified() override;
          
        static int32_t getMaximumRetainedHistoryCount();
          
          
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
        void copyHelperChartTwoLineSeriesHistory(const ChartTwoLineSeriesHistory& obj);

        void initializeInstance();

        void updateDisplayedHistoryItems();
        
        void addHistoryItemNoDefaults(ChartTwoDataCartesian* historyItem);
        
        static CaretColorEnum::Enum generateDefaultColor();
        
        void validateDefaultColor();

        bool m_loadingEnabled = false;
        
        SceneClassAssistant* m_sceneAssistant = NULL;

        std::deque<ChartTwoDataCartesian*> m_chartHistory;
        
        CaretColorEnum::Enum m_defaultColor = CaretColorEnum::BLUE;
        
        float m_defaultLineWidth;
        
        int32_t m_displayCount;
        
        static int32_t s_defaultColorIndexGenerator;
        
        static const int32_t s_maximumRetainedHistoryCount;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_LINE_SERIES_HISTORY_DECLARE__
    int32_t ChartTwoLineSeriesHistory::s_defaultColorIndexGenerator = 0;
    const int32_t ChartTwoLineSeriesHistory::s_maximumRetainedHistoryCount = 20;
#endif // __CHART_TWO_LINE_SERIES_HISTORY_DECLARE__

} // namespace
#endif  //__CHART_TWO_LINE_SERIES_HISTORY_H__
