
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <QFile>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

#define __SCENE_FILE_XML_STREAM_READER_DECLARE__
#include "SceneFileXmlStreamReader.h"
#undef __SCENE_FILE_XML_STREAM_READER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SceneInfo.h"
#include "SceneInfoXmlStreamReader.h"
#include "ScenePathName.h"
#include "SceneTypeEnum.h"
#include "SceneXmlStreamReader.h"

using namespace caret;


    
/**
 * \class caret::SceneFileXmlStreamReader 
 * \brief XML Stream Writer for Scene File
 * \ingroup Files
 */

/**
 * Constructor.
 */
SceneFileXmlStreamReader::SceneFileXmlStreamReader()
: SceneFileXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneFileXmlStreamReader::~SceneFileXmlStreamReader()
{
}

/**
 * Read into the given scene file from the file with the given name
 *
 * @param filename
 *     Name of the scene file
 * @param sceneFile
 *     Name of the scene file
 * @throws
 *     Data file exception
 */
void
SceneFileXmlStreamReader::readFile(const AString& filename,
                                   SceneFile* sceneFile)
{
    CaretAssert(sceneFile);
    if (sceneFile == NULL) {
        throw DataFileException("Scene file is invalid (NULL).");
    }
    
    if (filename.isEmpty()) {
        throw DataFileException("Scene file name is empty");
    }
    
    m_filename = filename;
    
    QFile file(m_filename);
    if ( ! file.open(QFile::ReadOnly)) {
        throw DataFileException("Unable to open for reading: "
                                + m_filename
                                + " Reason: "
                                + file.errorString());
    }
    
    QXmlStreamReader xmlReader(&file);
    readFileContent(xmlReader,
                    sceneFile);

    AString errorMessage;
    if (xmlReader.hasError()) {
        errorMessage = xmlReader.errorString();
        errorMessage.appendWithNewLine("Line "
                                       + AString::number(xmlReader.lineNumber())
                                       + " column "
                                       + AString::number(xmlReader.columnNumber()));
    }

    file.close();
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(errorMessage);
    }
}

/**
 * Read the file's content
 *
 * @param xmlReader
 *     The XML stream reader
 * @param sceneFile
 *     Into this scene file
 */
void
SceneFileXmlStreamReader::readFileContent(QXmlStreamReader& xmlReader,
                                          SceneFile* sceneFile)
{
    CaretAssert(sceneFile);
    
    if (xmlReader.atEnd()) {
        xmlReader.raiseError("At end of file when starting to read.  Is file empty?");
        return;
    }
    
    xmlReader.readNextStartElement();
    if (xmlReader.name() != ELEMENT_SCENE_FILE) {
        xmlReader.raiseError("First element is \""
                             + xmlReader.name().toString()
                             + "\" but should be "
                             + ELEMENT_SCENE_FILE);
        return;
    }
    
    const QXmlStreamAttributes atts = xmlReader.attributes();
    const QStringRef versionAtt     = atts.value(ATTRIBUTE_SCENE_FILE_VERSION);
    m_fileVersion = 1;
    if ( ! versionAtt.isEmpty()) {
        /*
         * Note: version was float in previous versions, so get as float and convert to int
         */
        m_fileVersion = static_cast<int32_t>(versionAtt.toFloat());
    }
    
    /*
     * Set when ending scene file element is found
     */
    bool endElementFound(false);
    
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == GiftiXmlElements::TAG_METADATA) {
                    GiftiMetaData* metaData = sceneFile->getFileMetaData();
                    metaData->readSceneFile3(xmlReader);
                }
                else if (xmlReader.name() == ELEMENT_SCENE_FILE_INFO_DIRECTORY) {
                    readSceneInfoDirectory(xmlReader,
                                           sceneFile);
                }
                else if (xmlReader.name() == SceneXmlStreamReader::ELEMENT_SCENE) {
                    const QXmlStreamAttributes attributes = xmlReader.attributes();
                    const QStringRef typeString  = attributes.value(SceneXmlStreamReader::ATTRIBUTE_SCENE_TYPE);
                    bool valid(false);
                    SceneTypeEnum::Enum sceneType = SceneTypeEnum::fromName(typeString.toString(),
                                                                            &valid);
                    
                    const QStringRef indexString = attributes.value(SceneXmlStreamReader::ATTRIBUTE_SCENE_INDEX);
                    const int32_t sceneIndex = indexString.toInt();
                    
                    Scene* scene = new Scene(sceneType);
                    SceneXmlStreamReader sceneReader;
                    sceneReader.readScene(xmlReader,
                                          scene,
                                          m_filename);
                    if ( ! xmlReader.hasError()) {
                        auto mapIter = m_sceneInfoMap.find(sceneIndex);
                        SceneInfo* sceneInfo = ((mapIter != m_sceneInfoMap.end())
                                                ? mapIter->second
                                                : NULL);
                        scene->setSceneInfo(sceneInfo);
                        sceneFile->addScene(scene);
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_SCENE_FILE) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
}

/**
 * Read the scene info directory
 *
 * @param xmlReader
 *     The XML stream reader
 * @param sceneFile
 *     Into this scene file
 */
void
SceneFileXmlStreamReader::readSceneInfoDirectory(QXmlStreamReader& xmlReader,
                                                 SceneFile* sceneFile)
{
    
    /*
     * Default to CUSTOM base path since some older scenes do not
     * have AUTOMATIC/CUSTOM element (ELEMENT_SCENE_FILE_BALSA_BASE_PATH_TYPE(
     */
    sceneFile->setBasePathType(SceneFileBasePathTypeEnum::CUSTOM);

    /*
     * Gets set when ending scene info directory element is read
     */
    bool endElementFound(false);
    
    while ( (! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_SCENE_FILE_BALSA_STUDY_ID) {
                    sceneFile->setBalsaStudyID(xmlReader.readElementText());
                }
                else if (xmlReader.name() == ELEMENT_SCENE_FILE_BALSA_STUDY_TITLE) {
                    sceneFile->setBalsaStudyTitle(xmlReader.readElementText());
                }
                else if ((xmlReader.name() == ELEMENT_SCENE_FILE_BALSA_BASE_DIRECTORY)
                         || (xmlReader.name() == ELEMENT_SCENE_FILE_OBSOLETE_BASE_DIRECTORY)) {
                    /*
                     * Note: Base path is relative since the scene file may be used
                     * on different computers that contain different directory hierarchies.
                     */
                    const AString sceneFileBasePath = xmlReader.readElementText();

                    ScenePathName basePathName("customBaseDir",
                                               sceneFileBasePath);
                    basePathName.setValueToAbsolutePath(m_filename,
                                                        sceneFileBasePath);
                    sceneFile->setBalsaCustomBaseDirectory(basePathName.stringValue());
                }
                else if (xmlReader.name() == ELEMENT_SCENE_FILE_BALSA_EXTRACT_TO_DIRECTORY) {
                    sceneFile->setBalsaExtractToDirectoryName(xmlReader.readElementText());
                }
                else if (xmlReader.name() == ELEMENT_SCENE_FILE_BALSA_BASE_PATH_TYPE) {
                    const AString name = xmlReader.readElementText();
                    bool valid(false);
                    const SceneFileBasePathTypeEnum::Enum basePathType = SceneFileBasePathTypeEnum::fromName(name,
                                                                                                             &valid);
                    if (valid) {
                        sceneFile->setBasePathType(basePathType);
                    }
                    else {
                        sceneFile->setBasePathType(SceneFileBasePathTypeEnum::AUTOMATIC);
                        CaretLogWarning(m_filename
                                        + "has invalid base path type: "
                                        + name);
                    }
                    
                }
                else if (xmlReader.name() == SceneInfoXmlStreamReader::ELEMENT_SCENE_INFO) {
                    const QXmlStreamAttributes attributes = xmlReader.attributes();
                    const QStringRef indexAttribute = attributes.value(SceneInfoXmlStreamReader::ATTRIBUTE_SCENE_INDEX);
                    if ( ! indexAttribute.isEmpty()) {
                        const int32_t sceneIndex = indexAttribute.toInt();
                        SceneInfoXmlStreamReader infoReader;
                        SceneInfo* sceneInfo = new SceneInfo();
                        infoReader.readSceneInfo(xmlReader,
                                                 sceneInfo);
                        
                        if ( ! xmlReader.hasError()) {
                            m_sceneInfoMap.insert(std::make_pair(sceneIndex,
                                                                 sceneInfo));
                        }
                    }
                }
                else {
                    m_unexpectedXmlElements.insert(xmlReader.name().toString());
                    xmlReader.skipCurrentElement();
                }
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_SCENE_FILE_INFO_DIRECTORY) {
                    endElementFound = true;
                }
                break;
            default:
                break;
        }
    }
}

