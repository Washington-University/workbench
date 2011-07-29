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
/*LICENSE_END*/

#include <iostream>
#include <sstream>

#include "GiftiEndianEnum.h"
#include "GiftiLabel.h"
#include "GiftiFile.h"
#include "GiftiFileSaxReader.h"
#include "GiftiXmlElements.h"
#include "StringUtilities.h"
#include "NiftiIntentEnum.h"
#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
GiftiFileSaxReader::GiftiFileSaxReader(GiftiFile* giftiFileIn)
{
   giftiFile = giftiFileIn;
   state = STATE_NONE;
   stateStack.push(state);
   metaDataName = "";
   metaDataValue = "";
   elementText = "";
   dataArray = NULL;
   labelTable = NULL;
   metaData  = NULL;
}

/**
 * destructor.
 */
GiftiFileSaxReader::~GiftiFileSaxReader()
{
}


/**
 * start an element.
 */
void 
GiftiFileSaxReader::startElement(const std::string& /* namespaceURI */,
                                         const std::string& /* localName */,
                                         const std::string& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//    std::cout << "Start Element: " << qName << std::endl;
//   }
   
   const STATE previousState = state;
   switch (state) {
      case STATE_NONE:
         if (qName == GiftiXmlElements::TAG_GIFTI) {
            state = STATE_GIFTI;
            
            //
            // Check version of file being read
            //
             const float version = attributes.getValueAsFloat(GiftiXmlElements::ATTRIBUTE_GIFTI_VERSION);
            if (version > GiftiFile::getCurrentFileVersion()) {
               std::ostringstream str;
               str << "File version is " << version << " but this Caret"
                   << " does not support versions newer than "
                   << GiftiFile::getCurrentFileVersion() << ".\n"
                   << "You may need a newer version of Caret.";
               throw XmlSaxParserException(str.str());
            }
            else if (version < 1.0) {
                throw XmlSaxParserException(
                    "File version is " + StringUtilities::fromNumber(version) + " but this Caret"
                    " does not support versions before 1.0");
            }
         }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName << "\" but should be "
                << GiftiXmlElements::TAG_GIFTI;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_GIFTI:
         if (qName == GiftiXmlElements::TAG_METADATA) {
            state = STATE_METADATA;
            metaData = giftiFile->getMetaData();
         }
         else if (qName == GiftiXmlElements::TAG_DATA_ARRAY) {
            state = STATE_DATA_ARRAY;
             createDataArray(attributes);         }
         else if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
            state = STATE_LABEL_TABLE;
            labelTable = giftiFile->getLabelTable();
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_GIFTI << " is \"" << qName 
                << "\" but should be one of \n"
                << "   " << GiftiXmlElements::TAG_METADATA << "\n"
                << "   " << GiftiXmlElements::TAG_DATA_ARRAY << "\n"
             << "   " << GiftiXmlElements::TAG_LABEL_TABLE;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_METADATA:
           if (qName == GiftiXmlElements::TAG_METADATA_ENTRY) {
            state = STATE_METADATA_MD;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_METADATA << " is \"" << qName 
             << "\" but should be " << GiftiXmlElements::TAG_METADATA_ENTRY;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_METADATA_MD:
         if (qName == GiftiXmlElements::TAG_METADATA_NAME) {
            state = STATE_METADATA_MD_NAME;
         }
         else if (qName == GiftiXmlElements::TAG_METADATA_VALUE) {
            state = STATE_METADATA_MD_VALUE;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_METADATA_ENTRY << " is \"" << qName 
                << "\" but should be one of \n"
                << "   " << GiftiXmlElements::TAG_METADATA_NAME << "\n"
                << "   " << GiftiXmlElements::TAG_METADATA_VALUE;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_METADATA_MD_NAME:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_METADATA_NAME << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_METADATA_MD_VALUE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_METADATA_VALUE << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_LABEL_TABLE:
         if (qName == GiftiXmlElements::TAG_LABEL) {
            state = STATE_LABEL_TABLE_LABEL;
            std::string s = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_KEY);
            if (s == "") {
               s = attributes.getValue("Index");
            }
            else {
               s = attributes.getValue("Index");
            }
            if (s.empty()) {
               std::ostringstream str;
               str << "Tag "
                   << GiftiXmlElements::TAG_LABEL
                   << " is missing its "
                   << GiftiXmlElements::ATTRIBUTE_LABEL_KEY;
                throw XmlSaxParserException(str.str());
            }
             labelIndex = StringUtilities::toInt(s);

            {
                float* defaultRGBA = GiftiLabel::getDefaultColor();
                labelRed = defaultRGBA[0];
                labelGreen = defaultRGBA[1];
                labelBlue = defaultRGBA[2];
                labelAlpha = defaultRGBA[3];
               const std::string redString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_RED);
               if (redString.empty() == false) {
                  labelRed = StringUtilities::toFloat(redString);
               }
               const std::string greenString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN);
               if (greenString.empty() == false) {
                  labelGreen = StringUtilities::toFloat(greenString);
               }
               const std::string blueString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE);
               if (blueString.empty() == false) {
                  labelBlue = StringUtilities::toFloat(blueString);
               }
               const std::string alphaString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA);
               if (alphaString.empty() == false) {
                  labelAlpha = StringUtilities::toFloat(alphaString);
               }
            }
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_LABEL_TABLE << " is \"" << qName 
                << "\" but should be " << GiftiXmlElements::TAG_LABEL;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_LABEL_TABLE_LABEL:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_LABEL << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY:
         if (qName == GiftiXmlElements::TAG_METADATA) {
            state = STATE_METADATA;
            metaData = dataArray->getMetaData();
         }
         else if (qName == GiftiXmlElements::TAG_DATA) {
            state = STATE_DATA_ARRAY_DATA;
         }
         else if (qName == GiftiXmlElements::TAG_COORDINATE_TRANSFORMATION_MATRIX) {
            state = STATE_DATA_ARRAY_MATRIX;
            dataArray->addMatrix(Matrix4x4());
            matrix = dataArray->getMatrix(dataArray->getNumberOfMatrices() - 1);
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_DATA_ARRAY << " is \"" << qName 
                << "\" but should be one of \n"
                << "   " << GiftiXmlElements::TAG_METADATA << "\n"
                << "   " << GiftiXmlElements::TAG_COORDINATE_TRANSFORMATION_MATRIX << "\n"
                << "   " << GiftiXmlElements::TAG_DATA;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY_DATA:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_DATA << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY_MATRIX:
         if (qName == GiftiXmlElements::TAG_MATRIX_DATA_SPACE) {
            state = STATE_DATA_ARRAY_MATRIX_DATA_SPACE;
         }
         else if (qName == GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE) {
            state = STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE;
         }
         else if (qName == GiftiXmlElements::TAG_MATRIX_DATA) {
            state = STATE_DATA_ARRAY_MATRIX_DATA;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_COORDINATE_TRANSFORMATION_MATRIX << " is \"" << qName 
                << "\" but should be one of \n"
                << "   " << GiftiXmlElements::TAG_MATRIX_DATA_SPACE << "\n"
                << "   " << GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE << "\n"
                << "   " << GiftiXmlElements::TAG_MATRIX_DATA;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_DATA_SPACE << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_DATA << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(str.str());
         }
         break;
   }
   
   //
   // Save previous state
   //
   stateStack.push(previousState);
   
   elementText = "";
}

/**
 * end an element.
 */
void 
GiftiFileSaxReader::endElement(const std::string& /* namspaceURI */,
                                       const std::string& /* localName */,
                                       const std::string& qName) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "End Element: " << qName << std::endl;
//   }

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_GIFTI:
         break;
      case STATE_METADATA:
         metaData = NULL;
         break;
      case STATE_METADATA_MD:
         if ((metaDataName.empty() == false) &&
             (metaDataValue.empty() == false)) {
            if (metaData != NULL) {
               metaData->set(metaDataName, metaDataValue);
            }
            else {
                throw XmlSaxParserException("ERROR: Have metadata name/value but no MetaDeta.");
            }
            metaDataName = "";
            metaDataValue = "";
         }
         break;
      case STATE_METADATA_MD_NAME:
         metaDataName = elementText;
         break;
      case STATE_METADATA_MD_VALUE:
         metaDataValue = elementText;
         break;
      case STATE_LABEL_TABLE:
         labelTable = NULL;
         break;
      case STATE_LABEL_TABLE_LABEL:
           labelTable->setLabel(labelIndex, elementText, labelRed, labelGreen, labelBlue, labelAlpha);
         break;
      case STATE_DATA_ARRAY:
         if (dataArray != NULL) {
            giftiFile->addDataArray(dataArray);
            dataArray = NULL;
         }
         else {
         }
         break;
      case STATE_DATA_ARRAY_DATA:
           this->processArrayData();
           break;
      case STATE_DATA_ARRAY_MATRIX:
         matrix = NULL;
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         matrix->setDataSpaceName(elementText);
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         matrix->setTransformedSpaceName(elementText);
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA:
         {
             std::istringstream istr(elementText);
             double m[4][4];
             for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                   istr >> m[i][j];
                }
             }
             matrix->setMatrix(m);
         }
         break;
   }

   //
   // Clear out for new elements
   //
   elementText = "";
   
   //
   // Go to previous state
   //
   if (stateStack.empty()) {
       throw XmlSaxParserException("State stack is empty while reading XML NiftDataFile.");
   }
   state = stateStack.top();
   stateStack.pop();
}

/**
 * create a data array.
 */
void 
GiftiFileSaxReader::createDataArray(const XmlAttributes& attributes) throw (XmlSaxParserException)

{
   //
   // Intent
   //
   std::string intentName = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INTENT);
   if (intentName.empty()) {
      intentName = attributes.getValue("Intent");
   }
   if (intentName.empty()) {
       throw XmlSaxParserException(
                     "Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INTENT
                     + " not found for DataArray"); 
   }
    bool intentValid = false;
    NiftiIntentEnum::Enum intent = NiftiIntentEnum::fromName(intentName, &intentValid);
   if (intentValid == false) {
      throw XmlSaxParserException("Intent name invalid: "
                     + intentName);
   }
   
   //
   // Data type name
   //
   const std::string dataTypeName = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DATA_TYPE);
   if (dataTypeName.empty()) {
      throw XmlSaxParserException("Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DATA_TYPE
                     + " not found for DataArray"); 
   }
   bool dataTypeNameValid = false;
   dataTypeForReadingArrayData = NiftiDataTypeEnum::fromName(dataTypeName,
                                                                     &dataTypeNameValid);
   if (dataTypeNameValid == false) {
      throw XmlSaxParserException("Attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DATA_TYPE
                     + "is invalid: "
                     + dataTypeName);
   }
      
   //
   // Encoding
   //
   const std::string encodingName = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENCODING);
   if (encodingName.empty()) {
       throw XmlSaxParserException("Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENCODING
                     + " not found for DataArray"); 
   }
   bool validEncoding = false;
    encodingForReadingArrayData = GiftiEncodingEnum::fromGiftiName(encodingName,
                                                                     &validEncoding);
   if (validEncoding == false) {
      throw XmlSaxParserException("Attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENCODING
                     + "is invalid: "
                     + encodingName);
   }
    
   //
   // External File Name
   //
   externalFileNameForReadingData = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME);
   
   //
   // External File Offset
   //
   externalFileOffsetForReadingData = 0;
   const std::string offsetString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET);
   if (offsetString.empty() == false) {
      bool validOffsetFlag = false;
       externalFileOffsetForReadingData = StringUtilities::toLong(offsetString);
      if (validOffsetFlag == false) {
         throw XmlSaxParserException("File Offset is not an integer ("
                             + offsetString
                             + ")");
      }
   }
   
   //
   // Endian
   //
    std::string endianAttributeNameForReadingArrayData = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENDIAN);
   if (endianAttributeNameForReadingArrayData.empty()) {
      throw XmlSaxParserException("Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENDIAN
                     + " not found for DataArray"); 
   }
    bool endianValid = false;
    this->endianForReadingArrayData = GiftiEndianEnum::fromGiftiName(endianAttributeNameForReadingArrayData, &endianValid);
    if (endianValid == false) {
      throw XmlSaxParserException("Attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_ENDIAN
                     + "is invalid: "
                     + endianAttributeNameForReadingArrayData);
   }
   
   //
   // Dimensions
   // 
   const std::string dimString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY);
   if (dimString.empty()) {
      throw XmlSaxParserException("Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_DIMENSIONALITY
                     + " not found for DataArray"); 
   }
    dimensionsForReadingArrayData.clear();
    const int numDimensions = StringUtilities::toInt(dimString);
   for (int i = 0; i < numDimensions; i++) {
      const std::string dimNumString = attributes.getValue(GiftiXmlElements::getAttributeDimension(i));
      if (dimNumString.empty()) {
         throw XmlSaxParserException("Required dimension "
                        + GiftiXmlElements::getAttributeDimension(i)
                        + " not found for DataArray");
      }
      
       const int dim = StringUtilities::toInt(dimNumString);
      dimensionsForReadingArrayData.push_back(dim);
   }
   
   //
   // Data Location
   //
/*
   const std::string dataLocationString = attributes.getValue(GiftiXmlElements::attDataLocation);
   if (dataLocationString.empty()) {
      errorMessage = "Required attribute "
                     + GiftiXmlElements::attDataLocation
                     + " not found for DataArray"; 
      return false;
   }
   bool validDataLocation = false;
   dataLocationForReadingArrayData = GiftiDataArray::getDataLocationFromName(dataLocationString,
                                                                             &validDataLocation);
   if (validDataLocation == false) {
      errorMessage = "Attribute "
                     + GiftiXmlElements::attDataLocation
                     + "is invalid: "
                     + dataLocationString;
      return false;
   }
   if (dataLocationForReadingArrayData == GiftiDataArray::DATA_LOCATION_EXTERNAL) {
      errorMessage = "External data storage not supported.";
      return false;
   }
*/
   //
   // Subscript order
   //
   const std::string subscriptOrderString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER);
   if (subscriptOrderString.empty()) {
      throw XmlSaxParserException("Required attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER
                     + " not found for DataArray"); 
   }
   bool validArraySubscriptingOrder = false;
    arraySubscriptingOrderForReadingArrayData = GiftiArrayIndexingOrderEnum::fromGiftiName(
                                                     subscriptOrderString,
                                                     &validArraySubscriptingOrder);   
   if (validArraySubscriptingOrder == false) {
      throw XmlSaxParserException("Attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER
                     + "is invalid: "
                     + subscriptOrderString);
   }
         
   dataArray = new GiftiDataArray(giftiFile,
                                  intent);
}

/**
 * process the array data into numbers.
 */
void 
GiftiFileSaxReader::processArrayData() throw (XmlSaxParserException)
{
   //
   // Should the data arrays be read ?
   //
   //if (giftiFile->getReadMetaDataOnlyFlag()) {
   //   return true;
   //}
   
   try {
      dataArray->readFromText(elementText, 
                           this->endianForReadingArrayData,
                           arraySubscriptingOrderForReadingArrayData,
                           dataTypeForReadingArrayData,
                           dimensionsForReadingArrayData,
                           encodingForReadingArrayData,
                           externalFileNameForReadingData,
                           externalFileOffsetForReadingData);
   }
   catch (GiftiException& e) {
       throw XmlSaxParserException(e.whatString());
   }
}

/**
 * get characters in an element.
 */
void 
GiftiFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "Characters (50 max): " << s.substr(0, 50) << std::endl;
//   }
   
   elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
GiftiFileSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
/*
   std::ostringstream str;
   str << "Fatal Error at line number: " << e.getLineNumber() << "\n"
       << "Column number: " << e.getColumnNumber() << "\n"
       << "Message: " << e.whatString();
   if (errorMessage.empty() == false) {
      str << "\n"
          << errorMessage;
   }
   errorMessage = str.str();
*/   
   //
   // Stop parsing
   //
   throw e;
}

// a warning occurs
void 
GiftiFileSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    std::cout << "XML Parser Warning: " + e.whatString() << std::endl;
}

// an error occurs
void 
GiftiFileSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    throw e;
}

void 
GiftiFileSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
GiftiFileSaxReader::endDocument() throw (XmlSaxParserException)
{
}

