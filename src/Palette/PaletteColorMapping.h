#ifndef __PALETTECOLORMAPPING_H__
#define __PALETTECOLORMAPPING_H__

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

#include "CaretObject.h"

#include "NumericFormatModeEnum.h"
#include "PaletteColorBarValuesModeEnum.h"
#include "PaletteEnums.h"
#include "PaletteThresholdRangeModeEnum.h"
#include "XmlException.h"

namespace caret {

    class FastStatistics;
    class XmlWriter;
    
    /**
     * Controls color mapping using a palette.
     */
    class PaletteColorMapping : public CaretObject {
        
    public:
        PaletteColorMapping();
        
        PaletteColorMapping(const PaletteColorMapping& o);
        
        PaletteColorMapping& operator=(const PaletteColorMapping& o);
        
        bool operator==(const PaletteColorMapping& pcm) const;
        
        bool operator!=(const PaletteColorMapping& pcm) const { return !((*this) == pcm); }
        
        virtual ~PaletteColorMapping();
        
        void copy(const PaletteColorMapping& pcm);
        
    private:
        void copyHelper(const PaletteColorMapping& o);
        
        void initializeMembersPaletteColorMapping();
        
    public:
        void writeAsXML(XmlWriter& xmlWriter);
        
        AString encodeInXML();
        
        
        void decodeFromStringXML(const AString& xml);
        
        float getAutoScalePercentageNegativeMaximum() const;
        
        void setAutoScalePercentageNegativeMaximum(const float autoScalePercentageNegativeMaximum);
        
        float getAutoScalePercentageNegativeMinimum() const;
        
        void setAutoScalePercentageNegativeMinimum(const float autoScalePercentageNegativeMinimum);
        
        float getAutoScalePercentagePositiveMaximum() const;
        
        void setAutoScalePercentagePositiveMaximum(const float autoScalePercentagePositiveMaximum);
        
        float getAutoScalePercentagePositiveMinimum() const;
        
        void setAutoScalePercentagePositiveMinimum(const float autoScalePercentagePositiveMinimum);
        
        float getAutoScaleAbsolutePercentageMinimum() const;
        
        void setAutoScaleAbsolutePercentageMinimum(const float autoScaleAbsolutePercentageMinimum);
        
        float getAutoScaleAbsolutePercentageMaximum() const;
        
        void setAutoScaleAbsolutePercentageMaximum(const float autoScaleAbsolutePercentageMaximum);
        
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
        
        float getThresholdMappedAverageAreaMinimum() const;
        
        void setThresholdMappedAverageAreaMinimum(const float thresholdMappedAverageAreaMinimum);
        
        float getThresholdMappedAverageAreaMaximum() const;
        
        void setThresholdMappedAverageAreaMaximum(const float thresholdMappedAverageAreaPositive);
        
        float getThresholdMappedMinimum() const;
        
        void setThresholdMappedMinimum(const float thresholdMappedMinimum);
        
        float getThresholdMappedMaximum() const;
        
        void setThresholdMappedMaximum(const float thresholdMappedPositive);
        
        float getThresholdNormalMinimum() const;
        
        void setThresholdNormalMinimum(const float thresholdNormalMinimum);
        
        float getThresholdNormalMaximum() const;
        
        void setThresholdNormalMaximum(const float thresholdNormalPositive);
        
        float getThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType) const;
        
        float getThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType) const;
        
        void setThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                 const float thresholdMinimum);
        
        void setThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                 const float thresholdMaximum);
        
        PaletteThresholdTestEnum::Enum getThresholdTest() const;
        
        void setThresholdTest(const PaletteThresholdTestEnum::Enum thresholdTest);
        
        PaletteThresholdTypeEnum::Enum getThresholdType() const;
        
        void setThresholdType(const PaletteThresholdTypeEnum::Enum thresholdType);
        
        PaletteThresholdRangeModeEnum::Enum getThresholdRangeMode() const;
        
        void setThresholdRangeMode(const PaletteThresholdRangeModeEnum::Enum rangeMode);
        
        AString getThresholdDataName() const;
        
        void setThresholdDataName(const AString& thresholdDataName);
        
        bool isShowThresholdFailureInGreen() const;
        
        void setShowThresholdFailureInGreen(const bool showInGreenFlag);
        
        bool isThresholdNegMinPosMaxLinked() const;
        
        void setThresholdNegMinPosMaxLinked(const bool linked);
        
        NumericFormatModeEnum::Enum getNumericFormatMode() const;
        
        int32_t getPrecisionDigits() const;
        
        int32_t getNumericSubdivisionCount() const;
        
        void setNumericFormatMode(const NumericFormatModeEnum::Enum numericFormatMode);
        
        void setPrecisionDigits(const int32_t precisionDigits);
        
        void setNumericSubdivisionCount(const int32_t numericSubdivisionCount);
        
        PaletteColorBarValuesModeEnum::Enum getColorBarValuesMode() const;
        
        void setColorBarValuesMode(const PaletteColorBarValuesModeEnum::Enum colorBarValuesMode);
        
        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
        void mapDataToPaletteNormalizedValues(const FastStatistics* statistics,
                                              const float* dataValues,
                                              float* normalizedValuesOut,
                                              const int64_t numberOfData) const;
        
//        void getPaletteColorBarScaleText(const FastStatistics* statistics,
//                                         AString& minimumValueTextOut,
//                                         AString& zeroValueTextOut,
//                                         AString& maximumValueTextOut) const;
        
        void getPaletteColorBarScaleText(const FastStatistics* statistics,
                                         std::vector<std::pair<float, AString> >& normalizedPositionAndTextOut) const;
        
        /** A positive value near zero - may be zero! */
        static const float SMALL_POSITIVE;
        
        /** A negative value near zero - may be zero! */
        static const float SMALL_NEGATIVE;
        
    private:
        PaletteScaleModeEnum::Enum scaleMode;
        
        float autoScalePercentageNegativeMaximum;
        
        float autoScalePercentageNegativeMinimum;
        
        float autoScalePercentagePositiveMinimum;
        
        float autoScalePercentagePositiveMaximum;
        
        float autoScaleAbsolutePercentageMinimum;
        
        float autoScaleAbsolutePercentageMaximum;
        
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
        
        PaletteThresholdRangeModeEnum::Enum thresholdRangeMode;
        
        float thresholdNormalMinimum;
        
        float thresholdNormalMaximum;
        
        float thresholdMappedMinimum;
        
        float thresholdMappedMaximum;
        
        float thresholdMappedAverageAreaMinimum;
        
        float thresholdMappedAverageAreaMaximum;
        
        AString thresholdDataName;
        
        bool thresholdShowFailureInGreen;
        
        bool thresholdNegMinPosMaxLinked;
        
        NumericFormatModeEnum::Enum numericFormatMode;
        
        int32_t precisionDigits;
        
        int32_t numericSubdivisionCount;
        
        PaletteColorBarValuesModeEnum::Enum colorBarValuesMode;
    
        /**Tracks modification, DO NOT copy */
        bool modifiedFlag;
        
    };

#ifdef __PALETTE_COLOR_MAPPING_DECLARE__
    const float PaletteColorMapping::SMALL_POSITIVE = 0.0;  // JWH 24 April 2015  0.00001;
    const float PaletteColorMapping::SMALL_NEGATIVE = 0.0;  // JWH 24 April 2015 -0.00001;
#endif // __PALETTE_COLOR_MAPPING_DECLARE__
    
} // namespace

#endif // __PALETTECOLORMAPPING_H__
