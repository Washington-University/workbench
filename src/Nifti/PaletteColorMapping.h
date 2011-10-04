#ifndef __PALETTECOLORMAPPING_H__
#define __PALETTECOLORMAPPING_H__

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

#include <AString.h>

#include "CaretObject.h"

#include "PaletteEnums.h"
#include "XmlException.h"

namespace caret {

class XmlWriter;
    
/**
 * Controls color mapping using a palette.
 */
class PaletteColorMapping : public CaretObject {

public:
    PaletteColorMapping();

    PaletteColorMapping(const PaletteColorMapping& o);

    PaletteColorMapping& operator=(const PaletteColorMapping& o);

    virtual ~PaletteColorMapping();

private:
    void copyHelper(const PaletteColorMapping& o);

    void initializeMembersPaletteColorMapping();

public:
    void writeAsXML(XmlWriter& xmlWriter)
            throw (XmlException);

    AString encodeInXML()
            throw (XmlException);


    void decodeFromStringXML(const AString& xml)
            throw (XmlException);

    float getAutoScalePercentageNegativeMaximum() const;

    void setAutoScalePercentageNegativeMaximum(const float autoScalePercentageNegativeMaximum);

    float getAutoScalePercentageNegativeMinimum() const;

    void setAutoScalePercentageNegativeMinimum(const float autoScalePercentageNegativeMinimum);

    float getAutoScalePercentagePositiveMaximum() const;

    void setAutoScalePercentagePositiveMaximum(const float autoScalePercentagePositiveMaximum);

    float getAutoScalePercentagePositiveMinimum() const;

    void setAutoScalePercentagePositiveMinimum(const float autoScalePercentagePositiveMinimum);

    bool isDisplayNegativeDataFlag() const;

    void setDisplayNegativeDataFlag(const bool displayNegativeDataFlag);

    bool isDisplayPositiveDataFlag() const;

    void setDisplayPositiveDataFlag(const bool displayPositiveDataFlag);

    bool isDisplayZeroDataFlag() const;

    void setDisplayZeroDataFlag(const bool displayZeroDataFlag);

    bool isInterpolatePaletteFlag() const;

    void setInterpolatePaletteFlag(const bool interpolatePaletteFlag);

    PaletteScaleModeEnum::Enum getScaleMode() const;

    void setScaleMode(const PaletteScaleModeEnum::Enum scaleMode);

    AString getSelectedPaletteName() const;

    void setSelectedPaletteName(const AString& selectedPaletteName);

    void setSelectedPaletteToPsych();

    void setSelectedPaletteToPsychNoNone();

    void setSelectedPaletteToOrangeYellow();

    void setSelectedPaletteToGrayInterpolated();

    float getUserScaleNegativeMaximum() const;

    void setUserScaleNegativeMaximum(const float userScaleNegativeMaximum);

    float getUserScaleNegativeMinimum() const;

    void setUserScaleNegativeMinimum(const float userScaleNegativeMinimum);

    float getUserScalePositiveMaximum() const;

    void setUserScalePositiveMaximum(const float userScalePositiveMaximum);

    float getUserScalePositiveMinimum() const;

    void setUserScalePositiveMinimum(const float userScalePositiveMinimum);

    float getThresholdMappedAverageAreaNegative() const;

    bool performThresholdTest(const float value);

    void setThresholdMappedAverageAreaNegative(const float thresholdMappedAverageAreaNegative);

    float getThresholdMappedAverageAreaPositive() const;

    void setThresholdMappedAverageAreaPositive(const float thresholdMappedAverageAreaPositive);

    float getThresholdMappedNegative() const;

    void setThresholdMappedNegative(const float thresholdMappedNegative);

    float getThresholdMappedPositive() const;

    void setThresholdMappedPositive(const float thresholdMappedPositive);

    float getThresholdNormalNegative() const;

    void setThresholdNormalNegative(const float thresholdNormalNegative);

    float getThresholdNormalPositive() const;

    void setThresholdNormalPositive(const float thresholdNormalPositive);

    PaletteThresholdTestEnum::Enum getThresholdTest() const;

    void setThresholdTest(const PaletteThresholdTestEnum::Enum thresholdTest);

    PaletteThresholdTypeEnum::Enum getThresholdType() const;

    void setThresholdType(const PaletteThresholdTypeEnum::Enum thresholdType);

    AString getThresholdDataName() const;

    void setThresholdDataName(const AString& thresholdDataName);

    bool isShowThresholdFailureInGreen() const;

    void setShowThresholdFailureInGreen(const bool showInGreenFlag);

    void setModified();

    void clearModified();

    bool isModified() const;

private:
    PaletteScaleModeEnum::Enum scaleMode;

    float autoScalePercentageNegativeMaximum;

    float autoScalePercentageNegativeMinimum;

    float autoScalePercentagePositiveMinimum;

    float autoScalePercentagePositiveMaximum;

    float userScaleNegativeMaximum;

    float userScaleNegativeMinimum;

    float userScalePositiveMinimum;

    float userScalePositiveMaximum;

    AString selectedPaletteName;

    bool interpolatePaletteFlag;

    bool displayPositiveDataFlag;

    bool displayZeroDataFlag;

    bool displayNegativeDataFlag;

    PaletteThresholdTypeEnum::Enum thresholdType;

    PaletteThresholdTestEnum::Enum thresholdTest;

    float thresholdNormalNegative;

    float thresholdNormalPositive;

    float thresholdMappedNegative;

    float thresholdMappedPositive;

    float thresholdMappedAverageAreaNegative;

    float thresholdMappedAverageAreaPositive;

    AString thresholdDataName;

    bool thresholdShowFailureInGreen;

    /**Tracks modification, DO NOT copy */
    bool modifiedFlag;

};

} // namespace

#endif // __PALETTECOLORMAPPING_H__
