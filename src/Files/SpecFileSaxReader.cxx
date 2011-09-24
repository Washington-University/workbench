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

#include <sstream>

#include "CaretLogger.h"
#include "SpecFile.h"
#include "SpecFileSaxReader.h"
#include "GiftiMetaDataSaxReader.h"

#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
SpecFileSaxReader::SpecFileSaxReader(SpecFile* specFileIn)
{
   this->specFile = specFileIn;
   this->state = STATE_NONE;
   this->stateStack.push(this->state);
   this->elementText = "";
   this->specFileGroup = NULL;
   this->metaDataSaxReader = NULL;
}

/**
 * destructor.
 */
SpecFileSaxReader::~SpecFileSaxReader()
{
}


/**
 * start an element.
 */
void 
SpecFileSaxReader::startElement(const AString& namespaceURI,
                                         const AString& localName,
                                         const AString& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
/*
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
               throw XmlSaxParserException(AString::fromStdString(str.str()));
            }
            else if (version < 1.0) {
                throw XmlSaxParserException(
                    "File version is " + AString::number(version) + " but this Caret"
                    " does not support versions before 1.0");
            }
         }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName << "\" but should be "
                << GiftiXmlElements::TAG_GIFTI;
             throw XmlSaxParserException(AString::fromStdString(str.str()));
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
             throw XmlSaxParserException(AString::fromStdString(str.str()));
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
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_DATA_ARRAY_DATA:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_DATA << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
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
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA_SPACE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_DATA_SPACE << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_TRANSFORMED_SPACE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_TRANSFORMED_SPACE << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_DATA_ARRAY_MATRIX_DATA:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_MATRIX_DATA << " has child \"" << qName 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
   }
   
   //
   // Save previous state
   //
   stateStack.push(previousState);
   
   elementText = "";
*/
}

/**
 * end an element.
 */
void 
SpecFileSaxReader::endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qName) throw (XmlSaxParserException)
{
/*
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
             std::istringstream istr(elementText.toStdString());
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
*/
}

/**
 * get characters in an element.
 */
void 
SpecFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
    if (this->metaDataSaxReader != NULL) {
        this->metaDataSaxReader->characters(ch);
    }
    else {
        elementText += ch;
    }
}

/**
 * a fatal error occurs.
 */
void 
SpecFileSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
/*
   std::ostringstream str;
   str << "Fatal Error at line number: " << e.getLineNumber() << "\n"
       << "Column number: " << e.getColumnNumber() << "\n"
       << "Message: " << e.whatString();
   if (errorMessage.isEmpty() == false) {
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
SpecFileSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SpecFileSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
SpecFileSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
SpecFileSaxReader::endDocument() throw (XmlSaxParserException)
{
}

