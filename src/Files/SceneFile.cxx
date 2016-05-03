
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

#include <QTextStream>

#include <algorithm>
#include <memory>

#define __SCENE_FILE_DECLARE__
#include "SceneFile.h"
#undef __SCENE_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "Scene.h"
#include "SceneFileSaxReader.h"
#include "SceneInfo.h"
#include "SceneXmlElements.h"
#include "SceneWriterXml.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::SceneFile 
 * \brief Contains scenes that reproduce Workbench state
 */

/**
 * Constructor.
 */
SceneFile::SceneFile()
: CaretDataFile(DataFileTypeEnum::SCENE)
{
    m_balsaStudyID = "";
    m_metadata = new GiftiMetaData();
}

/**
 * Destructor.
 */
SceneFile::~SceneFile()
{
    delete m_metadata;
    
    for (std::vector<Scene*>::iterator iter = m_scenes.begin();
         iter != m_scenes.end();
         iter++) {
        delete *iter;
    }
    m_scenes.clear();
}

/**
 * Clear the contents of this file.
 */
void 
SceneFile::clear()
{
    CaretDataFile::clear();
    
    m_metadata->clear();
    
    m_balsaStudyID = "";
    
    for (std::vector<Scene*>::iterator iter = m_scenes.begin();
         iter != m_scenes.end();
         iter++) {
        delete *iter;
    }
    m_scenes.clear();
}

/**
 * @return true if the file is empty (no scenes) else false.
 */
bool 
SceneFile::isEmpty() const
{
    return m_scenes.empty();
}


/**
 * Add the given scene to the file.  The file then
 * takes ownership of the scene.
 * 
 * @param scene
 *    Scene that is added.
 */
void 
SceneFile::addScene(Scene* scene)
{
    CaretAssert(scene);
    m_scenes.push_back(scene);
    setModified();
}

/**
 * Insert a scene above the given scene.  The file then
 * takes ownership of the scene.
 *
 * @param newScene
 *     New scene that is inserted.
 * @param insertAboveThisScene
 *     The new scene is inserted above (before) this scene.
 */
void
SceneFile::insertScene(Scene* newScene,
                 const Scene* insertAboveThisScene)
{
    CaretAssert(newScene);
    CaretAssert(insertAboveThisScene);
    
    std::vector<Scene*> tempSceneVector;
    bool newSceneInsertedFlag = false;
    
    for (std::vector<Scene*>::iterator iter = m_scenes.begin();
         iter != m_scenes.end();
         iter++) {
        Scene* scene = *iter;
        if (scene == insertAboveThisScene) {
            newSceneInsertedFlag = true;
            tempSceneVector.push_back(newScene);
        }
        tempSceneVector.push_back(scene);
    }
    
    if ( ! newSceneInsertedFlag) {
        m_scenes.push_back(newScene);
        CaretLogSevere("Scene insertion did not find \"insert above scene\"");
    }
    
    m_scenes = tempSceneVector;
}


/**
 * Replace a scene.
 * @param newScene
 *    New scene
 * @param sceneThatIsReplacedAndDeleted
 *    Scene that is replaced and delete so DO NOT
 *    reference this scene after calling this method.
 */
void
SceneFile::replaceScene(Scene* newScene,
                        Scene* sceneThatIsReplacedAndDeleted)
{
    CaretAssert(newScene);
    CaretAssert(sceneThatIsReplacedAndDeleted);
    
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        if (m_scenes[i] == sceneThatIsReplacedAndDeleted) {
            delete m_scenes[i];
            m_scenes[i] = newScene;
            setModified();
            return;
        }
    }
    
    CaretAssertMessage(0, "Replacing scene failed due to scene not found.");
    CaretLogSevere("Replacing scene failed due to scene not found.");
}

/**
 * @return The number of scenes.
 */
int32_t 
SceneFile::getNumberOfScenes() const
{
    return m_scenes.size();
}

/**
 * Get the scene at the given index.
 * @param indx
 *     Index of the scene.
 * @return
 *     Scene at the given index.
 */
Scene* 
SceneFile::getSceneAtIndex(const int32_t indx) const
{
    CaretAssertVectorIndex(m_scenes, indx);
    return m_scenes[indx];
}

/**
 * Get the index of the given scene.
 *
 * @param scene
 *     Scene for which index is requested.
 * @return
 *     Index of the scene or negative if scene not found.
 */
int32_t
SceneFile::getIndexOfScene(const Scene* scene) const
{
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        if (scene == getSceneAtIndex(i)) {
            return i;
        }
    }

    return -1;
}

/**
 * Get the scene with the given name.
 * @param sceneName
 *    Name of scene.
 * @return 
 *    Scene with given name or NULL if no scene with
 *    the given name.
 */
Scene*
SceneFile::getSceneWithName(const AString& sceneName)
{
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* scene = getSceneAtIndex(i);
        if (scene->getName() == sceneName) {
            return scene;
        }
    }
    return NULL;
}

/**
 * Remove the given scene.
 * @param scene
 *    Scene that should be removed.
 */
void 
SceneFile::removeScene(Scene* scene)
{
    CaretAssert(scene);
    std::vector<Scene*>::iterator iter = std::find(m_scenes.begin(),
                                                   m_scenes.end(),
                                                   scene);
    if (iter != m_scenes.end()) {
        m_scenes.erase(iter);
        delete scene;
        setModified();
    }
}


/**
 * Remove the scene at the given index.
 * @param indx
 *     Index of the scene.
 */
void 
SceneFile::removeSceneAtIndex(const int32_t indx)
{
    CaretAssertVectorIndex(m_scenes, indx);
    Scene* scene = getSceneAtIndex(indx);
    removeScene(scene);
}

/**
 * Move the scene in the file by the given change in index.
 * 
 * @param scene
 *     Scene that is moved.
 * @param indexDelta
 *     Change of index of scene in the file.
 */
void
SceneFile::moveScene(Scene* scene,
                     const int32_t indexDelta)
{
    if (indexDelta == 0) {
        return;
    }
    
    const int32_t numberOfScenes = getNumberOfScenes();
    if (numberOfScenes == 1) {
        return;
    }
    
    const int32_t sceneIndex = getIndexOfScene(scene);
    
    const int32_t newSceneIndex = sceneIndex + indexDelta;
    
    if ((sceneIndex >= 0)
        && (sceneIndex < numberOfScenes)) {
        if (newSceneIndex < sceneIndex) {
            if (newSceneIndex >= 0) {
                /*
                 * Remove scene from its index
                 * insert scene into its new index
                 */
                m_scenes.erase(m_scenes.begin() + sceneIndex);
                m_scenes.insert(m_scenes.begin() + newSceneIndex,
                                scene);
                setModified();
            }
        }
        else {
            CaretAssert(newSceneIndex > sceneIndex);
            
            if (newSceneIndex < numberOfScenes) {
                /*
                 * Innsert scene into its new index
                 * Remove scene from its index
                 */
                m_scenes.insert(m_scenes.begin() + newSceneIndex + 1,
                                scene);
                m_scenes.erase(m_scenes.begin() + sceneIndex);
                setModified();
            }
        }
    }
}

/**
 * Reorder the scenes given the newly ordered scenes.
 * Any existing scenes not in the newly ordered scenes are 
 * removed.
 *
 * @param orderedScenes
 *    Newly ordered scenes.
 */
void 
SceneFile::reorderScenes(std::vector<Scene*>& orderedScenes)
{
    /*
     * Make copy of pointers to existing scenes
     */
    std::vector<Scene*> oldSceneVector = m_scenes;
    
    /*
     * Replace scenes with newly ordered scenes
     */
    m_scenes = orderedScenes;
    
    /*
     * If an existing scene is not in the newly ordered scenes,
     * remove it.
     */
    for (std::vector<Scene*>::iterator iter = oldSceneVector.begin();
         iter != oldSceneVector.end();
         iter++) {
        Scene* scene = *iter;
        if (std::find(m_scenes.begin(),
                      m_scenes.end(),
                      scene) == m_scenes.end()) {
            delete scene;
        }
    }
    
    setModified();
}


/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
SceneFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
SceneFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* ignore, not a structure related file */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
SceneFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
SceneFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * @return The BALSA Study ID.
 */
AString
SceneFile::getBalsaStudyID() const
{
    return m_balsaStudyID;
}

/**
 * Set the BALSA Study ID.
 *
 * @param balsaStudyID
 *     New value for BALSA Study ID.
 */
void
SceneFile::setBalsaStudyID(const AString& balsaStudyID)
{
    if (balsaStudyID != m_balsaStudyID) {
        m_balsaStudyID = balsaStudyID;
        setModified();
    }
}

/**
 * Read the scene file.
 * @param filenameIn
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
SceneFile::readFile(const AString& filenameIn)
{
    clear();
    
    AString filename = filenameIn;
    if (DataFile::isFileOnNetwork(filename) == false) {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    checkFileReadability(filename);
    
    this->setFileName(filename);
    SceneFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg = "Parse Error while reading:";
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(filenameIn,
                              msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }
    
    this->setFileName(filename);

    this->clearModified();
}

/**
 * Write the scene file.
 * @param filename
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
SceneFile::writeFile(const AString& filename)
{
    checkFileWritability(filename);
    
    this->setFileName(filename);
    
    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = AString::number(SceneFile::getFileVersion(),
                                                      'f',
                                                      1);
        
        //
        // Open the file
        //
        FileAdapter file;
        AString errorMessage;
        QTextStream* textStream = file.openQTextStreamForWritingFile(this->getFileName(),
                                                                     errorMessage);
        if (textStream == NULL) {
            throw DataFileException(filename,
                                    errorMessage);
        }
        
        //
        // Create the xml writer
        //
        XmlWriter xmlWriter(*textStream);
        
        //
        // Write header info
        //
        xmlWriter.writeStartDocument("1.0");
        
        //
        // Write root element
        //
        XmlAttributes attributes;
        
        //attributes.addAttribute("xmlns:xsi",
        //                        "http://www.w3.org/2001/XMLSchema-instance");
        //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
        //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
        attributes.addAttribute(SceneFile::XML_ATTRIBUTE_VERSION,
                                versionString);
        xmlWriter.writeStartElement(SceneFile::XML_TAG_SCENE_FILE,
                                    attributes);
        
        //
        // Write Metadata
        //
        if (m_metadata != NULL) {
            m_metadata->writeAsXML(xmlWriter);
        }

        const int32_t numScenes = this->getNumberOfScenes();

        /*
         * Write the scene info directory
         */
        xmlWriter.writeStartElement(SceneFile::XML_TAG_SCENE_INFO_DIRECTORY_TAG);
        xmlWriter.writeElementCData(SceneXmlElements::SCENE_INFO_BALSA_STUDY_ID_TAG,
                                    getBalsaStudyID());
        for (int32_t i = 0; i < numScenes; i++) {
            m_scenes[i]->getSceneInfo()->writeSceneInfo(xmlWriter,
                                                        i);
        }
        xmlWriter.writeEndElement();
        
        //
        // Write scenes
        //
        SceneWriterXml sceneWriter(xmlWriter,
                                   this->getFileName());
        for (int32_t i = 0; i < numScenes; i++) {
            sceneWriter.writeScene(*m_scenes[i], 
                                   i);
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        file.close();
        
        this->clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(e);
    }
    catch (const XmlException& e) {
        throw DataFileException(e);
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
SceneFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    const int32_t numScenes = getNumberOfScenes();
    if (numScenes > 0) {
        AString sceneNamesText = "SCENE NAMES";
        for (int32_t i = 0; i < numScenes; i++) {
            const Scene* scene = getSceneAtIndex(i);
            sceneNamesText.appendWithNewLine("    "
                                             + scene->getName());
            
        }
        
        dataFileInformation.addText(sceneNamesText);
    }
}

