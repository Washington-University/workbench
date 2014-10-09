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
#include "StudyMetaDataLink.h"
#include "StudyMetaDataLinkSet.h"
#include "StudyMetaDataLinkSetSaxReader.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * constructor.
 */
StudyMetaDataLinkSetSaxReader::StudyMetaDataLinkSetSaxReader(StudyMetaDataLinkSet* studyMetaDataLinkSet)
{
    m_state = STATE_NONE;
    m_stateStack.push(this->m_state);
    m_elementText = "";
    m_studyMetaDataLinkSet = studyMetaDataLinkSet;
    m_studyMetaDataLinkBeingRead = NULL;
}

/**
 * destructor.
 */
StudyMetaDataLinkSetSaxReader::~StudyMetaDataLinkSetSaxReader()
{
}


/**
 * start an element.
 */
void 
StudyMetaDataLinkSetSaxReader::startElement(const AString& /* namespaceURI */,
                                            const AString& /* localName */,
                                            const AString& qName,
                                            const XmlAttributes& /*attributes*/) 
{
    const STATE previousState = m_state;
    switch (m_state) {
        case STATE_NONE:
            if (qName == StudyMetaDataLinkSet::XML_TAG_STUDY_META_DATA_LINK_SET) {
                m_state = STATE_STUDY_META_DATA_LINK_SET;
            }
            else {
                AString txt = XmlUtilities::createInvalidRootElementMessage(StudyMetaDataLinkSet::XML_TAG_STUDY_META_DATA_LINK_SET,
                                                                            qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_STUDY_META_DATA_LINK_SET:
            if (qName == StudyMetaDataLink::XML_TAG_STUDY_META_DATA_LINK) {
                m_state = STATE_STUDY_META_DATA_LINK;
                m_studyMetaDataLinkBeingRead = new StudyMetaDataLink();
            }
            else {
                AString txt = XmlUtilities::createInvalidChildElementMessage(StudyMetaDataLink::XML_TAG_STUDY_META_DATA_LINK,
                                                                             qName);
                XmlSaxParserException e(txt);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_STUDY_META_DATA_LINK:
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
StudyMetaDataLinkSetSaxReader::endElement(const AString& /* namspaceURI */,
                                          const AString& /* localName */,
                                          const AString& qName)
{
    const AString text = m_elementText.trimmed();
    
    switch (m_state) {
        case STATE_NONE:
            break;
        case STATE_STUDY_META_DATA_LINK_SET:
            if (m_studyMetaDataLinkBeingRead != NULL) {
                this->m_studyMetaDataLinkSet->addStudyMetaDataLink(*m_studyMetaDataLinkBeingRead);
                delete m_studyMetaDataLinkBeingRead;
                m_studyMetaDataLinkBeingRead = NULL;
            }
            break;
        case STATE_STUDY_META_DATA_LINK:
            if (qName != StudyMetaDataLink::XML_TAG_STUDY_META_DATA_LINK) {
                m_studyMetaDataLinkBeingRead->setElementFromText(qName, 
                                                                 text);
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
        throw XmlSaxParserException("State stack is empty while reading XML MetaData.");
    }
    m_state = m_stateStack.top();
    m_stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
StudyMetaDataLinkSetSaxReader::characters(const char* ch)
{
    m_elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
StudyMetaDataLinkSetSaxReader::fatalError(const XmlSaxParserException& e)
{
    //
    // Stop parsing
    //
    CaretLogSevere("XML Parser Fatal Error: " + e.whatString());
    throw e;
}

// a warning occurs
void 
StudyMetaDataLinkSetSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
StudyMetaDataLinkSetSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
StudyMetaDataLinkSetSaxReader::startDocument() 
{    
}

void 
StudyMetaDataLinkSetSaxReader::endDocument()
{
}

