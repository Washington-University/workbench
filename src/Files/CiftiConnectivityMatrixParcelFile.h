#ifndef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__
#define __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__

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
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "ChartableMatrixInterface.h"

namespace caret {

    class ChartMatrixDisplayProperties;
    class PaletteFile;
    
    class CiftiConnectivityMatrixParcelFile : public CiftiMappableConnectivityMatrixDataFile, public ChartableMatrixInterface {
        
    public:
        CiftiConnectivityMatrixParcelFile();
        
        virtual ~CiftiConnectivityMatrixParcelFile();
        
    private:
        CiftiConnectivityMatrixParcelFile(const CiftiConnectivityMatrixParcelFile&);

        CiftiConnectivityMatrixParcelFile& operator=(const CiftiConnectivityMatrixParcelFile&);

    public:
        virtual bool getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                       int32_t& numberOfColumnsOut,
                                       std::vector<float>& rgbaOut) const;
        
        virtual bool getMatrixCellAttributes(const int32_t rowIndex,
                                             const int32_t columnIndex,
                                             float& cellValueOut,
                                             AString& rowNameOut,
                                             AString& columnNameOut) const;
        
        virtual bool isChartingEnabled(const int32_t tabIndex) const;

        virtual bool isChartingSupported() const;

        virtual void setChartingEnabled(const int32_t tabIndex,
                                        const bool enabled);

        virtual void getSupportedChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const;
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex);
        
        virtual CiftiParcelColoringModeEnum::Enum getSelectedParcelColoringMode() const;
        
        virtual void setSelectedParcelColoringMode(const CiftiParcelColoringModeEnum::Enum coloringMode);
        
        virtual CaretColorEnum::Enum getSelectedParcelColor() const;
        
        virtual void setSelectedParcelColor(const CaretColorEnum::Enum color);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        

        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        ChartMatrixDisplayProperties* m_chartMatrixDisplayProperties[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CiftiParcelColoringModeEnum::Enum m_selectedParcelColoringMode;
        
        CaretColorEnum::Enum m_selectedParcelColor;
        
    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_H__
