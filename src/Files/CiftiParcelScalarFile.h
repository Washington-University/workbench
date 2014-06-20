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
#include "ChartableBrainordinateInterface.h"
#include "ChartableMatrixInterface.h"
#include "CiftiMappableDataFile.h"

namespace caret {

    class SceneClassAssistant;
    
    class CiftiParcelScalarFile : 
    public CiftiMappableDataFile,
    public ChartableBrainordinateInterface,
    public ChartableMatrixInterface {
    
    public:
        CiftiParcelScalarFile();
        
        virtual ~CiftiParcelScalarFile();
        
        virtual bool isBrainordinateChartingEnabled(const int32_t tabIndex) const;
        
        virtual void setBrainordinateChartingEnabled(const int32_t tabIndex,
                                        const bool enabled);
        
        virtual bool isBrainordinateChartingSupported() const;

        virtual ChartDataCartesian* loadBrainordinateChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                                                   const int32_t nodeIndex) throw (DataFileException);
        
        virtual ChartDataCartesian* loadAverageBrainordinateChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                               const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        virtual ChartDataCartesian* loadBrainordinateChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
        virtual void getSupportedBrainordinateChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        virtual bool getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                       int32_t& numberOfColumnsOut,
                                       std::vector<float>& rgbaOut) const;
        
        virtual bool getMatrixCellAttributes(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             float& cellValueOut,
                                             AString& rowNameOut,
                                             AString& columnNameOut) const;
        
        virtual bool isMatrixChartingEnabled(const int32_t tabIndex) const;
        
        virtual bool isMatrixChartingSupported() const;
        
        virtual void setMatrixChartingEnabled(const int32_t tabIndex,
                                              const bool enabled);
        
        virtual void getSupportedMatrixChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const;
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex);
        
        virtual CiftiParcelColoringModeEnum::Enum getSelectedParcelColoringMode() const;
        
        virtual void setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode);
        
        virtual CaretColorEnum::Enum getSelectedParcelColor() const;
        
        virtual void setSelectedParcelColor(const CaretColorEnum::Enum color);
        
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
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CIFTI_PARCEL_SCALAR_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_PARCEL_SCALAR_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_PARCEL_SCALAR_FILE_H__
