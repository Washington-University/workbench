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
#include "GiftiMetaDataSaxReader.h"
#include "GiftiXmlElements.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneFileSaxReader.h"
#include "SceneXmlElements.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * \class caret::SceneFileSaxReader
 * \brief Reads a scene file using a SAX XML Parser.
 */

/**
 * constructor.
 */
SceneFileSaxReader::SceneFileSaxReader(SceneFile* sceneFile)
{
    CaretAssert(sceneFile);
    m_sceneFile = sceneFile;
    m_state = STATE_NONE;
    m_stateStack.push(m_state);
    m_elementText = "";
    m_metaDataSaxReader = NULL;
    m_sceneSaxReader = NULL;
    m_scene = NULL;
}

/**
 * destructor.
 */
SceneFileSaxReader::~SceneFileSaxReader()
{
    /*
     * If reading fails, allocated items need to be deleted.
     */
    if (m_metaDataSaxReader != NULL) {
        delete m_metaDataSaxReader;
    }
    if (m_sceneSaxReader != NULL) {
        delete m_sceneSaxReader;
    }
    if (m_scene != NULL) {
        delete m_scene;
    }
}


/**
 * start an element.
 */
void 
SceneFileSaxReader::startElement(const AString& namespaceURI,
                                 const AString& localName,
                                 const AString& qName,
                                 const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
    const STATE previousState = m_state;
    switch (m_state) {
        case STATE_NONE:
            if (qName == SceneFile::XML_TAG_SCENE_FILE) {
                m_state = STATE_SCENE_FILE;
                
                //
                // Check version of file being read
                //
                const float version = attributes.getValueAsFloat(SceneFile::XML_ATTRIBUTE_VERSION);
                if (version > SceneFile::getFileVersion()) {
                    AString msg = XmlUtilities::createInvalidVersionMessage(SceneFile::getFileVersion(), 
                                                                            version);
                    XmlSaxParserException e(msg);
                    CaretLogThrowing(e);
                    throw e;
                }
            }
            else {
                const AString msg = XmlUtilities::createInvalidRootElementMessage(SceneFile::XML_TAG_SCENE_FILE,
                                                                                  qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_SCENE:
            m_sceneSaxReader->startElement(namespaceURI, localName, qName, attributes);
            break;
        case STATE_SCENE_FILE:
            if (qName == GiftiXmlElements::TAG_METADATA) {
                m_state = STATE_METADATA;
                m_metaDataSaxReader = new GiftiMetaDataSaxReader(m_sceneFile->getFileMetaData());
                m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
            }
            else if (qName == SceneXmlElements::SCENE_TAG) {
                m_state = STATE_SCENE;
                const AString sceneTypeName = attributes.getValue(SceneXmlElements::SCENE_TYPE_ATTRIBUTE);
                bool validName = false;
                const SceneTypeEnum::Enum sceneType = SceneTypeEnum::fromName(sceneTypeName,
                                                                              &validName);
                if (validName == false) {
                    const AString msg = XmlUtilities::createInvalidAttributeMessage(SceneXmlElements::SCENE_TAG, 
                                                                                    SceneXmlElements::SCENE_TYPE_ATTRIBUTE,
                                                                                    sceneTypeName);
                    XmlSaxParserException e(msg);
                    CaretLogThrowing(e);
                    throw e;
                }
                m_scene = new Scene(sceneType);
                m_sceneSaxReader = new SceneSaxReader(m_scene);
                m_sceneSaxReader->startElement(namespaceURI, localName, qName, attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::SCENE_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_METADATA:
            m_metaDataSaxReader->startElement(namespaceURI, localName, qName, attributes);
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
SceneFileSaxReader::endElement(const AString& namespaceURI,
                               const AString& localName,
                               const AString& qName) throw (XmlSaxParserException)
{
    switch (m_state) {
        case STATE_NONE:
            break;
        case STATE_SCENE:
            CaretAssert(m_scene);
            CaretAssert(m_sceneSaxReader);
            m_sceneSaxReader->endElement(namespaceURI, localName, qName);
            if (qName == SceneXmlElements::SCENE_TAG) {
                m_sceneFile->addScene(m_scene);
                m_scene = NULL;  // do not delete since added to border file
                delete m_sceneSaxReader;
                m_sceneSaxReader = NULL;
            }
            break;
        case STATE_SCENE_FILE:
            break;
        case STATE_METADATA:
            CaretAssert(m_metaDataSaxReader);
            m_metaDataSaxReader->endElement(namespaceURI, localName, qName);
            if (qName == GiftiXmlElements::TAG_METADATA) {
                delete m_metaDataSaxReader;
                m_metaDataSaxReader = NULL;
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
SceneFileSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
    if (m_metaDataSaxReader != NULL) {
        m_metaDataSaxReader->characters(ch);
    }
    else if (m_sceneSaxReader != NULL) {
        m_sceneSaxReader->characters(ch);
    }
    else {
        m_elementText += ch;
    }
}

/**
 * a fatal error occurs.
 */
void 
SceneFileSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
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
SceneFileSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SceneFileSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
SceneFileSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
SceneFileSaxReader::endDocument() throw (XmlSaxParserException)
{
}

