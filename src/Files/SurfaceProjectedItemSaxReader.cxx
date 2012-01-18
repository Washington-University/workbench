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
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectedItemSaxReader.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * constructor.
 */
SurfaceProjectedItemSaxReader::SurfaceProjectedItemSaxReader(SurfaceProjectedItem* surfaceProjectedItem)
{
   this->state = STATE_NONE;
   this->stateStack.push(this->state);
   this->elementText = "";
   this->surfaceProjectedItem  = surfaceProjectedItem;
}

/**
 * destructor.
 */
SurfaceProjectedItemSaxReader::~SurfaceProjectedItemSaxReader()
{
}


/**
 * start an element.
 */
void 
SurfaceProjectedItemSaxReader::startElement(const AString& /* namespaceURI */,
                                         const AString& /* localName */,
                                         const AString& qName,
                                         const XmlAttributes& /*attributes*/)  throw (XmlSaxParserException)
{
   const STATE previousState = this->state;
    switch (state) {
        case STATE_NONE:
            if (qName == SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM) {
                this->state = STATE_SURFACE_PROJECTED_ITEM;
            }
            else {
                AString txt = XmlUtilities::createInvalidRootElementMessage(SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM,
                                                                            qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_SURFACE_PROJECTED_ITEM:
            if (qName == SurfaceProjectionBarycentric::XML_TAG_PROJECTION_BARYCENTRIC) {
                state = STATE_BARYCENTRIC;
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_PROJECTION_VAN_ESSEN) {
                state = STATE_VAN_ESSEN;
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ) {
                // nothing
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_STRUCTURE) {
                // nothing
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_VOLUME_XYZ) {
                // nothing
            }
            else {
                AString txt = XmlUtilities::createInvalidChildElementMessage(SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM,
                                                                            qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_BARYCENTRIC:
            if (qName == SurfaceProjectionBarycentric::XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE) {
                // nothing
            }
            else if (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_AREAS) {
                // nothing
            }
            else if (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES) {
                // nothing
            }
            else {
                AString txt = XmlUtilities::createInvalidChildElementMessage(SurfaceProjectionBarycentric::XML_TAG_PROJECTION_BARYCENTRIC,
                                                                             qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_VAN_ESSEN:
            break;
    }
/*
   switch (this->state) {
      case STATE_NONE:
           if (qName == GiftiXmlElements::TAG_METADATA) {
               this->state = STATE_SURFACE_PROJECTED_ITEM;
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
*/
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
SurfaceProjectedItemSaxReader::endElement(const AString& /* namspaceURI */,
                                       const AString& /* localName */,
                                       const AString& qName) throw (XmlSaxParserException)
{
    const AString text = this->elementText.trimmed();
    
    switch (state) {
        case STATE_NONE:
            break;
        case STATE_SURFACE_PROJECTED_ITEM:
            if (qName == SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ) {
                std::vector<float> xyz;
                AString::toNumbers(text, xyz);
                if (xyz.size() == 3) {
                    this->surfaceProjectedItem->setStereotaxicXYZ(xyz.data());
                }
                else {
                    AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ,
                                                                       3,
                                                                       xyz.size());
                    XmlSaxParserException e(txt);
                    CaretLogThrowing(e);
                    throw e;
                }
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_STRUCTURE) {
                bool isValid = false;
                this->surfaceProjectedItem->setStructure(StructureEnum::fromName(text,
                                                                                 &isValid));
                if (isValid == false) {
                    CaretLogWarning("Invalid structure name: " 
                                    + text);
                }
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_VOLUME_XYZ) {
                std::vector<float> xyz;
                AString::toNumbers(text, xyz);
                if (xyz.size() == 3) {
                    this->surfaceProjectedItem->setVolumeXYZ(xyz.data());
                }
                else {
                    AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(SurfaceProjectedItem::XML_TAG_VOLUME_XYZ,
                                                                                     3,
                                                                                     xyz.size());
                    XmlSaxParserException e(txt);
                    CaretLogThrowing(e);
                    throw e;
                }
            }
            break;
        case STATE_BARYCENTRIC:
        {
            SurfaceProjectionBarycentric* bp = this->surfaceProjectedItem->getBarycentricProjection();

            if (qName == SurfaceProjectionBarycentric::XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE) {
                bp->setSignedDistanceAboveSurface(text.toFloat());
            }
            else if (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_AREAS) {
                std::vector<float> areas;
                AString::toNumbers(text, areas);
                if (areas.size() == 3) {
                    bp->setTriangleAreas(areas.data());
                }
                else {
                    AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_AREAS,
                                                                                     3,
                                                                                     areas.size());
                    XmlSaxParserException e(txt);
                    CaretLogThrowing(e);
                    throw e;
                }
            }
            else if (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES) {
                std::vector<int32_t> nodes;
                AString::toNumbers(text, nodes);
                if (nodes.size() == 3) {
                    bp->setTriangleNodes(nodes.data());
                }
                else {
                    AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES,
                                                                                     3,
                                                                                     nodes.size());
                    XmlSaxParserException e(txt);
                    CaretLogThrowing(e);
                    throw e;
                }
            }
        }
            break;
        case STATE_VAN_ESSEN:
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
SurfaceProjectedItemSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
   this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
SurfaceProjectedItemSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
   //
   // Stop parsing
   //
   CaretLogSevere("XML Parser Fatal Error: " + e.whatString());
   throw e;
}

// a warning occurs
void 
SurfaceProjectedItemSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SurfaceProjectedItemSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    throw e;
}

void 
SurfaceProjectedItemSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
SurfaceProjectedItemSaxReader::endDocument() throw (XmlSaxParserException)
{
}

