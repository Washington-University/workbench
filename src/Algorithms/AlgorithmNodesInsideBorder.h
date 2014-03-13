#ifndef __ALGORITHM_NODES_INSIDE_BORDER__H_
#define __ALGORITHM_NODES_INSIDE_BORDER__H_

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

#include "AbstractAlgorithm.h"

namespace caret {

    class Border;
    class MetricFile;
    class LabelFile;
    class SurfaceFile;
    
    class AlgorithmNodesInsideBorder : public AbstractAlgorithm {
    public:
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
                                   const SurfaceFile* surfaceFile,
                                   const Border* border, 
                                   const bool isInverseSelection,
                                   const int32_t assignToMetricMapIndex,
                                   const float assignMetricValue,
                                   MetricFile* metricFileInOut);
        
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
                                   const SurfaceFile* surfaceFile,
                                   const Border* border, 
                                   const bool isInverseSelection,
                                   const int32_t assignToLabelMapIndex,
                                   const int32_t assignLabelKey,
                                   LabelFile* labelFileInOut);
    private:
        AlgorithmNodesInsideBorder();

        void findNodesInsideBorder(const SurfaceFile* surfaceFile,
                                   const Border* border,
                                   std::vector<int32_t>& nodesInsideBorderOut);
        
        void findNodesEnclosedByUnconnectedPath(const SurfaceFile* surfaceFile,
                                                   const std::vector<int32_t>& unconnectedNodesPath,
                                                   std::vector<int32_t>& nodesEnclosedByPathOut);
        
        void findNodesEnclosedByUnconnectedPathCCW(const SurfaceFile* surfaceFile,
                                                   const std::vector<int32_t>& unconnectedNodesPath,
                                                   std::vector<int32_t>& nodesEnclosedByPathOut);
        
//        void findNodesEnclosedByConnectedNodesPath(const SurfaceFile* surfaceFile,
//                                           const std::vector<int32_t>& connectedNodesPath,
//                                           std::vector<int32_t>& nodesInsidePathOut);
        
        void findNodesEnclosedByConnectedNodesPathCounterClockwise(const SurfaceFile* surfaceFile,
                                           const std::vector<int32_t>& connectedNodesPath,
                                           std::vector<int32_t>& nodesInsidePathOut);
        
        void createConnectedNodesPath(const SurfaceFile* surfaceFile,
                                      const std::vector<int32_t>& unconnectedNodesPath,
                                      std::vector<int32_t>& connectedNodesPathOut);
        
        void cleanConnectedNodesPath(std::vector<int32_t>& connectedNodesPath);
        
        void validateConnectedNodesPath(const SurfaceFile* surfaceFile,
                                        const std::vector<int32_t>& connectedNodesPath);
        
        bool isInverseSelection;
  
//        static OperationParameters* getParameters();
//        static void useParameters(OperationParameters* myParams, 
//                                  ProgressObject* myProgObj);
//        static AString getCommandSwitch();
//        static AString getShortDescription();
//    
//        typedef TemplateAutoOperation<AlgorithmNodesInsideBorder> AutoAlgorithmNodesInsideBorder;
    };
    
#ifdef __ALGORITHM_NODES_INSIDE_BORDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ALGORITHM_NODES_INSIDE_BORDER_DECLARE__

} // namespace
#endif  //__ALGORITHM_NODES_INSIDE_BORDER__H_
