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
#include <sstream>

#include "AnnotationColorBar.h"
#include "AnnotationColorBarNumericText.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "FastStatistics.h"
#include "MathFunctions.h"
#include "NumericTextFormatting.h"
//because the ROY_BIG_BL palette name is a constant defined in Palette.h
#include "Palette.h"
#define __PALETTE_COLOR_MAPPING_DECLARE__
#include "PaletteColorMapping.h"
#undef __PALETTE_COLOR_MAPPING_DECLARE__
#include "PaletteColorMappingSaxReader.h"
#include "PaletteColorMappingXmlElements.h"
#include "PaletteScalarAndColor.h"
#include "XmlSaxParser.h"
#include "XmlUtilities.h"
#include "XmlWriter.h"
#include <limits>

using namespace caret;


/**
 * Constructor.
 *
 */
PaletteColorMapping::PaletteColorMapping()
    : CaretObject()
{
    this->initializeMembersPaletteColorMapping();
}

/**
 * Destructor
 */
PaletteColorMapping::~PaletteColorMapping()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
PaletteColorMapping::PaletteColorMapping(const PaletteColorMapping& o)
    : CaretObject(o)
{
    this->initializeMembersPaletteColorMapping();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
PaletteColorMapping&
PaletteColorMapping::operator=(const PaletteColorMapping& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Copy the palette color mapping from the given palette
 * color mapping.
 * @param pcm
 *    Color mapping that is copied to this.
 */
void 
PaletteColorMapping::copy(const PaletteColorMapping& pcm)
{
    this->copyHelper(pcm);
    setModified();
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
PaletteColorMapping::copyHelper(const PaletteColorMapping& pcm)
{
    this->autoScalePercentageNegativeMaximum = pcm.autoScalePercentageNegativeMaximum;
    this->autoScalePercentageNegativeMinimum = pcm.autoScalePercentageNegativeMinimum;
    this->autoScalePercentagePositiveMaximum = pcm.autoScalePercentagePositiveMaximum;
    this->autoScalePercentagePositiveMinimum = pcm.autoScalePercentagePositiveMinimum;
    this->autoScaleAbsolutePercentageMaximum = pcm.autoScaleAbsolutePercentageMaximum;
    this->autoScaleAbsolutePercentageMinimum = pcm.autoScaleAbsolutePercentageMinimum;
    this->displayNegativeDataFlag = pcm.displayNegativeDataFlag;
    this->displayPositiveDataFlag = pcm.displayPositiveDataFlag;
    this->displayZeroDataFlag     = pcm.displayZeroDataFlag;
    this->interpolatePaletteFlag  = pcm.interpolatePaletteFlag;
    this->scaleMode = pcm.scaleMode;
    this->selectedPaletteName = pcm.selectedPaletteName;
    this->userScaleNegativeMaximum = pcm.userScaleNegativeMaximum;
    this->userScaleNegativeMinimum = pcm.userScaleNegativeMinimum;
    this->userScalePositiveMaximum = pcm.userScalePositiveMaximum;
    this->userScalePositiveMinimum = pcm.userScalePositiveMinimum;
    this->thresholdType = pcm.thresholdType;
    this->thresholdTest = pcm.thresholdTest;
    this->thresholdNormalMinimum = pcm.thresholdNormalMinimum;
    this->thresholdNormalMaximum = pcm.thresholdNormalMaximum;
    this->thresholdMappedMinimum= pcm.thresholdMappedMinimum;
    this->thresholdMappedMaximum = pcm.thresholdMappedMaximum;
    this->thresholdMappedAverageAreaMinimum = pcm.thresholdMappedAverageAreaMinimum;
    this->thresholdMappedAverageAreaMaximum = pcm.thresholdMappedAverageAreaMaximum;
    this->thresholdDataName = pcm.thresholdDataName;
    this->thresholdShowFailureInGreen = pcm.thresholdShowFailureInGreen;
    this->thresholdRangeMode = pcm.thresholdRangeMode;
    this->thresholdNegMinPosMaxLinked = pcm.thresholdNegMinPosMaxLinked;
    this->histogramRangeMode = pcm.histogramRangeMode;
    this->histogramChartType = pcm.histogramChartType;
    this->histogramColor = pcm.histogramColor;
    this->numericFormatMode = pcm.numericFormatMode;
    this->precisionDigits = pcm.precisionDigits;
    this->numericSubdivisionCount = pcm.numericSubdivisionCount;
    this->colorBarValuesMode = pcm.colorBarValuesMode;
    this->showTickMarksSelected = pcm.showTickMarksSelected;
    
    this->clearModified();
}

/**
 * Equality operator.
 * @param pcm
 *    Palette color mapping compared to 'this' palette color mapping.
 * @return
 *    True if their members are the same.
 */
bool
PaletteColorMapping::operator==(const PaletteColorMapping& pcm) const
{
    if ((this->autoScalePercentageNegativeMaximum == pcm.autoScalePercentageNegativeMaximum)
        && (this->autoScalePercentageNegativeMinimum == pcm.autoScalePercentageNegativeMinimum)
        && (this->autoScalePercentagePositiveMaximum == pcm.autoScalePercentagePositiveMaximum)
        && (this->autoScalePercentagePositiveMinimum == pcm.autoScalePercentagePositiveMinimum)
        && (this->autoScaleAbsolutePercentageMaximum == pcm.autoScaleAbsolutePercentageMaximum)
        && (this->autoScaleAbsolutePercentageMinimum == pcm.autoScaleAbsolutePercentageMinimum)
        && (this->displayNegativeDataFlag == pcm.displayNegativeDataFlag)
        && (this->displayPositiveDataFlag == pcm.displayPositiveDataFlag)
        && (this->displayZeroDataFlag     == pcm.displayZeroDataFlag)
        && (this->interpolatePaletteFlag  == pcm.interpolatePaletteFlag)
        && (this->scaleMode == pcm.scaleMode)
        && (this->selectedPaletteName == pcm.selectedPaletteName)
        && (this->userScaleNegativeMaximum == pcm.userScaleNegativeMaximum)
        && (this->userScaleNegativeMinimum == pcm.userScaleNegativeMinimum)
        && (this->userScalePositiveMaximum == pcm.userScalePositiveMaximum)
        && (this->userScalePositiveMinimum == pcm.userScalePositiveMinimum)
        && (this->thresholdType == pcm.thresholdType)
        && (this->thresholdTest == pcm.thresholdTest)
        && (this->thresholdNormalMinimum == pcm.thresholdNormalMinimum)
        && (this->thresholdNormalMaximum == pcm.thresholdNormalMaximum)
        && (this->thresholdMappedMinimum== pcm.thresholdMappedMinimum)
        && (this->thresholdMappedMaximum == pcm.thresholdMappedMaximum)
        && (this->thresholdMappedAverageAreaMinimum == pcm.thresholdMappedAverageAreaMinimum)
        && (this->thresholdMappedAverageAreaMaximum == pcm.thresholdMappedAverageAreaMaximum)
        && (this->thresholdDataName == pcm.thresholdDataName)
        && (this->thresholdShowFailureInGreen == pcm.thresholdShowFailureInGreen)
        && (this->thresholdRangeMode == pcm.thresholdRangeMode)
        && (this->thresholdNegMinPosMaxLinked == pcm.thresholdNegMinPosMaxLinked)
        && (this->histogramRangeMode == pcm.histogramRangeMode)
        && (this->histogramChartType == pcm.histogramChartType)
        && (this->histogramColor == pcm.histogramColor)
        && (this->numericFormatMode == pcm.numericFormatMode)
        && (this->precisionDigits == pcm.precisionDigits)
        && (this->numericSubdivisionCount == pcm.numericSubdivisionCount)
        && (this->colorBarValuesMode == pcm.colorBarValuesMode)
        && (this->showTickMarksSelected == pcm.showTickMarksSelected)) {
        return true;
    }
    
    return false;
}


void
PaletteColorMapping::initializeMembersPaletteColorMapping()
{
    this->scaleMode = PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE;
    this->autoScalePercentageNegativeMaximum = 98.0f;
    this->autoScalePercentageNegativeMinimum = 2.0f;
    this->autoScalePercentagePositiveMinimum = 2.0f;
    this->autoScalePercentagePositiveMaximum = 98.0f;
    this->autoScaleAbsolutePercentageMaximum = 98.0f;
    this->autoScaleAbsolutePercentageMinimum = 2.0f;
    this->userScaleNegativeMaximum = -100.0f;
    this->userScaleNegativeMinimum = 0.0f;
    this->userScalePositiveMinimum = 0.0f;
    this->userScalePositiveMaximum = 100.0f;
    this->selectedPaletteName = Palette::ROY_BIG_BL_PALETTE_NAME;
    this->interpolatePaletteFlag = true;
    this->displayPositiveDataFlag = true;
    this->displayZeroDataFlag = false;
    this->displayNegativeDataFlag = true;
    this->thresholdType = PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF;
    this->thresholdTest = PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE;
    this->thresholdNormalMinimum = -1.0f;
    this->thresholdNormalMaximum = 1.0f;
    this->thresholdMappedMinimum = -1.0f;
    this->thresholdMappedMaximum = 1.0f;
    this->thresholdMappedAverageAreaMinimum = -1.0f;
    this->thresholdMappedAverageAreaMaximum =  1.0f;
    this->thresholdDataName = "";
    this->thresholdShowFailureInGreen = false;
    //this->thresholdRangeMode = PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_MAP;
    this->thresholdRangeMode = PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE;
    this->thresholdNegMinPosMaxLinked = false;
    this->histogramRangeMode = PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_ALL;
    this->histogramChartType = PaletteHistogramChartTypeEnum::PALETTE_HISTOGRAM_CHART_BARS;
    this->histogramColor = CaretColorEnum::CUSTOM;  // CUSTOM is color with palette
    this->numericFormatMode = NumericFormatModeEnum::AUTO;
    this->precisionDigits = 2;
    this->numericSubdivisionCount = 0;
    this->modifiedFlag = false;
    this->colorBarValuesMode = PaletteColorBarValuesModeEnum::DATA;
    this->showTickMarksSelected = false;
}

/**
 * Write the object as XML.
 * @param xmlWriter - write to this->
 * @throws XmlException - If an error occurs.
 *
 */
void
PaletteColorMapping::writeAsXML(XmlWriter& xmlWriter)
                      
{
    XmlAttributes attributes;
    attributes.addAttribute(
                            PaletteColorMappingXmlElements::XML_ATTRIBUTE_VERSION_NUMBER,
                            PaletteColorMappingXmlElements::XML_VERSION_NUMBER);
    xmlWriter.writeStartElement(
                                PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING,
                                attributes);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_SCALE_MODE,
                                     PaletteScaleModeEnum::toName(this->scaleMode));
    float autoScaleValues[4] = {
        this->autoScalePercentageNegativeMaximum,
        this->autoScalePercentageNegativeMinimum,
        this->autoScalePercentagePositiveMinimum,
        this->autoScalePercentagePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES,
                                     autoScaleValues,
                                     4);
    
    float autoScaleAbsolutePercentageValues[2] = {
        this->autoScaleAbsolutePercentageMinimum,
        this->autoScaleAbsolutePercentageMaximum
    };
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_ABSOLUTE_PERCENTAGE_VALUES,
                                     autoScaleAbsolutePercentageValues,
                                     2);
    
    float userScaleValues[4] = {
        this->userScaleNegativeMaximum,
        this->userScaleNegativeMinimum,
        this->userScalePositiveMinimum,
        this->userScalePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_USER_SCALE_VALUES,
                                     userScaleValues,
                                     4);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_PALETTE_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->selectedPaletteName));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_INTERPOLATE,
                                     this->interpolatePaletteFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_POSITIVE,
                                     this->displayPositiveDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_ZERO,
                                     this->displayZeroDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_DISPLAY_NEGATIVE,
                                     this->displayNegativeDataFlag);
    
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TEST,
                                     PaletteThresholdTestEnum::toName(this->thresholdTest));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TYPE,
                                     PaletteThresholdTypeEnum::toName(this->thresholdType));
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_FAILURE_IN_GREEN,
                                     this->thresholdShowFailureInGreen);
    float normalValues[2] = {
        this->thresholdNormalMinimum,
        this->thresholdNormalMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NORMAL_VALUES,
                                     normalValues,
                                     2);
    
    float mappedValues[2] = {
        this->thresholdMappedMinimum,
        this->thresholdMappedMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_VALUES,
                                     mappedValues,
                                     2);
    
    float mappedAvgAreaValues[2] = {
        this->thresholdMappedAverageAreaMinimum,
        this->thresholdMappedAverageAreaMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES,
                                     mappedAvgAreaValues,
                                     2);
    
    xmlWriter.writeElementCharacters(
                                     PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_DATA_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->thresholdDataName));
    
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_RANGE_MODE,
                                     PaletteThresholdRangeModeEnum::toName(this->thresholdRangeMode));
    
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NEG_MIN_POS_MAX_LINKED,
                                     this->thresholdNegMinPosMaxLinked);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_RANGE_MODE,
                                     PaletteHistogramRangeModeEnum::toName(this->histogramRangeMode));
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_CHART_TYPE,
                                     PaletteHistogramChartTypeEnum::toName(this->histogramChartType));
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_COLOR,
                                     CaretColorEnum::toName(this->histogramColor));
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_NUMERIC_FORMAT_MODE,
                                     NumericFormatModeEnum::toName(this->numericFormatMode));
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_PRECISION_DIGITS,
                                     this->precisionDigits);
    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_NUMERIC_SUBDIVISIONS,
                                     this->numericSubdivisionCount);

    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_COLOR_BAR_VALUES_MODE,
                                     PaletteColorBarValuesModeEnum::toName(this->colorBarValuesMode));

    xmlWriter.writeElementCharacters(PaletteColorMappingXmlElements::XML_TAG_SHOW_TICK_MARKS,
                                     this->showTickMarksSelected);
    
    xmlWriter.writeEndElement();
}

/**
 * Returns the XML representation of this object in a String.
 *
 * @return String containing XML.
 * @throws XmlException if an error occurs.
 *
 */
AString
PaletteColorMapping::encodeInXML()
           
{
    std::ostringstream str;
    XmlWriter xmlWriter(str);
    this->writeAsXML(xmlWriter);
    AString s = AString::fromStdString(str.str());
    return s;
}

/**
 * Decode this object from a String containing XML.
 * @param xml - String containing XML.
 * @throws XmlException If an error occurs.
 *
 */
void
PaletteColorMapping::decodeFromStringXML(const AString& xml)
           
{
    PaletteColorMappingSaxReader saxReader(this);
    CaretPointer<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseString(xml, &saxReader);
    }
    catch (XmlSaxParserException& e) {
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        std::ostringstream str;
        str << "Parse Error while reading PaletteColorMapping XML";
        if ((lineNum >= 0) && (colNum >= 0)) {
            str << " line/col ("
            << e.getLineNumber()
            << "/"
            << e.getColumnNumber()
            << ")";
        }
        str << ": "
        << e.whatString().toStdString();
        throw XmlException(AString::fromStdString(str.str()));
    }
}

/**
 * Setup an annotation color bar with its color sections.
 *
 * @param statistics
 *     Statistics of data for colorbar.
 * @param colorBar
 *     The annotation colorbar that has its color sections set.
 */
void
PaletteColorMapping::setupAnnotationColorBar(const FastStatistics* statistics,
                                             AnnotationColorBar* colorBar)
{
    CaretAssert(statistics);
    CaretAssert(colorBar);
    
    colorBar->clearSections();
    
    const AString paletteName = getSelectedPaletteName();
    EventPaletteGetByName paletteEvent(paletteName);
    EventManager::get()->sendEvent(paletteEvent.getPointer());
    const Palette* palette = paletteEvent.getPalette();
    if (palette == NULL) {
        CaretLogSevere("Unable to find palette named \""
                       + paletteName
                       + "\"");
        return;
    }
    
    
    /*
     * Types of values for display
     */
    float xMinimum = 0.0;
    float xMaximum = 0.0;
    if (this->displayPositiveDataFlag) {
        xMaximum = 1.0;
    }
    if (this->displayNegativeDataFlag) {
        xMinimum = -1.0;
    }
    
    /*
     * Always interpolate if the palette has only two colors
     */
    bool interpolateColor = this->interpolatePaletteFlag;
    if (palette->getNumberOfScalarsAndColors() <= 2) {
        interpolateColor = true;
    }
    
    float zeroRGBA[4] = { 0.0, 0.0, 0.0, 0.0 };
    
    /*
     * Palettes start with most positive scalar value that in on right side of palette
     */
    const int32_t numberOfColors = palette->getNumberOfScalarsAndColors();
    const bool interpolationOnFlag = interpolateColor;
    const int32_t lastColorIndex = numberOfColors - 1;
    for (int32_t index = 0; index < numberOfColors; index++) {
        const PaletteScalarAndColor* rightSideScalarColor = palette->getScalarAndColor(index);
        if (rightSideScalarColor->isNoneColor()) {
            /*
             * No drawing for "none" color; skip to next color
             */
            continue;
        }
        
        /*
         * Add vertices for right side of quad
         */
        float rightSideScalar = rightSideScalarColor->getScalar();
        if (index == 0) {
            rightSideScalar = xMaximum; // First color extends to right side
        }
        float rightSideRGBA[4];
        rightSideScalarColor->getColor(rightSideRGBA);
        
        //addXyRgba(quadsXYZ, quadsRGBA, rightSideScalar, -halfHeight, rightSideRGBA);
        //addXyRgba(quadsXYZ, quadsRGBA, rightSideScalar,  halfHeight, rightSideRGBA);
        
        /*
         * Default left side of palette subsection to far left side
         * and same color as right side of palette subsection
         */
        float leftSideScalar = xMinimum;
        float leftSideRGBA[4] = { rightSideRGBA[0], rightSideRGBA[1], rightSideRGBA[2], rightSideRGBA[3] };
        
        /*
         * Determine the next scalar and color index for left side of quad
         * Left side color is same color as right if:
         *    # At last color in palette
         *    # Next color is "none"
         *    # Interpolation is OFF
         */
        int32_t nextIndex = index + 1;
        if (nextIndex <= lastColorIndex) {
            /*
             * Set scalar value for left side of subsection of palette
             */
            const PaletteScalarAndColor* leftSideScalarColor = palette->getScalarAndColor(nextIndex);
            leftSideScalar = leftSideScalarColor->getScalar();
            
            if (palette->getScalarAndColor(nextIndex)->isNoneColor()) {
                /*
                 * No interpolation if left side of subsection is "none" color
                 */
            }
            else if (interpolationOnFlag) {
                /*
                 * Set color for left side of subsection of palette
                 */
                leftSideScalarColor->getColor(leftSideRGBA);
            }
        }
        
        if ((rightSideScalar >= 0.0)
            && (leftSideScalar <= 0.0)) {
            /*
             * Used for special case when zero is one but both 
             * negative and postive are off
             */
            const float diff = rightSideScalar - leftSideScalar;
            if (diff > 0.0) {
                const float rightWeight = rightSideScalar;
                const float leftWeight  = -leftSideScalar;
                for (int32_t m = 0; m < 4; m++) {
                    zeroRGBA[m] = ((leftSideRGBA[m] * leftWeight)
                                   + (rightSideRGBA[m] * rightWeight));
                }
            }
            else {
                for (int32_t m = 0; m < 4; m++) {
                    zeroRGBA[m] = rightSideRGBA[m];
                }
            }
        }
        
        /*
         * Add a section to the color bar
         */
        leftSideScalar = MathFunctions::limitRange(leftSideScalar, xMinimum, xMaximum);
        rightSideScalar = MathFunctions::limitRange(rightSideScalar, xMinimum, xMaximum);
        if (rightSideScalar > leftSideScalar) {
            colorBar->addSection(leftSideScalar, rightSideScalar, leftSideRGBA, rightSideRGBA);
        }
    }
    
    float backgroundRGBA[4];
    colorBar->getBackgroundColorRGBA(backgroundRGBA);
    
    /*
     * Draw over thresholded regions with background color
     */
    if (this->thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
        const float minMaxThresholds[2] = {
            getThresholdMinimum(thresholdType),
            getThresholdMaximum(thresholdType)
        };
        float normalizedThresholds[2];
        
        mapDataToPaletteNormalizedValues(statistics,
                                         minMaxThresholds,
                                         normalizedThresholds,
                                         2);
        
        switch (this->thresholdTest) {
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                if (normalizedThresholds[0] >= xMinimum) {
                    colorBar->addSection(xMinimum,
                                         normalizedThresholds[0],
                                         backgroundRGBA,
                                         backgroundRGBA);
                }
                if (normalizedThresholds[1] < xMaximum) {
                    colorBar->addSection(normalizedThresholds[1],
                                         xMaximum,
                                         backgroundRGBA,
                                         backgroundRGBA);
                }
                break;
            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
            {
                const float xMin = MathFunctions::limitRange(normalizedThresholds[0],
                                                             xMinimum,
                                                             xMaximum);
                const float xMax = MathFunctions::limitRange(normalizedThresholds[1],
                                                             xMinimum,
                                                             xMaximum);
                if (xMin < xMax) {
                    colorBar->addSection(normalizedThresholds[0],
                                         normalizedThresholds[1],
                                         backgroundRGBA,
                                         backgroundRGBA);
                }
            }
                break;
        }
    }
    
    /*
     * If zeros are not displayed, draw a line in the
     * background color at zero in the palette.
     */
    if ( ! this->displayZeroDataFlag) {
        colorBar->addSection(0.0, 0.0, backgroundRGBA, backgroundRGBA);
    }
    
    /*
     * If only zero is displayed
     */
    if (this->displayZeroDataFlag
        && ( ! this->displayPositiveDataFlag)
        && ( ! this->displayNegativeDataFlag)) {
        if (zeroRGBA[3] > 0.0) {
            colorBar->addSection(-1.0, 0.0, backgroundRGBA, backgroundRGBA);
            colorBar->addSection( 0.0, 0.0, zeroRGBA, zeroRGBA);
            colorBar->addSection( 1.0, 0.0, backgroundRGBA, backgroundRGBA);
        }
    }
    
    /**
     * Add numeric text to the color bar.
     */
    setupAnnotationColorBarNumericText(statistics,
                                       colorBar);
}

///**
// * Setup an annotation color bar with its color sections.
// *
// * @param statistics
// *     Statistics of data for colorbar.
// * @param colorBar
// *     The annotation colorbar that has its color sections set.
// */
//void
//PaletteColorMapping::setupAnnotationColorBar(const FastStatistics* statistics,
//                                             AnnotationColorBar* colorBar)
//{
//    CaretAssert(statistics);
//    CaretAssert(colorBar);
//    
//    colorBar->clearSections();
//    
//    const AString paletteName = getSelectedPaletteName();
//    EventPaletteGetByName paletteEvent(paletteName);
//    EventManager::get()->sendEvent(paletteEvent.getPointer());
//    const Palette* palette = paletteEvent.getPalette();
//    if (palette == NULL) {
//        CaretLogSevere("Unable to find palette named \""
//                       + paletteName
//                       + "\"");
//        return;
//    }
//    
//    
//    /*
//     * Types of values for display
//     */
//    const bool isPositiveOnly = (this->displayPositiveDataFlag && ( ! this->displayNegativeDataFlag));
//    const bool isNegativeOnly = (( ! this->displayPositiveDataFlag) && this->displayNegativeDataFlag);
//    
//    float xMinimum = -1.0;
//    float xMaximum =  1.0;
//    if (isPositiveOnly) {
//        xMinimum = 0.0;
//    }
//    else if (isNegativeOnly) {
//        xMaximum = 0.0;
//    }
//    
//    /*
//     * Always interpolate if the palette has only two colors
//     */
//    bool interpolateColor = this->interpolatePaletteFlag;
//    if (palette->getNumberOfScalarsAndColors() <= 2) {
//        interpolateColor = true;
//    }
//    
//    /*
//     * Draw the colorbar starting with the color assigned
//     * to the negative end of the palette.
//     * Colorbar scalars range from -1 to 1.
//     */
//    const int iStart = palette->getNumberOfScalarsAndColors() - 1;
//    const int iEnd = 1;
//    const int iStep = -1;
//    for (int i = iStart; i >= iEnd; i += iStep) {
//        /*
//         * palette data for 'left' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* sc = palette->getScalarAndColor(i);
//        float scalar = sc->getScalar();
//        float rgba[4];
//        sc->getColor(rgba);
//        
//        /*
//         * palette data for 'right' side of a color in the palette.
//         */
//        const PaletteScalarAndColor* nextSC = palette->getScalarAndColor(i - 1);
//        float nextScalar = nextSC->getScalar();
//        float nextRGBA[4];
//        nextSC->getColor(nextRGBA);
//        const bool isNoneColorFlag = nextSC->isNoneColor();
//        
//        /*
//         * Exclude negative regions if not displayed.
//         */
//        if ( ! this->displayNegativeDataFlag) {
//            if (nextScalar < 0.0) {
//                continue;
//            }
//            else if (scalar < 0.0) {
//                scalar = 0.0;
//            }
//        }
//        
//        /*
//         * Exclude positive regions if not displayed.
//         */
//        if ( ! this->displayPositiveDataFlag) {
//            if (scalar > 0.0) {
//                continue;
//            }
//            else if (nextScalar > 0.0) {
//                nextScalar = 0.0;
//            }
//        }
//        
//        /*
//         * Normally, the first entry has a scalar value of -1.
//         * If it does not, use the first color draw from
//         * -1 to the first scalar value.
//         */
//        if (i == iStart) {
//            if ( ! sc->isNoneColor()) {
//                if (scalar > -1.0) {
//                    const float xStart = -1.0;
//                    const float xEnd   = scalar;
//                    
//                    colorBar->addSection(xStart,
//                                         xEnd,
//                                         rgba,
//                                         rgba);
//                }
//            }
//        }
//        
//        /*
//         * If the 'next' color is none, drawing
//         * is skipped to let the background show
//         * throw the 'none' region of the palette.
//         */
//        if ( ! isNoneColorFlag) {
//            /*
//             * left and right region of an entry in the palette
//             */
//            const float xStart = scalar;
//            const float xEnd   = nextScalar;
//            
//            /*
//             * Unless interpolating, use the 'next' color.
//             */
//            float* startRGBA = nextRGBA;
//            float* endRGBA   = nextRGBA;
//            if (interpolateColor) {
//                startRGBA = rgba;
//            }
//            
//            /*
//             * Draw the region in the palette.
//             */
//            colorBar->addSection(xStart,
//                                 xEnd,
//                                 startRGBA,
//                                 endRGBA);
//            
//            /*
//             * The last scalar value is normally 1.0.  If the last
//             * scalar is less than 1.0, then fill in the rest of
//             * the palette from the last scalar to 1.0.
//             */
//            if (i == iEnd) {
//                if (nextScalar < 1.0) {
//                    const float xStart = nextScalar;
//                    const float xEnd   = 1.0;
//                    colorBar->addSection(xStart,
//                                         xEnd,
//                                         nextRGBA,
//                                         nextRGBA);
//                }
//            }
//        }
//    }
//    
//    float backgroundRGBA[4];
//    colorBar->getBackgroundColorRGBA(backgroundRGBA);
//    
//    /*
//     * Draw over thresholded regions with background color
//     */
//    if (this->thresholdType != PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
//        const float minMaxThresholds[2] = {
//            getThresholdMinimum(thresholdType),
//            getThresholdMaximum(thresholdType)
//        };
//        float normalizedThresholds[2];
//        
//        mapDataToPaletteNormalizedValues(statistics,
//                                         minMaxThresholds,
//                                         normalizedThresholds,
//                                         2);
//        
//        switch (this->thresholdTest) {
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
//                if (normalizedThresholds[0] >= xMinimum) {
//                    colorBar->addSection(xMinimum,
//                                         normalizedThresholds[0],
//                                         backgroundRGBA,
//                                         backgroundRGBA);
//                }
//                if (normalizedThresholds[1] < xMaximum) {
//                    colorBar->addSection(normalizedThresholds[1],
//                                         xMaximum,
//                                         backgroundRGBA,
//                                         backgroundRGBA);
//                }
//                break;
//            case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
//            {
//                const float xMin = MathFunctions::limitRange(normalizedThresholds[0],
//                                                             xMinimum,
//                                                             xMaximum);
//                const float xMax = MathFunctions::limitRange(normalizedThresholds[1],
//                                                             xMinimum,
//                                                             xMaximum);
//                if (xMin < xMax) {
//                    colorBar->addSection(normalizedThresholds[0],
//                                         normalizedThresholds[1],
//                                         backgroundRGBA,
//                                         backgroundRGBA);
//                }
//            }
//                break;
//        }
//    }
//    
//    /*
//     * If zeros are not displayed, draw a line in the
//     * background color at zero in the palette.
//     */
//    if ( ! this->displayZeroDataFlag) {
//        colorBar->addSection(0.0, 0.0, backgroundRGBA, backgroundRGBA);
//    }
//
//    /**
//     * Add numeric text to the color bar.
//     */
//    setupAnnotationColorBarNumericText(statistics,
//                                       colorBar);
//}

/**
 * Get auto scale percentage negative maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentageNegativeMaximum() const
{
    return autoScalePercentageNegativeMaximum;
}

/**
 * Set auto scale percentage negative maximum.
 * @param autoScalePercentageNegativeMaximum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentageNegativeMaximum(const float autoScalePercentageNegativeMaximum)
{
    if (this->autoScalePercentageNegativeMaximum != autoScalePercentageNegativeMaximum) {
        this->autoScalePercentageNegativeMaximum = autoScalePercentageNegativeMaximum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage negative minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentageNegativeMinimum() const
{
    return autoScalePercentageNegativeMinimum;
}

/**
 * Set auto scale percentage negative minimum.
 * @param autoScalePercentageNegativeMinimum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentageNegativeMinimum(const float autoScalePercentageNegativeMinimum)
{
    if (this->autoScalePercentageNegativeMinimum != autoScalePercentageNegativeMinimum) {
        this->autoScalePercentageNegativeMinimum = autoScalePercentageNegativeMinimum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentagePositiveMaximum() const
{
    return autoScalePercentagePositiveMaximum;
}

/**
 * Set auto scale percentage positive maximum.
 * @param autoScalePercentagePositiveMaximum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentagePositiveMaximum(const float autoScalePercentagePositiveMaximum)
{
    if (this->autoScalePercentagePositiveMaximum != autoScalePercentagePositiveMaximum) {
        this->autoScalePercentagePositiveMaximum = autoScalePercentagePositiveMaximum;
        this->setModified();
    }
}

/**
 * Get auto scale percentage positive minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getAutoScalePercentagePositiveMinimum() const
{
    return autoScalePercentagePositiveMinimum;
}

/**
 * Set auto scale percentage positive minimum.
 * @param autoScalePercentagePositiveMinimum - new value.
 *
 */
void
PaletteColorMapping::setAutoScalePercentagePositiveMinimum(const float autoScalePercentagePositiveMinimum)
{
    if (this->autoScalePercentagePositiveMinimum != autoScalePercentagePositiveMinimum) {
        this->autoScalePercentagePositiveMinimum = autoScalePercentagePositiveMinimum;
        this->setModified();
    }
}

/**
 * @return The auto scale absolute percentage minimum.
 */
float
PaletteColorMapping::getAutoScaleAbsolutePercentageMinimum() const
{
    return this->autoScaleAbsolutePercentageMinimum;
}

/**
 * Set the auto scale absolute percentage minimum.
 *
 * @param autoScaleAbsolutePercentageMinimum
 *     New value for auto scale absolute percentage minimum.
 */
void
PaletteColorMapping::setAutoScaleAbsolutePercentageMinimum(const float autoScaleAbsolutePercentageMinimum)
{
    
    if (this->autoScaleAbsolutePercentageMinimum != autoScaleAbsolutePercentageMinimum) {
        this->autoScaleAbsolutePercentageMinimum = autoScaleAbsolutePercentageMinimum;
        this->setModified();
    }
}

/**
 * @return The auto scale absolute percentage maximum.
 */
float
PaletteColorMapping::getAutoScaleAbsolutePercentageMaximum() const
{
    return this->autoScaleAbsolutePercentageMaximum;
}

/**
 * Set the auto scale absolute percentage maximum.
 *
 * @param autoScaleAbsolutePercentageMaximum
 *     New value for auto scale absolute percentage maximum.
 */
void
PaletteColorMapping::setAutoScaleAbsolutePercentageMaximum(const float autoScaleAbsolutePercentageMaximum)
{
    if (this->autoScaleAbsolutePercentageMaximum != autoScaleAbsolutePercentageMaximum) {
        this->autoScaleAbsolutePercentageMaximum = autoScaleAbsolutePercentageMaximum;
        this->setModified();
    }
}

/**
 * See if negative data should be displayed.
 * @return true if negative data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayNegativeDataFlag() const
{
    return displayNegativeDataFlag;
}

/**
 * Set negative data should be displayed.
 * @param displayNegativeDataFlag - true to display negative data, else false
 *
 */
void
PaletteColorMapping::setDisplayNegativeDataFlag(const bool displayNegativeDataFlag)
{
    if (this->displayNegativeDataFlag != displayNegativeDataFlag) {
        this->displayNegativeDataFlag = displayNegativeDataFlag;
        this->setModified();
    }
}

/**
 * See if positive data should be displayed.
 * @return true if positive data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayPositiveDataFlag() const
{
    return displayPositiveDataFlag;
}

/**
 * Set positive data should be displayed.
 * @param displayPositiveDataFlag - true to display positive data, else false
 *
 */
void
PaletteColorMapping::setDisplayPositiveDataFlag(const bool displayPositiveDataFlag)
{
    if (this->displayPositiveDataFlag != displayPositiveDataFlag) {
        this->displayPositiveDataFlag = displayPositiveDataFlag;
        this->setModified();
    }
}

/**
 * See if zero data should be displayed.
 * @return true if zero data displayed, else false.
 *
 */
bool
PaletteColorMapping::isDisplayZeroDataFlag() const
{
    return displayZeroDataFlag;
}

/**
 * Set zero data should be displayed.
 * @param displayZeroDataFlag - true to display zero data, else false
 *
 */
void
PaletteColorMapping::setDisplayZeroDataFlag(const bool displayZeroDataFlag)
{
    if (this->displayZeroDataFlag != displayZeroDataFlag) {
        this->displayZeroDataFlag = displayZeroDataFlag;
        this->setModified();
    }
}

/**
 * Interpolate palette colors when displaying data?
 * @return true to interpolate data, else false.
 *
 */
bool
PaletteColorMapping::isInterpolatePaletteFlag() const
{
    return interpolatePaletteFlag;
}

/**
 * Set palette colors should be interpolated when displaying data.
 * @param interpolatePaletteFlag - true to interpolate, else false.
 *
 */
void
PaletteColorMapping::setInterpolatePaletteFlag(const bool interpolatePaletteFlag)
{
    if (this->interpolatePaletteFlag != interpolatePaletteFlag) {
        this->interpolatePaletteFlag = interpolatePaletteFlag;
        this->setModified();
    }
}

/**
 * Get how the data is scaled to the palette.
 * @return  Enumerated type indicating how data is scaled to the palette.
 *
 */
PaletteScaleModeEnum::Enum
PaletteColorMapping::getScaleMode() const
{
    return scaleMode;
}

/**
 * Set how the data is scaled to the palette.
 * @param scaleMode - Enumerated type indicating how data is scaled
 *    to the palette.
 *
 */
void
PaletteColorMapping::setScaleMode(const PaletteScaleModeEnum::Enum scaleMode)
{
    if (this->scaleMode != scaleMode) {
        this->scaleMode = scaleMode;
        this->setModified();
    }
}

/**
 * Get the name of the selected palette.
 * @return Name of the selected palette.
 *
 */
AString
PaletteColorMapping::getSelectedPaletteName() const
{
    return selectedPaletteName;
}

/**
 * Set the name of the selected palette.
 * @param selectedPaletteName - Name of selected palette.
 *
 */
void
PaletteColorMapping::setSelectedPaletteName(const AString& selectedPaletteName)
{
    if (this->selectedPaletteName != selectedPaletteName) {
        this->selectedPaletteName = selectedPaletteName;
        this->setModified();
    }
}

/**
 * Set the selected palette to PSYCH.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToPsych()
{
    this->setSelectedPaletteName("PSYCH");
}

/**
 * Set the selected palette to PSYCH-NO-NONE
 *
 */
void
PaletteColorMapping::setSelectedPaletteToPsychNoNone()
{
    this->setSelectedPaletteName("PSYCH-NO-NONE");
}

/**
 * Set the selected palette to Orange-Yellow.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToOrangeYellow()
{
    this->setSelectedPaletteName("Orange-Yellow");
}

/**
 * Set the selected palette to Gray Interpolated.
 *
 */
void
PaletteColorMapping::setSelectedPaletteToGrayInterpolated()
{
    this->setSelectedPaletteName("Gray_Interp");
}

/**
 * Get auto user scale negative maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScaleNegativeMaximum() const
{
    return userScaleNegativeMaximum;
}

/**
 * Set user scale negative maximum.
 * @param userScaleNegativeMaximum - new value.
 *
 */
void
PaletteColorMapping::setUserScaleNegativeMaximum(const float userScaleNegativeMaximum)
{
    if (this->userScaleNegativeMaximum != userScaleNegativeMaximum) {
        this->userScaleNegativeMaximum = userScaleNegativeMaximum;
        this->setModified();
    }
}

/**
 * Get auto user scale negative minimum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScaleNegativeMinimum() const
{
    return userScaleNegativeMinimum;
}

/**
 * Set user scale negative minimum.
 * @param userScaleNegativeMinimum - new value.
 *
 */
void
PaletteColorMapping::setUserScaleNegativeMinimum(const float userScaleNegativeMinimum)
{
    if (this->userScaleNegativeMinimum != userScaleNegativeMinimum) {
        this->userScaleNegativeMinimum = userScaleNegativeMinimum;
        this->setModified();
    }
}

/**
 * Get auto user scale positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScalePositiveMaximum() const
{
    return userScalePositiveMaximum;
}

/**
 * Set user scale positive maximum.
 * @param userScalePositiveMaximum - new value.
 *
 */
void
PaletteColorMapping::setUserScalePositiveMaximum(const float userScalePositiveMaximum)
{
    if (this->userScalePositiveMaximum != userScalePositiveMaximum) {
        this->userScalePositiveMaximum = userScalePositiveMaximum;
        this->setModified();
    }
}

/**
 * Get auto user scale positive maximum.
 * @return Its value.
 *
 */
float
PaletteColorMapping::getUserScalePositiveMinimum() const
{
    return userScalePositiveMinimum;
}

/**
 * Set user scale positive minimum.
 * @param userScalePositiveMinimum - new value.
 *
 */
void
PaletteColorMapping::setUserScalePositiveMinimum(const float userScalePositiveMinimum)
{
    if (this->userScalePositiveMinimum != userScalePositiveMinimum) {
        this->userScalePositiveMinimum = userScalePositiveMinimum;
        this->setModified();
    }
}

/**
 * Get the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * @return the threshold's value.
 *
 */
float 
PaletteColorMapping::getThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType) const
{
    float value = 0.0;
    
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            value = this->thresholdNormalMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            value = this->thresholdMappedMinimum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            value = this->thresholdMappedAverageAreaMinimum;
            break;            
    }
    
    return value;
}

/**
 * Get the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * @return the threshold's value.
 *
 */
float 
PaletteColorMapping::getThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType) const
{
    float value = 0.0;
    
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            value = this->thresholdNormalMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            value = this->thresholdMappedMaximum;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            value = this->thresholdMappedAverageAreaMaximum;
            break;            
    }
    
    return value;
    
}

/**
 * Set the minimum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * param thresholdMinimum the threshold's new value.
 *
 */
void 
PaletteColorMapping::setThresholdMinimum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                         const float thresholdMinimum)
{
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            setThresholdNormalMinimum(thresholdMinimum);
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            setThresholdMappedMinimum(thresholdMinimum);
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            setThresholdMappedAverageAreaMinimum(thresholdMinimum);
            break;
    }
}

/**
 * Set the maximum threshold for the given threshold type.
 *
 * @param thresholdType
 *   The threshold type.
 * param thresholdMaximum the threshold's new value.
 *
 */
void 
PaletteColorMapping::setThresholdMaximum(const PaletteThresholdTypeEnum::Enum thresholdType,
                                         const float thresholdMaximum)
{
    switch (thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            setThresholdNormalMaximum(thresholdMaximum);
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            setThresholdMappedMaximum(thresholdMaximum);
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            setThresholdMappedAverageAreaMaximum(thresholdMaximum);
            break;
    }
}

/**
 * Get mapped average area minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaMinimum() const
{
    return thresholdMappedAverageAreaMinimum;
}

/**
 * Set the mapped average area minimum threshold.
 *
 * @param thresholdMappedAverageAreaNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaMinimum(const float thresholdMappedAverageAreaMinimum)
{
    if (this->thresholdMappedAverageAreaMinimum != thresholdMappedAverageAreaMinimum) {
        this->thresholdMappedAverageAreaMinimum = thresholdMappedAverageAreaMinimum;
        this->setModified();
    }
}

/**
 * Get mapped average area maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaMaximum() const
{
    return thresholdMappedAverageAreaMaximum;
}

/**
 * Set the mapped average area maximum threshold.
 *
 * @param thresholdMappedAverageAreaPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaMaximum(const float thresholdMappedAverageAreaMaximum)
{
    if (this->thresholdMappedAverageAreaMaximum != thresholdMappedAverageAreaMaximum) {
        this->thresholdMappedAverageAreaMaximum = thresholdMappedAverageAreaMaximum;
        this->setModified();
    }
}

/**
 * Get mapped minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedMinimum() const
{
    return thresholdMappedMinimum;
}

/**
 * Set the mapped minimum threshold.
 *
 * @param thresholdMappedNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedMinimum(const float thresholdMappedMinimum)
{
    if (this->thresholdMappedMinimum != thresholdMappedMinimum) {
        this->thresholdMappedMinimum = thresholdMappedMinimum;
        this->setModified();
    }
}

/**
 * Get mapped maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedMaximum() const
{
    return thresholdMappedMaximum;
}

/**
 * Set the mapped maximum threshold.
 *
 * @param thresholdMappedPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedMaximum(const float thresholdMappedMaximum)
{
    if (this->thresholdMappedMaximum != thresholdMappedMaximum) {
        this->thresholdMappedMaximum = thresholdMappedMaximum;
        this->setModified();
    }
}

/**
 * Get normal minimum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalMinimum() const
{
    return thresholdNormalMinimum;
}

/**
 * Set the normal minimum threshold.
 *
 * @param thresholdNormalNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalMinimum(const float thresholdNormalMinimum)
{
    if (this->thresholdNormalMinimum != thresholdNormalMinimum) {
        this->thresholdNormalMinimum = thresholdNormalMinimum;
        this->setModified();
    }
}

/**
 * Get normal maximum threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalMaximum() const
{
    return thresholdNormalMaximum;
}

/**
 * Set the normal maximum threshold.
 *
 * @param thresholdNormalPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalMaximum(const float thresholdNormalMaximum)
{
    if (this->thresholdNormalMaximum != thresholdNormalMaximum) {
        this->thresholdNormalMaximum = thresholdNormalMaximum;
        this->setModified();
    }
}

/**
 * Get the threshold test.
 * @return Threshold test.
 *
 */
PaletteThresholdTestEnum::Enum
PaletteColorMapping::getThresholdTest() const
{
    return thresholdTest;
}

/**
 * Set the threshold test.
 * @param thresholdTest - The threshold test.
 *
 */
void
PaletteColorMapping::setThresholdTest(const PaletteThresholdTestEnum::Enum thresholdTest)
{
    if (this->thresholdTest != thresholdTest) {
        this->thresholdTest = thresholdTest;
        this->setModified();
    }
}

/**
 * Get the threshold type.
 * @return  Threshold type.
 *
 */
PaletteThresholdTypeEnum::Enum
PaletteColorMapping::getThresholdType() const
{
    return thresholdType;
}

/**
 * Set the threshold type.
 *
 * @param thresholdType - The threshold type.
 */
void
PaletteColorMapping::setThresholdType(const PaletteThresholdTypeEnum::Enum thresholdType)
{
    if (this->thresholdType != thresholdType) {
        this->thresholdType = thresholdType;
        this->setModified();
    }
}

/**
 * @return The threshold range mode.
 */
PaletteThresholdRangeModeEnum::Enum
PaletteColorMapping::getThresholdRangeMode() const
{
    return this->thresholdRangeMode;
}

/**
 * Set the threshold range mode.
 *
 * @param rangeMode
 *    New value for range mode.
 */
void
PaletteColorMapping::setThresholdRangeMode(const PaletteThresholdRangeModeEnum::Enum rangeMode)
{
    if (this->thresholdRangeMode != rangeMode) {
        this->thresholdRangeMode = rangeMode;
        setModified();
    }
}

/**
 * Get the name of the threshold data which may be the name of a
 * functional volume or a metric column.
 * @return  Name of data used as a threshold.
 *
 */
AString
PaletteColorMapping::getThresholdDataName() const
{
    return thresholdDataName;
}

/**
 * Set the name of the threshold data which may be the name of a
 * functional volume or a metric column.
 * @param thresholdDataName - name of data used as a threshold.
 *
 */
void
PaletteColorMapping::setThresholdDataName(const AString& thresholdDataName)
{
    if (this->thresholdDataName != thresholdDataName) {
        this->thresholdDataName = thresholdDataName;
        this->setModified();
    }
}

/**
 * Display non-zero data that fails the threshold test in green?
 * @return true if displayed, else false.
 *
 */
bool
PaletteColorMapping::isShowThresholdFailureInGreen() const
{
    return this->thresholdShowFailureInGreen;
}

/**
 * Set display non-zero data that fails the threshold test in green?
 * @param showInGreenFlag - true if displayed, else false.
 *
 */
void
PaletteColorMapping::setShowThresholdFailureInGreen(const bool showInGreenFlag)
{
    if (this->thresholdShowFailureInGreen != showInGreenFlag) {
        this->thresholdShowFailureInGreen = showInGreenFlag;
        setModified();
    }
}

/**
 * Set this object has been modified.
 *
 */
void
PaletteColorMapping::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
PaletteColorMapping::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object
 * or any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
PaletteColorMapping::isModified() const
{
    return this->modifiedFlag;
}

/**
 * Map data values to palette normalized values using the 
 * settings in this palette color mapping.
 *
 * @param statistics
 *    Statistics containing min.max values.
 * @param data 
 *    The data values.
 * @param normalizedValuesOut
 *    Result of mapping data values to palette normalized 
 *    values which range [-1.0, 1.0].  This array MUST contain
 *    the same number of values as 'data'.
 * @param numberOfData  
 *    Number of values in both data and normalizedValuesOut.
 */
void 
PaletteColorMapping::mapDataToPaletteNormalizedValues(const FastStatistics* statistics,
                                                      const float* dataValues,
                                                      float* normalizedValuesOut,
                                                      const int64_t numberOfData) const
{
    if (numberOfData <= 0) {
        return;
    }

    /*
     * Minimum and maximum values used when mapping scalar into color palette.
     */
    float mappingMostNegative  = 0.0;
    float mappingLeastNegative = 0.0;
    float mappingLeastPositive  = 0.0;
    float mappingMostPositive  = 0.0;
    switch (this->getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
            statistics->getNonzeroRanges(mappingMostNegative, mappingLeastNegative, mappingLeastPositive, mappingMostPositive);
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
        {
            const float mostPercentage  = this->getAutoScaleAbsolutePercentageMaximum();
            const float leastPercentage = this->getAutoScaleAbsolutePercentageMinimum();
            mappingMostNegative  = -statistics->getApproxAbsolutePercentile(mostPercentage);
            mappingLeastNegative = -statistics->getApproxAbsolutePercentile(leastPercentage);
            mappingLeastPositive =  statistics->getApproxAbsolutePercentile(leastPercentage);
            mappingMostPositive  =  statistics->getApproxAbsolutePercentile(mostPercentage);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float mostNegativePercentage  = this->getAutoScalePercentageNegativeMaximum();
            const float leastNegativePercentage = this->getAutoScalePercentageNegativeMinimum();
            const float leastPositivePercentage = this->getAutoScalePercentagePositiveMinimum();
            const float mostPositivePercentage  = this->getAutoScalePercentagePositiveMaximum();
            mappingMostNegative  = statistics->getApproxNegativePercentile(mostNegativePercentage);
            mappingLeastNegative = statistics->getApproxNegativePercentile(leastNegativePercentage);
            mappingLeastPositive = statistics->getApproxPositivePercentile(leastPositivePercentage);
            mappingMostPositive  = statistics->getApproxPositivePercentile(mostPositivePercentage);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            mappingMostNegative  = this->getUserScaleNegativeMaximum();
            mappingLeastNegative = this->getUserScaleNegativeMinimum();
            mappingLeastPositive = this->getUserScalePositiveMinimum();
            mappingMostPositive  = this->getUserScalePositiveMaximum();
            break;
    }
    //TSC: the excluded zone of normalization is a SEPARATE issue to zero detection in the data
    //specifically, it is a HACK, in order for palettes to be able to specify a special color for data that is 0, which is not involved in color interpolation
    const float PALETTE_ZERO_COLOR_ZONE = 0.00001f;
    bool settingsValidPos = true, settingsValidNeg = true;
    float mappingPositiveDenominator = (mappingMostPositive - mappingLeastPositive) / (1.0f - PALETTE_ZERO_COLOR_ZONE);//this correction prevents normalization from assigning most positive a normalized value greater than 1
    if (mappingPositiveDenominator == 0.0) {//if we don't want backwards pos/neg settings to invert bright/dark, then change both these tests to be >= 0.0f
        settingsValidPos = false;
    }
    float mappingNegativeDenominator = (mappingMostNegative - mappingLeastNegative) / (-1.0f + PALETTE_ZERO_COLOR_ZONE);//ditto, but most negative maps to -1
    if (mappingNegativeDenominator == 0.0) {
        settingsValidNeg = false;
    }
    
    for (int64_t i = 0; i < numberOfData; i++) {
        float scalar    = dataValues[i];
        
        /*
         * Color scalar using palette
         */
        float normalized = 0.0f;
        if (scalar > 0.0) {
            if (settingsValidPos)
            {
                normalized = (scalar - mappingLeastPositive) / mappingPositiveDenominator + PALETTE_ZERO_COLOR_ZONE;
                if (normalized > 1.0f) {
                    normalized = 1.0f;
                }
                else if (normalized < PALETTE_ZERO_COLOR_ZONE) {
                    normalized = PALETTE_ZERO_COLOR_ZONE;
                }
            } else {
                normalized = 1.0f;
            }
        }
        else if (scalar < 0.0) {
            if (settingsValidNeg)
            {
                normalized = (scalar - mappingLeastNegative) / mappingNegativeDenominator - PALETTE_ZERO_COLOR_ZONE;
                if (normalized < -1.0f) {
                    normalized = -1.0f;
                }
                else if (normalized > -PALETTE_ZERO_COLOR_ZONE) {
                    normalized = -PALETTE_ZERO_COLOR_ZONE;
                }
            } else {
                normalized = -1.0f;
            }
        }
        normalizedValuesOut[i] = normalized;
    }
}

/**
 * Setup the numeric text for the annotation colorbar.
 *
 * @param statistics
 *     Statistics for the data.
 * @param colorBar
 *     Colorbar that receives the numeric text.
 */
void
PaletteColorMapping::setupAnnotationColorBarNumericText(const FastStatistics* statistics,
                                                        AnnotationColorBar* colorBar)
{
    colorBar->clearNumericText();
    
    std::vector<AnnotationColorBarNumericText*> colorBarNumericText;
    getPaletteColorBarScaleText(statistics,
                                colorBarNumericText);
    
//    std::vector<std::pair<float, AString> > normalizedPositionAndText;
//    getPaletteColorBarScaleText(statistics,
//                                normalizedPositionAndText);
    
    for (std::vector<AnnotationColorBarNumericText*>::iterator iter = colorBarNumericText.begin();
         iter != colorBarNumericText.end();
         iter++) {
        const AnnotationColorBarNumericText* nt = *iter;
        colorBar->addNumericText(nt->getScalar(),
                                 nt->getNumericText(),
                                 nt->getHorizontalAlignment(),
                                 nt->isDrawTickMarkAtScalar());
        delete nt;
    }
    
    colorBar->setShowTickMarksSelected(this->showTickMarksSelected);
}

/**
 * Get the text characters for drawing the scale above the palette
 * color bar.
 *
 * @param statistics
 *     Statistics for the data.
 * @param colorBarNumericTextOut,
 *     Contains attributes for numeric text displayed above color bar.
 */
void
PaletteColorMapping::getPaletteColorBarScaleText(const FastStatistics* statistics,
                                 std::vector<AnnotationColorBarNumericText*>& colorBarNumericTextOut) const
{
    colorBarNumericTextOut.clear();
    
    /*
     * Processing for Sign Only Mode
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
        {
            const int emDash = 0x2014;
            
            const AString positive("(+)"); //"POS");
            const AString zero("0");
            const AString negative("(" + QString(QChar(emDash)) + ")");   //"NEG");
            
            if (isDisplayPositiveDataFlag()
                && isDisplayNegativeDataFlag()) {
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.0,
                                                                                   negative,
                                                                                   AnnotationTextAlignHorizontalEnum::LEFT,
                                                                                   true));
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.5,
                                                                                   zero,
                                                                                   AnnotationTextAlignHorizontalEnum::CENTER,
                                                                                   true));
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(1.0,
                                                                                   positive,
                                                                                   AnnotationTextAlignHorizontalEnum::RIGHT,
                                                                                   true));
//                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, negative));
//                normalizedPositionAndTextOut.push_back(std::make_pair(0.5, zero));
//                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, positive));
            }
            else if (isDisplayPositiveDataFlag()) {
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.0,
                                                                                   zero,
                                                                                   AnnotationTextAlignHorizontalEnum::LEFT,
                                                                                   true));
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(1.0,
                                                                                   positive,
                                                                                   AnnotationTextAlignHorizontalEnum::RIGHT,
                                                                                   true));
//                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, zero));
//                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, positive));
            }
            else if (isDisplayNegativeDataFlag()) {
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.0,
                                                                                   negative,
                                                                                   AnnotationTextAlignHorizontalEnum::LEFT,
                                                                                   true));
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(1.0,
                                                                                   zero,
                                                                                   AnnotationTextAlignHorizontalEnum::CENTER,
                                                                                   true));
//                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, negative));
//                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, zero));
            }
            else if (isDisplayZeroDataFlag()) {
                colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.5,
                                                                                   zero,
                                                                                   AnnotationTextAlignHorizontalEnum::CENTER,
                                                                                   true));
//                normalizedPositionAndTextOut.push_back(std::make_pair(0.5, zero));
            }
            
            return;
        }
            break;
    }
    
    
    float negMax = -1.0;
    float negMin =  0.0;
    float posMin =  0.0;
    float posMax =  1.0;
    switch (getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
        {
            float dummy;
            statistics->getNonzeroRanges(negMax, dummy, dummy, posMax);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
        {
            const float maxPct = getAutoScaleAbsolutePercentageMaximum();
            const float minPct = getAutoScaleAbsolutePercentageMinimum();
            
            negMax = -statistics->getApproxAbsolutePercentile(maxPct);
            negMin = -statistics->getApproxAbsolutePercentile(minPct);
            posMin =  statistics->getApproxAbsolutePercentile(minPct);
            posMax =  statistics->getApproxAbsolutePercentile(maxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float negMaxPct = getAutoScalePercentageNegativeMaximum();
            const float negMinPct = getAutoScalePercentageNegativeMinimum();
            const float posMinPct = getAutoScalePercentagePositiveMinimum();
            const float posMaxPct = getAutoScalePercentagePositiveMaximum();
            
            negMax = statistics->getApproxNegativePercentile(negMaxPct);
            negMin = statistics->getApproxNegativePercentile(negMinPct);
            posMin = statistics->getApproxPositivePercentile(posMinPct);
            posMax = statistics->getApproxPositivePercentile(posMaxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            negMax = getUserScaleNegativeMaximum();
            negMin = getUserScaleNegativeMinimum();
            posMin = getUserScalePositiveMinimum();
            posMax = getUserScalePositiveMaximum();
            break;
    }
    
    /*
     * numeric values displayed for negative data
     */
    std::vector<float> negativeValues;
    negativeValues.push_back(negMax);
    if (this->numericSubdivisionCount > 0) {
        const float range    = negMin - negMax;
        const float interval = range / (this->numericSubdivisionCount + 1);
        for (int32_t i = 0; i < this->numericSubdivisionCount; i++) {
            negativeValues.push_back(negMax
                                     + (interval * (i + 1)));
        }
    }
    negativeValues.push_back(negMin);
    
    /*
     * numeric values displayed for positive data
     */
    std::vector<float> positiveValues;
    positiveValues.push_back(posMin);
    if (this->numericSubdivisionCount > 0) {
        const float range    = posMax - posMin;
        const float interval = range / (this->numericSubdivisionCount + 1);
        for (int32_t i = 0; i < this->numericSubdivisionCount; i++) {
            positiveValues.push_back(posMin
                                     + (interval * (i + 1)));
        }
    }
    positiveValues.push_back(posMax);
    
    /*
     * Will need to override these values when percentile
     */
    NumericFormatModeEnum::Enum numericFormatModeForTextFormatting = this->numericFormatMode;
    int32_t precisionDigitsForTextFormatting = this->precisionDigits;
    
    /*
     * Processing for percentile mode
     * Convert numeric values to percentiles
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            for (std::vector<float>::iterator iter = positiveValues.begin();
                 iter != positiveValues.end();
                 iter++) {
                const float percentile = statistics->getPositiveValuePercentile(*iter);
                *iter = percentile;
            }
            for (std::vector<float>::iterator iter = negativeValues.begin();
                 iter != negativeValues.end();
                 iter++) {
                const float percentile = statistics->getNegativeValuePercentile(*iter);
                *iter = percentile;
            }
            
            /*
             * Decimal mode integers for percentile
             */
            numericFormatModeForTextFormatting = NumericFormatModeEnum::DECIMAL;
            //precisionDigitsForTextFormatting = 0;
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
            CaretAssertMessage(0, "Should never get here.  Sign only handled above");
            break;
    }
    
    /*
     * Create text representations for negative values
     */
    const int32_t numberOfNegValues = static_cast<int32_t>(negativeValues.size());
    std::vector<AString> negativeValuesText(numberOfNegValues);
    NumericTextFormatting::formatValueRange(numericFormatModeForTextFormatting,
                                            precisionDigitsForTextFormatting,
                                            &negativeValues[0],
                                            &negativeValuesText[0],
                                            numberOfNegValues);
    
    /*
     * Create text representations for positive values
     */
    const int32_t numberOfPosValues = static_cast<int32_t>(positiveValues.size());
    std::vector<AString> positiveValuesText(numberOfPosValues);
    NumericTextFormatting::formatValueRange(numericFormatModeForTextFormatting,
                                            precisionDigitsForTextFormatting,
                                            &positiveValues[0],
                                            &positiveValuesText[0],
                                            numberOfPosValues);
    
    CaretAssert(negativeValues.size() == negativeValuesText.size());
    CaretAssert(positiveValues.size() == positiveValuesText.size());
    
    /*
     * Types of values for display
     */
    const bool positiveDataDisplayedFlag = isDisplayPositiveDataFlag();
    const bool negativeDataDisplayedFlag = isDisplayNegativeDataFlag();
    
    /*
     * Are both negative and positive values displayed?
     */
    AString zeroValueText;
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        CaretAssert(negativeValuesText.size() > 0);
        const AString negMinText = negativeValuesText.back();
        CaretAssert(positiveValuesText.size() > 0);
        const AString posMinText = positiveValuesText.front();
        
        if (negMinText == posMinText) {
            /*
             * When the negative min and positive min values are the
             * same, there is no need to display both of them.
             */
            zeroValueText = negMinText;
        }
        else {
            /*
             * When the negative min and positive min values are the
             * different, display both an separate with a slash
             */
            zeroValueText = negMinText + "/" + posMinText;
        }
        
        /*
         * Since the negative min and positive min text values
         * are dislayed together by above code, remove these
         * values from their respective text values.
         */
        negativeValuesText.resize(negativeValuesText.size() - 1);
        negativeValues.resize(negativeValues.size() - 1);
        CaretAssert(negativeValues.size() == negativeValuesText.size());
        positiveValuesText.erase(positiveValuesText.begin());
        positiveValues.erase(positiveValues.begin());
        CaretAssert(positiveValues.size() == positiveValuesText.size());
    }
    
    /*
     * The positions of the text are normalized in the range zero
     * to one where zero is at the left side of the color bar
     * and one is at the right side of the color bar.
     */
    float negPositionStart = 0.0;
    float negPositionInterval = 0.0;
    float posPositionStart = 0.0;
    float posPositionInterval = 0.0;
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        negPositionStart = 0.0;
        CaretAssert(negativeValuesText.size() > 0);
        negPositionInterval = (0.5 / negativeValuesText.size());
        
        CaretAssert(positiveValuesText.size() > 0);
        posPositionInterval = (0.5 / positiveValuesText.size());
        posPositionStart = 0.5 + posPositionInterval;
    }
    else if (negativeDataDisplayedFlag) {
        negPositionStart = 0.0;
        negPositionInterval = 1.0;
        if (negativeValuesText.size() > 1) {
            CaretAssert(negativeValuesText.size() > 0);
            negPositionInterval = (1.0 / (negativeValuesText.size() - 1));
        }
    }
    else if (positiveDataDisplayedFlag) {
        posPositionStart = 0.0;
        posPositionInterval = 1.0;
        if (positiveValuesText.size() > 1) {
            CaretAssert(positiveValuesText.size() > 0);
            posPositionInterval = (1.0 / (positiveValuesText.size() - 1));
        }
    }
    
    /*
     * Output the negative values text
     */
    if (negativeDataDisplayedFlag) {
        const int32_t numValues = static_cast<int32_t>(negativeValuesText.size());
        for (int32_t i = 0; i < numValues; i++) {
            CaretAssertVectorIndex(negativeValuesText, i);
            const float value = (negPositionStart
                                 + (i * negPositionInterval));
            
            AnnotationTextAlignHorizontalEnum::Enum alignment = AnnotationTextAlignHorizontalEnum::CENTER;
            if (i == 0) {
                alignment = AnnotationTextAlignHorizontalEnum::LEFT;
            }
            else if (i == (numValues - 1)) {
                if ( ! positiveDataDisplayedFlag) {
                    alignment = AnnotationTextAlignHorizontalEnum::RIGHT;
                }
            }
            colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(value,
                                                                               negativeValuesText[i],
                                                                               alignment,
                                                                               true));
//            normalizedPositionAndTextOut.push_back(std::make_pair(value,
//                                                                  negativeValuesText[i]));
        }
    }
    
    /*
     * Add the zero value text
     */
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.5,
                                                                           zeroValueText,
                                                                           AnnotationTextAlignHorizontalEnum::CENTER,
                                                                           true));
//        normalizedPositionAndTextOut.push_back(std::make_pair(0.5,
//                                                              zeroValueText));
    }
    
    /*
     * When zero only
     */
    if (isDisplayZeroDataFlag()) {
        if (( ! negativeDataDisplayedFlag)
            && ( ! positiveDataDisplayedFlag)) {
            colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.0,
                                                                               "",
                                                                               AnnotationTextAlignHorizontalEnum::CENTER,
                                                                               true));
            colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(0.5,
                                                                               "0.0",
                                                                               AnnotationTextAlignHorizontalEnum::CENTER,
                                                                               true));
            colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(1.0,
                                                                               "",
                                                                               AnnotationTextAlignHorizontalEnum::CENTER,
                                                                               true));
        }
    }
    
    /*
     * Add the positive values text
     */
    if (positiveDataDisplayedFlag) {
        const int32_t numValues = static_cast<int32_t>(positiveValuesText.size());
        for (int32_t i = 0; i < numValues; i++) {
            CaretAssertVectorIndex(positiveValuesText, i);
            const float value = (posPositionStart
                                 + (i * posPositionInterval));
            
            AnnotationTextAlignHorizontalEnum::Enum alignment = AnnotationTextAlignHorizontalEnum::CENTER;
            if (i == (numValues - 1)) {
                alignment = AnnotationTextAlignHorizontalEnum::RIGHT;
            }
            else if (i == 0) {
                if ( ! negativeDataDisplayedFlag) {
                    alignment = AnnotationTextAlignHorizontalEnum::LEFT;
                }
            }
            colorBarNumericTextOut.push_back(new AnnotationColorBarNumericText(value,
                                                                               positiveValuesText[i],
                                                                               alignment,
                                                                               true));
//            normalizedPositionAndTextOut.push_back(std::make_pair(value,
//                                                                  positiveValuesText[i]));
        }
    }
    
    /*
     * Add percentage signs when percentile is selected
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            for (std::vector<AnnotationColorBarNumericText*>::iterator iter = colorBarNumericTextOut.begin();
                 iter != colorBarNumericTextOut.end();
                 iter++) {
                AnnotationColorBarNumericText* nt = *iter;
                nt->setNumericText(nt->getNumericText() + "%");
            }
//            for (std::vector<std::pair<float, AString> >::iterator iter = normalizedPositionAndTextOut.begin();
//                 iter != normalizedPositionAndTextOut.end();
//                 iter++) {
//                iter->second.append("%");
//            }
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
            break;
    }
    
    const bool debugFlag = false;
    if (debugFlag) {
        const int numItems = static_cast<int32_t>(colorBarNumericTextOut.size());
        std::cout << "Colorbar: " << std::endl;
        for (int32_t i = 0; i < numItems; i++) {
            const AnnotationColorBarNumericText* nt = colorBarNumericTextOut[i];
            std::cout << "    " << qPrintable(QString::number(nt->getScalar())
                                              + ": "
                                              + nt->getNumericText()
                                              + " "
                                              + AnnotationTextAlignHorizontalEnum::toGuiName(nt->getHorizontalAlignment())
                                              + " show-tick="
                                              + AString::fromBool(nt->isDrawTickMarkAtScalar())) << std::endl;
//            std::cout << "    " << qPrintable(QString::number(normalizedPositionAndTextOut[i].first)
//                                              + ": "
//                                              + normalizedPositionAndTextOut[i].second) << std::endl;
        }
        std::cout << std::endl;
    }
}


/**
 * Get the text characters for drawing the scale above the palette
 * color bar.
 *
 * @param statistics
 *     Statistics for the data.
 * @param normalizedPositionAndTextOut,
 *     Horizontal position ranging 0.0 to 1.0
 *     and text values.
 */
void
PaletteColorMapping::getPaletteColorBarScaleText(const FastStatistics* statistics,
                                                 std::vector<std::pair<float, AString> >& normalizedPositionAndTextOut) const
{
    normalizedPositionAndTextOut.clear();
    
    /*
     * Processing for Sign Only Mode
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
        {
            const int emDash = 0x2014;
            
            const AString positive("(+)"); //"POS");
            const AString zero("0");
            const AString negative("(" + QString(QChar(emDash)) + ")");   //"NEG");
            
            if (isDisplayPositiveDataFlag()
                && isDisplayNegativeDataFlag()) {
                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, negative));
                normalizedPositionAndTextOut.push_back(std::make_pair(0.5, zero));
                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, positive));
            }
            else if (isDisplayPositiveDataFlag()) {
                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, zero));
                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, positive));
            }
            else if (isDisplayNegativeDataFlag()) {
                normalizedPositionAndTextOut.push_back(std::make_pair(0.0, negative));
                normalizedPositionAndTextOut.push_back(std::make_pair(1.0, zero));
            }
            else if (isDisplayZeroDataFlag()) {
                normalizedPositionAndTextOut.push_back(std::make_pair(0.5, zero));
            }
            
            return;
        }
            break;
    }
    
    
    float negMax = -1.0;
    float negMin =  0.0;
    float posMin =  0.0;
    float posMax =  1.0;
    switch (getScaleMode()) {
        case PaletteScaleModeEnum::MODE_AUTO_SCALE:
        {
            float dummy;
            statistics->getNonzeroRanges(negMax, dummy, dummy, posMax);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
        {
            const float maxPct = getAutoScaleAbsolutePercentageMaximum();
            const float minPct = getAutoScaleAbsolutePercentageMinimum();
            
            negMax = -statistics->getApproxAbsolutePercentile(maxPct);
            negMin = -statistics->getApproxAbsolutePercentile(minPct);
            posMin =  statistics->getApproxAbsolutePercentile(minPct);
            posMax =  statistics->getApproxAbsolutePercentile(maxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
        {
            const float negMaxPct = getAutoScalePercentageNegativeMaximum();
            const float negMinPct = getAutoScalePercentageNegativeMinimum();
            const float posMinPct = getAutoScalePercentagePositiveMinimum();
            const float posMaxPct = getAutoScalePercentagePositiveMaximum();
            
            negMax = statistics->getApproxNegativePercentile(negMaxPct);
            negMin = statistics->getApproxNegativePercentile(negMinPct);
            posMin = statistics->getApproxPositivePercentile(posMinPct);
            posMax = statistics->getApproxPositivePercentile(posMaxPct);
        }
            break;
        case PaletteScaleModeEnum::MODE_USER_SCALE:
            negMax = getUserScaleNegativeMaximum();
            negMin = getUserScaleNegativeMinimum();
            posMin = getUserScalePositiveMinimum();
            posMax = getUserScalePositiveMaximum();
            break;
    }
    
    /*
     * numeric values displayed for negative data
     */
    std::vector<float> negativeValues;
    negativeValues.push_back(negMax);
    if (this->numericSubdivisionCount > 0) {
        const float range    = negMin - negMax;
        const float interval = range / (this->numericSubdivisionCount + 1);
        for (int32_t i = 0; i < this->numericSubdivisionCount; i++) {
            negativeValues.push_back(negMax
                                     + (interval * (i + 1)));
        }
    }
    negativeValues.push_back(negMin);
    
    /*
     * numeric values displayed for positive data
     */
    std::vector<float> positiveValues;
    positiveValues.push_back(posMin);
    if (this->numericSubdivisionCount > 0) {
        const float range    = posMax - posMin;
        const float interval = range / (this->numericSubdivisionCount + 1);
        for (int32_t i = 0; i < this->numericSubdivisionCount; i++) {
            positiveValues.push_back(posMin
                                     + (interval * (i + 1)));
        }
    }
    positiveValues.push_back(posMax);
    
    /*
     * Wil need to override these values when percentile
     */
    NumericFormatModeEnum::Enum numericFormatModeForTextFormatting = this->numericFormatMode;
    int32_t precisionDigitsForTextFormatting = this->precisionDigits;
    
    /*
     * Processing for percentile mode
     * Convert numeric values to percentiles
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            for (std::vector<float>::iterator iter = positiveValues.begin();
                 iter != positiveValues.end();
                 iter++) {
                const float percentile = statistics->getPositiveValuePercentile(*iter);
                *iter = percentile;
            }
            for (std::vector<float>::iterator iter = negativeValues.begin();
                 iter != negativeValues.end();
                 iter++) {
                const float percentile = statistics->getNegativeValuePercentile(*iter);
                *iter = percentile;
            }
            
            /*
             * Decimal mode integers for percentile
             */
            numericFormatModeForTextFormatting = NumericFormatModeEnum::DECIMAL;
            precisionDigitsForTextFormatting = 0;
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
            CaretAssertMessage(0, "Should never get here.  Sign only handled above");
            break;
    }
    
    /*
     * Create text representations for negative values
     */
    const int32_t numberOfNegValues = static_cast<int32_t>(negativeValues.size());
    std::vector<AString> negativeValuesText(numberOfNegValues);
    NumericTextFormatting::formatValueRange(numericFormatModeForTextFormatting,
                                            precisionDigitsForTextFormatting,
                                            &negativeValues[0],
                                            &negativeValuesText[0],
                                            numberOfNegValues);
    
    /*
     * Create text representations for positive values
     */
    const int32_t numberOfPosValues = static_cast<int32_t>(positiveValues.size());
    std::vector<AString> positiveValuesText(numberOfPosValues);
    NumericTextFormatting::formatValueRange(numericFormatModeForTextFormatting,
                                            precisionDigitsForTextFormatting,
                                            &positiveValues[0],
                                            &positiveValuesText[0],
                                            numberOfPosValues);
    
    CaretAssert(negativeValues.size() == negativeValuesText.size());
    CaretAssert(positiveValues.size() == positiveValuesText.size());
    
    /*
     * Types of values for display
     */
    const bool positiveDataDisplayedFlag = isDisplayPositiveDataFlag();
    const bool negativeDataDisplayedFlag = isDisplayNegativeDataFlag();
    
    /*
     * Are both negative and positive values displayed?
     */
    AString zeroValueText;
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        CaretAssert(negativeValuesText.size() > 0);
        const AString negMinText = negativeValuesText.back();
        CaretAssert(positiveValuesText.size() > 0);
        const AString posMinText = positiveValuesText.front();
        
        if (negMinText == posMinText) {
            /*
             * When the negative min and positive min values are the
             * same, there is no need to display both of them.
             */
            zeroValueText = negMinText;
        }
        else {
            /*
             * When the negative min and positive min values are the
             * different, display both an separate with a slash
             */
            zeroValueText = negMinText + "/" + posMinText;
        }
        
        /*
         * Since the negative min and positive min text values
         * are dislayed together by above code, remove these
         * values from their respective text values.
         */
        negativeValuesText.resize(negativeValuesText.size() - 1);
        negativeValues.resize(negativeValues.size() - 1);
        CaretAssert(negativeValues.size() == negativeValuesText.size());
        positiveValuesText.erase(positiveValuesText.begin());
        positiveValues.erase(positiveValues.begin());
        CaretAssert(positiveValues.size() == positiveValuesText.size());
    }
    
    /*
     * The positions of the text are normalized in the range zero
     * to one where zero is at the left side of the color bar
     * and one is at the right side of the color bar.
     */
    float negPositionStart = 0.0;
    float negPositionInterval = 0.0;
    float posPositionStart = 0.0;
    float posPositionInterval = 0.0;
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        negPositionStart = 0.0;
        CaretAssert(negativeValuesText.size() > 0);
        negPositionInterval = (0.5 / negativeValuesText.size());
        
        CaretAssert(positiveValuesText.size() > 0);
        posPositionInterval = (0.5 / positiveValuesText.size());
        posPositionStart = 0.5 + posPositionInterval;
    }
    else if (negativeDataDisplayedFlag) {
        negPositionStart = 0.0;
        negPositionInterval = 1.0;
        if (negativeValuesText.size() > 1) {
            CaretAssert(negativeValuesText.size() > 0);
            negPositionInterval = (1.0 / (negativeValuesText.size() - 1));
        }
    }
    else if (positiveDataDisplayedFlag) {
        posPositionStart = 0.0;
        posPositionInterval = 1.0;
        if (positiveValuesText.size() > 1) {
            CaretAssert(positiveValuesText.size() > 0);
            posPositionInterval = (1.0 / (positiveValuesText.size() - 1));
        }
    }
    
    /*
     * Output the negative values text
     */
    if (negativeDataDisplayedFlag) {
        const int32_t numValues = static_cast<int32_t>(negativeValuesText.size());
        for (int32_t i = 0; i < numValues; i++) {
            CaretAssertVectorIndex(negativeValuesText, i);
            const float value = (negPositionStart
                                 + (i * negPositionInterval));
            normalizedPositionAndTextOut.push_back(std::make_pair(value,
                                                                  negativeValuesText[i]));
        }
    }
    
    /*
     * Add the zero value text
     */
    if (negativeDataDisplayedFlag
        && positiveDataDisplayedFlag) {
        normalizedPositionAndTextOut.push_back(std::make_pair(0.5,
                                                              zeroValueText));
    }
    
    /*
     * Add the positive values text
     */
    if (positiveDataDisplayedFlag) {
        const int32_t numValues = static_cast<int32_t>(positiveValuesText.size());
        for (int32_t i = 0; i < numValues; i++) {
            CaretAssertVectorIndex(positiveValuesText, i);
            const float value = (posPositionStart
                                 + (i * posPositionInterval));
            normalizedPositionAndTextOut.push_back(std::make_pair(value,
                                                                  positiveValuesText[i]));
        }
    }
    
    /*
     * Add percentage signs when percentile is selected
     */
    switch (this->colorBarValuesMode) {
        case PaletteColorBarValuesModeEnum::DATA:
            break;
        case PaletteColorBarValuesModeEnum::PERCENTILE:
            for (std::vector<std::pair<float, AString> >::iterator iter = normalizedPositionAndTextOut.begin();
                 iter != normalizedPositionAndTextOut.end();
                 iter++) {
                iter->second.append("%");
            }
            break;
        case PaletteColorBarValuesModeEnum::SIGN_ONLY:
            break;
    }
    
    const bool debugFlag = false;
    if (debugFlag) {
        const int numItems = static_cast<int32_t>(normalizedPositionAndTextOut.size());
        std::cout << "Colorbar: " << std::endl;
        for (int32_t i = 0; i < numItems; i++) {
            std::cout << "    " << qPrintable(QString::number(normalizedPositionAndTextOut[i].first)
                                              + ": "
                                              + normalizedPositionAndTextOut[i].second) << std::endl;
        }
        std::cout << std::endl;
    }
}

/**
 * @return True if thresholding is linked meaning
 * that the high threshold is restricted to a positive value
 * the low threshold = -high.
 *
 * This is just a status and it is up to the user of this class
 * to properly set the threshold values.
 */
bool
PaletteColorMapping::isThresholdNegMinPosMaxLinked() const
{
    return this->thresholdNegMinPosMaxLinked;
}

/**
 * Set thresholding is linked meaning
 * that the high threshold is restricted to a positive value
 * the low threshold = -high.
 *
 * This is just a status and it is up to the user of this class
 * to properly set the threshold values.
 *
 * @param linked
 *    New status of low/high linked thresholding.
 */
void
PaletteColorMapping::setThresholdNegMinPosMaxLinked(const bool linked)
{
    if (this->thresholdNegMinPosMaxLinked != linked) {
        this->thresholdNegMinPosMaxLinked = linked;
        setModified();
    }
}

/**
 * @return The histogram range mode.
 */
PaletteHistogramRangeModeEnum::Enum
PaletteColorMapping::getHistogramRangeMode() const
{
    return this->histogramRangeMode;
}

/**
 * Set the histogram range mode.
 *
 * @param histogramRangeMode
 *     New value for histogram range mode.
 */
void
PaletteColorMapping::setHistogramRangeMode(const PaletteHistogramRangeModeEnum::Enum histogramRangeMode)
{
    if (histogramRangeMode != this->histogramRangeMode) {
        this->histogramRangeMode = histogramRangeMode;
        setModified();
    }
}

/**
 * @return The histogram chart type.
 */
PaletteHistogramChartTypeEnum::Enum PaletteColorMapping::getHistogramChartType() const
{
    return this->histogramChartType;
}

/**
 * Set the histogram chart type.
 *
 * @param histogramChartType
 *    New value for histogram chart type
 */
void
PaletteColorMapping::setHistogramChartType(const PaletteHistogramChartTypeEnum::Enum histogramChartType)
{
    if (histogramChartType != this->histogramChartType) {
        this->histogramChartType = histogramChartType;
        setModified();
    }
}


/**
 * @return Color for drawing histogram.
 */
CaretColorEnum::Enum
PaletteColorMapping::getHistogramColor() const
{
    return histogramColor;
}

/**
 * Set the color for drawing the histogram.
 *
 * @param histogramColor
 *     New color for the histogram.
 */
void
PaletteColorMapping::setHistogramColor(const CaretColorEnum::Enum histogramColor)
{
    if (histogramColor != this->histogramColor) {
        this->histogramColor = histogramColor;
        setModified();
    }
}

/**
 * @return The numeric format mode.
 */
NumericFormatModeEnum::Enum
PaletteColorMapping::getNumericFormatMode() const
{
    return this->numericFormatMode;
}

/**
 * Set the numeric format mode.
 *
 * @param numericFormatMode
 *     New value for precision mode.
 */
void
PaletteColorMapping::setNumericFormatMode(const NumericFormatModeEnum::Enum numericFormatMode)
{
    if (numericFormatMode != this->numericFormatMode) {
        this->numericFormatMode = numericFormatMode;
        setModified();
    }
}

/**
 * @return The precision digits (right of decimal).
 */
int32_t
PaletteColorMapping::getPrecisionDigits() const
{
    return this->precisionDigits;
}

/**
 * Set the precision digits (right of decimal)
 *
 * @param precisionDigits
 *     New value for number of digits right of decimal.
 */
void
PaletteColorMapping::setPrecisionDigits(const int32_t precisionDigits)
{
    if (precisionDigits != this->precisionDigits) {
        this->precisionDigits = precisionDigits;
        setModified();
    }
}

/**
 * @return The numeric subdivision count which is the number of 
 * numeric values uniformly spaced between zero and the maximum 
 * value.
 */
int32_t
PaletteColorMapping::getNumericSubdivisionCount() const
{
    return this->numericSubdivisionCount;
}

/**
 * Set the numeric subdivision count which is the number of
 * numeric values uniformly spaced between zero and the maximum
 * value.
 *
 * @param numericSubvisionCount
 *     New value for subdivision count.
 */
void
PaletteColorMapping::setNumericSubdivisionCount(const int32_t numericSubdivisionCount)
{
    if (numericSubdivisionCount != this->numericSubdivisionCount) {
        this->numericSubdivisionCount = numericSubdivisionCount;
        setModified();
    }
}

/**
 * @return The color bar values mode.
 */
PaletteColorBarValuesModeEnum::Enum PaletteColorMapping::getColorBarValuesMode() const
{
    return this->colorBarValuesMode;
}

/**
 * Set the color bar values mode.
 *
 * @param colorBarValuesMode
 *     New value for color bar values mode.
 */
void PaletteColorMapping::setColorBarValuesMode(const PaletteColorBarValuesModeEnum::Enum colorBarValuesMode)
{
    if (colorBarValuesMode != this->colorBarValuesMode) {
        this->colorBarValuesMode = colorBarValuesMode;
        setModified();
    }
}

/**
 * @param Is show tick marks selected?
 */
bool
PaletteColorMapping::isShowTickMarksSelected() const
{
    return this->showTickMarksSelected;
}

/**
 * Set show tick marks selected.
 *
 * @param selected
 *     New selection status.
 */
void
PaletteColorMapping::setShowTickMarksSelected(const bool selected)
{
    if (selected != this->showTickMarksSelected) {
        this->showTickMarksSelected = selected;
        setModified();
    }
}



