#ifndef __NODE_AND_VOXEL_COLORING__H_
#define __NODE_AND_VOXEL_COLORING__H_

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

#include <stdint.h>

#include "CaretColorEnum.h"
#include "DisplayGroupEnum.h"
#include "LabelDrawingTypeEnum.h"
#include "PaletteThresholdOutlineDrawingModeEnum.h"

namespace caret {
    class FastStatistics;
    class GiftiLabelTable;
    class PaletteColorMapping;
    class TabDrawingInfo;
    
    class NodeAndVoxelColoring {
        
    public:
        static void colorScalarsWithPalette(const FastStatistics* statistics,
                                            const PaletteColorMapping* paletteColorMapping,
                                            const float* scalars,
                                            const PaletteColorMapping* thresholdPaletteColorMapping,
                                            const float* scalarThresholds,
                                            const int64_t numberOfScalars,
                                            float* rgbaOut,
                                            const bool ignoreThresholding = false);
        
        static void colorScalarsWithPalette(const FastStatistics* statistics,
                                            const PaletteColorMapping* paletteColorMapping,
                                            const float* scalars,
                                            const PaletteColorMapping* thresholdPaletteColorMapping,
                                            const float* scalarThresholds,
                                            const int64_t numberOfScalars,
                                            uint8_t* rgbaOut,
                                            const bool ignoreThresholding = false);
        
        static void colorScalarsWithRGBA(const float* redComponents,
                                         const float* greenComponents,
                                         const float* blueComponents,
                                         const float* alphaComponents,
                                         const int64_t numberOfComponents,
                                         const uint8_t rgbThreshold[3],
                                         uint8_t* rgbaOut);
        
        static void colorIndicesWithLabelTableForObliqueVolume(const GiftiLabelTable* labelTable,
                                                               const float* labelIndices,
                                                               const int64_t numberOfIndices,
                                                               const TabDrawingInfo& tabDrawingInfo,
                                                               uint8_t* rgbv);

        static void colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                               const float* labelIndices,
                                               const int64_t numberOfIndices,
                                               float* rgbv);
        
        static void colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                               const float* labelIndices,
                                               const int64_t numberOfIndices,
                                               uint8_t* rgbv);
        
        static void convertSliceColoringToOutlineMode(uint8_t* rgbaInOut,
                                                      const LabelDrawingTypeEnum::Enum labelDrawingType,
                                                      const CaretColorEnum::Enum labelOutlineColor,
                                                      const int64_t xdim,
                                                      const int64_t ydim);
        
        static void convertPaletteSliceColoringToOutlineMode(uint8_t* rgbaInOut,
                                                             const PaletteThresholdOutlineDrawingModeEnum::Enum outlineMode,
                                                             const CaretColorEnum::Enum outlineColor,
                                                             const int64_t xdim,
                                                             const int64_t ydim);
        
    private:
        enum ColorDataType {
            COLOR_TYPE_FLOAT,
            COLOR_TYPE_UNSIGNED_BTYE
        };
        
        static void colorScalarsWithPalettePrivate(const FastStatistics* statistics,
                                                   const PaletteColorMapping* paletteColorMapping,
                                                   const float* scalars,
                                                   const PaletteColorMapping* thresholdPaletteColorMapping,
                                                   const float* scalarThresholds,
                                                   const int64_t numberOfScalars,
                                                   const ColorDataType colorDataType,
                                                   void* rgbaOutPointer,
                                                   const bool ignoreThresholding);
        
        static void colorIndicesWithLabelTableForDisplayGroupTabPrivate(const GiftiLabelTable* labelTable,
                                                      const float* labelIndices,
                                                      const int64_t numberOfIndices,
                                                      const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      const ColorDataType colorDataType,
                                                      void* rgbaOutPointer);
        
        static void colorScalarsWithRGBAPrivate(const float* redComponents,
                                                const float* greenComponents,
                                                const float* blueComponents,
                                                const float* alphaComponents,
                                                const int64_t numberOfComponents,
                                                const ColorDataType colorDataType,
                                                const uint8_t* rgbThreshold,
                                                uint8_t* rgbaOutPointer);
        
        
        NodeAndVoxelColoring();
        
        virtual ~NodeAndVoxelColoring();
        
        NodeAndVoxelColoring(const NodeAndVoxelColoring&);

        NodeAndVoxelColoring& operator=(const NodeAndVoxelColoring&);

        static const int32_t INVALID_TAB_INDEX;
    };
    
#ifdef __NODE_AND_VOXEL_COLORING_DECLARE__
  // JWH 24 April 2015  const float NodeAndVoxelColoring::SMALL_POSITIVE =  0.00001;
  // JWH 24 April 2015  const float NodeAndVoxelColoring::SMALL_NEGATIVE = -0.00001;
    const int32_t NodeAndVoxelColoring::INVALID_TAB_INDEX = -1;
#endif // __NODE_AND_VOXEL_COLORING_DECLARE__

} // namespace
#endif  //__NODE_AND_VOXEL_COLORING__H_
