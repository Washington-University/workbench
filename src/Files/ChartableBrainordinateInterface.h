#ifndef __CHARTABLE_BRAINORDINATE_INTERFACE_H__
#define __CHARTABLE_BRAINORDINATE_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include "ChartDataTypeEnum.h"
#include "ChartableInterface.h"
#include "DataFileException.h"
#include "StructureEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class ChartDataCartesian;
    
    /**
     * \class caret::ChartableBrainordinateInterface
     * \brief Interface for files that are able to produce brainordinate charts.
     * \ingroup Files
     */
    class ChartableBrainordinateInterface : public ChartableInterface {
        
    public:
//        ChartableBrainordinateInterface() { }
//        
//        virtual ~ChartableBrainordinateInterface() { }
        

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
        virtual ChartDataCartesian* loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
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
        virtual ChartDataCartesian* loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
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
        virtual ChartDataCartesian* loadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException) = 0;
        
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
