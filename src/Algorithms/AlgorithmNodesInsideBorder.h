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
    class BorderFile;
    class CiftiBrainordinateLabelFile;
    class CiftiBrainordinateScalarFile;
    class MetricFile;
    class LabelFile;
    class SurfaceFile;
    class TopologyHelper;
    
    class AlgorithmNodesInsideBorder : public AbstractAlgorithm {
    public:
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj, 
                                   const SurfaceFile* surfaceFile,
                                   const Border* border, 
                                   const bool isInverseSelection,
                                   const int32_t assignToMetricMapIndex,
                                   const float assignMetricValue,
                                   MetricFile* metricFileInOut,
                                   const bool& includeBorder = false);
        
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                   const SurfaceFile* surfaceFile,
                                   const Border* border,
                                   const bool isInverseSelection,
                                   const int32_t assignToCiftiScalarMapIndex,
                                   const float assignScalarValue,
                                   CiftiBrainordinateScalarFile* ciftiScalarFileInOut,
                                   const bool& includeBorder = false);
        
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                   const SurfaceFile* surfaceFile,
                                   const Border* border,
                                   const bool isInverseSelection,
                                   const int32_t assignToCiftiLabelMapIndex,
                                   const int32_t assignLabelKey,
                                   CiftiBrainordinateLabelFile* ciftiLabelFileInOut,
                                   const bool& includeBorder = false);
        
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                   const SurfaceFile* surfaceFile,
                                   const Border* border, 
                                   const bool isInverseSelection,
                                   const int32_t assignToLabelMapIndex,
                                   const int32_t assignLabelKey,
                                   LabelFile* labelFileInOut,
                                   const bool& includeBorder = false);
        
        AlgorithmNodesInsideBorder(ProgressObject* myProgObj,
                                   const SurfaceFile* surfaceFile,
                                   const Border* border,
                                   const bool isInverseSelection,
                                   std::vector<int32_t>& nodesInsideBorderOut,
                                   const bool& includeBorder = false);
        
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, 
                                  ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    
        static const BorderFile* getDebugBorderFile() { return NULL; }
        
    private:
        static std::vector<int32_t> findNodesInsideBorder(const SurfaceFile* mySurf, const Border* myBorder, const bool& inverse, const bool& includeBorder);
        
        static std::vector<int32_t> findNodesOutsideOfConnectedPath(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath);
        
        static int32_t findNodeFurthestFromConnectedPathCenterOfGravity(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath,
                                                                        std::vector<int>& nodeSearchStatus);
        
        static std::vector<int32_t> createConnectedNodesPath(const SurfaceFile* mySurf, const Border* myBorder, const std::vector<int32_t>& unconnectedNodesPath);
        
        static void cleanNodePath(std::vector<int32_t>& nodePath);
        
        static std::vector<int32_t> moveBorderPointsToNearestNodes(const SurfaceFile* mySurf, const Border* myBorder);
        
        static void validateConnectedNodesPath(const SurfaceFile* mySurf, const std::vector<int32_t>& connectedNodesPath);
        
    };
    
    typedef TemplateAutoOperation<AlgorithmNodesInsideBorder> AutoAlgorithmNodesInsideBorder;

} // namespace
#endif  //__ALGORITHM_NODES_INSIDE_BORDER__H_
