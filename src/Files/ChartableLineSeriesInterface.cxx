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

#define __CHARTABLE_LINE_SERIES_INTERFACE_DECLARE__
#include "ChartableLineSeriesInterface.h"
#undef __CHARTABLE_LINE_SERIES_INTERFACE_DECLARE__

#include "CaretMappableDataFile.h"

using namespace caret;

/**
 * \class caret::ChartableLineSeriesInterface
 * \brief Interface for files that are able to produce line charts charts.
 * \ingroup Files
 */


/**
 * Is the given chart data type supported by this file.
 *
 * @param chartDataType
 *    Chart data type for testing support.
 * @return
 *    True if chart data type is supported by the file, else false.
 */
bool
ChartableLineSeriesInterface::isLineSeriesChartDataTypeSupported(const ChartOneDataTypeEnum::Enum chartDataType) const
{
    std::vector<ChartOneDataTypeEnum::Enum> validTypes;
    getSupportedLineSeriesChartDataTypes(validTypes);
    
    if (std::find(validTypes.begin(),
                  validTypes.end(),
                  chartDataType) != validTypes.end()) {
        return true;
    }
    
    return false;
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 * Will be NULL if this interface is not implemented by a CaretMappableDataFile.
 */
CaretMappableDataFile*
ChartableLineSeriesInterface::getLineSeriesChartCaretMappableDataFile()
{
    CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(this);
    CaretAssert(cmdf);
    return cmdf;
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 * Will be NULL if this interface is not implemented by a CaretMappableDataFile.
 */
const CaretMappableDataFile*
ChartableLineSeriesInterface::getLineSeriesChartCaretMappableDataFile() const
{
    const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(this);
    CaretAssert(cmdf);
    return cmdf;
}

