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

#include "ChartableLineSeriesInterface.h"
#include "StructureEnum.h"

namespace caret {

    class ChartDataCartesian;
    
    /**
     * \class caret::ChartableLineSeriesBrainordinateInterface
     * \brief Interface for files that are able to produce line series brainordinate charts.
     * \ingroup Files
     */
    
    class ChartableLineSeriesBrainordinateInterface : public ChartableLineSeriesInterface {
        
    protected:
        ChartableLineSeriesBrainordinateInterface() { }
        
        virtual ~ChartableLineSeriesBrainordinateInterface() { }
   
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
        virtual ChartDataCartesian* loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                       const int32_t nodeIndex) = 0;
        
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
        virtual ChartDataCartesian* loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                            const std::vector<int32_t>& nodeIndices) = 0;

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
        virtual ChartDataCartesian* loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3]) = 0;
        
    private:
//        ChartableLineSeriesBrainordinateInterface(const ChartableLineSeriesBrainordinateInterface&);
//
//        ChartableLineSeriesBrainordinateInterface& operator=(const ChartableLineSeriesBrainordinateInterface&);
        
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
