#ifndef __CHARTABLE_INTERFACE_H__
#define __CHARTABLE_INTERFACE_H__

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


#include "DataFileException.h"
#include "StructureEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class TimeLine;
    
    /**
     * \class caret::ChartableInterface
     * \brief Interface for files produces charts (time lines)
     * \ingroup Files
     */
    class ChartableInterface {
        
    public:
//        ChartableInterface() { }
//        
//        virtual ~ChartableInterface() { }
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual CaretMappableDataFile* getCaretMappableDataFile() = 0;
        
        /**
         * @return The CaretMappableDataFile that implements this interface.
         */
        virtual const CaretMappableDataFile* getCaretMappableDataFile() const = 0;
        
        /**
         * @return Is charting enabled for this file?
         */
        virtual bool isChartingEnabled() const = 0;
        
        /**
         * @return Return true if the file's current state supports
         * charting data, else false.  Typically a brainordinate file
         * is chartable if it contains more than one map.
         */
        virtual bool isChartingSupported() const = 0;
        
        /**
         * Set charting enabled for this file.
         *
         * @param enabled
         *    New status for charting enabled.
         */
        virtual void setChartingEnabled(const bool enabled) = 0;
        
        /**
         * Load the average of chart data for a group of surface nodes
         * Note: This method will return a chart even if charting for
         * this file is disabled.
         *
         * @param structure
         *     The surface's structure
         * @param nodeIndices
         *     Indices of nodes whose chart data is averaged
         * @param timeLineOut
         *     Output charting data.
         * @return
         *     True if chart data is valid, else false.
         */
        virtual bool loadAverageChartForSurfaceNodes(const StructureEnum::Enum structure,
                                                     const std::vector<int32_t>& nodeIndices,
                                                     TimeLine& timeLineOut) throw (DataFileException) = 0;
        
        /**
         * Load chart data for a surface node
         * Note: This method will return a chart even if charting for
         * this file is disabled.
         *
         * @param structure
         *     The surface's structure
         * @param nodeIndex
         *     Index of node
         * @param timeLineOut
         *     Output charting data.
         * @return
         *     True if chart data is valid, else false.
         */
        virtual bool loadChartForSurfaceNode(const StructureEnum::Enum structure,
                                             const int32_t nodeIndex,
                                             TimeLine& timeLineOut) throw (DataFileException) = 0;
        
        /**
         * Load chart data for a voxel
         * Note: This method will return a chart even if charting for
         * this file is disabled.
         *
         * @param xyz
         *     Coordinate of voxel.
         * @param timeLineOut
         *     Output charting data.
         * @return
         *     True if chart data is valid, else false.
         */
        virtual bool loadChartForVoxelAtCoordinate(const float xyz[3],
                                                   TimeLine& timeLineOut) throw (DataFileException) = 0;
        
    private:
//        ChartableInterface(const ChartableInterface&);
//
//        ChartableInterface& operator=(const ChartableInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTABLE_INTERFACE_DECLARE__

} // namespace
#endif  //__CHARTABLE_INTERFACE_H__
