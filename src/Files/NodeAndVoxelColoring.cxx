
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
#include "CaretOMP.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyItem.h"
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
 * Color scalars using a palette that accepts a void* type for the 
 * color array to that multiple data types are supported without
 * having to allocate memory for conversion to one data type or 
 * the other nor duplicate lots of code for each data type.
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
                                              const Palette* palette,
                                              const float* scalarValues,
                                              const float* thresholdValues,
                                              const int64_t numberOfScalars,
                                              const ColorDataType colorDataType,
                                              void* rgbaOutPointer,
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
        if (thresholdPassedFlag == false) {
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
                                              const int64_t numberOfScalars,
                                              float* rgbaOut,
                                              const bool ignoreThresholding)
{
    colorScalarsWithPalettePrivate(statistics,
                                   paletteColorMapping,
                                   palette,
                                   scalarValues,
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
                                              const int64_t numberOfScalars,
                                              uint8_t* rgbaOut,
                                              const bool ignoreThresholding)
{
    colorScalarsWithPalettePrivate(statistics,
                                   paletteColorMapping,
                                   palette,
                                   scalarValues,
                                   thresholdValues,
                                   numberOfScalars,
                                   COLOR_TYPE_UNSIGNED_BTYE,
                                   (void*)rgbaOut,
                                   ignoreThresholding);
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
NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTab(const GiftiLabelTable* labelTable,
                                                 const float* labelIndices,
                                                 const int64_t numberOfIndices,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 float* rgbv)
{
    NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTabPrivate(labelTable,
                                                            labelIndices,
                                                            numberOfIndices,
                                                            displayGroup,
                                                            tabIndex,
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
NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTab(const GiftiLabelTable* labelTable,
                                                 const float* labelIndices,
                                                 const int64_t numberOfIndices,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 uint8_t* rgbv)
{
    NodeAndVoxelColoring::colorIndicesWithLabelTableForDisplayGroupTabPrivate(labelTable,
                                                            labelIndices,
                                                            numberOfIndices,
                                                            displayGroup,
                                                            tabIndex,
                                                            COLOR_TYPE_UNSIGNED_BTYE,
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
    CaretColorEnum::toRGBByte(labelOutlineColor,
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


