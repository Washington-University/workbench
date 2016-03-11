
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

//#include <algorithm>
//#include <limits>
//#include <numeric>

#define __ANNOTATION_MANAGER_DECLARE__
#include "AnnotationManager.h"
#undef __ANNOTATION_MANAGER_DECLARE__

#include "Annotation.h"
#include "AnnotationArrangerExecutor.h"
#include "AnnotationColorBar.h"
#include "AnnotationFile.h"
#include "AnnotationGroup.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationSelectionInformation.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventAnnotationColorBarGet.h"
#include "EventAnnotationGroupGetWithKey.h"
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
        m_selectionInformation[i] = new AnnotationSelectionInformation(i);
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
        
        delete m_selectionInformation[i];
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
 *
 * @param windowIndex
 *     Index of window for deselection of window annotations.
 */
void
AnnotationManager::deselectAllAnnotations(const int32_t windowIndex)
{
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        file->setAllAnnotationsSelected(windowIndex,
                                        false);
    }

    EventAnnotationColorBarGet colorBarEvent;
    EventManager::get()->sendEvent(colorBarEvent.getPointer());
    std::vector<AnnotationColorBar*> colorBars = colorBarEvent.getAnnotationColorBars();

    for (std::vector<AnnotationColorBar*>::iterator iter = colorBars.begin();
         iter != colorBars.end();
         iter++) {
        AnnotationColorBar* cb = *iter;
        cb->setSelected(windowIndex,
                        false);
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
    CaretAssertMessage(0, "Use annotation's getAnnotationFile() method.");
//    std::vector<AnnotationFile*> allFiles;
//    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allFiles);
    
    std::vector<AnnotationFile*> filesOut;
    
//    for (std::vector<Annotation*>::const_iterator annIter = annotations.begin();
//         annIter != annotations.end();
//         annIter++) {
//        Annotation* ann = *annIter;
//        
//        AnnotationFile* file = NULL;
//        for (std::vector<AnnotationFile*>::const_iterator fileIter = allFiles.begin();
//             fileIter != allFiles.end();
//             fileIter++) {
//            AnnotationFile* annFile = *fileIter;
//            if (annFile->containsAnnotation(ann)) {
//                file = annFile;
//                break;
//            }
//        }
//        
//        filesOut.push_back(file);
//    }

    
    CaretAssert(filesOut.size() == annotations.size());
    return filesOut;
}


/**
 * Select the given annotation using the given mode.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectionMode
 *     The annotation selection mode.
 * @param shiftKeyDown
 *     The shift key is down
 * @param selectedAnnotation
 *     Annotation whose selection is updated.
 *     May be NULL.
 */
void
AnnotationManager::selectAnnotation(const int32_t windowIndex,
                                    const SelectionMode selectionMode,
                                    const bool shiftKeyDownFlag,
                                    Annotation* selectedAnnotation)
{
    switch (selectionMode) {
        case SELECTION_MODE_EXTENDED:
            processExtendedModeSelection(windowIndex,
                                         shiftKeyDownFlag,
                                         selectedAnnotation);
            break;
        case SELECTION_MODE_SINGLE:
            processSingleModeSelection(windowIndex,
                                       selectedAnnotation);
            break;
    }
    
    /*
     * If the annotation is in a user group, select all
     * annotations in the group.
     */
    if (selectedAnnotation != NULL) {
        const bool selectedStatus = selectedAnnotation->isSelected(windowIndex);
        const AnnotationGroupKey groupKey = selectedAnnotation->getAnnotationGroupKey();
        if (groupKey.getGroupType() == AnnotationGroupTypeEnum::USER) {
            EventAnnotationGroupGetWithKey getGroupEvent(groupKey);
            EventManager::get()->sendEvent(getGroupEvent.getPointer());
            
            AnnotationGroup* annotationGroup = getGroupEvent.getAnnotationGroup();
            if (annotationGroup != NULL) {
                annotationGroup->setAllAnnotationsSelected(windowIndex,
                                                           selectedStatus);
            }
        }
    }
}

/**
 * Process extended mode annotation selection.
 * 
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param shiftKeyDownFlag
 *     True if the shift key is down.
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processExtendedModeSelection(const int32_t windowIndex,
                                                const bool shiftKeyDownFlag,
                                                Annotation* selectedAnnotation)
{
    if (selectedAnnotation != NULL) {
        if (shiftKeyDownFlag) {
            /*
             * When shift key is down, the annotation's selection status
             * is toggled
             */
            selectedAnnotation->setSelected(windowIndex,
                                            ! selectedAnnotation->isSelected(windowIndex));
        }
        else {
            if (selectedAnnotation->isSelected(windowIndex)) {
                /* cannot deselect an annotation WITHOUT shift key down */
            }
            else {
                /*
                 * Clicking an annotation without shift key selects the
                 * annotation but deselects all other annotations.
                 */
                deselectAllAnnotations(windowIndex);
                selectedAnnotation->setSelected(windowIndex,
                                                true);
            }
        }
    }
    else {
        if (shiftKeyDownFlag) {
            /* do nothing with shift key down and no annotation clicked */
        }
        else {
            deselectAllAnnotations(windowIndex);
        }
    }
}

/**
 * Process single mode annotation selection.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processSingleModeSelection(const int32_t windowIndex,
                                              Annotation* selectedAnnotation)
{
    deselectAllAnnotations(windowIndex);
    
    if (selectedAnnotation != NULL) {
        selectedAnnotation->setSelected(windowIndex,
                                        true);
    }
}

/**
 * @return True if all of the selected annotations are deletable.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 */
bool
AnnotationManager::isSelectedAnnotationsDeletable(const int32_t windowIndex) const
{
    bool selectedAnnotationsDeletableFlag = false;
    
    std::vector<Annotation*> selectedAnnotations = getSelectedAnnotations(windowIndex);
    if ( ! selectedAnnotations.empty()) {
        selectedAnnotationsDeletableFlag = true;
        for (std::vector<Annotation*>::const_iterator iter = selectedAnnotations.begin();
             iter != selectedAnnotations.end();
             iter++) {
            const Annotation* ann = *iter;
            if ( ! ann->isDeletable()) {
                selectedAnnotationsDeletableFlag = false;
                break;
            }
        }
    }
    
    return selectedAnnotationsDeletableFlag;
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
        
        std::vector<Annotation*> annotations;
        file->getAllAnnotations(annotations);
        if ( ! annotations.empty()) {
            allAnnotations.insert(allAnnotations.end(),
                                  annotations.begin(),
                                  annotations.end());
        }
    }

    EventAnnotationColorBarGet colorBarEvent;
    EventManager::get()->sendEvent(colorBarEvent.getPointer());
    std::vector<AnnotationColorBar*> colorBars = colorBarEvent.getAnnotationColorBars();
    
    for (std::vector<AnnotationColorBar*>::iterator iter = colorBars.begin();
         iter != colorBars.end();
         iter++) {
        allAnnotations.push_back(*iter);
    }

    return allAnnotations;
}

/**
 * Get the annotation selection information for the given window.
 *
 * @param windowIndex
 *     Index of window.
 * @return 
 *     Annotation selection information.
 */
const
AnnotationSelectionInformation*
AnnotationManager::getSelectionInformation(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_selectionInformation, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    
    AnnotationSelectionInformation* asi = m_selectionInformation[windowIndex];
    
    std::vector<Annotation*> allAnnotations = getAllAnnotations();
    std::vector<Annotation*> selectedAnnotations;
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        Annotation* annotation = *annIter;
        
        if (annotation->isSelected(windowIndex)) {
            selectedAnnotations.push_back(annotation);
        }
    }
    
    asi->update(selectedAnnotations);
    
    return asi;
}

/**
 * @return A vector containing all SELECTED annotations.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 */
std::vector<Annotation*>
AnnotationManager::getSelectedAnnotations(const int32_t windowIndex) const
{
    std::vector<Annotation*> selectedAnnotations;

    const AnnotationSelectionInformation* asi = getSelectionInformation(windowIndex);
    asi->getSelectedAnnotations(selectedAnnotations);
    
    return selectedAnnotations;
    
//    std::vector<Annotation*> allAnnotations = getAllAnnotations();
//    std::vector<Annotation*> selectedAnnotations;
//    
//    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
//         annIter != allAnnotations.end();
//         annIter++) {
//        Annotation* annotation = *annIter;
//        
//        if (annotation->isSelected(windowIndex)) {
//            selectedAnnotations.push_back(annotation);
//        }
//    }
//    
//    return selectedAnnotations;
}

/**
 * @return A vector containing all SELECTED annotations and in the 
 * given spaces.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param spaces
 *     Limit returned annotations to annotations in these spaces.
 */
std::vector<Annotation*>
AnnotationManager::getSelectedAnnotationsInSpaces(const int32_t windowIndex,
                                                  const std::vector<AnnotationCoordinateSpaceEnum::Enum>& spaces) const
{
    std::vector<Annotation*> annotations = getSelectedAnnotations(windowIndex);
    
    std::vector<Annotation*> annotationsOut;
    for (std::vector<Annotation*>::iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* ann = *iter;
        CaretAssert(ann);
        const AnnotationCoordinateSpaceEnum::Enum annSpace = ann->getCoordinateSpace();
        if (std::find(spaces.begin(),
                      spaces.end(),
                      annSpace) != spaces.end()) {
            annotationsOut.push_back(ann);
        }
    }
    
    return annotationsOut;
}



/**
 * Get the selected annotations and the files that contain them.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param annotationsAndFileOut
 *    A 'pair' containing a selected annotation and the file that contains the annotation.
 */
void
AnnotationManager::getSelectedAnnotations(const int32_t windowIndex,
                                          std::vector<std::pair<Annotation*, AnnotationFile*> >& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();

    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* file = *fileIter;
        CaretAssert(file);
        
        std::vector<Annotation*> annotations;
        file->getAllAnnotations(annotations);
        for (std::vector<Annotation*>::iterator annIter = annotations.begin();
             annIter != annotations.end();
             annIter++) {
            Annotation* ann = *annIter;
            if (ann->isSelected(windowIndex)) {
                annotationsAndFileOut.push_back(std::make_pair(ann, file));
            }
        }
    }
}

/**
 * Align annotations.
 * 
 * @param arrangerInputs
 *    Inputs to algorithm that aligns the annotations.
 * @param errorMessageOut
 *    Contains error message upon exit.
 * @return
 *    True if successful, false if error.
 */
bool
AnnotationManager::alignAnnotations(const AnnotationArrangerInputs& arrangerInputs,
                                    const AnnotationAlignmentEnum::Enum alignment,
                                    AString& errorMessageOut)
{
    AnnotationArrangerExecutor arranger;
    
    return arranger.alignAnnotations(this,
                                     arrangerInputs,
                                     alignment,
                                     errorMessageOut);
}

/**
 * Align annotations.
 *
 * @param arrangerInputs
 *    Inputs to algorithm that aligns the annotations.
 * @param errorMessageOut
 *    Contains error message upon exit.
 * @return
 *    True if successful, false if error.
 */
bool
AnnotationManager::distributeAnnotations(const AnnotationArrangerInputs& arrangerInputs,
                                         const AnnotationDistributeEnum::Enum distribute,
                                         AString& errorMessageOut)
{
    AnnotationArrangerExecutor arranger;
    
    return arranger.distributeAnnotations(this,
                                     arrangerInputs,
                                     distribute,
                                     errorMessageOut);
}

/**
 * Apply given grouping mode valid in the given window.
 *
 * @param windowIndex
 *     Index of the window.
 * @param groupingMode
 *     The grouping mode.
 * @param errorMessageOut
 *     Contains error message if grouping fails.
 * @return
 *     True if successful, else false.
 */
bool
AnnotationManager::applyGroupingMode(const int32_t windowIndex,
                                     const AnnotationGroupingModeEnum::Enum groupingMode,
                                     AString& errorMessageOut)
{
    const AnnotationSelectionInformation* selectionInfo = getSelectionInformation(windowIndex);
    CaretAssert(selectionInfo);
    
    if ( ! selectionInfo->isGroupingModeValid(groupingMode)) {
        const QString msg("PROGRAM ERROR: AnnotationMenuArrange::applyGrouping "
                          "should not have been called.  Grouping mode "
                          + AnnotationGroupingModeEnum::toGuiName(groupingMode)
                          + " is invalid for the selected annotations.");
        CaretAssertMessage(0, msg);
        errorMessageOut = msg;
        return false;
    }
    
    std::vector<AnnotationGroupKey> groupKeys = selectionInfo->getSelectedAnnotationGroupKeys();
    std::vector<Annotation*> annotations = selectionInfo->getSelectedAnnotations();
    
    switch (groupingMode) {
        case AnnotationGroupingModeEnum::GROUP:
        {
            if (groupKeys.size() != 1) {
                const QString msg("PROGRAM ERROR: AnnotationMenuArrange::applyGrouping "
                                  "should not have been called.  More than one selected group.");
                CaretAssertMessage(0, msg);
                errorMessageOut = msg;
                return false;
            }
            CaretAssertVectorIndex(groupKeys, 0);
            const AnnotationGroupKey annotationGroupKey = groupKeys[0];
            
            EventAnnotationGrouping groupEvent;
            groupEvent.setModeGroupAnnotations(annotationGroupKey,
                                               annotations);
            EventManager::get()->sendEvent(groupEvent.getPointer());
            
            if (groupEvent.isError()) {
                errorMessageOut = groupEvent.getErrorMessage();
                return false;
            }
        }
            break;
        case AnnotationGroupingModeEnum::REGROUP:
            CaretAssertMessage(0, "REGROUP not implemented");
            break;
        case AnnotationGroupingModeEnum::UNGROUP:
        {
            if (groupKeys.size() != 1) {
                const QString msg("PROGRAM ERROR: AnnotationMenuArrange::applyGrouping "
                                  "should not have been called.  More than one selected group.");
                CaretAssertMessage(0, msg);
                errorMessageOut = msg;
                return false;
            }
            CaretAssertVectorIndex(groupKeys, 0);
            const AnnotationGroupKey annotationGroupKey = groupKeys[0];
            
            EventAnnotationGrouping groupEvent;
            groupEvent.setModeUngroupAnnotations(annotationGroupKey,
                                                 annotations);
            EventManager::get()->sendEvent(groupEvent.getPointer());
            
            if (groupEvent.isError()) {
                errorMessageOut = groupEvent.getErrorMessage();
                return false;
            }
        }
            break;
    }
    return true;
}

/**
 * Is the given grouping mode valid in the given window.
 * 
 * @param windowIndex
 *     Index of the window.
 * @param groupingMode
 *     The grouping mode.
 */
bool
AnnotationManager::isGroupingModeValid(const int32_t windowIndex,
                                       const AnnotationGroupingModeEnum::Enum groupingMode) const
{
    return getSelectionInformation(windowIndex)->isGroupingModeValid(groupingMode);
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
AnnotationManager::getDisplayedAnnotationFiles(EventGetDisplayedDataFiles* displayedFilesEvent,
                                               std::vector<AnnotationFile*>& displayedAnnotationFilesOut) const
{
    displayedAnnotationFilesOut.clear();
    
    const std::vector<int32_t> tabIndices = displayedFilesEvent->getTabIndices();
    
    const DisplayPropertiesAnnotation* annProps = m_brain->getDisplayPropertiesAnnotation();
    const bool stereotaxicDisplayedFlag = annProps->isDisplayModelAnnotationsInTabs(tabIndices);
    const bool surfaceDisplayedFlag     = annProps->isDisplaySurfaceAnnotationsInTabs(tabIndices);
    
    std::vector<AnnotationFile*> annotationFiles;
    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    const int32_t numAnnFiles = static_cast<int32_t>(annotationFiles.size());
    for (int32_t iFile = 0; iFile < numAnnFiles; iFile++) {
        CaretAssertVectorIndex(annotationFiles, iFile);
        const AnnotationFile* annFile = annotationFiles[iFile];
        
        std::vector<Annotation*> annotations;
        annFile->getAllAnnotations(annotations);
        const int32_t numberOfAnnotations = static_cast<int32_t>(annotations.size());
        for (int32_t i = 0; i < numberOfAnnotations; i++) {
            CaretAssertVectorIndex(annotations, i);
            const Annotation* ann = annotations[i];
            CaretAssert(ann);

            bool displayedFlag = false;
            switch (ann->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    if (stereotaxicDisplayedFlag) {
                        displayedFlag = true;
                    }
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    if (surfaceDisplayedFlag) {
                        AnnotationCoordinate* coord = NULL;
                        const AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<const AnnotationTwoDimensionalShape*>(ann);
                        if (twoDimAnn != NULL) {
                            coord = const_cast<AnnotationCoordinate*>(twoDimAnn->getCoordinate());
                        }
                        else {
                            const AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<const AnnotationOneDimensionalShape*>(ann);
                            if (oneDimAnn != NULL) {
                                coord = const_cast<AnnotationCoordinate*>(oneDimAnn->getStartCoordinate());
                            }
                        }
                        if (coord != NULL) {
                            if (displayedFilesEvent->isTestForDisplayedSurfaceStructure(coord->getSurfaceStructure())) {
                                displayedFlag = true;
                            }
                        }
                    }
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    if (displayedFilesEvent->isTestForDisplayedDataFileInTabIndex(ann->getTabIndex())) {
                        if (annProps->isDisplayTabAnnotationsInTab(ann->getTabIndex())) {
                            displayedFlag = true;
                        }
                    }
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    if (displayedFilesEvent->isTestForDisplayedDataFileInWindowIndex(ann->getWindowIndex())) {
                        if (annProps->isDisplayWindowAnnotationsInTab(ann->getWindowIndex())) {
                            displayedFlag = true;
                        }
                    }
                    break;
            }
            
            if (displayedFlag) {
                displayedFilesEvent->addDisplayedDataFile(annFile);
                break;
            }
        }
    }
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



///* ============================================================================================================ */
//
///**
// * \class caret::AnnotationManagerDeleteUndoCommand
// * \brief Undo command for annotations deleted by AnnotationManager.
// * \ingroup Brain
// */
//
///**
// * Constructor.
// * 
// * @param annotationFile
// *     File that contained deleted annotation.
// * @param annotation
// *     Annotation that was removed from the file.
// */
//AnnotationManagerDeleteUndoCommand::AnnotationManagerDeleteUndoCommand(Brain* brain,
//                                                                       AnnotationFile* annotationFile,
//                                                                       Annotation* annotation)
//{
//    CaretAssert(brain);
//    CaretAssert(annotationFile);
//    CaretAssert(annotation);
// 
//    m_brain          = brain;
//    m_annotationFile = annotationFile;
//    m_annotation     = annotation;
//}
//
///**
// * Destructor.
// */
//AnnotationManagerDeleteUndoCommand::~AnnotationManagerDeleteUndoCommand()
//{
//    /*
//     * We DO NOT own file
//     * We DO own annotation
//     * If annotation is NULL, it was "undone".
//     * If annotation is NOT NULL, it was not "undone" and must be deleted.
//     */
//    m_annotationFile = NULL;
//    if (m_annotation != NULL) {
//        delete m_annotation;
//        m_annotation = NULL;
//    }
//}
//
///**
// * Operation that "redoes" the command.
// */
//void
//AnnotationManagerDeleteUndoCommand::redo()
//{
//    
//}
//
///**
// * Operation that "undoes" the command.
// * This will restore the annotation.
// */
//void
//AnnotationManagerDeleteUndoCommand::undo()
//{
//    std::vector<AnnotationFile*> files;
//    m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(files);
//    
//    /*
//     * Cannot add annotation to file if the file is not valid
//     */
//    std::vector<AnnotationFile*>::iterator fileIter = std::find(files.begin(),
//                                                                files.end(),
//                                                                m_annotationFile);
//    if (fileIter != files.end()) {
//        m_annotationFile->addAnnotation(m_annotation);
//        m_annotation = NULL;
//        m_annotationFile = NULL;
//    }
//    else {
//        CaretLogWarning("Attempted to undo the deletion of an annotation "
//                        "whose file is no longer valid.  Calling getUndoStack() "
//                        "should have prevented this from happenning.");
//    }
//}
//
///**
// * @return The annotation file that contained the annotation.
// */
//AnnotationFile*
//AnnotationManagerDeleteUndoCommand::getAnnotationFile() const
//{
//    return m_annotationFile;
//}
//
///**
// * @return The annotation for undeleting it.
// * Method is const since this class "owns" and will delete the annotation.
// * Caller will need to clone the returned annotation.
// */
//const Annotation*
//AnnotationManagerDeleteUndoCommand::getAnnotation() const
//{
//    return m_annotation;
//}


