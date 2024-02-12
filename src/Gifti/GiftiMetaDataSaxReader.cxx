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

#include "CaretLogger.h"
#include "GiftiMetaData.h"
#include "GiftiMetaDataSaxReader.h"
#include "GiftiXmlElements.h"

#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
GiftiMetaDataSaxReader::GiftiMetaDataSaxReader(GiftiMetaData* metaData)
{
   this->state = STATE_NONE;
   this->stateStack.push(this->state);
   this->metaDataName = "";
   this->metaDataValue = "";
   this->elementText = "";
   this->metaData  = metaData;
}

/**
 * destructor.
 */
GiftiMetaDataSaxReader::~GiftiMetaDataSaxReader()
{
    /*
     * Should be ok to call from here (JWH)
     */
    this->metaData->afterReadingProcessing();
}


/**
 * start an element.
 */
void 
GiftiMetaDataSaxReader::startElement(const AString& /* namespaceURI */,
                                         const AString& /* localName */,
                                         const AString& qName,
                                         const XmlAttributes& /*attributes*/) 
{
   const STATE previousState = this->state;
   switch (this->state) {
      case STATE_NONE:
           if (qName == GiftiXmlElements::TAG_METADATA) {
               this->state = STATE_METADATA;
           }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName.toStdString() << "\" but should be "
                << GiftiXmlElements::TAG_METADATA.toStdString();
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_METADATA:
           if (qName == GiftiXmlElements::TAG_METADATA_ENTRY) {
            this->state = STATE_METADATA_MD;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_METADATA.toStdString() << " is \"" << qName.toStdString() 
             << "\" but should be " << GiftiXmlElements::TAG_METADATA_ENTRY.toStdString();
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_METADATA_MD:
         if (qName == GiftiXmlElements::TAG_METADATA_NAME) {
            this->state = STATE_METADATA_MD_NAME;
         }
         else if (qName == GiftiXmlElements::TAG_METADATA_VALUE) {
            this->state = STATE_METADATA_MD_VALUE;
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_METADATA_ENTRY.toStdString() << " is \"" << qName.toStdString() 
                << "\" but should be one of \n"
                << "   " << GiftiXmlElements::TAG_METADATA_NAME.toStdString() << "\n"
                << "   " << GiftiXmlElements::TAG_METADATA_VALUE.toStdString();
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_METADATA_MD_NAME:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_METADATA_NAME.toStdString() << " has child \"" << qName.toStdString()
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_METADATA_MD_VALUE:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_METADATA_VALUE.toStdString() << " has child \"" << qName.toStdString() 
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
   }
   
   //
   // Save previous state
   //
   this->stateStack.push(previousState);
   
   this->elementText = "";
}

/**
 * end an element.
 */
void 
GiftiMetaDataSaxReader::endElement(const AString& /* namspaceURI */,
                                       const AString& /* localName */,
                                       const AString& /*qName*/)
{

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_METADATA:
         break;
      case STATE_METADATA_MD:
         if ((this->metaDataName.isEmpty() == false) &&
             (this->metaDataValue.isEmpty() == false)) {
            if (this->metaData != NULL) {
               this->metaData->set(metaDataName, metaDataValue);
            }
            else {
                throw XmlSaxParserException("ERROR: Have metadata name/value but no MetaDeta.");
            }
            this->metaDataName = "";
            this->metaDataValue = "";
         }
         break;
      case STATE_METADATA_MD_NAME:
         this->metaDataName = elementText;
         break;
      case STATE_METADATA_MD_VALUE:
         this->metaDataValue = elementText;
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
       throw XmlSaxParserException("State stack is empty while reading XML MetaData.");
   }
   this->state = this->stateStack.top();
   this->stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
GiftiMetaDataSaxReader::characters(const char* ch)
{
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
GiftiMetaDataSaxReader::fatalError(const XmlSaxParserException& e)
{
   //
   // Stop parsing
   //
   CaretLogSevere("XML Parser Fatal Error: " + e.whatString());
   throw e;
}

// a warning occurs
void 
GiftiMetaDataSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
GiftiMetaDataSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
GiftiMetaDataSaxReader::startDocument() 
{    
}

void 
GiftiMetaDataSaxReader::endDocument()
{
}

