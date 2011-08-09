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
#include "GiftiLabelTableSaxReader.h"
#include "GiftiXmlElements.h"
#include "StringUtilities.h"
#include "NiftiIntentEnum.h"
#include "XmlAttributes.h"
#include "XmlException.h"

using namespace caret;

/**
 * constructor.
 */
GiftiLabelTableSaxReader::GiftiLabelTableSaxReader(GiftiLabelTable* labelTable)
{
    this->state = STATE_NONE;
    this->stateStack.push(state);
    this->elementText = "";
    this->labelTable = labelTable;
}

/**
 * destructor.
 */
GiftiLabelTableSaxReader::~GiftiLabelTableSaxReader()
{
}


/**
 * start an element.
 */
void 
GiftiLabelTableSaxReader::startElement(const QString& /* namespaceURI */,
                                         const QString& /* localName */,
                                         const QString& qName,
                                         const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//    std::cout << "GIFTI LABEL TABLE: Start Element: " << qName << std::endl;
//   }
   
   const STATE previousState = this->state;
   switch (this->state) {
      case STATE_NONE:
           if (qName == GiftiXmlElements::TAG_LABEL_TABLE) {
               this->state = STATE_LABEL_TABLE;
           }
         else {
            std::ostringstream str;
            str << "Root element is \"" << qName.toStdString() << "\" but should be "
                << GiftiXmlElements::TAG_LABEL_TABLE.toStdString();
             throw XmlSaxParserException(QString::fromStdString(str.str()));
         }
         break;
      case STATE_LABEL_TABLE:
         if (qName == GiftiXmlElements::TAG_LABEL) {
            this->state = STATE_LABEL_TABLE_LABEL;
            QString s = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_KEY);
            if (s == "") {
               s = attributes.getValue("Index");
            }
            if (s.isEmpty()) {
               std::ostringstream str;
               str << "Tag "
                   << GiftiXmlElements::TAG_LABEL.toStdString()
                   << " is missing its "
                   << GiftiXmlElements::ATTRIBUTE_LABEL_KEY.toStdString();
                throw XmlSaxParserException(QString::fromStdString(str.str()));
            }
             this->labelIndex = s.toInt();

             float defaultRGBA[4];
             GiftiLabel::getDefaultColor(defaultRGBA);
                this->labelRed = defaultRGBA[0];
                this->labelGreen = defaultRGBA[1];
                this->labelBlue = defaultRGBA[2];
                this->labelAlpha = defaultRGBA[3];
               const QString redString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_RED);
               if (redString.isEmpty() == false) {
                  this->labelRed = redString.toFloat();
               }
               const QString greenString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN);
               if (greenString.isEmpty() == false) {
                  this->labelGreen = greenString.toFloat();
               }
               const QString blueString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE);
               if (blueString.isEmpty() == false) {
                  this->labelBlue = blueString.toFloat();
               }
               const QString alphaString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA);
               if (alphaString.isEmpty() == false) {
                  this->labelAlpha = alphaString.toFloat();
               }
             
             this->labelX = attributes.getValueAsFloat(GiftiXmlElements::ATTRIBUTE_LABEL_X);
             this->labelY = attributes.getValueAsFloat(GiftiXmlElements::ATTRIBUTE_LABEL_Y);
             this->labelZ = attributes.getValueAsFloat(GiftiXmlElements::ATTRIBUTE_LABEL_Z);
         }
         else {
            std::ostringstream str;
            str << "Child of " << GiftiXmlElements::TAG_LABEL_TABLE.toStdString() << " is \"" << qName.toStdString() 
                << "\" but should be " << GiftiXmlElements::TAG_LABEL.toStdString();
             throw XmlSaxParserException(QString::fromStdString(str.str()));
         }
         break;
      case STATE_LABEL_TABLE_LABEL:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_LABEL.toStdString() << " has child \"" << qName.toStdString()  
                << "\" but should not have any child nodes";
             throw XmlSaxParserException(QString::fromStdString(str.str()));
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
GiftiLabelTableSaxReader::endElement(const QString& /* namspaceURI */,
                                       const QString& /* localName */,
                                       const QString& qName) throw (XmlSaxParserException)
{
//   if (DebugControl::getDebugOn()) {
//      std::cout << "GIFTI LABLE TABLE: End Element: " << qName << std::endl;
//   }

   switch (state) {
      case STATE_NONE:
         break;
      case STATE_LABEL_TABLE:
         break;
      case STATE_LABEL_TABLE_LABEL:
           this->labelTable->setLabel(this->labelIndex, 
                                      this->elementText, 
                                      this->labelRed, 
                                      this->labelGreen, 
                                      this->labelBlue, 
                                      this->labelAlpha,
                                      this->labelX,
                                      this->labelY,
                                      this->labelZ);
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
       throw XmlSaxParserException("State stack is empty while reading GiftiLabelTable.");
   }
   this->state = stateStack.top();
   this->stateStack.pop();
}


/**
 * get characters in an element.
 */
void 
GiftiLabelTableSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
GiftiLabelTableSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
   throw e;
}

/**
 * A warning occurs
 */
void 
GiftiLabelTableSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    std::cout << "XML Parser Warning: " + e.whatString().toStdString() << std::endl;
}

// an error occurs
void 
GiftiLabelTableSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    throw e;
}

void 
GiftiLabelTableSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
GiftiLabelTableSaxReader::endDocument() throw (XmlSaxParserException)
{
}

