#ifndef __CHART_MODEL_H__
#define __CHART_MODEL_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <deque>

#include "CaretObject.h"
#include "ChartDataTypeEnum.h"
#include "SceneableInterface.h"


namespace caret {

    class ChartAxis;
    class ChartData;
    class SceneClassAssistant;
    
    class ChartModel : public CaretObject, public SceneableInterface {
        
    public:
        /** 
         * Does this type of chart allow the display of multiple charts
         * of the same chart data type?
         */
        enum SelectionMode {
            /** Allows only one chart */
            SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES,
            /** Allows unlimited charts of the same chart data model type */
            SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO,
        };
        
        ChartModel(const ChartDataTypeEnum::Enum chartDataType,
                   const SelectionMode selectionMode);
        
        ChartModel(const ChartModel&);
        
        ChartModel& operator=(const ChartModel&);
        
        virtual ~ChartModel();
        
        void removeChartData();
        
        ChartDataTypeEnum::Enum getChartDataType() const;
        
        SelectionMode getSelectionMode() const;
        
        void addChartData(const ChartData* chartDataIn);
        
        virtual int32_t getMaximumNumberOfChartDatasToDisplay() const;
        
        void setMaximumNumberOfChartDatasToDisplay(const int32_t numberToDisplay);
        
        std::vector<const ChartData*> getAllChartDatas() const;
        
        std::vector<ChartData*> getAllChartDatas();
        
        std::vector<const ChartData*> getAllSelectedChartDatas() const;
        
        /**
         * @return Is an average of data supported?
         */
        virtual bool isAverageChartDisplaySupported() const = 0;
        
        /**
         * @return The average chart data.  Will return NULL if either
         * no data to average or model does not support an average.
         */
        virtual const ChartData* getAverageChartDataForDisplay() const = 0;
        
        bool isAverageChartDisplaySelected() const;
        
        void setAverageChartDisplaySelected(const bool selected);
        
        ChartAxis* getLeftAxis();
        
        const ChartAxis* getLeftAxis() const;

        ChartAxis* getRightAxis();
        
        const ChartAxis* getRightAxis() const;
        
        ChartAxis* getBottomAxis();
        
        const ChartAxis* getBottomAxis() const;
        
        ChartAxis* getTopAxis();
        
        const ChartAxis* getTopAxis() const;
        
        /**
         * Reset the axis to the default range for the data.
         */
        virtual void resetAxesToDefaultRange() = 0;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        void setChartDataSelected(ChartData* chartData,
                                  const bool selectionStatus);
        
        bool isChartDataSelected(const ChartData* chartData) const;
        
    private:
        void copyHelperChartModel(const ChartModel& obj);
        
        ChartDataTypeEnum::Enum m_chartDataType;
        
        SelectionMode m_selectionMode;
        
        std::deque<ChartData*> m_chartDatas;
        
        int32_t m_maximumNumberOfChartDatasToDisplay;
        
        ChartAxis* m_leftAxis;
        
        ChartAxis* m_rightAxis;
        
        ChartAxis* m_bottomAxis;
        
        ChartAxis* m_topAxis;
        
        bool m_averageChartDisplaySelected;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_MODEL_DECLARE__

} // namespace
#endif  //__CHART_MODEL_H__
