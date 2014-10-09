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
#include "GiftiEndianEnum.h"
#include "GiftiLabel.h"
#include "GiftiFile.h"
#include "GiftiLabelTableSaxReader.h"
#include "GiftiXmlElements.h"

#include "NiftiEnums.h"
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
    m_haveUnlabeled = false;
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
GiftiLabelTableSaxReader::startElement(const AString& /* namespaceURI */,
                                         const AString& /* localName */,
                                         const AString& qName,
                                         const XmlAttributes& attributes) 
{
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
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_LABEL_TABLE:
         if (qName == GiftiXmlElements::TAG_LABEL) {
            this->state = STATE_LABEL_TABLE_LABEL;
            AString s = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_KEY);
            if (s == "") {
               s = attributes.getValue("Index");
            }
            if (s.isEmpty()) {
               std::ostringstream str;
               str << "Tag "
                   << GiftiXmlElements::TAG_LABEL.toStdString()
                   << " is missing its "
                   << GiftiXmlElements::ATTRIBUTE_LABEL_KEY.toStdString();
                throw XmlSaxParserException(AString::fromStdString(str.str()));
            }
             this->labelIndex = s.toInt();

             float defaultRGBA[4];
             GiftiLabel::getDefaultColor(defaultRGBA);
                this->labelRed = defaultRGBA[0];
                this->labelGreen = defaultRGBA[1];
                this->labelBlue = defaultRGBA[2];
                this->labelAlpha = defaultRGBA[3];
               const AString redString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_RED);
               if (redString.isEmpty() == false) {
                  this->labelRed = redString.toFloat();
               }
               const AString greenString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN);
               if (greenString.isEmpty() == false) {
                  this->labelGreen = greenString.toFloat();
               }
               const AString blueString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE);
               if (blueString.isEmpty() == false) {
                  this->labelBlue = blueString.toFloat();
               }
               const AString alphaString = attributes.getValue(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA);
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
             throw XmlSaxParserException(AString::fromStdString(str.str()));
         }
         break;
      case STATE_LABEL_TABLE_LABEL:
         {
            std::ostringstream str;
            str << GiftiXmlElements::TAG_LABEL.toStdString() << " has child \"" << qName.toStdString()  
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
GiftiLabelTableSaxReader::endElement(const AString& /* namspaceURI */,
                                       const AString& /* localName */,
                                       const AString& /*qName*/)
{
   switch (state) {
      case STATE_NONE:
         break;
      case STATE_LABEL_TABLE:
         break;
      case STATE_LABEL_TABLE_LABEL:
         if ((elementText == "unknown" || elementText == "Unknown") && labelAlpha == 0.0f)
         {
            if (!m_haveUnlabeled)//if we already have an unlabeled key, don't change things, and warn
            {
               m_haveUnlabeled = true;
               CaretLogFiner("Using '" + elementText + "' label as unlabeled key");
               elementText = "???";//pretend these strings are actually "???" when alpha is 0, as that means it is unlabeled
            } else {
               CaretLogWarning("found multiple label elements that should be interpreted as unlabeled");
            }
         } else if (elementText == "???") {
            if (m_haveUnlabeled)
            {
               CaretLogWarning("found multiple label elements that should be interpreted as unlabeled");
            }
            m_haveUnlabeled = true;//that identifier always means unlabeled to us, don't let it slip by without setting m_haveUnlabeled
         }
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
GiftiLabelTableSaxReader::characters(const char* ch)
{
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
GiftiLabelTableSaxReader::fatalError(const XmlSaxParserException& e)
{
   throw e;
}

/**
 * A warning occurs
 */
void 
GiftiLabelTableSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
GiftiLabelTableSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
GiftiLabelTableSaxReader::startDocument() 
{    
}

void 
GiftiLabelTableSaxReader::endDocument()
{
}

