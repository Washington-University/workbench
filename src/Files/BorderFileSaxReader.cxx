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
#include "GiftiLabelTable.h"
#include "GiftiLabelTableSaxReader.h"
#include "GiftiMetaDataSaxReader.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectedItemSaxReader.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * \class caret::BorderFileSaxReader
 * \brief Reads a border file using a SAX XML Parser.
 */

/**
 * constructor.
 */
BorderFileSaxReader::BorderFileSaxReader(BorderFile* borderFile)
{
   CaretAssert(borderFile);
   m_borderFile = borderFile;
   m_state = STATE_NONE;
   m_stateStack.push(m_state);
   m_elementText = "";
   m_metaDataSaxReader = NULL;
    m_labelTableSaxReader = NULL;
    m_surfaceProjectedItemSaxReader = NULL;
    m_border = NULL;
    m_surfaceProjectedItem = NULL;
    m_versionOneColorTable = NULL;
}

/**
 * destructor.
 */
BorderFileSaxReader::~BorderFileSaxReader()
{
    /*
     * If reading fails, allocated items need to be deleted.
     */
    if (m_metaDataSaxReader != NULL) {
        delete m_metaDataSaxReader;
    }
    if (m_labelTableSaxReader != NULL) {
        delete m_labelTableSaxReader;
    }
    if (m_surfaceProjectedItemSaxReader != NULL) {
        delete m_surfaceProjectedItemSaxReader;
    }
    if (m_surfaceProjectedItem != NULL) {
        delete m_surfaceProjectedItem;
    }
    if (m_border != NULL) {
        delete m_border;
    }
    if (m_versionOneColorTable != NULL) {
        delete m_versionOneColorTable;
    }
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
   const STATE previousState = m_state;
   switch (m_state) {
      case STATE_NONE:
           if (qName == BorderFile::XML_TAG_BORDER_FILE) {
            m_state = STATE_BORDER_FILE;
            
               /*
                * At one time version was float, but now integer so if
                * getting the version fails as integer get as float
                * and convert to integer.
                */
               int32_t versionBeingRead = 0;
               try {
                   versionBeingRead = attributes.getValueAsInt(BorderFile::XML_ATTRIBUTE_VERSION);
               }
               catch (const XmlSaxParserException& /*e*/) {
                   const float floatVersion = attributes.getValueAsFloat(BorderFile::XML_ATTRIBUTE_VERSION);
                   versionBeingRead = static_cast<int32_t>(floatVersion);
               }
               if (versionBeingRead > BorderFile::getFileVersion()) {
                   AString msg = XmlUtilities::createInvalidVersionMessage(BorderFile::getFileVersion(),
                                                                           versionBeingRead);
                   XmlSaxParserException e(msg);
                   CaretLogThrowing(e);
                   throw e;
               }
           }
         else {
             const AString msg = XmlUtilities::createInvalidRootElementMessage(BorderFile::XML_TAG_BORDER_FILE,
                                                                               qName);
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_BORDER:
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               m_state = STATE_SURFACE_PROJECTED_ITEM;
               m_surfaceProjectedItem = new SurfaceProjectedItem();
               m_surfaceProjectedItemSaxReader = new SurfaceProjectedItemSaxReader(m_surfaceProjectedItem);
               m_surfaceProjectedItemSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else if (qName == "ColorName") {
               // ignore, obsolete tag
           }
           else if ((qName != Border::XML_TAG_NAME) 
                    && (qName != Border::XML_TAG_CLASS_NAME)) {
               const AString msg = XmlUtilities::createInvalidChildElementMessage(Border::XML_TAG_BORDER, 
                                                                                  qName);
               XmlSaxParserException e(msg);
               CaretLogThrowing(e);
               throw e;
           }
           break;
      case STATE_BORDER_FILE:
           if (qName == BorderFile::XML_TAG_CLASS_COLOR_TABLE) {
               m_state = STATE_CLASS_COLOR_TABLE;
           }
           else if (qName == BorderFile::XML_TAG_NAME_COLOR_TABLE) {
               m_state = STATE_NAME_COLOR_TABLE;
           }
           else if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               m_state = STATE_VERSION_ONE_COLOR_TABLE;
               m_versionOneColorTable = new GiftiLabelTable();
               m_labelTableSaxReader = new GiftiLabelTableSaxReader(m_versionOneColorTable);
               m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
         else if (qName == GiftiXmlElements::TAG_METADATA) {
             m_state = STATE_METADATA;
             m_metaDataSaxReader = new GiftiMetaDataSaxReader(m_borderFile->getFileMetaData());
             m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         }
         else if (qName == Border::XML_TAG_BORDER) {
             m_state = STATE_BORDER;
             m_border = new Border();
         }
         else {
             const AString msg = XmlUtilities::createInvalidChildElementMessage(BorderFile::XML_TAG_BORDER_FILE, 
                                                                                qName);
             XmlSaxParserException e(msg);
             CaretLogThrowing(e);
             throw e;
         }
         break;
      case STATE_METADATA:
           m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
         break;
       case STATE_VERSION_ONE_COLOR_TABLE:
           m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
       case STATE_CLASS_COLOR_TABLE:
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               m_labelTableSaxReader = new GiftiLabelTableSaxReader(m_borderFile->getClassColorTable());
               m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else if (qName == GiftiXmlElements::TAG_LABEL) {
               m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else {
               const AString msg = XmlUtilities::createInvalidChildElementMessage(BorderFile::XML_TAG_CLASS_COLOR_TABLE,
                                                                                  qName);
               XmlSaxParserException e(msg);
               CaretLogThrowing(e);
               throw e;
           }
           break;
       case STATE_NAME_COLOR_TABLE:
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               m_labelTableSaxReader = new GiftiLabelTableSaxReader(m_borderFile->getNameColorTable());
               m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else if (qName == GiftiXmlElements::TAG_LABEL) {
               m_labelTableSaxReader->startElement(namespaceURI, localName, qName, attributes);
           }
           else {
               const AString msg = XmlUtilities::createInvalidChildElementMessage(BorderFile::XML_TAG_NAME_COLOR_TABLE,
                                                                                  qName);
               XmlSaxParserException e(msg);
               CaretLogThrowing(e);
               throw e;
           }
           break;
      case STATE_SURFACE_PROJECTED_ITEM:
           m_surfaceProjectedItemSaxReader->startElement(namespaceURI, localName, qName, attributes);
           break;
   }
   
   //
   // Save previous state
   //
   m_stateStack.push(previousState);
   
   m_elementText = "";
}

/**
 * end an element.
 */
void 
BorderFileSaxReader::endElement(const AString& namespaceURI,
                                       const AString& localName,
                                       const AString& qName) throw (XmlSaxParserException)
{
   switch (m_state) {
      case STATE_NONE:
         break;
       case STATE_BORDER:
           CaretAssert(m_border);
           if (qName == Border::XML_TAG_NAME) {
               m_border->setName(m_elementText.trimmed());
           }
           else if (qName == Border::XML_TAG_CLASS_NAME) {
               m_border->setClassName(m_elementText.trimmed());
           }
           else if (qName == "ColorName") {
               // obsolete element
           }
           else if (qName == Border::XML_TAG_BORDER) {
               m_borderFile->addBorder(m_border);
               m_border = NULL;  // do not delete since added to border file
           }
           else {
           }
           break;
      case STATE_BORDER_FILE:
         break;
      case STATE_METADATA:
           CaretAssert(m_metaDataSaxReader);
           m_metaDataSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_METADATA) {
               delete m_metaDataSaxReader;
               m_metaDataSaxReader = NULL;
           }
         break;
       case STATE_VERSION_ONE_COLOR_TABLE:
           CaretAssert(m_labelTableSaxReader);
           m_labelTableSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               delete m_labelTableSaxReader;
               m_labelTableSaxReader = NULL;
           }
           break;
       case STATE_CLASS_COLOR_TABLE:
           if (qName != BorderFile::XML_TAG_CLASS_COLOR_TABLE) {
               CaretAssert(m_labelTableSaxReader);
               m_labelTableSaxReader->endElement(namespaceURI, localName, qName);
               if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
                   delete m_labelTableSaxReader;
                   m_labelTableSaxReader = NULL;
               }
           }
           break;
       case STATE_NAME_COLOR_TABLE:
           if (qName != BorderFile::XML_TAG_NAME_COLOR_TABLE) {
               CaretAssert(m_labelTableSaxReader);
               m_labelTableSaxReader->endElement(namespaceURI, localName, qName);
               if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
                   delete m_labelTableSaxReader;
                   m_labelTableSaxReader = NULL;
               }
           }
           break;
      case STATE_SURFACE_PROJECTED_ITEM:
           CaretAssert(m_surfaceProjectedItemSaxReader);
           m_surfaceProjectedItemSaxReader->endElement(namespaceURI, localName, qName);
           if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
               m_border->addPoint(m_surfaceProjectedItem);
               m_surfaceProjectedItem = NULL; // do not delete since added to border
               delete m_surfaceProjectedItemSaxReader;
               m_surfaceProjectedItemSaxReader = NULL;
           }
         break;
   }

   //
   // Clear out for new elements
   //
   m_elementText = "";
   
   //
   // Go to previous state
   //
   if (m_stateStack.empty()) {
       throw XmlSaxParserException("State stack is empty while reading XML NiftDataFile.");
   }
   m_state = m_stateStack.top();
   m_stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
BorderFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
    if (m_metaDataSaxReader != NULL) {
        m_metaDataSaxReader->characters(ch);
    }
    else if (m_labelTableSaxReader != NULL) {
        m_labelTableSaxReader->characters(ch);
    }
    else if (m_surfaceProjectedItemSaxReader != NULL) {
        m_surfaceProjectedItemSaxReader->characters(ch);
    }
    else {
        m_elementText += ch;
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
    if (m_versionOneColorTable != NULL) {
        m_borderFile->createNameAndClassColorTables(m_versionOneColorTable);
    }
}

