#ifndef __CHARTABLE_TWO_FILE_BASE_CHART_H__
#define __CHARTABLE_TWO_FILE_BASE_CHART_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include <memory>

#include "ChartAxisLocationEnum.h"
#include "CaretObjectTracksModification.h"
#include "ChartTwoCompoundDataType.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationPercentSizeText;
    class CaretMappableDataFile;
    class CiftiMappableDataFile;
    class SceneClassAssistant;

    class ChartableTwoFileBaseChart : public CaretObjectTracksModification, public EventListenerInterface, public SceneableInterface {
        
    protected:
        ChartableTwoFileBaseChart(const ChartTwoDataTypeEnum::Enum chartType,
                                          CaretMappableDataFile* parentCaretMappableDataFile);
        
    public:
        virtual ~ChartableTwoFileBaseChart();
        
        /**
         * @return Is this charting valid ?
         */
        virtual bool isValid() const = 0;
        
        /**
         * @return Is this charting empty (no data at this time)
         */
        virtual bool isEmpty() const = 0;
        
        CaretMappableDataFile* getCaretMappableDataFile();
        
        const CaretMappableDataFile* getCaretMappableDataFile() const;
        
        CiftiMappableDataFile* getCiftiMappableDataFile();
        
        const CiftiMappableDataFile* getCiftiMappableDataFile() const;

        ChartTwoDataTypeEnum::Enum getChartTwoDataType() const;
        
        ChartTwoCompoundDataType getChartTwoCompoundDataType() const;
        
        const AnnotationPercentSizeText* getBottomAxisTitle() const;
        
        AnnotationPercentSizeText* getBottomAxisTitle();
        
        const AnnotationPercentSizeText* getLeftRightAxisTitle() const;
        
        AnnotationPercentSizeText* getLeftRightAxisTitle();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const override;
        
        virtual void receiveEvent(Event* event) override;

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;

          
          
          
          
          
    protected:
        void updateChartTwoCompoundDataTypeAfterFileChanges(const ChartTwoCompoundDataType compoundChartDataType);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) = 0;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) = 0;

    private:
        ChartableTwoFileBaseChart() = delete;
        
        ChartableTwoFileBaseChart(const ChartableTwoFileBaseChart&);

        ChartableTwoFileBaseChart& operator=(const ChartableTwoFileBaseChart&);
        
        void initializeAxisLabel(AnnotationPercentSizeText* axisLabel,
                                 const ChartAxisLocationEnum::Enum axisLocation);
                                 
        const ChartTwoDataTypeEnum::Enum m_chartType;
        
        CaretMappableDataFile* m_parentCaretMappableDataFile;
        
        CiftiMappableDataFile* m_parentCiftiMappableDataFile;
        
        SceneClassAssistant* m_sceneAssistant;
        
        ChartTwoCompoundDataType m_compoundChartDataType;
        
        std::unique_ptr<AnnotationPercentSizeText> m_bottomAxisTitle;
        
        std::unique_ptr<AnnotationPercentSizeText> m_leftRightAxisTitle;
        
        static float constexpr s_defaultFontPercentViewportSize = 5.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_BASE_CHART_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_BASE_CHART_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_BASE_CHART_H__
