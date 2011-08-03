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

#include "GiftiMetaData.h"
#include "GiftiMetaDataSaxReader.h"
#include "GiftiXmlElements.h"
#include "StringUtilities.h"
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
}


/**
 * start an element.
 */
void 
GiftiMetaDataSaxReader::startElement(const std::string& /* namespaceURI */,
                                         const std::string& /* localName */,
                                         const std::string& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//    std::cout << "MetaData: Start Element: " << qName << std::endl;
//   }
   
   const STATE previousState = this->state;
   switch (this->state) {
      case STATE_NONE:
           if (qName == GiftiXmlElements::TAG_METADATA) {
               this->state = STATE_METADATA;
           }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName << "\" but should be "
                << GiftiXmlElements::TAG_METADATA;
             throw XmlSaxParserException(str.str());
         }
         break;
      case STATE_METADATA:
           if (qName == GiftiXmlElements::TAG_METADATA_ENTRY) {
            this->state = STATE_METADATA_MD;
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
            this->state = STATE_METADATA_MD_NAME;
         }
         else if (qName == GiftiXmlElements::TAG_METADATA_VALUE) {
            this->state = STATE_METADATA_MD_VALUE;
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
GiftiMetaDataSaxReader::endElement(const std::string& /* namspaceURI */,
                                       const std::string& /* localName */,
                                       const std::string& qName) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "MetaData: End Element: " << qName << std::endl;
//   }

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_METADATA:
         break;
      case STATE_METADATA_MD:
         if ((this->metaDataName.empty() == false) &&
             (this->metaDataValue.empty() == false)) {
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
GiftiMetaDataSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "Characters (50 max): " << s.substr(0, 50) << std::endl;
//   }
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
GiftiMetaDataSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
   //
   // Stop parsing
   //
   throw e;
}

// a warning occurs
void 
GiftiMetaDataSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    std::cout << "XML Parser Warning: " + e.whatString() << std::endl;
}

// an error occurs
void 
GiftiMetaDataSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    throw e;
}

void 
GiftiMetaDataSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
GiftiMetaDataSaxReader::endDocument() throw (XmlSaxParserException)
{
}

