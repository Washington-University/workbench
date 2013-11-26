
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

#define __CARET_MAPPABLE_DATA_FILE_DECLARE__
#include "CaretMappableDataFile.h"
#undef __CARET_MAPPABLE_DATA_FILE_DECLARE__

#include <limits>

#include "DataFileContentInformation.h"
#include "DescriptiveStatistics.h"
#include "Histogram.h"
#include "StringTableModel.h"

using namespace caret;


    
/**
 * Constructor.
 */
CaretMappableDataFile::CaretMappableDataFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    
}

/**
 * Destructor.
 */
CaretMappableDataFile::~CaretMappableDataFile()
{
    
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is copied.
 */
CaretMappableDataFile::CaretMappableDataFile(const CaretMappableDataFile& cmdf)
: CaretDataFile(cmdf)
{
    this->copyCaretMappableDataFile(cmdf);
}

/**
 * Constructor.
 * @param cmdf
 *   Instance that is assigned to this.
 * @return
 *   Reference to this instance.
 */
CaretMappableDataFile& 
CaretMappableDataFile::operator=(const CaretMappableDataFile& cmdf)
{
    if (this != &cmdf) {
        CaretDataFile::operator=(cmdf);
        this->copyCaretMappableDataFile(cmdf);
    }
    return *this;
}

/**
 * Assists with copying instances of this class.
 */
void 
CaretMappableDataFile::copyCaretMappableDataFile(const CaretMappableDataFile&)
{
    
}

// note: method is documented in header file
bool
CaretMappableDataFile::hasMapAttributes() const
{
    return true;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromNameOrNumber(const AString& mapName)
{
    bool ok = false;
    int32_t ret = mapName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)
    {
        if (ret < 0 || ret >= getNumberOfMaps())
        {
            ret = -1;
        }
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        ret = getMapIndexFromName(mapName);
    }
    return ret;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromName(const AString& mapName)
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (mapName == getMapName(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
int32_t
CaretMappableDataFile::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    int32_t numMaps = getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; ++i)
    {
        if (uniqueID == getMapUniqueID(i))
        {
            return i;
        }
    }
    return -1;
}

// note: method is documented in header file
void
CaretMappableDataFile::updateScalarColoringForAllMaps(const PaletteFile* paletteFile)
{
    const int32_t numMaps = getNumberOfMaps();
    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
        updateScalarColoringForMap(iMap,
                             paletteFile);
    }
}

// note: method is documented in header file
NiftiTimeUnitsEnum::Enum
CaretMappableDataFile::getMapIntervalUnits() const
{
    return NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
}

// note: method is documented in header file
void
CaretMappableDataFile::getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                        float& mapIntervalStepValueOut) const
{
    firstMapUnitsValueOut   = 1.0;
    mapIntervalStepValueOut = 1.0;
}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
CaretMappableDataFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    dataRangeMaximumOut = std::numeric_limits<float>::max();
    dataRangeMinimumOut = -dataRangeMaximumOut;
    
    return false;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
CaretMappableDataFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Maps to Surface",
                                        isSurfaceMappable());
    dataFileInformation.addNameAndValue("Maps to Volume",
                                        isVolumeMappable());
    dataFileInformation.addNameAndValue("Maps with LabelTable",
                                        isMappedWithLabelTable());
    dataFileInformation.addNameAndValue("Maps with Palette",
                                        isMappedWithPalette());
    
    const bool showMapFlag = ((isMappedWithLabelTable() || isMappedWithPalette())
                              && (isSurfaceMappable() || isVolumeMappable()));
    
    
    if (showMapFlag) {
        const int32_t numMaps = getNumberOfMaps();
        if (isSurfaceMappable()) {
            dataFileInformation.addNameAndValue("Number of Maps",
                                                numMaps);
        }
        
        if (numMaps > 0) {
            int columnCount = 0;
            const int COL_INDEX   = columnCount++;
            
            int32_t COL_MIN     = -1;
            int32_t COL_MAX     = -1;
            int32_t COL_MEAN    = -1;
            int32_t COL_DEV     = -1;
            int32_t COL_PCT_POS = -1;
            int32_t COL_PCT_NEG = -1;
            int32_t COL_INF_NAN = -1;
            
            if (isMappedWithPalette()) {
                COL_MIN = columnCount++;
                COL_MAX = columnCount++;
                COL_MEAN = columnCount++;
                COL_DEV = columnCount++;
                COL_PCT_POS = columnCount++;
                COL_PCT_NEG = columnCount++;
                COL_INF_NAN = columnCount++;
            }
            const int COL_NAME = columnCount++;
            
            /*
             * Include a row for the column titles
             */
            const int32_t tableRowCount = numMaps + 1;
            StringTableModel stringTable(tableRowCount,
                                         columnCount);
            
            stringTable.setElement(0, COL_INDEX, "Map");
            if (COL_MIN >= 0) {
                stringTable.setElement(0, COL_MIN, "Minimum");
            }
            if (COL_MAX >= 0) {
                stringTable.setElement(0, COL_MAX, "Maximum");
            }
            if (COL_MEAN >= 0) {
                stringTable.setElement(0, COL_MEAN, "Mean");
            }
            if (COL_DEV >= 0) {
                stringTable.setElement(0, COL_DEV, "Sample Dev");
            }
            if (COL_PCT_POS >= 0) {
                stringTable.setElement(0, COL_PCT_POS, "% Positive");
            }
            if (COL_PCT_NEG >= 0) {
                stringTable.setElement(0, COL_PCT_NEG, "% Negative");
            }
            if (COL_INF_NAN >= 0) {
                stringTable.setElement(0, COL_INF_NAN, "Inf/NaN");
            }
            
            stringTable.setElement(0, COL_NAME, "Map Name");
            stringTable.setColumnAlignment(COL_NAME, StringTableModel::ALIGN_LEFT);
            
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                
                const int32_t tableRow = mapIndex + 1;
                
                CaretAssert(COL_INDEX >= 0);
                CaretAssert(COL_NAME >= 0);
                stringTable.setElement(tableRow, COL_INDEX, (mapIndex + 1));
                stringTable.setElement(tableRow, COL_NAME, getMapName(mapIndex));
                
                if (isMappedWithPalette()) {
                    const DescriptiveStatistics* stats = const_cast<CaretMappableDataFile*>(this)->getMapStatistics(mapIndex);
                    
                    const Histogram* histogram = getMapHistogram(mapIndex);
                    int64_t posCount = 0;
                    int64_t zeroCount = 0;
                    int64_t negCount = 0;
                    int64_t infCount = 0;
                    int64_t negInfCount = 0;
                    int64_t nanCount = 0;
                    histogram->getCounts(posCount,
                                         zeroCount,
                                         negCount,
                                         infCount,
                                         negInfCount,
                                         nanCount);
                    const int64_t numInfinityAndNotANumber = (infCount
                                                + negInfCount
                                                + nanCount);
                    const double totalCount = (posCount
                                                + zeroCount
                                                + negCount
                                                + numInfinityAndNotANumber);
                    const double pctPositive = (posCount / totalCount) * 100.0;
                    const double pctNegative = (negCount / totalCount) * 100.0;
                    
                    CaretAssert(COL_MIN >= 0);
                    CaretAssert(COL_MAX >= 0);
                    CaretAssert(COL_MEAN >= 0);
                    CaretAssert(COL_DEV >= 0);
                    CaretAssert(COL_PCT_POS >= 0);
                    CaretAssert(COL_PCT_NEG >= 0);
                    CaretAssert(COL_INF_NAN >= 0);
                    stringTable.setElement(tableRow, COL_MIN, stats->getMinimumValue());
                    stringTable.setElement(tableRow, COL_MAX, stats->getMaximumValue());
                    stringTable.setElement(tableRow, COL_MEAN, stats->getMean());
                    stringTable.setElement(tableRow, COL_DEV, stats->getStandardDeviationSample());
                    stringTable.setElement(tableRow, COL_PCT_POS, pctPositive);
                    stringTable.setElement(tableRow, COL_PCT_NEG, pctNegative);
                    stringTable.setElement(tableRow, COL_INF_NAN, numInfinityAndNotANumber);
                }
            }
            
            dataFileInformation.addText(stringTable.getInString()
                                        + "\n");
        }
    }
}

