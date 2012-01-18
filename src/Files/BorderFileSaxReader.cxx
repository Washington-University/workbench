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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiXmlElements.h"
#include "Border.h"
#include "BorderFile.h"
#include "BorderFileSaxReader.h"
#include "GiftiMetaDataSaxReader.h"
#include "SurfaceProjectedItem.h"

#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
BorderFileSaxReader::BorderFileSaxReader(BorderFile* borderFile)
{
   CaretAssert(borderFile);
   this->borderFile = borderFile;
   this->state = STATE_NONE;
   this->stateStack.push(this->state);
   this->elementText = "";
   this->metaDataSaxReader = NULL;
    this->border = NULL;
    this->surfaceProjectedItem = NULL;
}

/**
 * destructor.
 */
BorderFileSaxReader::~BorderFileSaxReader()
{
}


/**
 * start an element.
 */
void 
BorderFileSaxReader::startElement(const AString& namespaceURI,
                                         const AString& localName,
                                         const AString& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
   const STATE previousState = this->state;
   switch (this->state) {
      case STATE_NONE:
           if (qName == BorderFile::XML_TAG_BORDER_FILE) {
            this->state = STATE_BORDER_FILE;
            
            //
            // Check version of file being read
            //
             const float version = attributes.getValueAsFloat(BorderFile::XML_ATTRIBUTE_VERSION);
            if (version > BorderFile::getFileVersion()) {
                AString msg =
                   "File version is " 
                + AString::number(version) 
                + " but versions newer than "
                + BorderFile::getFileVersionAsString()
                + " are not supported.  Update your software.";
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            else if (version < 1.0) {
                AString msg =
                "File version is " 
                + AString::number(version) 
                + " but versions before"
                + BorderFile::getFileVersionAsString()
                + " are not supported.  Update your software.";
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
         }
         else {
             const AString msg =
             "Root elements is "
             + qName
             + " but should be "
             + BorderFile::XML_TAG_BORDER_FILE;
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_BORDER:
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               this->state = STATE_SURFACE_PROJECTED_ITEM;
               this->surfaceProjectedItem = new SurfaceProjectedItem();
           }
           else if (qName != Border::XML_TAG_NAME) {
               const AString msg =
               "Invalid child of "
               + Border::XML_TAG_BORDER
               + " is "
               + qName;
               XmlSaxParserException e(msg);
               CaretLogThrowing(e);
               throw e;
           }
           break;
      case STATE_BORDER_FILE:
         if (qName == GiftiXmlElements::TAG_METADATA) {
             this->state = STATE_METADATA;
             this->metaDataSaxReader = new GiftiMetaDataSaxReader(this->borderFile->getFileMetaData());
             this->metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         }
         else if (qName == Border::XML_TAG_BORDER) {
             this->state = STATE_BORDER;
             this->border = new Border();
         }
         else {
             const AString msg =
             "Invalid child of "
             + BorderFile::XML_TAG_BORDER_FILE
             + " is "
             + qName;
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_METADATA:
           this->metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         break;
      case STATE_SURFACE_PROJECTED_ITEM:
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
BorderFileSaxReader::endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qName) throw (XmlSaxParserException)
{
   switch (this->state) {
      case STATE_NONE:
         break;
       case STATE_BORDER:
           CaretAssert(this->border);
           if (qName == Border::XML_TAG_NAME) {
               this->border->setName(this->elementText.trimmed());
           }
           else if (qName == Border::XML_TAG_BORDER) {
               this->borderFile->addBorder(this->border);
               this->border = NULL;
           }
           else {
           }
           break;
      case STATE_BORDER_FILE:
         break;
      case STATE_METADATA:
           CaretAssert(this->metaDataSaxReader);
           this->metaDataSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_METADATA) {
               delete this->metaDataSaxReader;
               this->metaDataSaxReader = NULL;
           }
         break;
      case STATE_SURFACE_PROJECTED_ITEM:
           CaretAssert(this->surfaceProjectedItem);
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               this->border->addPoint(this->surfaceProjectedItem);
               this->surfaceProjectedItem = NULL; 
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
 * get characters in an element.
 */
void 
BorderFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
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
BorderFileSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
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
BorderFileSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
BorderFileSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
BorderFileSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
BorderFileSaxReader::endDocument() throw (XmlSaxParserException)
{
}

