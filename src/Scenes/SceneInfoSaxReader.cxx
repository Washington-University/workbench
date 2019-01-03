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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFile.h"
#include "SceneInfo.h"
#include "SceneInfoSaxReader.h"
#include "SceneXmlElements.h"
#include "WuQMacroGroupXmlReader.h"
#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * constructor.
 * @param sceneFileName
 *   Name of scene file being read.
 * @param sceneInfo
 *   Scene Info that is being read.
 */
SceneInfoSaxReader::SceneInfoSaxReader(const AString& sceneFileName,
                               SceneInfo* sceneInfo)
: m_sceneFileName(sceneFileName)
{
    m_state = STATE_NONE;
    m_stateStack.push(m_state);
    m_elementText = "";
    m_sceneInfo = sceneInfo;
    m_sceneInfoIndex = -1;

    m_imageEncoding  = "";
    m_imageFormat    = "";
}

/**
 * destructor.
 */
SceneInfoSaxReader::~SceneInfoSaxReader()
{
}


/**
 * start an element.
 */
void 
SceneInfoSaxReader::startElement(const AString& /* namespaceURI */,
                             const AString& /* localName */,
                             const AString& qName,
                             const XmlAttributes& attributes) 
{
    const STATE previousState = m_state;
    switch (m_state) {
        case STATE_NONE:
            if (qName == SceneXmlElements::SCENE_INFO_TAG) {
                m_state = STATE_SCENE_INFO;
                
                m_sceneInfoIndex = attributes.getValueAsInt(SceneXmlElements::SCENE_INFO_INDEX_ATTRIBUTE);
            }
            else {
                const AString msg = ("While reading Scene XML, expected the XML tag to be "
                                     + SceneXmlElements::SCENE_INFO_TAG
                                     + " but found "
                                     + qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_SCENE_INFO_BALSA_ID:
            break;
        case STATE_SCENE_INFO:
            if (qName == SceneXmlElements::SCENE_INFO_NAME_TAG) {
                m_state = STATE_SCENE_INFO_NAME;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_DESCRIPTION_TAG) {
                m_state = STATE_SCENE_INFO_DESCRIPTION;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_IMAGE_TAG) {
                m_state          = STATE_SCENE_INFO_IMAGE_THUMBNAIL;
                m_imageFormat    = attributes.getValue(SceneXmlElements::SCENE_INFO_IMAGE_FORMAT_ATTRIBUTE);
                m_imageEncoding  = attributes.getValue(SceneXmlElements::SCENE_INFO_IMAGE_ENCODING_ATTRIBUTE);
            }
            else if (qName == SceneXmlElements::SCENE_INFO_BALSA_SCENE_ID_TAG) {
                m_state = STATE_SCENE_INFO_BALSA_ID;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_MACRO_GROUP) {
                m_state = STATE_SCENE_INFO_MACRO_GROUP;
                break;
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::SCENE_INFO_TAG,
                                                                                   qName);
                XmlSaxParserException e(msg);
                warning(e);
                m_state = STATE_SCENE_INFO_UNRECOGNIZED;
            }
            break;
        case STATE_SCENE_INFO_MACRO_GROUP:
            break;
        case STATE_SCENE_INFO_NAME:
            break;
        case STATE_SCENE_INFO_DESCRIPTION:
            break;
        case STATE_SCENE_INFO_IMAGE_THUMBNAIL:
            break;
        case STATE_SCENE_INFO_UNRECOGNIZED:
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
SceneInfoSaxReader::endElement(const AString& /* namspaceURI */,
                           const AString& /* localName */,
                           const AString& /*qName*/)
{
    const AString stringValue = m_elementText.trimmed();
    
    switch (m_state) {
        case STATE_NONE:
            break;
        case STATE_SCENE_INFO:
            break;
        case STATE_SCENE_INFO_BALSA_ID:
            CaretAssert(m_sceneInfo);
            m_sceneInfo->setBalsaSceneID(stringValue);
            break;
        case STATE_SCENE_INFO_NAME:
            CaretAssert(m_sceneInfo);
            m_sceneInfo->setName(stringValue);
            break;
        case STATE_SCENE_INFO_DESCRIPTION:
            CaretAssert(m_sceneInfo);
            m_sceneInfo->setDescription(stringValue);
            break;
        case STATE_SCENE_INFO_IMAGE_THUMBNAIL:
        {
            CaretAssert(m_sceneInfo);
            m_sceneInfo->setImageFromText(stringValue,
                                          m_imageEncoding,
                                          m_imageFormat);
            m_imageEncoding  = "";
            m_imageFormat    = "";
        }
            break;
        case STATE_SCENE_INFO_UNRECOGNIZED:
            break;
        case STATE_SCENE_INFO_MACRO_GROUP:
        {
            WuQMacroGroup* macroGroup = m_sceneInfo->getMacroGroup();
            WuQMacroGroupXmlReader reader;
            reader.readFromString(m_elementText,
                                  macroGroup);
            if (reader.hasError()) {
                CaretLogSevere(reader.getErrorMessage());
            }
            else if (reader.hasWarnings()) {
                CaretLogWarning(reader.getWarningMessage());
            }
        }
            break;
    }
    
    /*
     * Clear out for new elements
     */
    m_elementText = "";
    
    /*
     * Go to previous state
     */
    if (m_stateStack.empty()) {
        throw XmlSaxParserException("State stack is empty while reading Scene.");
    }
    CaretAssert(m_stateStack.empty() == false);
    m_state = m_stateStack.top();
    m_stateStack.pop();
}

/**
 * get characters in an element.
 */
void 
SceneInfoSaxReader::characters(const char* ch)
{
    m_elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
SceneInfoSaxReader::fatalError(const XmlSaxParserException& e)
{
    throw e;
}

/**
 * A warning occurs
 */
void 
SceneInfoSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SceneInfoSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
SceneInfoSaxReader::startDocument() 
{    
}

void 
SceneInfoSaxReader::endDocument()
{
}

