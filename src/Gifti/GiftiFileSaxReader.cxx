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
#include "GiftiLabelTableSaxReader.h"
#include "GiftiMetaDataSaxReader.h"
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
   this->giftiFile = giftiFileIn;
   this->state = STATE_NONE;
   this->stateStack.push(this->state);
   this->elementText = "";
   this->dataArray = NULL;
   this->labelTable = NULL;
    this->labelTableSaxReader = NULL;
    this->metaDataSaxReader = NULL;
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
GiftiFileSaxReader::startElement(const std::string& namespaceURI,
                                         const std::string& localName,
                                         const std::string& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//    std::cout << "Start Element: " << qName << std::endl;
//   }
   
   const STATE previousState = this->state;
   switch (this->state) {
      case STATE_NONE:
         if (qName == GiftiXmlElements::TAG_GIFTI) {
            this->state = STATE_GIFTI;
            
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
             this->state = STATE_METADATA;
             this->metaDataSaxReader = new GiftiMetaDataSaxReader(giftiFile->getMetaData());
             this->metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         }
         else if (qName == GiftiXmlElements::TAG_DATA_ARRAY) {
            this->state = STATE_DATA_ARRAY;
             this->createDataArray(attributes);         }
         else if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
            this->state = STATE_LABEL_TABLE;
             this->labelTableSaxReader = new GiftiLabelTableSaxReader(giftiFile->getLabelTable());
             this->labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
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
           this->metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         break;
      case STATE_LABEL_TABLE:
           this->labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
      case STATE_DATA_ARRAY:
         if (qName == GiftiXmlElements::TAG_METADATA) {
             this->state = STATE_METADATA;
             this->metaDataSaxReader = new GiftiMetaDataSaxReader(dataArray->getMetaData());
             this->metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         }
         else if (qName == GiftiXmlElements::TAG_DATA) {
            this->state = STATE_DATA_ARRAY_DATA;
         }
         else if (qName == GiftiXmlElements::TAG_COORDINATE_TRANSFORMATION_MATRIX) {
            this->state = STATE_DATA_ARRAY_MATRIX;
            this->dataArray->addMatrix(Matrix4x4());
            this->matrix = dataArray->getMatrix(dataArray->getNumberOfMatrices() - 1);
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
            this->state = STATE_DATA_ARRAY_MATRIX_DATA_SPACE;
         }
         else if (qName == GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE) {
            this->state = STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE;
         }
         else if (qName == GiftiXmlElements::TAG_MATRIX_DATA) {
            this->state = STATE_DATA_ARRAY_MATRIX_DATA;
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
GiftiFileSaxReader::endElement(const std::string& namespaceURI,
                                       const std::string& localName,
                                       const std::string& qName) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "End Element: " << qName << std::endl;
//   }

   switch (this->state) {
      case STATE_NONE:
         break;
      case STATE_GIFTI:
         break;
      case STATE_METADATA:
           this->metaDataSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_METADATA) {
               delete this->metaDataSaxReader;
               this->metaDataSaxReader = NULL;
           }
         break;
      case STATE_LABEL_TABLE:
           this->labelTableSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               delete this->labelTableSaxReader;
               this->labelTableSaxReader = NULL;
           }
         break;
      case STATE_DATA_ARRAY:
         if (this->dataArray != NULL) {
            this->giftiFile->addDataArray(this->dataArray);
            this->dataArray = NULL;
         }
         else {
         }
         break;
      case STATE_DATA_ARRAY_DATA:
           this->processArrayData();
           break;
      case STATE_DATA_ARRAY_MATRIX:
         this->matrix = NULL;
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         this->matrix->setDataSpaceName(elementText);
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         this->matrix->setTransformedSpaceName(elementText);
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
   this->elementText = "";
   
   //
   // Go to previous state
   //
   if (this->stateStack.empty()) {
       throw XmlSaxParserException("State stack is empty while reading XML NiftDataFile.");
   }
   this->state = stateStack.top();
   this->stateStack.pop();
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
   this->dataTypeForReadingArrayData = NiftiDataTypeEnum::fromName(dataTypeName,
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
   this->externalFileNameForReadingData = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_NAME);
   
   //
   // External File Offset
   //
   this->externalFileOffsetForReadingData = 0;
   const std::string offsetString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_EXTERNAL_FILE_OFFSET);
   if (offsetString.empty() == false) {
      bool validOffsetFlag = false;
       this->externalFileOffsetForReadingData = StringUtilities::toLong(offsetString);
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
    this->dimensionsForReadingArrayData.clear();
    const int numDimensions = StringUtilities::toInt(dimString);
   for (int i = 0; i < numDimensions; i++) {
      const std::string dimNumString = attributes.getValue(GiftiXmlElements::getAttributeDimension(i));
      if (dimNumString.empty()) {
         throw XmlSaxParserException("Required dimension "
                        + GiftiXmlElements::getAttributeDimension(i)
                        + " not found for DataArray");
      }
      
       const int dim = StringUtilities::toInt(dimNumString);
      this->dimensionsForReadingArrayData.push_back(dim);
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
    this->arraySubscriptingOrderForReadingArrayData = GiftiArrayIndexingOrderEnum::fromGiftiName(
                                                     subscriptOrderString,
                                                     &validArraySubscriptingOrder);   
   if (validArraySubscriptingOrder == false) {
      throw XmlSaxParserException("Attribute "
                     + GiftiXmlElements::ATTRIBUTE_DATA_ARRAY_INDEXING_ORDER
                     + "is invalid: "
                     + subscriptOrderString);
   }
         
   this->dataArray = new GiftiDataArray(this->giftiFile,
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
   
    if (this->metaDataSaxReader != NULL) {
        this->metaDataSaxReader->characters(ch);
    }
    else if (this->labelTableSaxReader != NULL) {
        this->labelTableSaxReader->characters(ch);
    }
    else {
        elementText += ch;
    }
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

