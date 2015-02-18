#ifndef __BORDER_OPTIMIZE_EXECUTOR_H__
#define __BORDER_OPTIMIZE_EXECUTOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "CaretObject.h"
#include "CaretPointer.h"


namespace caret {
    
    class Border;
    class CaretMappableDataFile;
    class Surface;
    
    class BorderOptimizeExecutor : public CaretObject {
        
    public:
        /**
         * Info about the data files
         */
        class DataFileInfo {
        public:
            DataFileInfo(const CaretMappableDataFile* mapFile,
                         const int32_t mapIndex,
                         const bool allMapsFlag,
                         const float smoothing,
                         const float weight,
                         const bool invertGradientFlag)
            : m_mapFile(mapFile),
            m_mapIndex(mapIndex),
            m_allMapsFlag(allMapsFlag),
            m_smoothing(smoothing),
            m_weight(weight),
            m_invertGradientFlag(invertGradientFlag) { }
            
            const CaretMappableDataFile* m_mapFile;
            const int32_t                m_mapIndex;
            const bool                   m_allMapsFlag;
            const float                  m_smoothing;
            const float                  m_weight;
            const bool                   m_invertGradientFlag;
        };
        
        /**
         * Update data for the algorithm
         */
        class InputData {
        public:
            InputData(std::vector<Border*> borders,
                      const Border* borderEnclosingROI,
                      const std::vector<int32_t>& nodesInsideROI,
                      const Surface* surface,
                      const std::vector<CaretPointer<DataFileInfo> >& dataFileInfo)
            : m_borders(borders),
            m_borderEnclosingROI(borderEnclosingROI),
            m_nodesInsideROI(nodesInsideROI),
            m_surface(surface),
            m_dataFileInfo(dataFileInfo) { }
            
            std::vector<Border*> m_borders;
            const Border* m_borderEnclosingROI;
            const std::vector<int32_t>& m_nodesInsideROI;
            const Surface* m_surface;
            const std::vector<CaretPointer<DataFileInfo> >& m_dataFileInfo;
        };
        
        BorderOptimizeExecutor();
        
        virtual ~BorderOptimizeExecutor();
        
        static void printInputs(const InputData& inputData);
        
        static bool run(const InputData& inputData,
                        AString& statisticsInformationOut,
                        AString& errorMessageOut);
        
    private:
        BorderOptimizeExecutor(const BorderOptimizeExecutor&);
        
        BorderOptimizeExecutor& operator=(const BorderOptimizeExecutor&);
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __BORDER_OPTIMIZE_EXECUTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_OPTIMIZE_EXECUTOR_DECLARE__
    
} // namespace
#endif  //__BORDER_OPTIMIZE_EXECUTOR_H__
