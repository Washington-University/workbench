
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
        
        std::vector<Annotation*> annotations = file->getAllAnnotations();
        allAnnotations.insert(allAnnotations.end(),
                              annotations.begin(),
                              annotations.end());
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

