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
#include "ChartableMatrixInterface.h"
#include "CiftiMappableDataFile.h"
#include "EventListenerInterface.h"


namespace caret {

    class CiftiScalarDataSeriesFile :
    public CiftiMappableDataFile,
    public ChartableMatrixInterface,
    public EventListenerInterface {
        
    public:
        CiftiScalarDataSeriesFile();
        
        virtual ~CiftiScalarDataSeriesFile();
        
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
        
        virtual void getSupportedMatrixChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const;
        
        const ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex) const;
        
        ChartMatrixDisplayProperties* getChartMatrixDisplayProperties(const int32_t tabIndex);
        
        // ADD_NEW_METHODS_HERE

    private:
        CiftiScalarDataSeriesFile(const CiftiScalarDataSeriesFile&);

        CiftiScalarDataSeriesFile& operator=(const CiftiScalarDataSeriesFile&);
        
        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        ChartMatrixDisplayProperties* m_chartMatrixDisplayProperties[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        // ADD_NEW_MEMBERS_HERE

        
    };
    
#ifdef __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_SCALAR_DATA_SERIES_FILE_H__
