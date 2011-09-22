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

#include <sstream>

#include "PaletteColorMapping.h"
#include "PaletteColorMappingSaxReader.h"
#include "PaletteXmlElements.h"
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
 * Helps with copy constructor and assignment operator.
 */
void
PaletteColorMapping::copyHelper(const PaletteColorMapping& pcm)
{
    this->autoScalePercentageNegativeMaximum = pcm.autoScalePercentageNegativeMaximum;
    this->autoScalePercentageNegativeMinimum = pcm.autoScalePercentageNegativeMinimum;
    this->autoScalePercentagePositiveMaximum = pcm.autoScalePercentagePositiveMaximum;
    this->autoScalePercentagePositiveMinimum = pcm.autoScalePercentagePositiveMinimum;
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
    this->thresholdNormalNegative = pcm.thresholdNormalNegative;
    this->thresholdNormalPositive = pcm.thresholdNormalPositive;
    this->thresholdMappedNegative= pcm.thresholdMappedNegative;
    this->thresholdMappedPositive = pcm.thresholdMappedPositive;
    this->thresholdMappedAverageAreaNegative = pcm.thresholdMappedAverageAreaNegative;
    this->thresholdMappedAverageAreaPositive = pcm.thresholdMappedAverageAreaPositive;
    this->thresholdDataName = pcm.thresholdDataName;
    this->thresholdShowFailureInGreen = pcm.thresholdShowFailureInGreen;
    
    this->clearModified();
}

void
PaletteColorMapping::initializeMembersPaletteColorMapping()
{
    this->scaleMode = PaletteScaleModeEnum::MODE_AUTO_SCALE;
    this->autoScalePercentageNegativeMaximum = 90.0f;
    this->autoScalePercentageNegativeMinimum = 2.0f;
    this->autoScalePercentagePositiveMinimum = 2.0f;
    this->autoScalePercentagePositiveMaximum = 98.0f;
    this->userScaleNegativeMaximum = -100.0f;
    this->userScaleNegativeMinimum = 0.0f;
    this->userScalePositiveMinimum = 0.0f;
    this->userScalePositiveMaximum = 100.0f;
    this->selectedPaletteName = "PSYCH";
    this->interpolatePaletteFlag = false;
    this->displayPositiveDataFlag = true;
    this->displayZeroDataFlag = false;
    this->displayNegativeDataFlag = true;
    this->thresholdType = PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF;
    this->thresholdTest = PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_ABOVE;
    this->thresholdNormalNegative = -1.0f;
    this->thresholdNormalPositive = 1.0f;
    this->thresholdMappedNegative = -1.0f;
    this->thresholdMappedPositive = 1.0f;
    this->thresholdMappedAverageAreaNegative = -1.0f;
    this->thresholdMappedAverageAreaPositive =  1.0f;
    this->thresholdDataName = "";
    this->thresholdShowFailureInGreen = false;
    this->modifiedFlag = false;
}

/**
 * Write the object as XML.
 * @param xmlWriter - write to this->
 * @throws XmlException - If an error occurs.
 *
 */
void
PaletteColorMapping::writeAsXML(XmlWriter& xmlWriter)
                       throw (XmlException)
{
    XmlAttributes attributes;
    attributes.addAttribute(
                            PaletteXmlElements::XML_ATTRIBUTE_VERSION_NUMBER,
                            PaletteXmlElements::XML_VERSION_NUMBER);
    xmlWriter.writeStartElement(
                                PaletteXmlElements::XML_TAG_PALETTE_COLOR_MAPPING,
                                attributes);
    xmlWriter.writeElementCharacters(PaletteXmlElements::XML_TAG_SCALE_MODE,
                                     PaletteScaleModeEnum::toName(this->scaleMode));
    float autoScaleValues[4] = {
        this->autoScalePercentageNegativeMaximum,
        this->autoScalePercentageNegativeMinimum,
        this->autoScalePercentagePositiveMinimum,
        this->autoScalePercentagePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES,
                                     autoScaleValues,
                                     4);
    float userScaleValues[4] = {
        this->userScaleNegativeMaximum,
        this->userScaleNegativeMinimum,
        this->userScalePositiveMinimum,
        this->userScalePositiveMaximum
    };
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_USER_SCALE_VALUES,
                                     userScaleValues,
                                     4);
    xmlWriter.writeElementCharacters(PaletteXmlElements::XML_TAG_PALETTE_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->selectedPaletteName));
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_INTERPOLATE,
                                     this->interpolatePaletteFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_DISPLAY_POSITIVE,
                                     this->displayPositiveDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_DISPLAY_ZERO,
                                     this->displayZeroDataFlag);
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_DISPLAY_NEGATIVE,
                                     this->displayNegativeDataFlag);
    
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_TEST,
                                     PaletteThresholdTestEnum::toName(this->thresholdTest));
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_TYPE,
                                     PaletteThresholdTypeEnum::toName(this->thresholdType));
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_FAILURE_IN_GREEN,
                                     this->thresholdShowFailureInGreen);
    float mappedValues[2] = {
        this->thresholdMappedNegative,
        this->thresholdMappedPositive
    };
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_MAPPED_VALUES,
                                     mappedValues,
                                     2);
    
    float mappedAvgAreaValues[2] = {
        this->thresholdMappedAverageAreaNegative,
        this->thresholdMappedAverageAreaPositive
    };
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES,
                                     mappedAvgAreaValues,
                                     2);
    
    xmlWriter.writeElementCharacters(
                                     PaletteXmlElements::XML_TAG_THRESHOLD_DATA_NAME,
                                     XmlUtilities::encodeXmlSpecialCharacters(this->thresholdDataName));
    
    
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
            throw (XmlException)
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
            throw (XmlException)
{
    PaletteColorMappingSaxReader saxReader(this);
    XmlSaxParser* parser = XmlSaxParser::createXmlParser();
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
    delete parser;
}

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
 * Get mapped average area negative threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaNegative() const
{
    return thresholdMappedAverageAreaNegative;
}

/**
 * See if a values passes the threshold requirements
 * @param value - Value to test against threshold requirements.
 * @return true if value passes threshold requirements, else false.
 *
 */
bool
PaletteColorMapping::performThresholdTest(const float value)
{
    float negMax = std::numeric_limits<float>::max();
    float negMin = 0.0f;
    float posMin = 0.0f;
    float posMax = -std::numeric_limits<float>::max();
    
    bool showAboveFlag =
    (this->thresholdTest == PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_ABOVE);
    
    bool passesFlag = false;
    switch (this->thresholdType) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            passesFlag = true;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            if (showAboveFlag) {
                negMin = this->thresholdNormalNegative;
                posMin = this->thresholdNormalPositive;
            }
            else {
                negMax = this->thresholdNormalNegative;
                posMax = this->thresholdNormalPositive;
            }
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            if (showAboveFlag) {
                negMin = this->thresholdMappedNegative;
                posMin = this->thresholdMappedPositive;
            }
            else {
                negMax = this->thresholdMappedNegative;
                posMax = this->thresholdMappedPositive;
            }
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            if (showAboveFlag) {
                negMin = this->thresholdMappedAverageAreaNegative;
                posMin = this->thresholdMappedAverageAreaPositive;
            }
            else {
                negMax = this->thresholdMappedAverageAreaNegative;
                posMax = this->thresholdMappedAverageAreaPositive;
            }
            break;
    }
    
    if (passesFlag == false) {
        if (value < 0.0f) {
            if ((value >= negMax) &&
                (value <= negMin)) {
                passesFlag = true;
            }
        }
        else {
            if ((value >= posMin) &&
                (value <= posMax)) {
                passesFlag = true;
            }
        }
    }
    return passesFlag;
}

/**
 * Set the mapped average area negative threshold.
 *
 * @param thresholdMappedAverageAreaNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaNegative(const float thresholdMappedAverageAreaNegative)
{
    if (this->thresholdMappedAverageAreaNegative != thresholdMappedAverageAreaNegative) {
        this->thresholdMappedAverageAreaNegative = thresholdMappedAverageAreaNegative;
        this->setModified();
    }
}

/**
 * Get mapped average area positive threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedAverageAreaPositive() const
{
    return thresholdMappedAverageAreaPositive;
}

/**
 * Set the mapped average area positive threshold.
 *
 * @param thresholdMappedAverageAreaPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedAverageAreaPositive(const float thresholdMappedAverageAreaPositive)
{
    if (this->thresholdMappedAverageAreaPositive != thresholdMappedAverageAreaPositive) {
        this->thresholdMappedAverageAreaPositive = thresholdMappedAverageAreaPositive;
        this->setModified();
    }
}

/**
 * Get mapped negative threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedNegative() const
{
    return thresholdMappedNegative;
}

/**
 * Set the mapped negative threshold.
 *
 * @param thresholdMappedNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedNegative(const float thresholdMappedNegative)
{
    if (this->thresholdMappedNegative != thresholdMappedNegative) {
        this->thresholdMappedNegative = thresholdMappedNegative;
        this->setModified();
    }
}

/**
 * Get mapped positive threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdMappedPositive() const
{
    return thresholdMappedPositive;
}

/**
 * Set the mapped positive threshold.
 *
 * @param thresholdMappedPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdMappedPositive(const float thresholdMappedPositive)
{
    if (this->thresholdMappedPositive != thresholdMappedPositive) {
        this->thresholdMappedPositive = thresholdMappedPositive;
        this->setModified();
    }
}

/**
 * Get normal negative threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalNegative() const
{
    return thresholdNormalNegative;
}

/**
 * Set the normal negative threshold.
 *
 * @param thresholdNormalNegative - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalNegative(const float thresholdNormalNegative)
{
    if (this->thresholdNormalNegative != thresholdNormalNegative) {
        this->thresholdNormalNegative = thresholdNormalNegative;
        this->setModified();
    }
}

/**
 * Get normal positive threshold
 *
 * @return the threshold's value.
 *
 */
float
PaletteColorMapping::getThresholdNormalPositive() const
{
    return thresholdNormalPositive;
}

/**
 * Set the normal positive threshold.
 *
 * @param thresholdNormalPositive - new value.
 *
 */
void
PaletteColorMapping::setThresholdNormalPositive(const float thresholdNormalPositive)
{
    if (this->thresholdNormalPositive != thresholdNormalPositive) {
        this->thresholdNormalPositive = thresholdNormalPositive;
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
 * @param thresholdType - The threshold type.
 *
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
    this->thresholdShowFailureInGreen = showInGreenFlag;
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

