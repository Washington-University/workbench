
#ifndef __METRIC_FILE_H__
#define __METRIC_FILE_H__

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

#include <vector>
#include <stdint.h>

#include "ChartableBrainordinateInterface.h"
#include "BrainConstants.h"
#include "GiftiTypeFile.h"

namespace caret {

    class GiftiDataArray;
    
    /**
     * \brief A Metric data file.
     */
    class MetricFile : public GiftiTypeFile, public ChartableBrainordinateInterface {
        
    public:
        MetricFile();
        
        MetricFile(const MetricFile& sf);
        
        MetricFile& operator=(const MetricFile& sf);
        
        virtual ~MetricFile();
        
        virtual void clear();
        
        virtual int32_t getNumberOfNodes() const;
        
        virtual int32_t getNumberOfColumns() const;
        
        virtual void setNumberOfNodesAndColumns(int32_t nodes, int32_t columns);

        virtual void addMaps(const int32_t numberOfNodes,
                             const int32_t numberOfMaps) throw (DataFileException);
        
        float getValue(const int32_t nodeIndex,
                       const int32_t columnIndex) const;
        
        void setValue(const int32_t nodeIndex,
                      const int32_t columnIndex,
                      const float value);
        
        const float* getValuePointerForColumn(const int32_t columnIndex) const;
        
        void setValuesForColumn(const int32_t columnIndex, const float* valuesIn);
        
        void initializeColumn(const int32_t columnIndex, const float& value = 0.0f);
        
        virtual bool getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                             float& dataRangeMaximumOut) const;
        
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
        
    protected:
        /**
         * Validate the contents of the file after it
         * has been read such as correct number of 
         * data arrays and proper data types/dimensions.
         */
        virtual void validateDataArraysAfterReading() throw (DataFileException);
        
        void copyHelperMetricFile(const MetricFile& sf);
        
        void initializeMembersMetricFile();
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        /** Points to actual data in each Gifti Data Array */
        std::vector<float*> columnDataPointers;

        bool m_chartingEnabledForTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    };

} // namespace

#endif // __METRIC_FILE_H__
