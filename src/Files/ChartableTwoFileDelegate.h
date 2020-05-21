#ifndef __CHARTABLE_TWO_FILE_DELEGATE_H__
#define __CHARTABLE_TWO_FILE_DELEGATE_H__

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

#include <memory>

#include "CaretObjectTracksModification.h"
#include "ChartTwoCompoundDataType.h"
#include "ChartTwoDataTypeEnum.h"
#include "SceneableInterface.h"

namespace caret {

    class CaretMappableDataFile;
    class ChartableTwoFileHistogramChart;
    class ChartableTwoFileLineLayerChart;
    class ChartableTwoFileLineSeriesChart;
    class ChartableTwoFileMatrixChart;
    
    class ChartableTwoFileDelegate : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        ChartableTwoFileDelegate(CaretMappableDataFile* caretMappableDataFile);
        
        virtual ~ChartableTwoFileDelegate();
        
        void updateAfterFileChanged();
        
        // do not make this virtual since called from constructor
        void clear();
        
        virtual void clearModified() override;
        
        virtual bool isModified() const override;
        
        CaretMappableDataFile* getCaretMappableDataFile();
        
        const CaretMappableDataFile* getCaretMappableDataFile() const;
        
        ChartableTwoFileHistogramChart* getHistogramCharting();
        
        const ChartableTwoFileHistogramChart* getHistogramCharting() const;
        
        ChartableTwoFileLineLayerChart* getLineLayerCharting();
        
        const ChartableTwoFileLineLayerChart* getLineLayerCharting() const;
        
        ChartableTwoFileLineSeriesChart* getLineSeriesCharting();
        
        const ChartableTwoFileLineSeriesChart* getLineSeriesCharting() const;
        
        ChartableTwoFileMatrixChart* getMatrixCharting();
        
        const ChartableTwoFileMatrixChart* getMatrixCharting() const;
        
        bool isChartingTwoSupported() const;
        
        bool isChartingSupportedForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType) const;
        
        bool isChartingSupportedForChartTwoCompoundDataType(const ChartTwoCompoundDataType& chartCompoundDataType) const;
        
        void getSupportedChartTwoDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        void getSupportedChartTwoCompoundDataTypes(std::vector<ChartTwoCompoundDataType>& chartCompoundDataTypesOut) const;
        
        bool getChartTwoCompoundDataTypeForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType,
                                                      ChartTwoCompoundDataType& chartCompoundDataTypeOut) const;
        
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;
        
        // ADD_NEW_METHODS_HERE

    private:
        ChartableTwoFileDelegate(const ChartableTwoFileDelegate&);

        ChartableTwoFileDelegate& operator=(const ChartableTwoFileDelegate&);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        /** points to parent caret mappable data file */
        CaretMappableDataFile* m_caretMappableDataFile = NULL;
        
        std::unique_ptr<ChartableTwoFileHistogramChart> m_histogramCharting;
        
        std::unique_ptr<ChartableTwoFileLineLayerChart> m_lineLayerCharting;
        
        std::unique_ptr<ChartableTwoFileLineSeriesChart> m_lineSeriesCharting;
        
        std::unique_ptr<ChartableTwoFileMatrixChart> m_matrixCharting;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_TWO_FILE_DELEGATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_TWO_FILE_DELEGATE_DECLARE__

} // namespace
#endif  //__CHARTABLE_TWO_FILE_DELEGATE_H__
