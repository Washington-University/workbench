
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

#include <cmath>
#include <limits>

#define __NODE_AND_VOXEL_COLORING_DECLARE__
#include "NodeAndVoxelColoring.h"
#undef __NODE_AND_VOXEL_COLORING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DescriptiveStatistics.h"
#include "Palette.h"
#include "PaletteColorMapping.h"

using namespace caret;

static const float positiveThresholdGreenColor[4] = {
    115.0f / 255.0f,
    255.0f / 255.0f,
    180.0f / 255.0f,
    255.0f / 255.0f
};

static const float negativeThresholdGreenColor[] = {
    180.0f / 255.0f,
    255.0f / 255.0f,
    115.0f / 255.0f,
    255.0f / 255.0f
};


    
/**
 * \class NodeAndVoxelColoring 
 * \brief Static methods for coloring nodes and voxels. 
 *
 * Provides methods for coloring nodes and voxels.
 */

/**
 * Color scalars using a palette.
 *
 * @param statistics
 *    Descriptive statistics for min/max values.
 * @param paletteColorMapping
 *    Specifies mapping of scalars to palette colors.
 * @param palette
 *    Color palette used to map scalars to colors.
 * @param scalarValues
 *    Scalars that are used to color the values.
 *    Number of elements is 'numberOfScalars'.
 * @param thresholdValues
 *    Thresholds for inhibiting coloring.
 *    Number of elements is 'numberOfScalars'.
 * @param numberOfScalars
 *    Number of scalars and thresholds.
 * @param rgbaOut
 *    RGBA Colors that are output.  The alpha
 *    value will be negative if the scalar does
 *    not receive any coloring.
 *    Number of elements is 'numberOfScalars' * 4.
 */
void 
NodeAndVoxelColoring::colorScalarsWithPalette(const DescriptiveStatistics* statistics,
                                              const PaletteColorMapping* paletteColorMapping,
                                              const Palette* palette,
                                              const float* scalarValues,
                                              const float* thresholdValues,
                                              const int32_t numberOfScalars,
                                              float* rgbaOut,
                                              const bool ignoreThresholding)
{
    if (numberOfScalars <= 0) {
        return;
    }
    
    CaretAssert(statistics);
    CaretAssert(paletteColorMapping);
    CaretAssert(palette);
    CaretAssert(scalarValues);
    CaretAssert(thresholdValues);
    CaretAssert(rgbaOut);
        
    /*
     * Type of threshold testing
     */
    bool showOutsideFlag = false;
    const PaletteThresholdTestEnum::Enum thresholdTest = paletteColorMapping->getThresholdTest();
    switch (thresholdTest) {
        case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:                
            showOutsideFlag = true;
            break;
        case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
            showOutsideFlag = false;
            break;
    }
    
    /*
     * Range of values allowed by thresholding
     */
    float thresholdMinimum = -std::numeric_limits<float>::max();
    float thresholdMaximum =  std::numeric_limits<float>::max();
    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            showOutsideFlag = false;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                thresholdMinimum = paletteColorMapping->getThresholdNormalMinimum();
                thresholdMaximum = paletteColorMapping->getThresholdNormalMaximum();
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                thresholdMinimum = paletteColorMapping->getThresholdMappedMinimum();
                thresholdMaximum = paletteColorMapping->getThresholdMappedMaximum();
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                thresholdMinimum = paletteColorMapping->getThresholdMappedAverageAreaMinimum();
                thresholdMaximum = paletteColorMapping->getThresholdMappedAverageAreaMaximum();
            break;
    }
    const float thresholdMappedPositive = paletteColorMapping->getThresholdMappedMaximum();
    const float thresholdMappedPositiveAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMaximum();
    const float thresholdMappedNegative = paletteColorMapping->getThresholdMappedMinimum();
    const float thresholdMappedNegativeAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMinimum();
    const bool showMappedThresholdFailuresInGreen = paletteColorMapping->isShowThresholdFailureInGreen();
    
    /*
     * Display of negative, zero, and positive values allowed.
     */
    const bool hidePositiveValues = (paletteColorMapping->isDisplayPositiveDataFlag() == false);
    const bool hideNegativeValues = (paletteColorMapping->isDisplayNegativeDataFlag() == false);
    const bool hideZeroValues =     (paletteColorMapping->isDisplayZeroDataFlag() == false);
    
    const bool interpolateFlag = paletteColorMapping->isInterpolatePaletteFlag();
    
    /*
     * Convert data values to normalized palette values.
     */
    std::vector<float> normalizedValues(numberOfScalars);
    paletteColorMapping->mapDataToPaletteNormalizedValues(statistics, 
                                                          scalarValues, 
                                                          &normalizedValues[0], 
                                                          numberOfScalars);
    
    /*
     * Color all scalars.
     */
    for (int32_t i = 0; i < numberOfScalars; i++) {
        const int32_t i4 = i * 4;
        rgbaOut[i4]   =  0.0;
        rgbaOut[i4+1] =  0.0;
        rgbaOut[i4+2] =  0.0;
        rgbaOut[i4+3] = -1.0;
        
        float scalar    = scalarValues[i];
        const float threshold = thresholdValues[i];
        
        /*
         * Positive/Zero/Negative Test
         */
        if (scalar > NodeAndVoxelColoring::SMALL_POSITIVE) {
            if (hidePositiveValues) {
                continue;
            }
        }
        else if (scalar < NodeAndVoxelColoring::SMALL_NEGATIVE) {
            if (hideNegativeValues) {
                continue;
            }
        }
        else {
            /*
             * May be very near zero so force to zero.
             */
            normalizedValues[i] = 0.0;
            if (hideZeroValues) {
                continue;
            }
        }
                
        /*
         * Color scalar using palette
         */
        float rgba[4];
        palette->getPaletteColor(normalizedValues[i],
                                 interpolateFlag,
                                 rgba);
        if (rgba[3] > 0.0f) {
            rgbaOut[i4]   = rgba[0];
            rgbaOut[i4+1] = rgba[1];
            rgbaOut[i4+2] = rgba[2];
            rgbaOut[i4+3] = rgba[3];
        }
        
        /*
         * Threshold Test
         * Threshold is done last so colors are still set
         * but if threshold test fails, alpha is set invalid.
         */
        bool thresholdPassedFlag = false;
        if (ignoreThresholding) {
            thresholdPassedFlag = true;
        }
        else if (showOutsideFlag) {
            if (threshold > thresholdMaximum) {
                thresholdPassedFlag = true;
            }
            else if (threshold < thresholdMinimum) {
                thresholdPassedFlag = true;
            }
        }
        else {
            if ((threshold >= thresholdMinimum) &&
                (threshold <= thresholdMaximum)) {
                thresholdPassedFlag = true;
            }
        }
        if (thresholdPassedFlag == false) {
            rgbaOut[i4+3] = -1.0;
            if (showMappedThresholdFailuresInGreen) {
                if (thresholdType == PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED) {
                    if (threshold > 0.0f) {
                        if ((threshold < thresholdMappedPositive) &&
                            (threshold > thresholdMappedPositiveAverageArea)) {
                            rgbaOut[i4]   = positiveThresholdGreenColor[0];
                            rgbaOut[i4+1] = positiveThresholdGreenColor[1];
                            rgbaOut[i4+2] = positiveThresholdGreenColor[2];
                            rgbaOut[i4+3] = positiveThresholdGreenColor[3];
                        }
                    }
                    else if (threshold < 0.0f) {
                        if ((threshold > thresholdMappedNegative) &&
                            (threshold < thresholdMappedNegativeAverageArea)) {
                            rgbaOut[i4]   = negativeThresholdGreenColor[0];
                            rgbaOut[i4+1] = negativeThresholdGreenColor[1];
                            rgbaOut[i4+2] = negativeThresholdGreenColor[2];
                            rgbaOut[i4+3] = negativeThresholdGreenColor[3];
                        }
                    }
                }
            }
        }
    }
}
