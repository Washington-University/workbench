#ifndef __NODE_AND_VOXEL_COLORING__H_
#define __NODE_AND_VOXEL_COLORING__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <stdint.h>

namespace caret {
    class DescriptiveStatistics;
    class FastStatistics;
    class GiftiLabelTable;
    class Palette;
    class PaletteColorMapping;
    
    class NodeAndVoxelColoring {
        
    public:
        static void colorScalarsWithPalette(const DescriptiveStatistics* statistics,
                                            const PaletteColorMapping* paletteColorMapping,
                                            const Palette* palette,
                                            const float* scalars,
                                            const float* scalarThresholds,
                                            const int32_t numberOfScalars,
                                            float* rgbaOut,
                                            const bool ignoreThresholding = false);
        
        static void colorScalarsWithPalette(const FastStatistics* statistics,
                                            const PaletteColorMapping* paletteColorMapping,
                                            const Palette* palette,
                                            const float* scalars,
                                            const float* scalarThresholds,
                                            const int32_t numberOfScalars,
                                            float* rgbaOut,
                                            const bool ignoreThresholding = false);
        
        static void colorScalarsWithPalette(const FastStatistics* statistics,
                                            const PaletteColorMapping* paletteColorMapping,
                                            const Palette* palette,
                                            const float* scalars,
                                            const float* scalarThresholds,
                                            const int32_t numberOfScalars,
                                            uint8_t* rgbaOut,
                                            const bool ignoreThresholding = false);
        
        static const float SMALL_POSITIVE;
        static const float SMALL_NEGATIVE;
        
        static void colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                               const int32_t* labelIndices,
                                               const int32_t numberOfIndices,
                                               float* rgbv);
        
        static void colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                               const int32_t* labelIndices,
                                               const int32_t numberOfIndices,
                                               uint8_t* rgbv);
    private:
        NodeAndVoxelColoring();
        
        virtual ~NodeAndVoxelColoring();
        
        NodeAndVoxelColoring(const NodeAndVoxelColoring&);

        NodeAndVoxelColoring& operator=(const NodeAndVoxelColoring&);
        
    private:
    };
    
#ifdef __NODE_AND_VOXEL_COLORING_DECLARE__
    const float NodeAndVoxelColoring::SMALL_POSITIVE =  0.00001;
    const float NodeAndVoxelColoring::SMALL_NEGATIVE = -0.00001;
#endif // __NODE_AND_VOXEL_COLORING_DECLARE__

} // namespace
#endif  //__NODE_AND_VOXEL_COLORING__H_
