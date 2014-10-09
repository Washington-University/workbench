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
                                            const XmlAttributes& /*attributes*/) 
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
            else if ((qName == SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ)
                     || (qName == SurfaceProjectedItem::XML_TAG_STRUCTURE)
                     || (qName == SurfaceProjectedItem::XML_TAG_VOLUME_XYZ)) {
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
            if ((qName == SurfaceProjectionBarycentric::XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE)
                || (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_AREAS)
                || (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES)) {
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
            if ((qName == SurfaceProjectionVanEssen::XML_TAG_DR)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_FRAC_RI)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_FRAC_RJ)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_PHI_R)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_POS_ANATOMICAL)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_PROJECTION_VAN_ESSEN)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_THETA_R)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_TRI_ANATOMICAL)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_TRI_VERTICES)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_VERTEX)
                || (qName == SurfaceProjectionVanEssen::XML_TAG_VERTEX_ANATOMICAL)) {
                // nothing
            }
            else {
                AString txt = XmlUtilities::createInvalidChildElementMessage(SurfaceProjectionVanEssen::XML_TAG_PROJECTION_VAN_ESSEN,
                                                                             qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
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
SurfaceProjectedItemSaxReader::endElement(const AString& /* namspaceURI */,
                                          const AString& /* localName */,
                                          const AString& qName)
{
    const AString text = this->elementText.trimmed();
    
    switch (state) {
        case STATE_NONE:
            break;
        case STATE_SURFACE_PROJECTED_ITEM:
            if (qName == SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ) {
                std::vector<float> xyz;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        3,
                                                        xyz);
                this->surfaceProjectedItem->setStereotaxicXYZ(xyz.data());
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_STRUCTURE) {
                bool isValid = false;
                this->surfaceProjectedItem->setStructure(StructureEnum::fromName(text,
                                                                                 &isValid));
                if (isValid == false) {
                    CaretLogWarning("Invalid structure name: " 
                                    + text);
                    throw XmlSaxParserException("Invalid structure name: " 
                                                + text);
                }
            }
            else if (qName == SurfaceProjectedItem::XML_TAG_VOLUME_XYZ) {
                std::vector<float> xyz;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        3,
                                                        xyz);
                this->surfaceProjectedItem->setVolumeXYZ(xyz.data());
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
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        3,
                                                        areas);
                bp->setTriangleAreas(areas.data());
            }
            else if (qName == SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES) {
                std::vector<int32_t> nodes;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        3,
                                                        nodes);
                bp->setTriangleNodes(nodes.data());
            }
            bp->setValid(true);
        }
            break;
        case STATE_VAN_ESSEN:
        {
            SurfaceProjectionVanEssen* ve = this->surfaceProjectedItem->getVanEssenProjection();
            
            if (qName == SurfaceProjectionVanEssen::XML_TAG_DR) {
                ve->setDR(text.toFloat());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_FRAC_RI) {
                ve->setFracRI(text.toFloat());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_FRAC_RJ) {
                ve->setFracRJ(text.toFloat());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_PHI_R) {
                ve->setPhiR(text.toFloat());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_POS_ANATOMICAL) {
                std::vector<float> xyz;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        3,
                                                        xyz);
                ve->setPosAnatomical(xyz.data());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_THETA_R) {
                ve->setThetaR(text.toFloat());
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_TRI_ANATOMICAL) {
                std::vector<float> data;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        18,
                                                        data);
                float ta[2][3][3];
                int32_t ctr = 0;
                for (int32_t i = 0; i < 2; i++) {
                    for (int32_t j = 0; j < 3; j++) {
                        for (int32_t k = 0; k < 3; k++) {
                            ta[i][j][k] = data[ctr];
                            ctr++;
                        }
                    }
                }
                ve->setTriAnatomical(ta);
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_TRI_VERTICES) {
                std::vector<int32_t> data;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        6,
                                                        data);
                int32_t tv[2][3];
                int32_t ctr = 0;
                for (int32_t i = 0; i < 2; i++) {
                    for (int32_t j = 0; j < 3; j++) {
                        tv[i][j] = data[ctr];
                        ctr++;
                    }
                }
                ve->setTriVertices(tv);
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_VERTEX) {
                std::vector<int32_t> data;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        2,
                                                        data);
                int32_t tv[2];
                int32_t ctr = 0;
                for (int32_t i = 0; i < 2; i++) {
                    tv[i] = data[ctr];
                    ctr++;
                }
                ve->setVertex(tv);
            }
            else if (qName == SurfaceProjectionVanEssen::XML_TAG_VERTEX_ANATOMICAL) {
                std::vector<float> data;
                XmlUtilities::getArrayOfNumbersFromText(qName,
                                                        text,
                                                        6,
                                                        data);
                float va[2][3];
                int32_t ctr = 0;
                for (int32_t i = 0; i < 2; i++) {
                    for (int32_t j = 0; j < 3; j++) {
                        va[i][j] = data[ctr];
                        ctr++;
                    }
                }
                ve->setVertexAnatomical(va);
            }
            ve->setValid(true);
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
        throw XmlSaxParserException("State stack is empty while reading XML MetaData.");
    }
    this->state = this->stateStack.top();
    this->stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
SurfaceProjectedItemSaxReader::characters(const char* ch)
{
    this->elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
SurfaceProjectedItemSaxReader::fatalError(const XmlSaxParserException& e)
{
    //
    // Stop parsing
    //
    CaretLogSevere("XML Parser Fatal Error: " + e.whatString());
    throw e;
}

// a warning occurs
void 
SurfaceProjectedItemSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SurfaceProjectedItemSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
SurfaceProjectedItemSaxReader::startDocument() 
{    
}

void 
SurfaceProjectedItemSaxReader::endDocument()
{
}

