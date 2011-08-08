#ifndef __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__
#define __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__

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

#include <string>

namespace PaletteColorMappingXmlElements {
    static const std::string XML_TAG_PALETTE_COLOR_MAPPING = "PaletteColorMapping";
    static const std::string XML_TAG_SCALE_MODE = "ScaleMode";
    static const std::string XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES = "AutoScalePercentageValues";
    static const std::string XML_TAG_USER_SCALE_VALUES = "UserScaleValues";
    static const std::string XML_TAG_PALETTE_NAME = "PaletteName";
    static const std::string XML_TAG_INTERPOLATE = "InterpolatePalette";
    static const std::string XML_TAG_DISPLAY_POSITIVE = "DisplayPositiveData";
    static const std::string XML_TAG_DISPLAY_NEGATIVE = "DisplayNegativeData";
    static const std::string XML_TAG_DISPLAY_ZERO = "DisplayZeroData";
    static const std::string XML_TAG_THRESHOLD_TEST = "ThresholdTest";
    static const std::string XML_TAG_THRESHOLD_TYPE = "ThresholdType";
    static const std::string XML_TAG_THRESHOLD_NORMAL_VALUES = "ThresholdNormalValues";
    static const std::string XML_TAG_THRESHOLD_MAPPED_VALUES = "ThresholdMappedValues";
    static const std::string XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES = "ThresholdMappedAvgAreaValues";
    static const std::string XML_TAG_THRESHOLD_DATA_NAME = "ThresholdDataName";
    static const std::string XML_TAG_THRESHOLD_FAILURE_IN_GREEN = "ThresholdFailureInGreen";
    
    static const std::string XML_ATTRIBUTE_VERSION_NUMBER = "Version";
    static const int VERSION_NUMBER = 1;    
}  // namespace

#endif // __PALETTE_COLOR_MAPPING_XML_ELEMENTS_H__
