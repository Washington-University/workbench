#ifndef __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__
#define __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__

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

#include <AString.h>

using namespace caret;

namespace PaletteColorMappingXmlElements {
    static const AString XML_TAG_PALETTE_COLOR_MAPPING = "PaletteColorMapping";
    static const AString XML_TAG_SCALE_MODE = "ScaleMode";
    static const AString XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES = "AutoScalePercentageValues";
    static const AString XML_TAG_AUTO_SCALE_ABSOLUTE_PERCENTAGE_VALUES = "AutoScaleAbsolutePercentageValues";
    static const AString XML_TAG_USER_SCALE_VALUES = "UserScaleValues";
    static const AString XML_TAG_PALETTE_NAME = "PaletteName";
    static const AString XML_TAG_INTERPOLATE = "InterpolatePalette";
    static const AString XML_TAG_DISPLAY_POSITIVE = "DisplayPositiveData";
    static const AString XML_TAG_DISPLAY_NEGATIVE = "DisplayNegativeData";
    static const AString XML_TAG_DISPLAY_ZERO = "DisplayZeroData";
    static const AString XML_TAG_THRESHOLD_TEST = "ThresholdTest";
    static const AString XML_TAG_THRESHOLD_TYPE = "ThresholdType";
    static const AString XML_TAG_THRESHOLD_NORMAL_VALUES = "ThresholdNormalValues";
    static const AString XML_TAG_THRESHOLD_MAPPED_VALUES = "ThresholdMappedValues";
    static const AString XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES = "ThresholdMappedAvgAreaValues";
    static const AString XML_TAG_THRESHOLD_DATA_NAME = "ThresholdDataName";
    static const AString XML_TAG_THRESHOLD_FAILURE_IN_GREEN = "ThresholdFailureInGreen";
    static const AString XML_TAG_THRESHOLD_RANGE_MODE = "ThresholdRangeMode";
    static const AString XML_TAG_THRESHOLD_NEG_MIN_POS_MAX_LINKED = "ThresholdLowHighLinked";
    static const AString XML_TAG_NUMERIC_FORMAT_MODE = "NumericFormatMode";
    static const AString XML_TAG_PRECISION_DIGITS = "PrecisionDigits";
    static const AString XML_TAG_NUMERIC_SUBDIVISIONS = "NumericSubivisions";
    static const AString XML_TAG_COLOR_BAR_VALUES_MODE = "ColorBarValuesMode";
    static const AString XML_ATTRIBUTE_VERSION_NUMBER = "Version";
    static const int XML_VERSION_NUMBER = 1;
}  // namespace

#endif // __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__
