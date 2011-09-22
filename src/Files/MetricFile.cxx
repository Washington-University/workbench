/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "CaretAssert.h"

#include "GiftiFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"

using namespace caret;

/**
 * Constructor.
 */
MetricFile::MetricFile()
: GiftiTypeFile()
{
    this->initializeMembersMetricFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
MetricFile::MetricFile(const MetricFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperMetricFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the MetricFile parameter.
 */
MetricFile& 
MetricFile::operator=(const MetricFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperMetricFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
MetricFile::~MetricFile()
{
    this->columnDataPointers.clear();
}

/**
 * Clear the surface file.
 */
void 
MetricFile::clear()
{
    GiftiTypeFile::clear();
    this->columnDataPointers.clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
MetricFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->columnDataPointers.clear();

    this->initializeMembersMetricFile();
        
    this->verifyDataArraysHaveSameNumberOfRows(0, 0);

    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfDataArrays; i++) {
        this->columnDataPointers.push_back(this->giftiFile->getDataArray(i)->getDataPointerFloat());
    }
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
MetricFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
MetricFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}

/**
 * Initialize members of this class.
 */
void 
MetricFile::initializeMembersMetricFile()
{
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
MetricFile::copyHelperMetricFile(const MetricFile& sf)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get value for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Value at the given node and column indices.
 */
float 
MetricFile::getValue(const int32_t nodeIndex,
                     const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), 
                       "Node Index out of range.");
    
    return this->columnDataPointers[columnIndex][nodeIndex];
}

/**
 * set label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * param value
 *     Value inserted at the given node and column indices.
 */
void 
MetricFile::setValue(const int32_t nodeIndex,
                     const int32_t columnIndex,
                     const float value)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), "Node Index out of range.");
    
    this->columnDataPointers[columnIndex][nodeIndex] = value;
}

const float* 
MetricFile::getValuePointerForColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    return this->columnDataPointers[columnIndex];
}

/**
 * Get minimum and maximum values.
 * @ param columnIndex
 * @ param minValue
 * @ param maxValue
 */
void 
MetricFile::getMinMaxValues(const int32_t columnIndex,
                            float& minValue, 
                            float& maxValue) const
{
    this->giftiFile->getDataArray(columnIndex)->getMinMaxValues(minValue, maxValue);
}

/**
 * Get data column min/max for the specified percentages.
 * @ param columnIndex
 *   Index of column for min/max value.
 * @ param negMaxPct
 * @ param negMinPct
 * @ param posMinPct
 * @ param posMaxPct
 * @ param negMaxValueOut
 * @ param negMinValueOut
 * @ param posMinValueOut
 * @ param posMaxValueOut
 */
void 
MetricFile::getMinMaxValuesFromPercentages(const int32_t columnIndex,
                                           const float negMaxPct,
                                           const float negMinPct,
                                           const float posMinPct,
                                           const float posMaxPct,
                                           float& negMaxValueOut,
                                           float& negMinValueOut,
                                           float& posMinValueOut,
                                           float& posMaxValueOut)
{

    GiftiDataArray* gda = this->giftiFile->getDataArray(columnIndex);
    gda->getMinMaxValuesFromPercentages(negMaxPct, 
                                        negMinPct, 
                                        posMinPct, 
                                        posMaxPct, 
                                        negMaxValueOut, 
                                        negMinValueOut, 
                                        posMinValueOut, 
                                        posMaxValueOut);
}

/**
 * Get the values for color mapping.
 *
 * @param displayColumn  - the display column.
 *
 * @return  Array of four floats: neg-max, neg-min, pos-min, pos-max.
 */
void
MetricFile::getMinMaxForColorMapping(int displayColumn,
                                        float minMaxOut[4]) {
    if ((displayColumn < 0) ||
        (displayColumn >= this->getNumberOfColumns())) {
        minMaxOut[0] = -1.0;
        minMaxOut[1] =  0.0;
        minMaxOut[2] =  0.0;
        minMaxOut[3] =  1.0;
        return;
    }
    
    //
    // Mapping min/max
    //
    float negMax = -1.0f;
    float negMin =  0.0f;
    float posMin =  0.0f;
    float posMax =  1.0f;
    
    PaletteColorMapping* paletteColorMapping =
        this->getPaletteColorMapping(displayColumn);
    
    switch (paletteColorMapping->getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
        {
            this->getMinMaxValues(displayColumn, negMax, posMax);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            this->getMinMaxValuesFromPercentages(displayColumn,
                                                 paletteColorMapping->getAutoScalePercentageNegativeMaximum(), 
                                                 paletteColorMapping->getAutoScalePercentageNegativeMinimum(),
                                                 paletteColorMapping->getAutoScalePercentagePositiveMinimum(), 
                                                 paletteColorMapping->getAutoScalePercentagePositiveMaximum(),
                                                 negMax,
                                                 negMin,
                                                 posMin, 
                                                 posMax);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            negMax = paletteColorMapping->getUserScaleNegativeMaximum();
            negMin = paletteColorMapping->getUserScaleNegativeMinimum();
            posMin = paletteColorMapping->getUserScalePositiveMinimum();
            posMax = paletteColorMapping->getUserScalePositiveMaximum();
            break;
    }
    
    minMaxOut[0] = negMax;
    minMaxOut[1] = negMin;
    minMaxOut[2] = posMin;
    minMaxOut[3] = posMax;
}

