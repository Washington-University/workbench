
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

#define __ANNOTATION_MANAGER_DECLARE__
#include "AnnotationManager.h"
#undef __ANNOTATION_MANAGER_DECLARE__

#include "Annotation.h"
#include "AnnotationFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"


using namespace caret;


    
/**
 * \class caret::AnnotationManager 
 * \brief Manager for annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
AnnotationManager::AnnotationManager(Brain* brain)
: CaretObject(),
m_brain(brain)
{
    CaretAssert(m_brain);
    
    m_clipboardAnnotationFile = NULL;
    m_clipboardAnnotation.grabNew(NULL);
    
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Destructor.
 */
AnnotationManager::~AnnotationManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_sceneAssistant;
}

/**
 * Deselect all annotations.
 */
void
AnnotationManager::deselectAllAnnotations()
{
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        file->setAllAnnotationsSelected(false);
    }
}

/**
 * Delete the given annotation.
 *
 * @param annotation
 *    Annotation that will be deleted.
 */
void
AnnotationManager::deleteAnnotation(Annotation* annotation)
{
    CaretAssert(annotation);
    
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        file->removeAnnotation(annotation);
    }
}

/**
 * Delete all selected annotations.
 */
void
AnnotationManager::deleteSelectedAnnotations()
{
    std::vector<Annotation*> allAnnotations = getSelectedAnnotations();
    
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
             annIter != allAnnotations.end();
             annIter++) {
            Annotation* annotation = *annIter;
            CaretAssert(annotation);
            
            file->removeAnnotation(annotation);
        }
    }
}

/**
 * Select the given annotation using the given mode.
 *
 * @param selectionMode
 *     The annotation selection mode.
 * @param selectedAnnotation
 *     Annotation whose selection is updated.
 *     May be NULL.
 */
void
AnnotationManager::selectAnnotation(const SelectionMode selectionMode,
                                    Annotation* selectedAnnotation)
{
    std::vector<Annotation*> allAnnotations = getAllAnnotations();
    
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        Annotation* annotation = *annIter;
        CaretAssert(annotation);
        
        switch (selectionMode) {
            case SELECTION_MODE_EXTENDED:
                /*
                 * Any number of annotations may be selected.
                 * Just toggle the status of the annotation.
                 */
                if (annotation == selectedAnnotation) {
                    annotation->setSelected( ! annotation->isSelected());
                }
                break;
            case SELECTION_MODE_SINGLE:
                /*
                 * Zero or one annotations may be selected
                 */
                if (annotation == selectedAnnotation) {
                    annotation->setSelected(true);
                }
                else {
                    annotation->setSelected(false);
                }
                break;
        }
    }
}

/**
 * @return A vector containing all annotations.
 */
std::vector<Annotation*>
AnnotationManager::getAllAnnotations() const
{
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    std::vector<Annotation*> allAnnotations;
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        std::cout << "Adding annotations from file " << qPrintable(file->getFileName()) << std::endl;
        
        std::vector<Annotation*> annotations = file->getAllAnnotations();
        if ( ! annotations.empty()) {
            allAnnotations.insert(allAnnotations.end(),
                                  annotations.begin(),
                                  annotations.end());
        }
    }
    
    return allAnnotations;
}

/**
 * @return A vector containing all SELECTED annotations.
 */
std::vector<Annotation*>
AnnotationManager::getSelectedAnnotations() const
{
    std::vector<Annotation*> allAnnotations = getAllAnnotations();
    std::vector<Annotation*> selectedAnnotations;
    
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        Annotation* annotation = *annIter;
        
        if (annotation->isSelected()) {
            selectedAnnotations.push_back(annotation);
        }
    }
    
    return selectedAnnotations;
}


/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationManager::receiveEvent(Event* /*event*/)
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
AnnotationManager::toString() const
{
    return "AnnotationManager";
}

/**
 * @return True if there is an annotation on the clipboard.
 */
bool
AnnotationManager::isAnnotationOnClipboardValid() const
{
    return (m_clipboardAnnotation != NULL);
}

/**
 * @return Pointer to annotation file on clipboard.
 *     If there is not annotation file on the clipboard,
 *     NULL is returned.
 */
AnnotationFile*
AnnotationManager::getAnnotationFileOnClipboard() const
{
    if (m_clipboardAnnotationFile != NULL) {
        /*
         * It is possible that the file has been destroyed.
         * If so, invalidate the file (set it to NULL).
         */
        std::vector<AnnotationFile*> allAnnotationFiles;
        m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
        
        if (std::find(allAnnotationFiles.begin(),
                      allAnnotationFiles.end(),
                      m_clipboardAnnotationFile) == allAnnotationFiles.end()) {
            m_clipboardAnnotationFile = NULL;
        }
    }
    
    return m_clipboardAnnotationFile;
}

/**
 * @return Pointer to annotation on clipboard.
 *     If there is not annotation on the clipboard,
 *     NULL is returned.
 */
const Annotation*
AnnotationManager::getAnnotationOnClipboard() const
{
    return m_clipboardAnnotation;
}

/**
 * @return A copy of the annotation on the clipboard.
 *     If there is not annotation on the clipboard,
 *     NULL is returned.
 */
Annotation*
AnnotationManager::getCopyOfAnnotationOnClipboard() const
{
    if (m_clipboardAnnotation != NULL) {
        return m_clipboardAnnotation->clone();
    }
    
    return NULL;
}

void
AnnotationManager::copyAnnotationToClipboard(const AnnotationFile* annotationFile,
                                             const Annotation* annotation)
{
    m_clipboardAnnotationFile = const_cast<AnnotationFile*>(annotationFile);
    m_clipboardAnnotation.grabNew(annotation->clone());
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
AnnotationManager::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationManager",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
AnnotationManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

