#ifndef __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_H__
#define __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_H__

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
#include "CiftiMappableDataFile.h"

namespace caret {
    class PaletteFile;

    class CiftiBrainordinateDataSeriesFile :
    public CiftiMappableDataFile,
    public ChartableBrainordinateInterface {
        
    public:
        CiftiBrainordinateDataSeriesFile();
        
        virtual ~CiftiBrainordinateDataSeriesFile();
        
        virtual void updateScalarColoringForAllMaps(const PaletteFile* paletteFile);
        
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
        
    private:
        CiftiBrainordinateDataSeriesFile(const CiftiBrainordinateDataSeriesFile&);

        CiftiBrainordinateDataSeriesFile& operator=(const CiftiBrainordinateDataSeriesFile&);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };
    
#ifdef __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__

} // namespace
#endif  //__CIFTI_BRAINORDINATE_DATA_SERIES_FILE_H__
