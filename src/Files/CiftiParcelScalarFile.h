#ifndef __CIFTI_PARCEL_SCALAR_FILE_H__
#define __CIFTI_PARCEL_SCALAR_FILE_H__

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
#include "ChartableLineSeriesBrainordinateInterface.h"
#include "ChartableMatrixParcelInterface.h"
#include "CiftiMappableDataFile.h"

namespace caret {
    class CiftiParcelReorderingModel;
    class SceneClassAssistant;
    
    class CiftiParcelScalarFile : 
    public CiftiMappableDataFile,
    public ChartableLineSeriesBrainordinateInterface,
    public ChartableMatrixParcelInterface {
    
    public:
        CiftiParcelScalarFile();
        
        virtual ~CiftiParcelScalarFile();
        
        virtual bool isLineSeriesChartingEnabled(const int32_t tabIndex) const;
        
        virtual void setLineSeriesChartingEnabled(const int32_t tabIndex,
                                        const bool enabled);
        
        virtual bool isLineSeriesChartingSupported() const;

        virtual ChartDataCartesian* loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                                                   const int32_t nodeIndex);
        
        virtual ChartDataCartesian* loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                               const std::vector<int32_t>& nodeIndices);
        
        virtual ChartDataCartesian* loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3]);
        
        virtual void getSupportedLineSeriesChartDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
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
        
        virtual void getSupportedMatrixChartDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const;
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex);
        
        virtual CiftiParcelColoringModeEnum::Enum getSelectedParcelColoringMode() const;
        
        virtual void setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode);
        
        virtual CaretColorEnum::Enum getSelectedParcelColor() const;
        
        virtual void setSelectedParcelColor(const CaretColorEnum::Enum color);
        
        virtual void getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& compatibleParcelLabelFilesOut,
                                                                   CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                                   int32_t& selectedParcelLabelFileMapIndexOut,
                                                                   bool& enabledStatusOut) const;
        
        virtual void setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                                   const int32_t selectedParcelLabelFileMapIndex,
                                                                   const bool enabledStatus);
        
        virtual bool createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                            const int32_t parcelLabelFileMapIndex,
                                            AString& errorMessageOut);
        
        virtual const CiftiParcelReordering* getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                                 const int32_t parcelLabelFileMapIndex) const;
        
        virtual bool isSupportsLoadingAttributes();
        
        virtual ChartMatrixLoadingDimensionEnum::Enum getMatrixLoadingDimension() const;
        
        virtual void setMatrixLoadingDimension(const ChartMatrixLoadingDimensionEnum::Enum matrixLoadingType);
        
        virtual YokingGroupEnum::Enum getYokingGroup() const;
        
        virtual void setYokingGroup(const YokingGroupEnum::Enum yokingType);
        
    private:
        CiftiParcelScalarFile(const CiftiParcelScalarFile&);

        CiftiParcelScalarFile& operator=(const CiftiParcelScalarFile&);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        SceneClassAssistant* m_sceneAssistant;
        
        bool m_brainordinateChartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        bool m_matrixChartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        ChartMatrixDisplayProperties* m_chartMatrixDisplayProperties[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CiftiParcelColoringModeEnum::Enum m_selectedParcelColoringMode;
        
        CaretColorEnum::Enum m_selectedParcelColor;

        CiftiParcelReorderingModel* m_parcelReorderingModel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_PARCEL_SCALAR_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_PARCEL_SCALAR_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_PARCEL_SCALAR_FILE_H__
