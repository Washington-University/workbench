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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "GiftiMetaDataSaxReader.h"
#include "GiftiXmlElements.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneFileSaxReader.h"
#include "SceneInfo.h"
#include "SceneInfoSaxReader.h"
#include "ScenePathName.h"
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
SceneFileSaxReader::SceneFileSaxReader(SceneFile* sceneFile,
                                       const AString& sceneFileName)
{
    CaretAssert(sceneFile);
    m_sceneFile = sceneFile;
    m_sceneFileName = sceneFileName;
    m_state = STATE_NONE;
    m_stateStack.push(m_state);
    m_elementText = "";
    m_metaDataSaxReader = NULL;
    m_sceneSaxReader = NULL;
    m_sceneInfoSaxReader = NULL;
    m_scene = NULL;
    m_baseBathTypeWasFoundFlag = false;
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
    if (m_sceneInfoSaxReader != NULL) {
        delete m_sceneInfoSaxReader;
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
                                 const XmlAttributes& attributes) 
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
        case STATE_SCENE_INFO_DIRECTORY:
            if (qName == SceneXmlElements::SCENE_INFO_TAG) {
                m_state = STATE_SCENE_INFO;
                m_sceneInfoIndex = attributes.getValueAsIntRequired(SceneXmlElements::SCENE_INFO_INDEX_ATTRIBUTE);
                m_sceneInfo = new SceneInfo();
                m_sceneInfoSaxReader = new SceneInfoSaxReader(m_sceneFile->getFileName(),
                                                              m_sceneInfo);
                m_sceneInfoSaxReader->startElement(namespaceURI, localName, qName, attributes);
            }
            else if (qName == SceneXmlElements::SCENE_INFO_BALSA_STUDY_ID_TAG) {
                m_state = STATE_SCENE_INFO_BALSA_STUDY_ID;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_BALSA_STUDY_TITLE_TAG) {
                m_state = STATE_SCENE_INFO_BALSA_STUDY_TITLE;
            }
            else if ((qName == SceneXmlElements::SCENE_INFO_BALSA_BASE_DIRECTORY_TAG)
                     || (qName == SceneXmlElements::SCENE_INFO_BALSA_BASE_DIRECTORY_TAG_OLD)) {
                m_state = STATE_SCENE_INFO_BALSA_BASE_DIRECTORY;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_BALSA_EXTRACT_TO_DIRECTORY_TAG) {
                m_state = STATE_SCENE_INFO_BALSA_EXTRACT_TO_DIRECTORY;
            }
            else if (qName == SceneXmlElements::SCENE_INFO_BASE_PATH_TYPE) {
                m_state = STATE_SCENE_INFO_BASE_PATH_TYPE;
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::SCENE_INFO_TAG,
                                                                                   qName);
                XmlSaxParserException e(msg);
                warning(e);
//                CaretLogThrowing(e);
//                throw e;
            }
            break;
        case STATE_SCENE_INFO_BALSA_STUDY_ID:
            break;
        case STATE_SCENE_INFO_BALSA_STUDY_TITLE:
            break;
        case STATE_SCENE_INFO_BALSA_BASE_DIRECTORY:
            break;
        case STATE_SCENE_INFO_BALSA_EXTRACT_TO_DIRECTORY:
            break;
        case STATE_SCENE_INFO_BASE_PATH_TYPE:
            break;
        case STATE_SCENE_INFO:
            m_sceneInfoSaxReader->startElement(namespaceURI, localName, qName, attributes);
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
            else if (qName == SceneFile::XML_TAG_SCENE_INFO_DIRECTORY_TAG) {
                m_state = STATE_SCENE_INFO_DIRECTORY;
                break;
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
                m_sceneSaxReader = new SceneSaxReader(m_sceneFile->getFileName(),
                                                      m_scene);
                m_sceneSaxReader->startElement(namespaceURI, localName, qName, attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::SCENE_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                warning(e);
//                CaretLogThrowing(e);
//                throw e;
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
                               const AString& qName)
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
        case STATE_SCENE_INFO_DIRECTORY:
            break;
        case STATE_SCENE_INFO_BALSA_STUDY_ID:
            m_sceneFile->setBalsaStudyID(m_elementText);
            break;
        case STATE_SCENE_INFO_BALSA_STUDY_TITLE:
            m_sceneFile->setBalsaStudyTitle(m_elementText);
            break;
        case STATE_SCENE_INFO_BALSA_BASE_DIRECTORY:
            m_sceneFile->setBalsaCustomBaseDirectory(m_elementText);
            break;
        case STATE_SCENE_INFO_BALSA_EXTRACT_TO_DIRECTORY:
            m_sceneFile->setBalsaExtractToDirectoryName(m_elementText);
            break;
        case STATE_SCENE_INFO_BASE_PATH_TYPE:
        {
            SceneFileBasePathTypeEnum::Enum basePathType = SceneFileBasePathTypeEnum::AUTOMATIC;
            bool validFlag = false;
            basePathType = SceneFileBasePathTypeEnum::fromName(m_elementText, &validFlag);
            if (validFlag) {
                m_baseBathTypeWasFoundFlag = true;
            }
            else {
                basePathType = SceneFileBasePathTypeEnum::AUTOMATIC;
                
                AString msg("Invalid BasePathType name: " + m_elementText);
                XmlSaxParserException e(msg);
                warning(e);
            }
            m_sceneFile->setBasePathType(basePathType);
        }
            break;
        case STATE_SCENE_INFO:
            CaretAssert(m_sceneInfo);
            CaretAssert(m_sceneInfoSaxReader);
            m_sceneInfoSaxReader->endElement(namespaceURI, localName, qName);
            if (qName == SceneXmlElements::SCENE_INFO_TAG) {
                m_sceneInfoMap.insert(std::make_pair(m_sceneInfoIndex,
                                                     m_sceneInfo));
                delete m_sceneInfoSaxReader;
                m_sceneInfoSaxReader = NULL;
            }
            break;
        case STATE_SCENE_FILE:
        {
            for (std::map<int32_t, SceneInfo*>::iterator iter = m_sceneInfoMap.begin();
                 iter != m_sceneInfoMap.end();
                 iter++) {
                const int32_t sceneIndex = iter->first;
                SceneInfo* sceneInfo     = iter->second;
                CaretAssert(sceneInfo);
                
                if ((sceneIndex >= 0)
                    && (sceneIndex < m_sceneFile->getNumberOfScenes())) {
                    Scene* scene = m_sceneFile->getSceneAtIndex(sceneIndex);
                    scene->setSceneInfo(sceneInfo);
                }
                else {
                    const AString msg = ("SceneInfo has bad index="
                                     + AString::number(sceneIndex)
                                     + " in file "
                                     + m_sceneFile->getFileName());
                    CaretAssertMessage(0, msg);
                    CaretLogSevere(msg);
                }
            }
            
            if (m_baseBathTypeWasFoundFlag) {
                switch (m_sceneFile->getBasePathType()) {
                    case SceneFileBasePathTypeEnum::AUTOMATIC:
                        m_sceneFile->setBalsaCustomBaseDirectory("");
                        break;
                    case SceneFileBasePathTypeEnum::CUSTOM:
                    {
                        AString basePath = m_sceneFile->getBalsaCustomBaseDirectory();
                        if ( ! basePath.isEmpty()) {
                            FileInformation basePathInfo(basePath);
                            if (basePathInfo.isRelative()) {
//                                FileInformation tempBaseFile(basePath,
//                                                             "temp.txt");
                                ScenePathName basePathName("basePathName",
                                                           "");
                                basePathName.setValueToAbsolutePath(m_sceneFileName,
                                                                    (basePath)); // + "/temp.txt"));
                                basePath = basePathName.stringValue();
                            }
                        }
                        m_sceneFile->setBalsaCustomBaseDirectory(basePath);
                    }
                        break;
                }
            }
            else {
                /*
                 * Base path type was added by WB- in Oct 2017.
                 * If it is not found, the file was created prior to adding the
                 * base path type.  If there is no base path, the go ahead and
                 * use AUTOMATIC mode.  If there is a base path assume it is valid
                 * and use CUSTOM mode.
                 */
                if (m_sceneFile->getBalsaCustomBaseDirectory().isEmpty()) {
                    m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::AUTOMATIC);
                }
                else {
                    m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::CUSTOM);
                }
            }
        }
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
SceneFileSaxReader::characters(const char* ch)
{
    if (m_metaDataSaxReader != NULL) {
        m_metaDataSaxReader->characters(ch);
    }
    else if (m_sceneSaxReader != NULL) {
        m_sceneSaxReader->characters(ch);
    }
    else if (m_sceneInfoSaxReader != NULL) {
        m_sceneInfoSaxReader->characters(ch);
    }
    else {
        m_elementText += ch;
    }
}

/**
 * a fatal error occurs.
 */
void 
SceneFileSaxReader::fatalError(const XmlSaxParserException& e)
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
SceneFileSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SceneFileSaxReader::error(const XmlSaxParserException& e)
{   
    CaretLogSevere("XML Parser Error: " + e.whatString());
    throw e;
}

void 
SceneFileSaxReader::startDocument() 
{    
}

void 
SceneFileSaxReader::endDocument()
{
}

