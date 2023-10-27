
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

#include <QDir>
#include <QTextStream>

#include <algorithm>
#include <memory>
#include <set>

#define __SCENE_FILE_DECLARE__
#include "SceneFile.h"
#undef __SCENE_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "DeveloperFlagsEnum.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneInfo.h"
#include "ScenePathName.h"
#include "ScenePathNameArray.h"
#include "SceneFileXmlStreamReader.h"
#include "SceneFileXmlStreamWriter.h"
#include "SpecFile.h"
#include "SystemUtilities.h"
#include "XmlException.h"
#include "WuQMacroGroup.h"

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
    m_balsaStudyTitle = "";
    m_balsaCustomBaseDirectory = "";
    m_balsaExtractToDirectoryName = "";
    m_basePathType = SceneFileBasePathTypeEnum::AUTOMATIC;
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
    m_balsaStudyTitle = "";
    m_balsaCustomBaseDirectory = "";
    m_balsaExtractToDirectoryName = "";
    m_basePathType = SceneFileBasePathTypeEnum::AUTOMATIC;
    
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
    
    setModified();
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
 * Get the scene with the given scene ID.
 * @param sceneID
 *    ID of scene.
 * @return
 *    Scene with given ID or NULL if no scene with
 *    the given ID.
 */
Scene*
SceneFile::getSceneWithSceneID(const AString& sceneID)
{
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* scene = getSceneAtIndex(i);
        if (scene->getSceneInfo()->getBalsaSceneID() == sceneID) {
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
 * Remove the scene at the given index, but return its pointer rather than deleting it.
 * @param index
 *     Index of the scene.
 */
Scene* SceneFile::releaseScene(const int32_t& index)
{
    CaretAssertVectorIndex(m_scenes, index);
    Scene* scene = m_scenes[index];
    m_scenes.erase(m_scenes.begin() + index);
    return scene;
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

int32_t SceneFile::getSceneIndexFromNumberOrName(const AString& numberOrName)
{
    bool ok = false;
    int32_t ret = numberOrName.toInt(&ok) - 1;//compensate for 1-indexing that command line parsing uses
    if (ok)
    {
        if (ret < 0 || ret >= getNumberOfScenes())
        {
            return -1;
        }
        return ret;
    } else {//DO NOT search by name if the string was parsed as an integer correctly, or some idiot who names their maps as integers will get confused
            //when getting map "12" out of a file after the file expands to more than 12 elements suddenly does something different
        const int32_t numScenes = getNumberOfScenes();
        for (int32_t i = 0; i < numScenes; i++) {
            if (numberOrName == getSceneAtIndex(i)->getName()) {
                return i;
            }
        }
    }
    return -1;
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
 * @return The BALSA Study Title.
 */
AString
SceneFile::getBalsaStudyTitle() const
{
    return m_balsaStudyTitle;
}

/**
 * Set the BALSA Study Title.
 *
 * @param balsaStudyTitle
 *     New value for BALSA Study Title.
 */
void
SceneFile::setBalsaStudyTitle(const AString& balsaStudyTitle)
{
    if (balsaStudyTitle != m_balsaStudyTitle) {
        m_balsaStudyTitle = balsaStudyTitle;
        setModified();
    }
}

/**
 * @return The Custom Base Directory
 */
AString
SceneFile::getBalsaCustomBaseDirectory() const
{
    return m_balsaCustomBaseDirectory;
}

/**
 * Set the Custom Base Directory.
 *
 * @param baseDirectory
 *     New value for Base Directory.
 */
void
SceneFile::setBalsaCustomBaseDirectory(const AString& balsaBaseDirectory)
{
    if (balsaBaseDirectory != m_balsaCustomBaseDirectory) {
        m_balsaCustomBaseDirectory = balsaBaseDirectory;
        setModified();
    }
}

/**
 * @return The base path type.
 */
SceneFileBasePathTypeEnum::Enum
SceneFile::getBasePathType() const
{
    return m_basePathType;
}

/**
 * Set the base path type.
 * 
 * @param basePathType
 *     New type for base path.
 */
void SceneFile::setBasePathType(const SceneFileBasePathTypeEnum::Enum basePathType)
{
    if (basePathType != m_basePathType) {
        m_basePathType = basePathType;
        setModified();
    }
}

/**
 * Get the type of base directory and the name of the base directory
 * @param basePathTypeOut
 *    Output with type of base path
 * @param basePathNameOut
 *    Output with name of base path
 * @param errorMessageOut
 *    Output with error information
 * @return
 *    True if the outputs are valid, else false.
 */
bool
SceneFile::getSelectedBasePathTypeAndName(SceneFileBasePathTypeEnum::Enum& basePathTypeOut,
                                          AString& basePathNameOut,
                                          AString& errorMessageOut) const
{
    basePathNameOut.clear();
    bool validFlag(false);
    
    switch (getBasePathType()) {
        case SceneFileBasePathTypeEnum::AUTOMATIC:
        {
            std::vector<AString> missingFileNames;
            validFlag = findBaseDirectoryForDataFiles(basePathNameOut,
                                                      missingFileNames,
                                                      errorMessageOut);
        }
            break;
        case SceneFileBasePathTypeEnum::CUSTOM:
        {
            basePathNameOut = getBalsaCustomBaseDirectory();
            if ( ! basePathNameOut.isEmpty()) {
                 validFlag = true;
            }
            else {
                errorMessageOut = "Custom path is invalid (empty)";
            }
        }
            break;
    }
    
    basePathTypeOut = getBasePathType();

    return validFlag;
}


/**
 * Set the name of the file.
 *
 * @param filename
 *    New name of file
 */
void
SceneFile::setFileName(const AString& filename)
{
    CaretDataFile::setFileName(filename);
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
    if ( ! DataFile::isFileOnNetwork(filename)) {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    checkFileReadability(filename);
    
    this->setFileName(filename);

    try {
        SceneFileXmlStreamReader streamReader;
        streamReader.readFile(filename,
                              this);
    }
    catch (const DataFileException& e) {
        DataFileException dfe(filename,
                              e.whatString());
        CaretLogThrowing(dfe);
        throw dfe;
    }

    this->setFileName(filename);

    this->clearModified();
}

/**
 * Read the scene file using the new Stream parser
 * @param filenameIn
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void
SceneFile::readFileStreamReader(const AString& filenameIn)
{
    clear();
    
    AString filename = filenameIn;
    if ( ! DataFile::isFileOnNetwork(filename)) {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    checkFileReadability(filename);
    
    this->setFileName(filename);
    
    try {
        SceneFileXmlStreamReader streamReader;
        streamReader.readFile(filename,
                              this);
    }
    catch (const DataFileException& e) {
        DataFileException dfe(filename,
                              e.whatString());
        CaretLogThrowing(dfe);
        throw dfe;
    }

    this->setFileName(filename);
    
    this->clearModified();
}


/**
 * Write the scene file.
 * @param filenameIn
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
SceneFile::writeFile(const AString& filenameIn)
{
    if (!(filenameIn.endsWith(".scene") || filenameIn.endsWith(".wb_scene")))
    {
        CaretLogWarning("scene file '" + filenameIn + "' should be saved ending in .scene");
    }
    AString filename(filenameIn);
    if ( ! DataFile::isFileOnNetwork(filename)) {
        FileInformation specInfo(filename);
        filename = specInfo.getAbsoluteFilePath();
    }
    checkFileWritability(filename);
    
    this->setFileName(filename);
    
    
    try {
        /*
         * Update file's metadata with info about Workbench
         */
        SceneInfo::addWorkbenchVersionInfoToMetaData(m_metadata);
        
        SceneFileXmlStreamWriter xmlStreamWriter;
        xmlStreamWriter.writeFile(this);

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
 * Write the scene file stream writer
 * @param filename
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void
SceneFile::writeFileStreamWriter(const AString& filename)
{
    if (!(filename.endsWith(".scene") || filename.endsWith(".wb_scene")))
    {
        CaretLogWarning("scene file '" + filename + "' should be saved ending in .scene");
    }
    checkFileWritability(filename);
    
    this->setFileName(filename);
    
    try {
        SceneFileXmlStreamWriter xmlStreamWriter;
        xmlStreamWriter.writeFile(this);

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
    
    dataFileInformation.addNameAndValue("Base Path Type", SceneFileBasePathTypeEnum::toGuiName(getBasePathType()));
    dataFileInformation.addNameAndValue("Custom Base Path", getBalsaCustomBaseDirectory());
    dataFileInformation.addNameAndValue("Extract Directory", getBalsaExtractToDirectoryName());
    dataFileInformation.addNameAndValue("BALSA Title", getBalsaStudyTitle());
    dataFileInformation.addNameAndValue("BALSA Study ID", getBalsaStudyID());
    
    const int32_t numScenes = getNumberOfScenes();
    if (numScenes > 0) {
        AString sceneNamesText = "Scenes:";
        for (int32_t i = 0; i < numScenes; i++) {
            const Scene* scene = getSceneAtIndex(i);
            sceneNamesText.appendWithNewLine("\n#" + AString::number(i + 1) + "  " +
                                             scene->getName());
            if (dataFileInformation.isOptionFlag(DataFileContentInformation::OPTION_SHOW_MAP_INFORMATION))
            {
                const AString indent("        ");
                const GiftiMetaData* sceneMetaData(scene->getSceneInfo()->getMetaData());
                const auto namesAndValues(sceneMetaData->getAsMap());
                if ( ! namesAndValues.empty()) {
                    sceneNamesText.appendWithNewLine(indent + "Metadata:");
                    for (const auto& nv : namesAndValues) {
                        sceneNamesText.appendWithNewLine(indent + "   " + nv.first + ":  " + nv.second);
                    }
                }

                //sceneNamesText += ":";
                const SceneAttributes* myAttrs = scene->getAttributes();
                const SceneClass* guiMgrClass = scene->getClassWithName("guiManager");
                if (guiMgrClass == NULL)
                {
                    sceneNamesText.appendWithNewLine("missing guiManager class");
                    continue;
                }
                const SceneClass* sessMgrClass = guiMgrClass->getClass("m_sessionManager");
                if (sessMgrClass == NULL)
                {
                    sceneNamesText.appendWithNewLine("missing m_sessionManager class");
                    continue;
                }
                const SceneClassArray* brainArray = sessMgrClass->getClassArray("m_brains");
                if (brainArray == NULL)
                {
                    sceneNamesText.appendWithNewLine("missing m_brains class array");
                    continue;
                }
                const int numBrainClasses = brainArray->getNumberOfArrayElements();
                for (int j = 0; j < numBrainClasses; ++j)
                {
                    const SceneClass* brainClass = brainArray->getClassAtIndex(j);
                    const SceneClass* specClass = brainClass->getClass("specFile");
                    if (specClass == NULL)
                    {
                        sceneNamesText.appendWithNewLine("missing specFile class in m_brains element " + AString::number(j));
                        continue;
                    }
                    SpecFile tempSpec;
                    tempSpec.restoreFromScene(myAttrs, specClass);
                    std::vector<AString> tempNames = tempSpec.getAllDataFileNamesSelectedForLoading();
                    int numNames = (int)tempNames.size();
                    for (int k = 0; k < numNames; ++k)
                    {
                        AString notFoundText;
                        if ( ! QFile::exists(tempNames[k])) {
                            notFoundText = " (NOT FOUND)";
                        }
                        sceneNamesText.appendWithNewLine(indent + tempNames[k] + notFoundText);
                    }
                }
            }
            
        }
        
        dataFileInformation.addText(sceneNamesText);
    }
}

/**
 * @return Extract to directory name for zip file
 */
AString
SceneFile::getBalsaExtractToDirectoryName() const
{
    return m_balsaExtractToDirectoryName;
}

/**
 * Set Extract to directory name for zip file
 *
 * @param extractToDirectoryName
 *    New value for Extract to directory name for zip file
 */
void
SceneFile::setBalsaExtractToDirectoryName(const AString& extractToDirectoryName)
{
    if (extractToDirectoryName != m_balsaExtractToDirectoryName) {
        setModified();
        m_balsaExtractToDirectoryName = extractToDirectoryName;
    }
}

/**
 * Find the base directory that is a directory that is parent to all loaded data files
 * and also including the scene file.
 *
 * @param baseDirectoryOut
 *    Output containing the base directory
 * @param missingFileNamesOut
 *    Will contain data files that are in scenes but do not exist.
 * @param errorMessageOut
 *    Error message if finding base directory fails
 * @return
 *    True if the base directory is valid, else false.
 */
bool
SceneFile::findBaseDirectoryForDataFiles(AString& baseDirectoryOut,
                                         std::vector<AString>& missingFileNamesOut,
                                         AString& errorMessageOut) const
{
    return SceneFile::findBaseDirectoryForDataFiles(getFileName(),
                                                    getAllDataFileNamesFromAllScenes(),
                                                    baseDirectoryOut,
                                                    missingFileNamesOut,
                                                    errorMessageOut);
}

/**
 * Find the base directory that is a directory that is parent to all loaded data files
 * and also including the scene file.
 *
 * @param sceneFileName
 *    Name of scene file
 * @param filesFromScenes
 *    Files in all scenes in the scene file
 * @param baseDirectoryOut
 *    Output containing the base directory
 * @param missingFileNamesOut
 *    Will contain data files that are in scenes but do not exist.
 * @param errorMessageOut
 *    Error message if finding base directory fails
 * @return
 *    True if the base directory is valid, else false.
 */
bool
SceneFile::findBaseDirectoryForDataFiles(const AString& sceneFileName,
                                         const std::set<FileAndSceneIndicesInfo>& filesFromScenes,
                                         AString& baseDirectoryOut,
                                         std::vector<AString>& missingFileNamesOut,
                                         AString& errorMessageOut)
{
    baseDirectoryOut.clear();
    missingFileNamesOut.clear();
    errorMessageOut.clear();
    
    const AString directorySeparator("/");
    
    std::vector<AString> allFileNames;
    for (const auto& nameInfo : filesFromScenes) {
        allFileNames.push_back(nameInfo.m_dataFileName);
    }
    if ( ! sceneFileName.isEmpty()) {
        allFileNames.push_back(sceneFileName);
    }
    
    /*
     * Find a unique set of directory names used by the data files and
     * also verify that all files exist.
     */
    std::set<AString> directoryNamesUniqueSet;
    std::set<AString> missingFileNames;
    for (auto name : allFileNames) {
        if (DataFile::isFileOnNetwork(name)) {
            /* assume file on network are valid */
        }
        else {
            FileInformation fileInfo(name);
            if (fileInfo.exists()) {
                AString dirName = fileInfo.getAbsolutePath().trimmed();
                if ( ! dirName.isEmpty()) {
                    /*
                     * QDir::cleanPath() removes multiple separators and resolves "." or ".."
                     * QDir::fromNativeSeparators() ensures "/" is used for the directory separator.
                     */
                    dirName = QDir::cleanPath(dirName);
                    dirName = QDir::fromNativeSeparators(dirName);
                    if ( ! dirName.isEmpty()) {
                        directoryNamesUniqueSet.insert(dirName);
                    }
                }
            }
            else {
                missingFileNames.insert(name);
            }
        }
    }
    
    AString sceneFilePath;
    if ( ! sceneFileName.isEmpty()) {
        const FileInformation fileInfo(sceneFileName);
        sceneFilePath = fileInfo.getAbsolutePath().trimmed();
    }
    
    missingFileNamesOut.insert(missingFileNamesOut.end(),
                               missingFileNames.begin(),
                               missingFileNames.end());
    
    const int32_t numDirs = static_cast<int32_t>(directoryNamesUniqueSet.size());
    if (numDirs <= 0) {
        /*
         * if no valid files in scene, use path of scene file.
         */
        if (sceneFilePath.isEmpty()) {
            errorMessageOut = "No data files available for determining base path";
            return false;
        }
        
        baseDirectoryOut = sceneFilePath;
        return true;
    }
    
    /*
     * Compare the first directory with all other directories
     * to find longest common directory path.
     * Each directory is split into its path components and they
     * are compared.
     */
    bool firstFlag = true;
    std::vector<AString> longestPathMatch;
    for (const auto& dirName : directoryNamesUniqueSet) {
        if (firstFlag) {
            firstFlag = false;
            longestPathMatch = AString::stringListToVector(dirName.split(directorySeparator));
        }
        else {
            const std::vector<AString> otherPath = AString::stringListToVector(dirName.split(directorySeparator));
            const int32_t matchCount = AString::matchingCount(longestPathMatch,
                                                              otherPath);
            longestPathMatch.resize(matchCount);
        }
    }
    
    if ( ! longestPathMatch.empty()) {
        /*
         * Assemble the path components into a directory.
         */
        baseDirectoryOut = AString::join(longestPathMatch,
                                         directorySeparator);
    }
    
    /*
     * If no "longest path", files are on multiple disks
     */
    if (baseDirectoryOut.isEmpty()) {
        if (SystemUtilities::isWindowsOperatingSystem()) {
            /*
             * On Windows, there is no "root directory"
             */
            baseDirectoryOut = "";
            errorMessageOut = ("Files appear to be on different disks.  On the Windows Operating System, "
                               "there is no directory that is parent to the disks.  Files will need to "
                               "be moved so that they are on one disk");
            return false;
        }
        else {
            /*
             * On Unix, user root directory
             */
            baseDirectoryOut = directorySeparator;
        }
    }
    
    return true;
}

/**
 * @return The base directory for all data files and all of 
 *         the base directorys ancestors (parent directory
 *         up to root directory).
 *
 * @param maximumAncestorCount
 *         Maximum number of ancestor directories for output
 */
std::vector<AString>
SceneFile::getBaseDirectoryHierarchyForDataFiles(const int32_t maximumAncestorCount)
{
    std::vector<AString> names;
    
    AString baseDirectoryName;
    std::vector<AString> missingFileNames;
    AString errorMessage;
    if (findBaseDirectoryForDataFiles(baseDirectoryName,
                                      missingFileNames,
                                      errorMessage)) {
        QDir dir(baseDirectoryName);
        
        for (int32_t i = 0; i < maximumAncestorCount; i++) {
            names.push_back(dir.absolutePath());
            
            if (dir.isRoot()) {
                break;
            }
            else {
                if ( ! dir.cdUp()) {
                    break;
                }
            }
        }
    }
    
    return names;
}

/**
 * @return A set containing the names of all data files from all scenes.
 */
std::set<SceneFile::FileAndSceneIndicesInfo>
SceneFile::getAllDataFileNamesFromAllScenes() const
{
    const bool includeSpecFileFlag = false;
    
    std::set<FileAndSceneIndicesInfo> fileInfoOut;
    
    /**
     * Find all 'path name' elements from ALL scenes
     */
    const int32_t numScenes = static_cast<int32_t>(m_scenes.size());
    for (int32_t sceneIndex = 0; sceneIndex < numScenes; sceneIndex++) {
        CaretAssertVectorIndex(m_scenes, sceneIndex);
        const Scene* scene = m_scenes[sceneIndex];
        CaretAssert(scene);
        std::vector<SceneObject*> children = scene->getDescendants();
        for (SceneObject* sceneObject : children) {
            CaretAssert(sceneObject);
            if (sceneObject->getDataType() == SceneObjectDataTypeEnum::SCENE_PATH_NAME) {
                const ScenePathName* scenePathName = dynamic_cast<ScenePathName*>(sceneObject);

                /*
                 * Will be NULL for 'path name arrays' which we ignore
                 */
                if (scenePathName != NULL) {
                    /*
                     * files in spec file are named "fileName" in the scene
                     * specFile is named "specFileName"
                     * and these names are unique to name of files in the spec file
                     */
                    bool useNameFlag = false;
                    if (sceneObject->getName() == "fileName") {
                        useNameFlag = true;
                    }
                    else if (sceneObject->getName() == "brainChildDataFile") {
                        useNameFlag = true;
                    }
                    else if (sceneObject->getName() == "specFileName") {
                        useNameFlag = includeSpecFileFlag;
                    }
                    if (useNameFlag) {
                        AString pathName = scenePathName->stringValue().trimmed();
                        
                        if ( ! pathName.isEmpty()) {
                            bool validExtensionFlag = false;
                            const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(pathName,
                                                                                                            &validExtensionFlag);
                            
                            bool validDiskFileFlag = true;
                            if (validExtensionFlag) {
                                switch (dataFileType) {
                                    case DataFileTypeEnum::ANNOTATION:
                                        break;
                                    case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                                        break;
                                    case DataFileTypeEnum::BORDER:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                                        validDiskFileFlag = false;
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                                        validDiskFileFlag = false;
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                                        break;
                                    case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                                        break;
                                    case DataFileTypeEnum::CZI_IMAGE_FILE:
                                        break;
                                    case DataFileTypeEnum::FOCI:
                                        break;
                                    case DataFileTypeEnum::HISTOLOGY_SLICES:
                                        break;
                                    case DataFileTypeEnum::IMAGE:
                                        break;
                                    case DataFileTypeEnum::LABEL:
                                        break;
                                    case DataFileTypeEnum::METRIC:
                                        break;
                                    case DataFileTypeEnum::METRIC_DYNAMIC:
                                        validDiskFileFlag = false;
                                        break;
                                    case DataFileTypeEnum::PALETTE:
                                        break;
                                    case DataFileTypeEnum::RGBA:
                                        break;
                                    case DataFileTypeEnum::SAMPLES:
                                        break;
                                    case DataFileTypeEnum::SCENE:
                                        break;
                                    case DataFileTypeEnum::SPECIFICATION:
                                        break;
                                    case DataFileTypeEnum::SURFACE:
                                        break;
                                    case DataFileTypeEnum::VOLUME:
                                        break;
                                    case DataFileTypeEnum::VOLUME_DYNAMIC:
                                        validDiskFileFlag = false;
                                        break;
                                    case DataFileTypeEnum::UNKNOWN:
                                        validDiskFileFlag = false;
                                        break;
                                }
                            }
                            
                            if (validDiskFileFlag) {
                                FileInformation fileInfo(pathName);
                                const QString absPathName = fileInfo.getAbsoluteFilePath();
                                if ( ! absPathName.isEmpty()) {
                                    pathName = absPathName;
                                }
                                
                                /*
                                 * Test to see if this file is already in the output file info
                                 */
                                bool foundFlag = false;
                                for (auto& dfi : fileInfoOut) {
                                    if (dfi.m_dataFileName == pathName) {
                                        dfi.addSceneIndex(sceneIndex);
                                        foundFlag = true;
                                        break;
                                    }
                                }
                                
                                if ( ! foundFlag) {
                                    fileInfoOut.insert(FileAndSceneIndicesInfo(pathName,
                                                                               sceneIndex));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return fileInfoOut;
}

/**
 * @return File info for all files in the scene file.
 */
std::vector<SceneDataFileInfo>
SceneFile::getAllDataFileInfoFromAllScenes() const
{
    const std::set<SceneFile::FileAndSceneIndicesInfo> allNamesAndIndices = getAllDataFileNamesFromAllScenes();
    
    AString basePath;
    AString errorMessage;
    std::vector<AString> missingFiles;
    const bool validFlag = findBaseDirectoryForDataFiles(basePath, missingFiles, errorMessage);
    if ( ! validFlag) {
        CaretLogSevere("Failed to find the base path for scene file: "
                       + getFileName());
    }
    
    std::vector<SceneDataFileInfo> fileInfoOut;
 
    for (const auto& nameAndIndices : allNamesAndIndices) {
        fileInfoOut.emplace_back(nameAndIndices.m_dataFileName,
                                 basePath,
                                 getFileName(),
                                 nameAndIndices.m_sceneIndices);
    }
    
    return fileInfoOut;
}


/**
 * @return Default name for a ZIP file containing the scene file and its data files.
 */
AString
SceneFile::getDefaultZipFileName() const
{
    return FileInformation::replaceExtension(getFileName(), ".zip");
}

/**
 * @return The default extract to directory name (end of default base directory)
 */
AString
SceneFile::getDefaultExtractToDirectoryName() const
{
    AString directoryName;

    AString baseDirectoryName;
    std::vector<AString> missingFileNames;
    AString errorMessage;
    if (findBaseDirectoryForDataFiles(baseDirectoryName,
                                      missingFileNames,
                                      errorMessage)) {
        QDir dir(baseDirectoryName);
        directoryName = dir.dirName();
    }
    
    if (directoryName.isEmpty()) {
        QDir dir(SystemUtilities::systemCurrentDirectory());
        directoryName = dir.dirName();
    }
    
    return directoryName;
}

/**
 * @return True if this scene is modified.
 */
bool
SceneFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    for (const auto scene : m_scenes) {
        if (scene->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status of this scene.
 */
void
SceneFile::clearModified()
{
    CaretDataFile::clearModified();
    for (auto scene : m_scenes) {
        scene->clearModified();
    }
}

/**
 * @return The version number to use when writing
 * an instance of a scene.  This number returned
 * may version depending upon the content of the scene
 * and may allow older versions of wb_view to read
 * the scene file when it does not contain new stuff.
 */
int32_t
SceneFile::getSceneFileVersionForWriting() const
{
    int32_t version = s_sceneFileVersionBeforeMacros;
    
    for (const auto s : m_scenes) {
        if ( ! s->getMacroGroup()->isEmpty()) {
            version = s_sceneFileVersionContainingMacros;
            break;
        }
    }
    return version;
}

/**
 * @return The maximum scene file version supported
 * by the scene file.
 */
int32_t
SceneFile::getMaxiumSupportedSceneFileVersion()
{
    return s_sceneFileVersionContainingMacros;
}

/**
 * @return The scene file version before macros were added.
 */
int32_t
SceneFile::getSceneFileVersionBeforeMacros()
{
    return s_sceneFileVersionBeforeMacros;
}



