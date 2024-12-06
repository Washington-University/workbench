
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __DISPLAY_PROPERTIES_ANNOTATION_EXT_SUBSTITUTION_DECLARE__
#include "DisplayPropertiesAnnotationTextSubstitution.h"
#undef __DISPLAY_PROPERTIES_ANNOTATION_EXT_SUBSTITUTION_DECLARE__

#include "Annotation.h"
#include "AnnotationManager.h"
#include "AnnotationTextSubstitutionFile.h"
#include "Brain.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventAnnotationTextSubstitutionGet.h"
#include "EventAnnotationTextSubstitutionInvalidate.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePathName.h"

using namespace caret;



/**
 * \class caret::DisplayPropertiesAnnotationTextSubstitution
 * \brief Contains display properties for annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DisplayPropertiesAnnotationTextSubstitution::DisplayPropertiesAnnotationTextSubstitution(Brain* parentBrain)
: DisplayProperties(),
m_parentBrain(parentBrain)
{
    CaretAssert(parentBrain);
    
    resetPrivate();
    
    m_sceneAssistant->add("m_enableSubstitutionsFlag",
                          &m_enableSubstitutionsFlag);
}

/**
 * Destructor.
 */
DisplayPropertiesAnnotationTextSubstitution::~DisplayPropertiesAnnotationTextSubstitution()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesAnnotationTextSubstitution::copyDisplayProperties(const int32_t /*sourceTabIndex*/,
                                                                   const int32_t /*targetTabIndex*/)
{
}

/**
 * Reset all settings to default.
 * NOTE: reset() is virtual so can/should not be called from constructor.
 */
void
DisplayPropertiesAnnotationTextSubstitution::resetPrivate()
{
    m_enableSubstitutionsFlag = true;
    m_selectedFile = NULL;
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesAnnotationTextSubstitution::reset()
{
    resetPrivate();
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesAnnotationTextSubstitution::update()
{
    
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
DisplayPropertiesAnnotationTextSubstitution::receiveEvent(Event* /*event*/)
{
}

/**
 * @return Status for enabling text annotation substitutions
 */
bool
DisplayPropertiesAnnotationTextSubstitution::isEnableSubstitutions() const
{
    return m_enableSubstitutionsFlag;
}

/**
 * Set status for enabling text annotation substitutions
 *
 * @param status
 *     New display status.
 */
void
DisplayPropertiesAnnotationTextSubstitution::setEnableSubstitutions(const bool status)
{
    m_enableSubstitutionsFlag = status;
}

/**
 * @return The selected file or NULL if no files available.
 */
AnnotationTextSubstitutionFile*
DisplayPropertiesAnnotationTextSubstitution::getSelectedFile()
{
    validateSelectedFile();
    return m_selectedFile;
}

/**
 * @return The selected file or NULL if no files available.
 */
const AnnotationTextSubstitutionFile*
DisplayPropertiesAnnotationTextSubstitution::getSelectedFile() const
{
    validateSelectedFile();
    return m_selectedFile;
}

/**
 * Validate the seleted annotation text sutbstitution file and if invalid, choose any
 * valid file.
 */
void
DisplayPropertiesAnnotationTextSubstitution::validateSelectedFile() const
{
    AnnotationTextSubstitutionFile* previousFile = m_selectedFile;
    
    std::vector<AnnotationTextSubstitutionFile*> files;
    m_parentBrain->getAnnotationTextSubstitutionFiles(files);
    
    if (std::find(files.begin(),
                  files.end(),
                  m_selectedFile) == files.end()) {
        m_selectedFile = NULL;
    }
    
    if (m_selectedFile == NULL) {
        if ( ! files.empty()) {
            CaretAssertVectorIndex(files, 0);
            m_selectedFile = files[0];
            CaretAssert(m_selectedFile);
        }
    }
    
    if (m_selectedFile != previousFile) {
        EventManager::get()->sendEvent(EventAnnotationTextSubstitutionInvalidate().getPointer());
    }
}

/**
 * Set the selected annotation text substitution file.
 *
 * @param selectedFile
 *     The selected file.
 */
void
DisplayPropertiesAnnotationTextSubstitution::setSelectedFile(AnnotationTextSubstitutionFile* selectedFile)
{
    if (selectedFile != m_selectedFile) {
        EventManager::get()->sendEvent(EventAnnotationTextSubstitutionInvalidate().getPointer());
    }
    m_selectedFile = selectedFile;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
DisplayPropertiesAnnotationTextSubstitution::saveToScene(const SceneAttributes* sceneAttributes,
                                         const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesAnnotationTextSubstitution",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    AnnotationTextSubstitutionFile* selectedFile = getSelectedFile();
    if (selectedFile != NULL) {
        sceneClass->addPathName("selectedFile",
                                selectedFile->getFileName());
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
DisplayPropertiesAnnotationTextSubstitution::restoreFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);

    m_selectedFile = NULL;
    
    const ScenePathName* filePathName = sceneClass->getPathName("selectedFile");
    if (filePathName != NULL) {
        const AString filename = filePathName->stringValue();
        
        FileInformation fileInfo(filename);
        const AString fileNameNoPath = fileInfo.getFileName();
        
        AnnotationTextSubstitutionFile* fileFullPath = NULL;
        AnnotationTextSubstitutionFile* fileNoPath   = NULL;
        std::vector<AnnotationTextSubstitutionFile*> allFiles;
        m_parentBrain->getAnnotationTextSubstitutionFiles(allFiles);
        for (auto file : allFiles) {
            if (file->getFileName() == filename) {
                fileFullPath = file;
                break;
            }
            if (file->getFileNameNoPath() == fileNameNoPath) {
                fileNoPath = file;
            }
        }
        
        if (fileFullPath != NULL) {
            m_selectedFile = fileFullPath;
        }
        else if (fileNoPath != NULL) {
            m_selectedFile = fileNoPath;
        }
    }

    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
}

