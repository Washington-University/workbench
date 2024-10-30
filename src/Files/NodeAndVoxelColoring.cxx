
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

#include <cmath>
#include <limits>

//#include <QRunnable>
//#include <QSemaphore>
//#include <QThreadPool>

#define __NODE_AND_VOXEL_COLORING_DECLARE__
#include "NodeAndVoxelColoring.h"
#undef __NODE_AND_VOXEL_COLORING_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretOMP.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyItem.h"
#include "LabelSelectionItem.h"
#include "LabelSelectionItemModel.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "MathFunctions.h"
#include "TabDrawingInfo.h"

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
 * Color scalars using a palette that accepts a void* type for the 
 * color array to that multiple data types are supported without
 * having to allocate memory for conversion to one data type or 
 * the other nor duplicate lots of code for each data type.
 *
 * @param statistics
 *    Descriptive statistics for min/max values.
 * @param paletteColorMapping
 *    Specifies mapping of scalars to palette colors.
 * @param scalarValues
 *    Scalars that are used to color the values.
 *    Number of elements is 'numberOfScalars'.
 * @param thresholdPaletteColorMapping
 *    Specifies thresholding for thresholding scalars.
 * @param thresholdValues
 *    Thresholds for inhibiting coloring.
 *    Number of elements is 'numberOfScalars'.
 * @param numberOfScalars
 *    Number of scalars and thresholds.
 * @param colorDataType
 *    Data type of the rgbaOut parameter
 * @param rgbaOutPointer
 *    RGBA Colors that are output.  This is a VOID type and its
 *    true type is provided by the previous parameter colorDataType.
 * @param ignoreThresholding
 *    If true, skip all threshold testing
 */
void
NodeAndVoxelColoring::colorScalarsWithPalettePrivate(const FastStatistics* statistics,
                                                     const PaletteColorMapping* paletteColorMapping,
                                                     const float* scalarValues,
                                                     const PaletteColorMapping* thresholdPaletteColorMapping,
                                                     const float* thresholdValues,
                                                     const int64_t numberOfScalars,
                                                     const ColorDataType colorDataType,
                                                     void* rgbaOutPointer,
                                                     const bool ignoreThresholding)
{
    if (numberOfScalars <= 0) {
        return;
    }
    
    const Palette* palette = paletteColorMapping->getPalette();
    CaretAssert(palette);
    
    CaretAssert(statistics);
    CaretAssert(paletteColorMapping);
    CaretAssert(scalarValues);
    CaretAssert(thresholdPaletteColorMapping);
    CaretAssert(thresholdValues);
    CaretAssert(rgbaOutPointer);
    
    /*
     * Cast to data type for rgba coloring
     */
    float* rgbaFloat = NULL;
    uint8_t* rgbaUnsignedByte = NULL;
    switch (colorDataType) {
        case COLOR_TYPE_FLOAT:
            rgbaFloat = (float*)rgbaOutPointer;
            break;
        case COLOR_TYPE_UNSIGNED_BTYE:
            rgbaUnsignedByte = (uint8_t*)rgbaOutPointer;
            break;
    }
    
    /*
     * Type of threshold testing
     */
    bool showOutsideFlag = false;
    const PaletteThresholdTestEnum::Enum thresholdTest = thresholdPaletteColorMapping->getThresholdTest();
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
    const float thresholdMinimum = thresholdPaletteColorMapping->getThresholdMinimum(thresholdType);
    const float thresholdMaximum = thresholdPaletteColorMapping->getThresholdMaximum(thresholdType);
    const float thresholdMappedPositive = thresholdPaletteColorMapping->getThresholdMappedMaximum();
    const float thresholdMappedPositiveAverageArea = thresholdPaletteColorMapping->getThresholdMappedAverageAreaMaximum();
    const float thresholdMappedNegative = thresholdPaletteColorMapping->getThresholdMappedMinimum();
    const float thresholdMappedNegativeAverageArea = thresholdPaletteColorMapping->getThresholdMappedAverageAreaMinimum();
    const bool showMappedThresholdFailuresInGreen = thresholdPaletteColorMapping->isShowThresholdFailureInGreen();
    
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
#pragma omp CARET_PARFOR schedule(dynamic, 4096)
        for (int64_t i = 0; i < numberOfScalars; i++) {
        const int64_t i4 = i * 4;
        
        /*
         * Initialize coloring for node since one of the
         * continue statements below may cause moving
         * on to next node
         */
        switch (colorDataType) {
            case COLOR_TYPE_FLOAT:
                rgbaFloat[i4]   =  0.0;
                rgbaFloat[i4+1] =  0.0;
                rgbaFloat[i4+2] =  0.0;
                rgbaFloat[i4+3] =  0.0;
                break;
            case COLOR_TYPE_UNSIGNED_BTYE:
                rgbaUnsignedByte[i4]   =  0;
                rgbaUnsignedByte[i4+1] =  0;
                rgbaUnsignedByte[i4+2] =  0;
                rgbaUnsignedByte[i4+3] =  0;
                break;
        }
        
        float scalar = scalarValues[i];
        const float threshold = thresholdValues[i];
        
        /*
         * Positive/Zero/Negative Test
         */
        if (scalar > PaletteColorMapping::SMALL_POSITIVE) {   // JWH 24 April 2015    NodeAndVoxelColoring::SMALL_POSITIVE) {
            if (hidePositiveValues) {
                continue;
            }
        }
        else if (scalar < PaletteColorMapping::SMALL_NEGATIVE) {  // JWH 24 April 2015  NodeAndVoxelColoring::SMALL_NEGATIVE) {
            if (hideNegativeValues) {
                continue;
            }
        }
        else if (MathFunctions::isNaN(scalar)) {
            continue;//TSC: never color NaN
        } else {
            /*
             * May be very near zero so force to zero.
             * 
             * TSC: that seems wrong, leave the normalized value alone
             *  if the data value is near zero, that doesn't mean the palette settings aren't also near zero
             *  therefore, normalized value may not be near zero, which is important
             * 
             */
            //normalizedValues[i] = 0.0;
            if (hideZeroValues) {
                continue;
            }
        }
        
        /*
         * Temporary for rgba coloring now that past possible
         * continue statements
         */
        float rgbaOut[4] = {
             0.0,
             0.0,
             0.0,
             0.0
        };
        
        const float normalValue = normalizedValues[i];
        
        /*
         * RGBA colors have been mapped for extreme values
         */
        if (normalValue >= 1.0) {
            if (rgbaPositiveOneValid) {
                rgbaOut[0] = rgbaPositiveOne[0];
                rgbaOut[1] = rgbaPositiveOne[1];
                rgbaOut[2] = rgbaPositiveOne[2];
                rgbaOut[3] = rgbaPositiveOne[3];
            }
        }
        else if (normalValue <= -1.0) {
            if (rgbaNegativeOneValid) {
                rgbaOut[0] = rgbaNegativeOne[0];
                rgbaOut[1] = rgbaNegativeOne[1];
                rgbaOut[2] = rgbaNegativeOne[2];
                rgbaOut[3] = rgbaNegativeOne[3];
            }
        }
        else {
            /*
             * Color scalar using palette
             */
            float rgba[4];
            palette->getPaletteColor(normalValue,
                                     interpolateFlag,
                                     rgba);
            if (rgba[3] > 0.0f) {
                rgbaOut[0] = rgba[0];
                rgbaOut[1] = rgba[1];
                rgbaOut[2] = rgba[2];
                rgbaOut[3] = rgba[3];
            }
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
        if ( ! thresholdPassedFlag) {
            /*
             * Need to clear RGB, in addition to alpha for volume Maximum Intensity Projection
             */
            rgbaOut[0] = 0.0;
            rgbaOut[1] = 0.0;
            rgbaOut[2] = 0.0;
            rgbaOut[3] = 0.0;
            if (showMappedThresholdFailuresInGreen) {
                if (thresholdType == PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED) {
                    if (threshold > 0.0f) {
                        if ((threshold < thresholdMappedPositive) &&
                            (threshold > thresholdMappedPositiveAverageArea)) {
                            rgbaOut[0] = positiveThresholdGreenColor[0];
                            rgbaOut[1] = positiveThresholdGreenColor[1];
                            rgbaOut[2] = positiveThresholdGreenColor[2];
                            rgbaOut[3] = positiveThresholdGreenColor[3];
                        }
                    }
                    else if (threshold < 0.0f) {
                        if ((threshold > thresholdMappedNegative) &&
                            (threshold < thresholdMappedNegativeAverageArea)) {
                            rgbaOut[0] = negativeThresholdGreenColor[0];
                            rgbaOut[1] = negativeThresholdGreenColor[1];
                            rgbaOut[2] = negativeThresholdGreenColor[2];
                            rgbaOut[3] = negativeThresholdGreenColor[3];
                        }
                    }
                }
            }
        }

        switch (colorDataType) {
            case COLOR_TYPE_FLOAT:
                CaretAssertArrayIndex(rgbaFloat, numberOfScalars * 4, i*4+3);
                rgbaFloat[i4]   = rgbaOut[0];
                rgbaFloat[i4+1] = rgbaOut[1];
                rgbaFloat[i4+2] = rgbaOut[2];
                rgbaFloat[i4+3] = rgbaOut[3];
                break;
            case COLOR_TYPE_UNSIGNED_BTYE:
                CaretAssertArrayIndex(rgbaUnsignedByte, numberOfScalars * 4, i*4+3);
                rgbaUnsignedByte[i4]   = rgbaOut[0] * 255.0;
                rgbaUnsignedByte[i4+1] = rgbaOut[1] * 255.0;
                rgbaUnsignedByte[i4+2] = rgbaOut[2] * 255.0;
                if (rgbaOut[3] > 0.0) {
                    rgbaUnsignedByte[i4+3] = rgbaOut[3] * 255.0;
                }
                else {
                    rgbaUnsignedByte[i4+3] = 0;
                }
                break;
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
 * @param scalarValues
 *    Scalars that are used to color the values.
 *    Number of elements is 'numberOfScalars'.
 * @param thresholdPaletteColorMapping
 *    Specifies thresholding for thresholding scalars.
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
                                              const float* scalarValues,
                                              const PaletteColorMapping* thresholdPaletteColorMapping,
                                              const float* thresholdValues,
                                              const int64_t numberOfScalars,
                                              float* rgbaOut,
                                              const bool ignoreThresholding)
{
    colorScalarsWithPalettePrivate(statistics,
                                   paletteColorMapping,
                                   scalarValues,
                                   thresholdPaletteColorMapping,
                                   thresholdValues,
                                   numberOfScalars,
                                   COLOR_TYPE_FLOAT,
                                   (void*)rgbaOut,
                                   ignoreThresholding);
}

/**
 * Color scalars using a palette.
 *
 * @param statistics
 *    Descriptive statistics for min/max values.
 * @param paletteColorMapping
 *    Specifies mapping of scalars to palette colors.
 * @param scalarValues
 *    Scalars that are used to color the values.
 *    Number of elements is 'numberOfScalars'.
 * @param thresholdPaletteColorMapping
 *    Specifies thresholding for thresholding scalars.
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
                                              const float* scalarValues,
                                              const PaletteColorMapping* thresholdPaletteColorMapping,
                                              const float* thresholdValues,
                                              const int64_t numberOfScalars,
                                              uint8_t* rgbaOut,
                                              const bool ignoreThresholding)
{
    colorScalarsWithPalettePrivate(statistics,
                                   paletteColorMapping,
                                   scalarValues,
                                   thresholdPaletteColorMapping,
                                   thresholdValues,
                                   numberOfScalars,
                                   COLOR_TYPE_UNSIGNED_BTYE,
                                   (void*)rgbaOut,
                                   ignoreThresholding);
}

/**
 * Color RGBA data.
 *
 * @param redComponents
 *    Values for red components.  Range [0, 255].
 * @param greenComponents
 *    Values for green components.   Range [0, 255].
 * @param blueComponents
 *    Values for blue components.   Range [0, 255].
 * @param alphaComponents
 *    Values for alpha components (NULL if alpha not valid)
 * @param colorDataType
 *    Data type of the rgbaOut parameter
 * @param rgbThreshold
 *    Threshold RGB (voxel not drawn if voxel RGB components LESS THAN these values).
 *    Range [0, 255].
 * @param rgbaOutPointer
 *    RGBA Colors that are output.  The alpha
 *    value will be negative if the scalar does
 *    not receive any coloring.
 *    Number of elements is 'numberOfScalars' * 4.
 */
void
NodeAndVoxelColoring::colorScalarsWithRGBAPrivate(const float* redComponents,
                                                  const float* greenComponents,
                                                  const float* blueComponents,
                                                  const float* alphaComponents,
                                                  const int64_t numberOfComponents,
                                                  const ColorDataType colorDataType,
                                                  const uint8_t* rgbThreshold,
                                                  uint8_t* rgbaOutPointer)
{
    /*
     * Cast to data type for rgba coloring
     */
    float*   rgbaFloat = NULL;
    uint8_t* rgbaUnsignedByte = NULL;
    float    thresholdRed   = -1.0;
    float    thresholdGreen = -1.0;
    float    thresholdBlue  = -1.0;
    switch (colorDataType) {
        case COLOR_TYPE_FLOAT:
        {
            rgbaFloat = (float*)rgbaOutPointer;
            const float* threshFloat = (float*)rgbThreshold;
            thresholdRed   = threshFloat[0];
            thresholdGreen = threshFloat[1];
            thresholdBlue  = threshFloat[2];
        }
            break;
        case COLOR_TYPE_UNSIGNED_BTYE:
        {
            rgbaUnsignedByte = (uint8_t*)rgbaOutPointer;
            const uint8_t* threshByte = (uint8_t*)rgbThreshold;
            thresholdRed   = threshByte[0];
            thresholdGreen = threshByte[1];
            thresholdBlue  = threshByte[2];
        }
            break;
    }
    
    /*
     * Examine data to see if it ranges [0,1].
     * Otherwise, it is [0, 255].
     */
    bool rangeOneFlag(false);
    bool autoRangeDetectFlag(true);
    if (autoRangeDetectFlag) {
        rangeOneFlag = true;
        const float valueOne(1.001);
        for (int64_t i = 0; i < numberOfComponents; i++) {
            if ((redComponents[i] > valueOne)
                || (redComponents[i] < -valueOne)
                || (greenComponents[i] > valueOne)
                || (greenComponents[i] < -valueOne)
                || (blueComponents[i] > valueOne)
                || (blueComponents[i] < -valueOne)) {
                rangeOneFlag = false;
                break;
            }
        }
    }
    
    const float scaleValue(rangeOneFlag
                           ? 255.0
                           : 1.0);
    
    for (int64_t i = 0; i < numberOfComponents; i++) {
        float red   = redComponents[i]   * scaleValue;
        float green = greenComponents[i] * scaleValue;
        float blue  = blueComponents[i]  * scaleValue;
        float alpha = 0.0;
        
        if (red < 0.0)   red   = -red;
        if (green < 0.0) green = -green;
        if (blue < 0.0)  blue  = -blue;
        if (red > 255.0)   red   = 255.0;
        if (green > 255.0) green = 255.0;
        if (blue > 255.0)  blue  = 255.0;

        if ((red      >= thresholdRed)
            && (green >= thresholdGreen)
            && (blue  >= thresholdBlue)) {
            alpha = ((alphaComponents == NULL) ? 255.0 : (alphaComponents[i] * scaleValue));
            if (alpha < 0.0) alpha = -alpha;
            if (alpha > 255.0) alpha = 255.0;
        }
        
        const int64_t i4 = i * 4;
        switch (colorDataType) {
            case COLOR_TYPE_FLOAT:
                rgbaFloat[i4]   =  red   / 255.0;
                rgbaFloat[i4+1] =  green / 255.0;
                rgbaFloat[i4+2] =  blue  / 255.0;
                rgbaFloat[i4+3] =  alpha / 255.0;
                break;
            case COLOR_TYPE_UNSIGNED_BTYE:
                rgbaUnsignedByte[i4]   =  static_cast<uint8_t>(red);
                rgbaUnsignedByte[i4+1] =  static_cast<uint8_t>(green);
                rgbaUnsignedByte[i4+2] =  static_cast<uint8_t>(blue);
                rgbaUnsignedByte[i4+3] =  static_cast<uint8_t>(alpha);
                break;
        }
    }
}

/**
 * Color RGBA data.
 *
 * @param redComponents
 *    Values for red components.
 * @param greenComponents
 *    Values for green components.
 * @param blueComponents
 *    Values for blue components.
 * @param alphaComponents
 *    Values for alpha components (NULL if alpha not valid)
 * @param numberOfComponents
 *    Number of components (each color component contains this number of values).
 * @param rgbThreshold
 *    Threshold RGB (voxel not drawn if voxel RGB components LESS THAN these values).
 *    Range is [0, 255].
 * @param rgbaOut
 *    RGBA Colors that are output.  The alpha
 *    value will be negative if the scalar does
 *    not receive any coloring.
 *    Number of elements is 'numberOfComponents' * 4.
 */
void
NodeAndVoxelColoring::colorScalarsWithRGBA(const float* redComponents,
                                           const float* greenComponents,
                                           const float* blueComponents,
                                           const float* alphaComponents,
                                           const int64_t numberOfComponents,
                                           const uint8_t rgbThreshold[3],
                                           uint8_t* rgbaOut)
{
    colorScalarsWithRGBAPrivate(redComponents,
                                greenComponents,
                                blueComponents,
                                alphaComponents,
                                numberOfComponents,
                                COLOR_TYPE_UNSIGNED_BTYE,
                                rgbThreshold,
                                rgbaOut);
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
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbv
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTableForObliqueVolume(const GiftiLabelTable* labelTable,
                                                                 const float* labelIndices,
                                                                 const int64_t numberOfIndices,
                                                                 const TabDrawingInfo& tabDrawingInfo,
                                                                 uint8_t* rgbaOut)
{
    /*
     * Invalidate all coloring.
     */
    for (int64_t i = 0; i < numberOfIndices; i++) {
        rgbaOut[i*4+3] = 0;
    }
    
    const int32_t tabIndex(tabDrawingInfo.getTabIndex());
    const DisplayGroupEnum::Enum displayGroup(tabDrawingInfo.getDisplayGroup());
    const LabelViewModeEnum::Enum labelViewMode(tabDrawingInfo.getLabelViewMode());
    const LabelSelectionItemModel* labelModel(tabDrawingInfo.getMapFile()->getLabelSelectionHierarchyForMapAndTab(tabDrawingInfo.getMapIndex(),
                                                                                                                  displayGroup,
                                                                                                                  tabIndex));
    /*
     * Assign colors from labels to nodes
     */
    float labelRGBA[4];
    for (int64_t i = 0; i < numberOfIndices; i++) {
        const int64_t labelKey = static_cast<int64_t>(labelIndices[i]);
        const GiftiLabel* gl = labelTable->getLabel(labelKey);
        if (gl != NULL) {
            bool colorDataFlag = false;
            switch (labelViewMode) {
                case LabelViewModeEnum::HIERARCHY:
                    if (labelModel != NULL) {
                        if (labelModel->isLabelChecked(labelKey)) {
                            colorDataFlag = true;
                        }
                    }
                    break;
                case LabelViewModeEnum::LIST:
                {
                    const GroupAndNameHierarchyItem* item = gl->getGroupNameSelectionItem();
                    if (item != NULL) {
                        if (tabIndex == NodeAndVoxelColoring::INVALID_TAB_INDEX) {
                            colorDataFlag = true;
                        }
                        else if (item->isSelected(tabDrawingInfo)) {
                            colorDataFlag = true;
                        }
                    }
                    else {
                        colorDataFlag = true;
                    }
                }
                    break;
            }
            
            if (colorDataFlag) {
                gl->getColor(labelRGBA);
                if (labelRGBA[3] > 0.0) {
                    const int64_t i4 = i * 4;
                    
                    CaretAssertArrayIndex(rgbaUnsignedByte, numberOfIndices * 4, i*4+3);
                    rgbaOut[i4]   = labelRGBA[0] * 255.0;
                    rgbaOut[i4+1] = labelRGBA[1] * 255.0;
                    rgbaOut[i4+2] = labelRGBA[2] * 255.0;
                    if (labelRGBA[3] > 0.0) {
                        rgbaOut[i4+3] = labelRGBA[3] * 255.0;
                    }
                    else {
                        rgbaOut[i4+3] = 0;
                    }
                }
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
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param colorDataType
 *    Data type of the rgbaOut parameter
 * @param rgbaOutPointer
 *    RGBA Colors that are output.  This is a VOID type and its
 *    true type is provided by the previous parameter colorDataType.
 * @param rgbv
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTabPrivate(const GiftiLabelTable* labelTable,
                                                        const float* labelIndices,
                                                        const int64_t numberOfIndices,
                                                        const DisplayGroupEnum::Enum displayGroup,
                                                        const int32_t tabIndex,
                                                        const ColorDataType colorDataType,
                                                        void* rgbaOutPointer)
{
    /*
     * Cast to data type for rgba coloring
     */
    float* rgbaFloat = NULL;
    uint8_t* rgbaUnsignedByte = NULL;
    switch (colorDataType) {
        case COLOR_TYPE_FLOAT:
            rgbaFloat = (float*)rgbaOutPointer;
            break;
        case COLOR_TYPE_UNSIGNED_BTYE:
            rgbaUnsignedByte = (uint8_t*)rgbaOutPointer;
            break;
    }
    
    
    /*
     * Invalidate all coloring.
     */
    switch (colorDataType) {
        case COLOR_TYPE_FLOAT:
            for (int64_t i = 0; i < numberOfIndices; i++) {
                rgbaFloat[i*4+3] = 0.0;
            }
            break;
        case COLOR_TYPE_UNSIGNED_BTYE:
            for (int64_t i = 0; i < numberOfIndices; i++) {
                rgbaUnsignedByte[i*4+3] = 0;
            }
            break;
    }
    
    /*
     * Assign colors from labels to nodes
     */
    float labelRGBA[4];
	for (int64_t i = 0; i < numberOfIndices; i++) {
        const int64_t labelKey = static_cast<int64_t>(labelIndices[i]);
        const GiftiLabel* gl = labelTable->getLabel(labelKey);
        if (gl != NULL) {
            const GroupAndNameHierarchyItem* item = gl->getGroupNameSelectionItem();
            bool colorDataFlag = false;
            if (item != NULL) {
                if (tabIndex == NodeAndVoxelColoring::INVALID_TAB_INDEX) {
                    colorDataFlag = true;
                }
                else if (item->isSelected(displayGroup, tabIndex)) {
                    colorDataFlag = true;
                }
            }
            else {
                colorDataFlag = true;
            }
            
            if (colorDataFlag) {
                gl->getColor(labelRGBA);
                if (labelRGBA[3] > 0.0) {
                    const int64_t i4 = i * 4;
                    
                    switch (colorDataType) {
                        case COLOR_TYPE_FLOAT:
                            CaretAssertArrayIndex(rgbaFloat, numberOfIndices * 4, i*4+3);
                            rgbaFloat[i*4] = labelRGBA[0];
                            rgbaFloat[i*4+1] = labelRGBA[1];
                            rgbaFloat[i*4+2] = labelRGBA[2];
                            rgbaFloat[i*4+3] = labelRGBA[3];
                            break;
                        case COLOR_TYPE_UNSIGNED_BTYE:
                            CaretAssertArrayIndex(rgbaUnsignedByte, numberOfIndices * 4, i*4+3);
                            rgbaUnsignedByte[i4]   = labelRGBA[0] * 255.0;
                            rgbaUnsignedByte[i4+1] = labelRGBA[1] * 255.0;
                            rgbaUnsignedByte[i4+2] = labelRGBA[2] * 255.0;
                            if (labelRGBA[3] > 0.0) {
                                rgbaUnsignedByte[i4+3] = labelRGBA[3] * 255.0;
                            }
                            else {
                                rgbaUnsignedByte[i4+3] = 0;
                            }
                            break;
                    }
                }
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
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbv
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                                                   const float* labelIndices,
                                                                   const int64_t numberOfIndices,
                                                                   float* rgbv)
{
    NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTabPrivate(labelTable,
                                                                              labelIndices,
                                                                              numberOfIndices,
                                                                              DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                                              NodeAndVoxelColoring::INVALID_TAB_INDEX,
                                                                              COLOR_TYPE_FLOAT,
                                                                              (void*)rgbv);
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
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbv
 *     Output with assigned colors.  Number of elements is (numberOfIndices * 4).
 */
void
NodeAndVoxelColoring::colorIndicesWithLabelTable(const GiftiLabelTable* labelTable,
                                                                   const float* labelIndices,
                                                                   const int64_t numberOfIndices,
                                                                   uint8_t* rgbv)
{
    
    NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTabPrivate(labelTable,
                                                                              labelIndices,
                                                                              numberOfIndices,
                                                                              DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                                              NodeAndVoxelColoring::INVALID_TAB_INDEX,
                                                                              COLOR_TYPE_UNSIGNED_BTYE,
                                                                              (void*)rgbv);
}

/**
 * Convert the slice coloring to outline mode.
 *
 * @param rgbaInOut
 *    Coloring for the slice (input and output)
 * @param labelDrawingType
 *    Type of drawing for label filling and outline.
 * @param labelOutlineColor
 *    Outline color of label.
 * @param xdim
 *    X-dimension of slice (number of columns)
 * @param ydim
 *    Y-dimension of slice (number of rows).
 */
void
NodeAndVoxelColoring::convertSliceColoringToOutlineMode(uint8_t* rgbaInOut,
                                                        const LabelDrawingTypeEnum::Enum labelDrawingType,
                                                        const CaretColorEnum::Enum labelOutlineColor,
                                                        const int64_t xdim,
                                                        const int64_t ydim)
{
    /*
     * Copy the rgba colors
     */
    const int64_t numRGBA = xdim * ydim * 4;
    if (numRGBA <= 0) {
        return;
    }
    std::vector<uint8_t> sliceCopyVector(numRGBA);
    uint8_t* rgba = &sliceCopyVector[0];
    
    for (int64_t i = 0; i < numRGBA; i++) {
        rgba[i] = rgbaInOut[i];
    }
    
    uint8_t outlineRGBA[4];
    CaretColorEnum::toRGBAByte(labelOutlineColor,
                               outlineRGBA);
    outlineRGBA[3] = 255;
    
    /*
     * Examine coloring for all voxels except those along the edge
     */
    const int64_t lastX = xdim - 1;
    const int64_t lastY = ydim - 1;
    for (int64_t i = 1; i < lastX; i++) {
        for (int64_t j = 1; j < lastY; j++) {
            const int iStart = i - 1;
            const int iEnd   = i + 1;
            const int jStart = j - 1;
            const int jEnd   = j + 1;
            
            const int64_t myOffset = (i + (xdim * j)) * 4;
            CaretAssert(myOffset < numRGBA);
            
            const uint8_t* myRGBA = &rgba[myOffset];
            if (myRGBA[3] <= 0) {
                continue;
            }
            
            /*
             * Determine if voxel colors match voxel coloring
             * of ALL immediate neighbors (8-connected).
             */
            bool isLabelBoundaryVoxel = false;
            for (int64_t iNeigh = iStart; iNeigh <= iEnd; iNeigh++) {
                for (int64_t jNeigh = jStart; jNeigh <= jEnd; jNeigh++) {
                    const int64_t neighOffset = (iNeigh + (xdim * jNeigh)) * 4;
                    CaretAssert(neighOffset < numRGBA);
                    const uint8_t* neighRGBA = &rgba[neighOffset];
                    
                    for (int64_t k = 0; k < 4; k++) {
                        if (myRGBA[k] != neighRGBA[k]) {
                            isLabelBoundaryVoxel = true;
                            break;
                        }
                    }
                    
                    if (isLabelBoundaryVoxel) {
                        break;
                    }
                }
                if (isLabelBoundaryVoxel) {
                    break;
                }
            }
            
            /*
             * Override the coloring as needed.
             */
            switch (labelDrawingType) {
                case LabelDrawingTypeEnum::DRAW_FILLED:
                    break;
                case LabelDrawingTypeEnum::DRAW_FILLED_WITH_OUTLINE_COLOR:
                    if (isLabelBoundaryVoxel) {
                        rgbaInOut[myOffset]     = outlineRGBA[0];
                        rgbaInOut[myOffset + 1] = outlineRGBA[1];
                        rgbaInOut[myOffset + 2] = outlineRGBA[2];
                        rgbaInOut[myOffset + 3] = outlineRGBA[3];
                    }
                    break;
                case LabelDrawingTypeEnum::DRAW_OUTLINE_COLOR:
                    if (isLabelBoundaryVoxel) {
                        rgbaInOut[myOffset]     = outlineRGBA[0];
                        rgbaInOut[myOffset + 1] = outlineRGBA[1];
                        rgbaInOut[myOffset + 2] = outlineRGBA[2];
                        rgbaInOut[myOffset + 3] = outlineRGBA[3];
                    }
                    else {
                        rgbaInOut[myOffset + 3] = 0;
                    }
                    break;
                case LabelDrawingTypeEnum::DRAW_OUTLINE_LABEL_COLOR:
                    if ( ! isLabelBoundaryVoxel) {
                        rgbaInOut[myOffset + 3] = 0;
                    }
                    break;
            }
        }
    }
}

/**
 * Convert the palette slice coloring to outline mode.
 *
 * @param rgbaInOut
 *    Coloring for the slice (input and output)
 * @param outlineMode
 *    Outline mode
 * @param outlineColor
 *    Type of drawing for label filling and outline.
 * @param labelOutlineColor
 *    Outline color of label.
 * @param xdim
 *    X-dimension of slice (number of columns)
 * @param ydim
 *    Y-dimension of slice (number of rows).
 */
void
NodeAndVoxelColoring::convertPaletteSliceColoringToOutlineMode(uint8_t* rgbaInOut,
                                                               const PaletteThresholdOutlineDrawingModeEnum::Enum outlineMode,
                                                               const CaretColorEnum::Enum outlineColor,
                                                               const int64_t xdim,
                                                               const int64_t ydim)
{
    bool hideDataFlag = false;
    switch (outlineMode) {
        case PaletteThresholdOutlineDrawingModeEnum::OFF:
            return;
            break;
        case PaletteThresholdOutlineDrawingModeEnum::OUTLINE:
            hideDataFlag = true;
            break;
        case PaletteThresholdOutlineDrawingModeEnum::OUTLINE_AND_DATA:
            break;
    }

    /*
     * Copy the rgba colors
     */
    const int64_t numRGBA = xdim * ydim * 4;
    if (numRGBA <= 0) {
        return;
    }
    std::vector<uint8_t> sliceCopyVector(numRGBA);
    uint8_t* rgba = &sliceCopyVector[0];
    
    for (int64_t i = 0; i < numRGBA; i++) {
        rgba[i] = rgbaInOut[i];
    }
    
    uint8_t outlineRGBA[4];
    CaretColorEnum::toRGBAByte(outlineColor,
                               outlineRGBA);
    outlineRGBA[3] = 255;
    
    /*
     * Examine coloring for all voxels except those along the edge
     */
    const int64_t lastX = xdim - 1;
    const int64_t lastY = ydim - 1;
    for (int64_t i = 1; i < lastX; i++) {
        for (int64_t j = 1; j < lastY; j++) {
            const int iStart = i - 1;
            const int iEnd   = i + 1;
            const int jStart = j - 1;
            const int jEnd   = j + 1;
            
            const int64_t myOffset = (i + (xdim * j)) * 4;
            CaretAssert(myOffset < numRGBA);
            
            const uint8_t* myRGBA = &rgba[myOffset];
            if (myRGBA[3] <= 0) {
                continue;
            }
            
            /*
             * Determine if voxel colors match voxel coloring
             * of ALL immediate neighbors (8-connected).
             */
            bool isLabelBoundaryVoxel = false;
            for (int64_t iNeigh = iStart; iNeigh <= iEnd; iNeigh++) {
                for (int64_t jNeigh = jStart; jNeigh <= jEnd; jNeigh++) {
                    if ((i != iNeigh)
                        || (j != jNeigh)) {
                        const int64_t neighOffset = (iNeigh + (xdim * jNeigh)) * 4;
                        CaretAssert(neighOffset < numRGBA);
                        const uint8_t* neighRGBA = &rgba[neighOffset];
                        if (neighRGBA[3] <= 0.0) {
                            isLabelBoundaryVoxel = true;
                        }
                    }
                    if (isLabelBoundaryVoxel) {
                        break;
                    }
                }
                if (isLabelBoundaryVoxel) {
                    break;
                }
            }
            
            if (isLabelBoundaryVoxel) {
                rgbaInOut[myOffset]     = outlineRGBA[0];
                rgbaInOut[myOffset + 1] = outlineRGBA[1];
                rgbaInOut[myOffset + 2] = outlineRGBA[2];
                rgbaInOut[myOffset + 3] = outlineRGBA[3];
            }
            else if (hideDataFlag) {
                rgbaInOut[myOffset + 3] = 0;
            }
        }
    }
}


