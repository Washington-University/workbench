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

#include <sstream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "PaletteColorMapping.h"
#include "PaletteColorMappingSaxReader.h"
#include "PaletteColorMappingXmlElements.h"
#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
PaletteColorMappingSaxReader::PaletteColorMappingSaxReader(PaletteColorMapping* paletteColorMapping)
{
    CaretAssert(paletteColorMapping);
    this->state = STATE_NONE;
    this->stateStack.push(state);
    this->elementText = "";
    this->paletteColorMapping = paletteColorMapping;
}

/**
 * destructor.
 */
PaletteColorMappingSaxReader::~PaletteColorMappingSaxReader()
{
}


/**
 * start an element.
 */
void 
PaletteColorMappingSaxReader::startElement(const AString& /* namespaceURI */,
                                         const AString& /* localName */,
                                         const AString& qName,
                                         const XmlAttributes& attributes) 
{
   const STATE previousState = this->state;

    switch (this->state) {
      case STATE_NONE:
            if (qName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING) {
               this->state = STATE_READING_ELEMENTS;
                
                int32_t version = attributes.getValueAsInt(PaletteColorMappingXmlElements::XML_ATTRIBUTE_VERSION_NUMBER);
                if (version > PaletteColorMappingXmlElements::XML_VERSION_NUMBER) {
                    std::ostringstream str;
                    str
                    << "Version of PaletteColorMapping ("
                    << version
                    << ") is greater than version(s) supported ("
                    << PaletteColorMappingXmlElements::XML_VERSION_NUMBER
                    << ").";
                    throw XmlSaxParserException(AString::fromStdString(str.str()));
                }
           }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName.toStdString() << "\" but should be "
                << PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING.toStdString();
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_READING_ELEMENTS:
            break;
   }

   //
   // Save previous state
   //
   this->stateStack.push(previousState);
   
   this->elementText = "";
}
/**
 * Convert the string representation of a bool to a bool.
 * @param s
 *   String containing boolean value.
 * @return
 *   The bool value.
 */
bool 
toBool(const AString& s)
{
    if ((s == "true") 
        || (s == "TRUE") 
        || (s == "True")
        || (s == "T")
        || (s == "t")
        || (s == "1")) {
        return true;
    }
    return false;
}

/**
 * Split up a string containing float values.
 * 
 * @param s
 *   String containing float values.
 * @return
 *   float vector containing values extracted from string.
 */
std::vector<float> 
toFloatVector(const AString& s)
{
    std::vector<float> fv;
    
    std::istringstream str(s.toStdString());
    while ((str.eof() == false) && (str.fail() == false)) {
        float value;
        str >> value;
        fv.push_back(value);
    }
    
    return fv;
}

/**
 * end an element.
 */
void 
PaletteColorMappingSaxReader::endElement(const AString& /* namspaceURI */,
                                       const AString& /* localName */,
                                       const AString& qName)
{
   switch (state) {
      case STATE_NONE:
         break;
      case STATE_READING_ELEMENTS:
           if (qName == PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_PERCENTAGE_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 4) {
                   this->paletteColorMapping->setAutoScalePercentageNegativeMaximum(values[0]);
                   this->paletteColorMapping->setAutoScalePercentageNegativeMinimum(values[1]);
                   this->paletteColorMapping->setAutoScalePercentagePositiveMinimum(values[2]);
                   this->paletteColorMapping->setAutoScalePercentagePositiveMaximum(values[3]);
               }
               else {
                   throw XmlSaxParserException("PaletteColorMappingXmlElements::auto scale percenter does not contain four values.");
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_AUTO_SCALE_ABSOLUTE_PERCENTAGE_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 2) {
                   this->paletteColorMapping->setAutoScaleAbsolutePercentageMinimum(values[0]);
                   this->paletteColorMapping->setAutoScaleAbsolutePercentageMaximum(values[1]);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_DISPLAY_NEGATIVE) {
               this->paletteColorMapping->setDisplayNegativeDataFlag(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_DISPLAY_POSITIVE) {
               this->paletteColorMapping->setDisplayPositiveDataFlag(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_DISPLAY_ZERO) {
               this->paletteColorMapping->setDisplayZeroDataFlag(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_INTERPOLATE) {
               this->paletteColorMapping->setInterpolatePaletteFlag(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_NAME) {
               this->paletteColorMapping->setSelectedPaletteName(this->elementText);
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_DATA_NAME) {
               this->paletteColorMapping->setThresholdDataName(this->elementText);
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_FAILURE_IN_GREEN) {
               /* ??? */
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_AVG_AREA_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 2) {
                   this->paletteColorMapping->setThresholdMappedAverageAreaMinimum(values[0]);
                   this->paletteColorMapping->setThresholdMappedAverageAreaMaximum(values[1]);
               }
               else {
                   throw XmlSaxParserException("PaletteColorMappingXmlElements::threshild mapped average area does not contain two values.");
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_MAPPED_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 2) {
                   this->paletteColorMapping->setThresholdMappedMinimum(values[0]);
                   this->paletteColorMapping->setThresholdMappedMaximum(values[1]);
               }
               else {
                   throw XmlSaxParserException("PaletteColorMappingXmlElements::threshild mapped does not contain two values.");
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NORMAL_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 2) {
                   this->paletteColorMapping->setThresholdNormalMinimum(values[0]);
                   this->paletteColorMapping->setThresholdNormalMaximum(values[1]);
               }
               else {
                   throw XmlSaxParserException("PaletteColorMappingXmlElements::threshild mapped normal does not contain two values.");
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TEST) {
               bool isValid = false;
               PaletteThresholdTestEnum::Enum thresoldTest = 
               PaletteThresholdTestEnum::fromName(this->elementText,
                                              &isValid);
               if (isValid) {
                   this->paletteColorMapping->setThresholdTest(thresoldTest);               
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteColorMapping::thresoldTest " 
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_TYPE) {
               bool isValid = false;
               PaletteThresholdTypeEnum::Enum thresholdType = 
               PaletteThresholdTypeEnum::fromName(this->elementText,
                                              &isValid);
               if (isValid) {
                   this->paletteColorMapping->setThresholdType(thresholdType);               
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteColorMapping::thresholdType: " 
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_SCALE_MODE) {
               bool isValid = false;
               PaletteScaleModeEnum::Enum scaleMode = 
                   PaletteScaleModeEnum::fromName(this->elementText,
                                              &isValid);
               if (isValid) {
                   this->paletteColorMapping->setScaleMode(scaleMode);               
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteColorMapping::scaleMode: " 
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_USER_SCALE_VALUES) {
               std::vector<float> values = toFloatVector(this->elementText);
               if (values.size() >= 4) {
                   this->paletteColorMapping->setUserScaleNegativeMaximum(values[0]);
                   this->paletteColorMapping->setUserScaleNegativeMinimum(values[1]);
                   this->paletteColorMapping->setUserScalePositiveMinimum(values[2]);
                   this->paletteColorMapping->setUserScalePositiveMaximum(values[3]);
               }
               else {
                   throw XmlSaxParserException("PaletteColorMappingXmlElements::auto scale percenter does not contain four values.");
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_RANGE_MODE) {
               bool isValid = false;
               PaletteThresholdRangeModeEnum::Enum rangeMode = PaletteThresholdRangeModeEnum::fromName(this->elementText,
                                                       &isValid);
               if (isValid) {
                   this->paletteColorMapping->setThresholdRangeMode(rangeMode);
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteThresholdRangeModeEnum::Enum: "
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_THRESHOLD_NEG_MIN_POS_MAX_LINKED) {
               this->paletteColorMapping->setThresholdNegMinPosMaxLinked(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_PALETTE_COLOR_MAPPING) {
               /* WB-  
                * We want to default the range mode to 'FILE'.  While we do that in the constructor
                * for PaletteColorMapping, most files contain PaletteColorMapping in the XML for 
                * each map and when the file is read, the default values are replaced.  So,
                * to avoid breaking older scenes, only change the range mode from 'MAP' to 'FILE'
                * if thresholding is off when reading palette color mapping from XML.
                */
               if (this->paletteColorMapping->getThresholdType()  == PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF) {
                   this->paletteColorMapping->setThresholdRangeMode(PaletteThresholdRangeModeEnum::PALETTE_THRESHOLD_RANGE_MODE_FILE);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_RANGE_MODE) {
               bool isValid = false;
               PaletteHistogramRangeModeEnum::Enum histogramRangeMode = PaletteHistogramRangeModeEnum::fromName(this->elementText,
                                                                                               &isValid);
               if (isValid) {
                   this->paletteColorMapping->setHistogramRangeMode(histogramRangeMode);
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteHistogramRangeModeEnum::Enum: "
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_BARS_VISIBLE) {
               this->paletteColorMapping->setHistogramBarsVisible(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_ENVELOPE_VISIBLE) {
               this->paletteColorMapping->setHistogramEnvelopeVisible(toBool(this->elementText));
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_COLOR) {
               bool isValid = false;
               CaretColorEnum::Enum histogramColor = CaretColorEnum::fromName(this->elementText,
                                                                                  &isValid);
               if (isValid) {
                   this->paletteColorMapping->setHistogramColor(histogramColor);
               }
               else {
                   throw XmlSaxParserException("Invalid CaretColorEnum::Enum: "
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_HISTOGRAM_NUMBER_OF_BUCKETS) {
               this->paletteColorMapping->setHistogramNumberOfBuckets(this->elementText.toInt());
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_NUMERIC_FORMAT_MODE) {
               bool isValid = false;
               NumericFormatModeEnum::Enum numericFormatMode = NumericFormatModeEnum::fromName(this->elementText,
                                                                                                       &isValid);
               if (isValid) {
                   this->paletteColorMapping->setColorBarNumericFormatMode(numericFormatMode);
               }
               else {
                   throw XmlSaxParserException("Invalid NumericFormatModeEnum::Enum: "
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_PRECISION_DIGITS) {
               this->paletteColorMapping->setColorBarPrecisionDigits(this->elementText.toInt());
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_NUMERIC_SUBDIVISIONS) {
               this->paletteColorMapping->setColorBarNumericSubdivisionCount(this->elementText.toInt());
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_COLOR_BAR_VALUES_MODE) {
               bool isValid = false;
               PaletteColorBarValuesModeEnum::Enum colorBarMode = PaletteColorBarValuesModeEnum::fromName(this->elementText,
                                                                                               &isValid);
               if (isValid) {
                   this->paletteColorMapping->setColorBarValuesMode(colorBarMode);
               }
               else {
                   throw XmlSaxParserException("Invalid PaletteColorBarValuesModeEnum::Enum: "
                                               + this->elementText);
               }
           }
           else if (qName == PaletteColorMappingXmlElements::XML_TAG_SHOW_TICK_MARKS) {
               this->paletteColorMapping->setColorBarShowTickMarksSelected(toBool(this->elementText));
           }
           else {
               std::ostringstream str;
               str
               << "Unrecognized (perhaps new) palette color mapping element ignored \""
               << qName.toStdString()
               << "\" with content: "
               << this->elementText.toStdString();
               warning(XmlSaxParserException(AString::fromStdString(str.str())));
           }
         break;
   }

   //
   // Clear out for new elements
   //
   this->elementText = "";
   
   //
   // Go to previous state
   //
   if (this->stateStack.empty()) {
       throw XmlSaxParserException("State stack is empty while reading PaletteColorMapping.");
   }
   this->state = stateStack.top();
   this->stateStack.pop();
}


/**
 * get characters in an element.
 */
void 
PaletteColorMappingSaxReader::characters(const char* ch)
{
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
PaletteColorMappingSaxReader::fatalError(const XmlSaxParserException& e)
{
   throw e;
}

/**
 * A warning occurs
 */
void 
PaletteColorMappingSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
PaletteColorMappingSaxReader::error(const XmlSaxParserException& e)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
PaletteColorMappingSaxReader::startDocument() 
{    
}

void 
PaletteColorMappingSaxReader::endDocument()
{
}

