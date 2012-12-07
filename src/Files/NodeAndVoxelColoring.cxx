
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
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "CaretOMP.h"

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
 * @param ignoreThresholding
 *    If true, skip all threshold testing
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
    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
    const float thresholdMinimum = paletteColorMapping->getThresholdMinimum(thresholdType);
    const float thresholdMaximum = paletteColorMapping->getThresholdMaximum(thresholdType);
    const float thresholdMappedPositive = paletteColorMapping->getThresholdMappedMaximum();
    const float thresholdMappedPositiveAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMaximum();
    const float thresholdMappedNegative = paletteColorMapping->getThresholdMappedMinimum();
    const float thresholdMappedNegativeAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMinimum();
    const bool showMappedThresholdFailuresInGreen = paletteColorMapping->isShowThresholdFailureInGreen();
    
    /*
     * Skip threshold testing?
     */
    const bool skipThresholdTesting = (ignoreThresholding
                                       || (thresholdType == PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF));
    
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
        if (skipThresholdTesting) {
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
 * @param ignoreThresholding
 *    If true, skip all threshold testing
 */
void 
NodeAndVoxelColoring::colorScalarsWithPalette(const FastStatistics* statistics,
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
    const PaletteThresholdTypeEnum::Enum thresholdType = paletteColorMapping->getThresholdType();
    const float thresholdMinimum = paletteColorMapping->getThresholdMinimum(thresholdType);
    const float thresholdMaximum = paletteColorMapping->getThresholdMaximum(thresholdType);
    const float thresholdMappedPositive = paletteColorMapping->getThresholdMappedMaximum();
    const float thresholdMappedPositiveAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMaximum();
    const float thresholdMappedNegative = paletteColorMapping->getThresholdMappedMinimum();
    const float thresholdMappedNegativeAverageArea = paletteColorMapping->getThresholdMappedAverageAreaMinimum();
    const bool showMappedThresholdFailuresInGreen = paletteColorMapping->isShowThresholdFailureInGreen();
    
    /*
     * Skip threshold testing?
     */
    const bool skipThresholdTesting = (ignoreThresholding
                                       || (thresholdType == PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF));
    
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
     * Get color for normalized values of -1.0 and 1.0.
     * Since there may be a large number of values that are -1.0 or 1.0
     * we can compute the color only once for these values and save time.
     */
    float rgbaPositiveOne[4], rgbaNegativeOne[4];
    palette->getPaletteColor(1.0,
                             interpolateFlag,
                             rgbaPositiveOne);
    const bool rgbaPositiveOneValid = (rgbaPositiveOne[3] > 0.0);
    palette->getPaletteColor(-1.0,
                             interpolateFlag,
                             rgbaNegativeOne);
    const bool rgbaNegativeOneValid = (rgbaNegativeOne[3] > 0.0);
    
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
        
        const float normalValue = normalizedValues[i];
        
        /*
         * RGBA colors have been mapped for extreme values
         */
        if (normalValue >= 1.0) {
            if (rgbaPositiveOneValid) {
                rgbaOut[i4]   = rgbaPositiveOne[0];
                rgbaOut[i4+1] = rgbaPositiveOne[1];
                rgbaOut[i4+2] = rgbaPositiveOne[2];
                rgbaOut[i4+3] = rgbaPositiveOne[3];
            }
            continue;
        }
        if (normalValue <= -1.0) {
            if (rgbaNegativeOneValid) {
                rgbaOut[i4]   = rgbaNegativeOne[0];
                rgbaOut[i4+1] = rgbaNegativeOne[1];
                rgbaOut[i4+2] = rgbaNegativeOne[2];
                rgbaOut[i4+3] = rgbaNegativeOne[3];
            }
            continue;
        }
        
        /*
         * Color scalar using palette
         */
        float rgba[4];
        palette->getPaletteColor(normalValue,
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
        if (skipThresholdTesting) {
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
 * @param ignoreThresholding
 *    If true, skip all threshold testing
 */
void
NodeAndVoxelColoring::colorScalarsWithPalette(const FastStatistics* statistics,
                                              const PaletteColorMapping* paletteColorMapping,
                                              const Palette* palette,
                                              const float* scalarValues,
                                              const float* thresholdValues,
                                              const int32_t numberOfScalars,
                                              uint8_t* rgbaOut,
                                              const bool ignoreThresholding)
{
    if (numberOfScalars <= 0) {
        return;
    }
    const int64_t numRGBA = numberOfScalars * 4;
    std::vector<float> rgbaFloatVector(numRGBA);
    float* rgbaFloat = &rgbaFloatVector[0];
    
    NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                            paletteColorMapping,
                            palette,
                            scalarValues,
                            thresholdValues,
                            numberOfScalars,
                            rgbaFloat,
                            ignoreThresholding);
    
    for (int64_t i = 0; i < numRGBA; i++) {
        if (rgbaFloat[i] < 0.0) {
            rgbaOut[i] = 0;
        }
        else {
            rgbaOut[i] = static_cast<uint8_t>(rgbaFloat[i] * 255.0);
        }
    }
}

/**
 * Assign colors to label indices using a GIFTI label table.
 *
 * @param labelTabl
 *     Label table used for coloring and indexing with label indices.
 * @param labelIndices
 *     The indices are are used to access colors in the label table.
 * @param numberOfIndices
 *     Number of indices.
 * @param rgbv 
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                                 const int32_t* labelIndices,
                                                 const int32_t numberOfIndices,
                                                 float* rgbv)
{
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfIndices; i++) {
        rgbv[i*4+3] = 0.0;
    }   
    /*
     * Assign colors from labels to nodes
     */
    float labelRGBA[4];
	for (int32_t i = 0; i < numberOfIndices; i++) {
        const GiftiLabel* gl = labelTable->getLabel(labelIndices[i]);
        if (gl != NULL) {
            gl->getColor(labelRGBA);
            if (labelRGBA[3] > 0.0) {
                const int32_t i4 = i * 4;
                rgbv[i4]   = labelRGBA[0];
                rgbv[i4+1] = labelRGBA[1];
                rgbv[i4+2] = labelRGBA[2];
                rgbv[i4+3] = 1.0;
            }
        }
    }
}

/**
 * Assign colors to label indices using a GIFTI label table.
 *
 * @param labelTabl
 *     Label table used for coloring and indexing with label indices.
 * @param labelIndices
 *     The indices are are used to access colors in the label table.
 * @param numberOfIndices
 *     Number of indices.
 * @param rgbv
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                                 const int32_t* labelIndices,
                                                 const int32_t numberOfIndices,
                                                 uint8_t* rgbv)
{
    if (numberOfIndices <= 0) {
        return;
    }
    const int64_t numRGBA = numberOfIndices * 4;
    std::vector<float> rgbaFloatVector(numRGBA);
    float* rgbaFloat = &rgbaFloatVector[0];
    
    NodeAndVoxelColoring::colorIndicesWithLabelTable(labelTable,
                                                     labelIndices,
                                                     numberOfIndices,
                                                     rgbaFloat);
    
    for (int64_t i = 0; i < numRGBA; i++) {
        if (rgbaFloat[i] < 0.0) {
            rgbv[i] = 0;
        }
        else {
            rgbv[i] = static_cast<uint8_t>(rgbaFloat[i] * 255.0);
        }
    }
}