
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
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGetDisplayedDataFiles.h"
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
    
    m_annotationRedoUndoStack.grabNew(new CaretUndoStack());
    m_annotationRedoUndoStack->setUndoLimit(500);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_annotationBeingDrawnInWindow[i] = NULL;
    }

    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Destructor.
 */
AnnotationManager::~AnnotationManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        if (m_annotationBeingDrawnInWindow[i] != NULL) {
            delete m_annotationBeingDrawnInWindow[i];
        }
    }

    m_annotationRedoUndoStack->clear();
    
    delete m_sceneAssistant;
}

/**
 * Reset the annotation manager.
 */
void
AnnotationManager::reset()
{
    m_annotationRedoUndoStack->clear();
}

/**
 * Apply a new command to the selected annotations.  After execution of
 * the command, the command is placed on the undo stack so that the
 * user can undo or redo the command.
 *
 * @param command
 *     Command that will be applied to the selected annotations.
 *     Annotation manager will take ownership of the command and
 *     destroy it at the appropriate time.
 */
void
AnnotationManager::applyCommand(AnnotationRedoUndoCommand* command)
{
     CaretAssert(command);
    
    /*
     * Ignore command if it does not apply to any annotations
     */
    if (command->count() <= 0) {
        delete command;
        return;
    }
    
    /*
     * "Redo" applies the command
     */
    command->redo();

    /*
     * Add command to undo stack
     */
    m_annotationRedoUndoStack->push(command);
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
 * Get the files containing the given annotations.  If a file is not found
 * for an annotation NULL is selected for the file.
 *
 * @param annotations
 *     Annotations for which file is found.
 * @return
 *     Files containing the annotations (NULL entry if file not found).  The
 *     size of this vector will ALWAYS be the same as the size of the 
 *     input vector.
 */
std::vector<AnnotationFile*>
AnnotationManager::getFilesContainingAnnotations(const std::vector<Annotation*> annotations) const
{
    std::vector<AnnotationFile*> allFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allFiles);
    
    std::vector<AnnotationFile*> filesOut;
    
    for (std::vector<Annotation*>::const_iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        Annotation* ann = *annIter;
        
        AnnotationFile* file = NULL;
        for (std::vector<AnnotationFile*>::const_iterator fileIter = allFiles.begin();
             fileIter != allFiles.end();
             fileIter++) {
            AnnotationFile* annFile = *fileIter;
            if (annFile->containsAnnotation(ann)) {
                file = annFile;
                break;
            }
        }
        
        filesOut.push_back(file);
    }

    
    CaretAssert(filesOut.size() == annotations.size());
    return filesOut;
}


/**
 * Select the given annotation using the given mode.
 *
 * @param selectionMode
 *     The annotation selection mode.
 * @param shiftKeyDown
 *     The shift key is down
 * @param selectedAnnotation
 *     Annotation whose selection is updated.
 *     May be NULL.
 */
void
AnnotationManager::selectAnnotation(const SelectionMode selectionMode,
                                    const bool shiftKeyDownFlag,
                                    Annotation* selectedAnnotation)
{
    switch (selectionMode) {
        case SELECTION_MODE_EXTENDED:
            processExtendedModeSelection(shiftKeyDownFlag,
                                         selectedAnnotation);
            break;
        case SELECTION_MODE_SINGLE:
            processSingleModeSelection(selectedAnnotation);
            break;
    }
}

/**
 * Process extended mode annotation selection.
 * 
 * @param shiftKeyDownFlag
 *     True if the shift key is down.
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processExtendedModeSelection(const bool shiftKeyDownFlag,
                                                Annotation* selectedAnnotation)
{
    if (selectedAnnotation != NULL) {
        if (shiftKeyDownFlag) {
            /*
             * When shift key is down, the annotation's selection status
             * is toggled
             */
            selectedAnnotation->setSelected( ! selectedAnnotation->isSelected());
        }
        else {
            if (selectedAnnotation->isSelected()) {
                /* cannot deselect an annotation WITHOUT shift key down */
            }
            else {
                /*
                 * Clicking an annotation without shift key selects the
                 * annotation but deselects all other annotations.
                 */
                deselectAllAnnotations();
                selectedAnnotation->setSelected(true);
            }
        }
    }
    else {
        if (shiftKeyDownFlag) {
            /* do nothing with shift key down and no annotation clicked */
        }
        else {
            deselectAllAnnotations();
        }
    }
}

/**
 * Process single mode annotation selection.
 *
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processSingleModeSelection(Annotation* selectedAnnotation)
{
    deselectAllAnnotations();
    
    if (selectedAnnotation != NULL) {
        selectedAnnotation->setSelected(true);
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
 * Get the selected annotations and the files that contain them.
 *
 * @param annotationsAndFileOut
 *    A 'pair' containing a selected annotation and the file that contains the annotation.
 */
void
AnnotationManager::getSelectedAnnotations(std::vector<std::pair<Annotation*, AnnotationFile*> >& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();

    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        std::vector<Annotation*> annotations = file->getAllAnnotations();
        for (std::vector<Annotation*>::iterator annIter = annotations.begin();
             annIter != annotations.end();
             annIter++) {
            Annotation* ann = *annIter;
            if (ann->isSelected()) {
                annotationsAndFileOut.push_back(std::make_pair(ann, file));
            }
        }
    }
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
 * Get the annotation being drawn in window with the given window index.
 *
 * @param windowIndex
 *     Index of window.
 * @return
 *     Pointer to the annotation (will be NULL if no annotation is being drawn).
 */
const Annotation*
AnnotationManager::getAnnotationBeingDrawnInWindow(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_annotationBeingDrawnInWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_annotationBeingDrawnInWindow[windowIndex];
}

/**
 * Set the annotation being drawn in window with the given window index.
 *
 * @param windowIndex
 *     Index of window.
 * @param annotation
 *     Pointer to the annotation (will be NULL if no annotation is being drawn).
 */
void
AnnotationManager::setAnnotationBeingDrawnInWindow(const int32_t windowIndex,
                                                   const Annotation* annotation)
{
    CaretAssertArrayIndex(m_annotationBeingDrawnInWindow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
   
    if (m_annotationBeingDrawnInWindow[windowIndex] != NULL) {
        delete m_annotationBeingDrawnInWindow[windowIndex];
        m_annotationBeingDrawnInWindow[windowIndex] = NULL;
    }
    
    if (annotation != NULL) {
        m_annotationBeingDrawnInWindow[windowIndex] = annotation->clone();
    }
}

/**
 * Find annotation files that are displayed.
 *
 * @param displayedFilesEvent
 *     Event that queries for displayed data files.
 * @param displayedAnnotationFilesOut
 *     Output that contains annotation files that are displayed.
 */
void
AnnotationManager::getDisplayedAnnotationFiles(EventGetDisplayedDataFiles* /*displayedFilesEvent*/,
                                               std::vector<AnnotationFile*>& displayedAnnotationFilesOut) const
{
    displayedAnnotationFilesOut.clear();
    
    // implementation put on hold
    //std::vector<AnnotationFile*> files;
    //m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(files);

    //const DisplayPropertiesAnnotation* annProps = m_brain->getDisplayPropertiesAnnotation();
}

/**
 * @return Pointer to the command redo undo stack
 */
CaretUndoStack*
AnnotationManager::getCommandRedoUndoStack()
{
    return m_annotationRedoUndoStack;
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



/* ============================================================================================================ */

/**
 * \class caret::AnnotationManagerDeleteUndoCommand
 * \brief Undo command for annotations deleted by AnnotationManager.
 * \ingroup Brain
 */

/**
 * Constructor.
 * 
 * @param annotationFile
 *     File that contained deleted annotation.
 * @param annotation
 *     Annotation that was removed from the file.
 */
AnnotationManagerDeleteUndoCommand::AnnotationManagerDeleteUndoCommand(Brain* brain,
                                                                       AnnotationFile* annotationFile,
                                                                       Annotation* annotation)
{
    CaretAssert(brain);
    CaretAssert(annotationFile);
    CaretAssert(annotation);
 
    m_brain          = brain;
    m_annotationFile = annotationFile;
    m_annotation     = annotation;
}

/**
 * Destructor.
 */
AnnotationManagerDeleteUndoCommand::~AnnotationManagerDeleteUndoCommand()
{
    /*
     * We DO NOT own file
     * We DO own annotation
     * If annotation is NULL, it was "undone".
     * If annotation is NOT NULL, it was not "undone" and must be deleted.
     */
    m_annotationFile = NULL;
    if (m_annotation != NULL) {
        delete m_annotation;
        m_annotation = NULL;
    }
}

/**
 * Operation that "redoes" the command.
 */
void
AnnotationManagerDeleteUndoCommand::redo()
{
    
}

/**
 * Operation that "undoes" the command.
 * This will restore the annotation.
 */
void
AnnotationManagerDeleteUndoCommand::undo()
{
    std::vector<AnnotationFile*> files;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(files);
    
    /*
     * Cannot add annotation to file if the file is not valid
     */
    std::vector<AnnotationFile*>::iterator fileIter = std::find(files.begin(),
                                                                files.end(),
                                                                m_annotationFile);
    if (fileIter != files.end()) {
        m_annotationFile->addAnnotation(m_annotation);
        m_annotation = NULL;
        m_annotationFile = NULL;
    }
    else {
        CaretLogWarning("Attempted to undo the deletion of an annotation "
                        "whose file is no longer valid.  Calling getUndoStack() "
                        "should have prevented this from happenning.");
    }
}

/**
 * @return The annotation file that contained the annotation.
 */
AnnotationFile*
AnnotationManagerDeleteUndoCommand::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * @return The annotation for undeleting it.
 * Method is const since this class "owns" and will delete the annotation.
 * Caller will need to clone the returned annotation.
 */
const Annotation*
AnnotationManagerDeleteUndoCommand::getAnnotation() const
{
    return m_annotation;
}


