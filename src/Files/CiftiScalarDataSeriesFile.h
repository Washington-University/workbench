#ifndef __CIFTI_SCALAR_DATA_SERIES_FILE_H__
#define __CIFTI_SCALAR_DATA_SERIES_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#include "ChartableLineSeriesRowColumnInterface.h"
#include "ChartableMatrixSeriesInterface.h"
#include "CiftiMappableDataFile.h"
#include "EventListenerInterface.h"


namespace caret {

    class SceneClassAssistant;
    
    class CiftiScalarDataSeriesFile :
    public CiftiMappableDataFile,
    public ChartableLineSeriesRowColumnInterface,
    public ChartableMatrixSeriesInterface {
        
    public:
        CiftiScalarDataSeriesFile();
        
        virtual ~CiftiScalarDataSeriesFile();
        
        virtual MapYokingGroupEnum::Enum getMatrixRowColumnMapYokingGroup(const int32_t tabIndex) const;
        
        virtual void setMatrixRowColumnMapYokingGroup(const int32_t tabIndex,
                                    const MapYokingGroupEnum::Enum yokingType);
        
        virtual int32_t getSelectedMapIndex(const int32_t tabIndex) const override;
        
        virtual void setSelectedMapIndex(const int32_t tabIndex,
                                         const int32_t mapIndex);
        
        virtual void receiveEvent(Event* event);
        
        virtual void getMatrixDimensions(int32_t& numberOfRowsOut,
                                         int32_t& numberOfColumnsOut) const;
        
        virtual bool getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                       int32_t& numberOfColumnsOut,
                                       std::vector<float>& rgbaOut) const;
        
        virtual bool getMatrixCellAttributes(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             AString& cellValueOut,
                                             AString& rowNameOut,
                                             AString& columnNameOut) const;
        
        virtual bool isMatrixChartingEnabled(const int32_t tabIndex) const;
        
        virtual bool isMatrixChartingSupported() const;
        
        virtual void setMatrixChartingEnabled(const int32_t tabIndex,
                                              const bool enabled);
        
        virtual void getSupportedMatrixChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const;
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex);
        
        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const;
        
        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                  const bool enabled);
        
        virtual bool isLineSeriesChartingSupported() const;
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForColumn(const int32_t columnIndex);
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForRow(const int32_t rowIndex);

        virtual void getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        virtual void readFile(const AString& ciftiMapFileName) override;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        CiftiScalarDataSeriesFile(const CiftiScalarDataSeriesFile&);

        CiftiScalarDataSeriesFile& operator=(const CiftiScalarDataSeriesFile&);
        
        SceneClassAssistant* m_sceneAssistant;
        
        /** yoking status */
        MapYokingGroupEnum::Enum m_yokingGroupForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_lineSeriesChartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_matrixChartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        ChartMatrixDisplayProperties* m_chartMatrixDisplayPropertiesForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        int32_t m_selectedMapIndices[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

        
    };
    
#ifdef __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_SCALAR_DATA_SERIES_FILE_H__
