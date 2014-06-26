#ifndef __CHARTABLE_BRAINORDINATE_INTERFACE_H__
#define __CHARTABLE_BRAINORDINATE_INTERFACE_H__

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

#include "ChartDataTypeEnum.h"
#include "DataFileException.h"
#include "StructureEnum.h"

namespace caret {

    //class CaretMappableDataFile;
    class ChartDataCartesian;
    class CaretMappableDataFile;
    
    class ChartableBrainordinateInterface {
        
    protected:
        ChartableBrainordinateInterface() { }
        
        virtual ~ChartableBrainordinateInterface() { }
   
    public:

        /**
         * Load charting data for the surface with the given structure and node index.
         *
         * @param structure
         *     The surface's structure.
         * @param nodeIndex
         *     Index of the node.
         * @return
         *     Pointer to the chart data.  If the data FAILED to load,
         *     the returned pointer will be NULL.  Caller takes ownership
         *     of the pointer and must delete it when no longer needed.
         */
        virtual ChartDataCartesian* loadBrainordinateChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                       const int32_t nodeIndex) throw (DataFileException) = 0;
        
        /**
         * Load average charting data for the surface with the given structure and node indices.
         *
         * @param structure
         *     The surface's structure.
         * @param nodeIndices
         *     Indices of the node.
         * @return
         *     Pointer to the chart data.  If the data FAILED to load,
         *     the returned pointer will be NULL.  Caller takes ownership
         *     of the pointer and must delete it when no longer needed.
         */
        virtual ChartDataCartesian* loadAverageBrainordinateChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException) = 0;

        /**
         * Load charting data for the voxel enclosing the given coordinate.
         *
         * @param xyz
         *     Coordinate of voxel.
         * @return
         *     Pointer to the chart data.  If the data FAILED to load,
         *     the returned pointer will be NULL.  Caller takes ownership
         *     of the pointer and must delete it when no longer needed.
         */
        virtual ChartDataCartesian* loadBrainordinateChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException) = 0;
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getBrainordinateChartCaretMappableDataFile();
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getBrainordinateChartCaretMappableDataFile() const;
        
        /**
         * @return Is charting enabled for this file in the given tab?
         */
        virtual bool isBrainordinateChartingEnabled(const int32_t tabIndex) const = 0;
        
        /**
         * @return Return true if the file's current state supports
         * charting data, else false.  Typically a brainordinate file
         * is chartable if it contains more than one map.
         */
        virtual bool isBrainordinateChartingSupported() const = 0;
        
        /**
         * Set charting enabled for this file in the given tab
         *
         * @param enabled
         *    New status for charting enabled.
         */
        virtual void setBrainordinateChartingEnabled(const int32_t tabIndex,
                                        const bool enabled) = 0;
        
        /**
         * Get chart data types supported by the file.
         *
         * @param chartDataTypesOut
         *    Chart types supported by this file.
         */
        virtual void getSupportedBrainordinateChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const = 0;
        
        bool isBrainordinateChartDataTypeSupported(const ChartDataTypeEnum::Enum chartDataType) const;

    private:
//        ChartableBrainordinateInterface(const ChartableBrainordinateInterface&);
//
//        ChartableBrainordinateInterface& operator=(const ChartableBrainordinateInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_BRAINORDINATE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_BRAINORDINATE_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_BRAINORDINATE_INTERFACE_H__
