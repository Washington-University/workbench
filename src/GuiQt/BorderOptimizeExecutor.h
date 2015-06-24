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
    class MetricFile;
    class Surface;
    
    class BorderOptimizeExecutor : public CaretObject {
        
    public:
        /**
         * Info about the data files
         */
        struct DataFileInfo {
            DataFileInfo(const CaretMappableDataFile* mapFile,
                         const int32_t mapIndex,
                         const bool allMapsFlag,
                         const float smoothing,
                         const float weight,
                         const bool invertGradientFlag,
                         const bool& skipGradient,
                         const float& corrGradExcludeDist)
            : m_mapFile(mapFile),
            m_mapIndex(mapIndex),
            m_allMapsFlag(allMapsFlag),
            m_smoothing(smoothing),
            m_weight(weight),
            m_invertGradientFlag(invertGradientFlag),
            m_skipGradient(skipGradient),
            m_corrGradExcludeDist(corrGradExcludeDist)
                { }
            
            const CaretMappableDataFile* m_mapFile;
            int32_t m_mapIndex;
            bool m_allMapsFlag;
            float m_smoothing;
            float m_weight;
            bool m_invertGradientFlag;
            bool m_skipGradient;
            float m_corrGradExcludeDist;
        };
        
        /**
         * Update data for the algorithm
         */
        struct InputData {
            InputData(std::vector<Border*> borders,
                      std::vector<Border*> borderPair,
                      const Border* borderEnclosingROI,
                      const std::vector<int32_t>& nodesInsideROI,
                      Surface* surface,
                      const std::vector<DataFileInfo>& dataFileInfo,
                      const MetricFile* vertexAreasMetricFile,
                      const float& gradientFollowingStrength,
                      Surface* upsamplingSphericalSurface,
                      const int32_t upsamplingResolution,
                      MetricFile* combinedGradientDataOut,
                      bool saveResults,
                      const AString& savingPath,
                      const AString& savingBaseName)
            : m_borders(borders),
            m_borderPair(borderPair),
            m_borderEnclosingROI(borderEnclosingROI),
            m_nodesInsideROI(nodesInsideROI),
            m_surface(surface),
            m_dataFileInfo(dataFileInfo),
            m_vertexAreasMetricFile(vertexAreasMetricFile),
            m_gradientFollowingStrength(gradientFollowingStrength),
            m_upsamplingSphericalSurface(upsamplingSphericalSurface),
            m_upsamplingResolution(upsamplingResolution),
            m_combinedGradientDataOut(combinedGradientDataOut),
            m_saveResults(saveResults),
            m_savingPath(savingPath),
            m_savingBaseName(savingBaseName)
                { }
            
            std::vector<Border*> m_borders;
            std::vector<Border*> m_borderPair;
            const Border* m_borderEnclosingROI;
            const std::vector<int32_t>& m_nodesInsideROI;
            Surface* m_surface;
            const std::vector<DataFileInfo>& m_dataFileInfo;
            const MetricFile* m_vertexAreasMetricFile;
            const float m_gradientFollowingStrength;
            Surface* m_upsamplingSphericalSurface;
            const int32_t m_upsamplingResolution;
            MetricFile* m_combinedGradientDataOut;
            bool m_saveResults;
            AString m_savingPath, m_savingBaseName;
        };
        
        BorderOptimizeExecutor();
        
        virtual ~BorderOptimizeExecutor();
        
        static void printInputs(const InputData& inputData);
        
        static bool run(const InputData& inputData,
                        AString& statisticsInformationOut,
                        AString& errorMessageOut);
        
        static void saveResults(const InputData& inputData, const AString& statisticsInformation);
        
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
