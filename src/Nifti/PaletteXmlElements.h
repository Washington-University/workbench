#ifndef __PALETTEXMLELEMENTS_H__
#define __PALETTEXMLELEMENTS_H__
/*
 * Copyright 1995-2009 Washington University School of Medicine.
 *
 * http://brainmap.wustl.edu
 *
 * CARET is free software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QString>

namespace caret {

/**
  * GIFTI XML Element tags
  */
namespace PaletteXmlElements {    
    static const QString XML_TAG_PALETTE_COLOR_MAPPING = "PaletteColorMapping";
    static const QString XML_TAG_SCALE_MODE = "ScaleMode";
    static const QString XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES = "AutoScalePercentageValues";
    static const QString XML_TAG_USER_SCALE_VALUES = "UserScaleValues";
    static const QString XML_TAG_PALETTE_NAME = "PaletteName";
    static const QString XML_TAG_INTERPOLATE = "InterpolatePalette";
    static const QString XML_TAG_DISPLAY_POSITIVE = "DisplayPositiveData";   
    static const QString XML_TAG_DISPLAY_NEGATIVE = "DisplayNegativeData";
    static const QString XML_TAG_DISPLAY_ZERO = "DisplayZeroData";
    static const QString XML_TAG_THRESHOLD_TEST = "ThresholdTest";
    static const QString XML_TAG_THRESHOLD_TYPE = "ThresholdType";
    static const QString XML_TAG_THRESHOLD_NORMAL_VALUES = "ThresholdNormalValues";
    static const QString XML_TAG_THRESHOLD_MAPPED_VALUES = "ThresholdMappedValues";
    static const QString XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES = "ThresholdMappedAvgAreaValues";
    static const QString XML_TAG_THRESHOLD_DATA_NAME = "ThresholdDataName";
    static const QString XML_TAG_THRESHOLD_FAILURE_IN_GREEN = "ThresholdFailureInGreen";
    
    static const QString XML_ATTRIBUTE_VERSION_NUMBER = "Version";
    static const QString XML_VERSION_NUMBER = "1";
};  // namespace

    
}  // namespace

#endif // __PALETTEXMLELEMENTS_H__

