
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
#include "AnnotationArrangerExecutor.h"
#include "AnnotationBrowserTab.h"
#include "AnnotationClipboard.h"
#include "AnnotationColorBar.h"
#include "AnnotationFile.h"
#include "AnnotationGroup.h"
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationEditingSelectionInformation.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationScaleBar.h"
#include "AnnotationStackingOrderOperation.h"
#include "AnnotationOneCoordinateShape.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventAnnotationChartLabelGet.h"
#include "EventAnnotationBarsGet.h"
#include "EventAnnotationGroupGetWithKey.h"
#include "EventAnnotationTextSubstitutionGetAllGroupIDs.h"
#include "EventAnnotationValidate.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserWindowContent.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "SamplesFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class caret::AnnotationManager 
 * \brief Manager for annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param brain
 *    The brain
 */
AnnotationManager::AnnotationManager(const UserInputModeEnum::Enum userInputMode,
                                     Brain* brain)
: CaretObject(),
m_userInputMode(userInputMode),
m_brain(brain)
{
    CaretAssert(m_brain);
    
    m_clipboard.reset(new AnnotationClipboard(m_brain));
    
    m_annotationsExceptBrowserTabsRedoUndoStack.grabNew(new CaretUndoStack());
    m_annotationsExceptBrowserTabsRedoUndoStack->setUndoLimit(500);
    
    m_browserTabAnnotationsRedoUndoStack.grabNew(new CaretUndoStack());
    m_browserTabAnnotationsRedoUndoStack->setUndoLimit(100);
    
    m_samplesAnnotationsRedoUndoStack.grabNew(new CaretUndoStack());
    m_samplesAnnotationsRedoUndoStack->setUndoLimit(100);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_selectionInformation[i] = new AnnotationEditingSelectionInformation(i);
    }

    m_sceneAssistant = new SceneClassAssistant();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_IDS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_VALIDATE);
}

/**
 * Destructor.
 */
AnnotationManager::~AnnotationManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        delete m_selectionInformation[i];
    }

    m_annotationsExceptBrowserTabsRedoUndoStack->clear();
    m_browserTabAnnotationsRedoUndoStack->clear();
    m_samplesAnnotationsRedoUndoStack->clear();
    
    delete m_sceneAssistant;
}

/**
 * Reset the annotation manager.
 */
void
AnnotationManager::reset()
{
    m_annotationsExceptBrowserTabsRedoUndoStack->clear();
    m_browserTabAnnotationsRedoUndoStack->clear();
    m_samplesAnnotationsRedoUndoStack->clear();
    Annotation::unlockAllPolyhedronsInAllWindows();
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
 * @param errorMessageOut
 *     Output with error information if command fails.
 */
bool
AnnotationManager::applyCommand(AnnotationRedoUndoCommand* command,
                                AString& errorMessageOut)
{
    return applyCommandInWindow(command,
                                -1,
                                errorMessageOut);
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
 * @param windowIndex
 *     Index of window in which command was requested.
 * @param errorMessageOut
 *     Output with error information if command fails.
 */
bool
AnnotationManager::applyCommandInWindow(AnnotationRedoUndoCommand* command,
                                        const int32_t windowIndex,
                                        AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CaretAssert(command);
    
    /*
     * Ignore command if it does not apply to any annotations
     */
    if ( ! command->isValid()) {
        delete command;
        errorMessageOut = "Command was not valid.";
        return false;
    }
    
    /*
     * "Redo" the command and add it to the undo stack
     */
    const bool result = getCommandRedoUndoStack()->pushAndRedo(command,
                                                               windowIndex,
                                                               errorMessageOut);
    return result;
}

/**
 * Deselect all annotations for editing in the given window.
 *
 * @param windowIndex
 *     Index of window for deselection of window annotations.
 */
void
AnnotationManager::deselectAllAnnotationsForEditing(const int32_t windowIndex)
{
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::INVALID:
            break;
        case UserInputModeEnum::Enum::ANNOTATIONS:
        {
            std::vector<AnnotationFile*> annotationFiles;
            m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
            
            for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
                 fileIter != annotationFiles.end();
                 fileIter++) {
                AnnotationFile* file = *fileIter;
                CaretAssert(file);
                
                file->setAllAnnotationsSelectedForEditing(windowIndex,
                                                          false);
            }

            EventAnnotationBarsGet barsEvent;
            EventManager::get()->sendEvent(barsEvent.getPointer());
            std::vector<AnnotationColorBar*> colorBars = barsEvent.getAnnotationColorBars();
            
            for (std::vector<AnnotationColorBar*>::iterator iter = colorBars.begin();
                 iter != colorBars.end();
                 iter++) {
                AnnotationColorBar* cb = *iter;
                cb->setSelectedForEditing(windowIndex,
                                          false);
            }
            
            std::vector<AnnotationScaleBar*> scaleBars = barsEvent.getAnnotationScaleBars();
            for (auto sb : scaleBars) {
                sb->setSelectedForEditing(windowIndex,
                                          false);
            }
            
            EventAnnotationChartLabelGet chartLabelEvent;
            EventManager::get()->sendEvent(chartLabelEvent.getPointer());
            std::vector<Annotation*> chartLabels = chartLabelEvent.getAnnotationChartLabels();
            for (auto label : chartLabels) {
                label->setSelectedForEditing(windowIndex,
                                             false);
            }
        }
            break;
        case UserInputModeEnum::Enum::BORDERS:
            break;
        case UserInputModeEnum::Enum::FOCI:
            break;
        case UserInputModeEnum::Enum::IMAGE:
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
        {
            std::vector<SamplesFile*> samplesFiles(m_brain->getAllSamplesFiles());
            for (auto& sf : samplesFiles) {
                sf->setAllAnnotationsSelectedForEditing(windowIndex,
                                                        false);
            }
//            Annotation::setSelectionLockedPolyhedronInWindow(windowIndex,
//                                                             NULL);
        }
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
        {
            EventBrowserTabGetAll allTabsEvent;
            EventManager::get()->sendEvent(allTabsEvent.getPointer());
            std::vector<BrowserTabContent*> allTabs = allTabsEvent.getAllBrowserTabs();
            for (auto tab : allTabs) {
                tab->getManualLayoutBrowserTabAnnotation()->setSelectedForEditing(windowIndex,
                                                                                  false);
            }
        }
            break;
        case UserInputModeEnum::Enum::VIEW:
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            break;
    }
}

/**
 * Select the given annotation for editing using the given mode.
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
AnnotationManager::selectAnnotationForEditing(const int32_t windowIndex,
                                    const SelectionMode selectionMode,
                                    const bool shiftKeyDownFlag,
                                    Annotation* selectedAnnotation)
{
    switch (selectionMode) {
        case SELECTION_MODE_EXTENDED:
            processExtendedModeSelectionForEditing(windowIndex,
                                         shiftKeyDownFlag,
                                         selectedAnnotation);
            break;
        case SELECTION_MODE_SINGLE:
            processSingleModeSelectionForEditing(windowIndex,
                                       selectedAnnotation);
            break;
    }
    
    /*
     * If the annotation is in a user group, select all
     * annotations in the group.
     */
    if (selectedAnnotation != NULL) {
        const bool selectedStatus = selectedAnnotation->isSelectedForEditing(windowIndex);
        const AnnotationGroupKey groupKey = selectedAnnotation->getAnnotationGroupKey();
        if (groupKey.getGroupType() == AnnotationGroupTypeEnum::USER) {
            EventAnnotationGroupGetWithKey getGroupEvent(groupKey);
            EventManager::get()->sendEvent(getGroupEvent.getPointer());
            
            AnnotationGroup* annotationGroup = getGroupEvent.getAnnotationGroup();
            if (annotationGroup != NULL) {
                annotationGroup->setAllAnnotationsSelectedForEditing(windowIndex,
                                                           selectedStatus);
            }
        }
    }
}

/**
 * Set the annotations for editing to the given annotations in the given window.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectedAnnotations
 *     Annotation that become the selected annotations.
 */
void
AnnotationManager::setAnnotationsForEditing(const int32_t windowIndex,
                                            const std::vector<Annotation*>& selectedAnnotations)
{
    /*
     * If only one annotation for selection
     */
    bool oneDeselectFlag(false);
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        if (selectedAnnotations[0]->isSelectedForEditing(windowIndex)) {
            /*
             * Annotation is selected so we want to turn off its
             * selection status which was done by call
             * to deselectAllAnnotationsForEditing().
             */
            oneDeselectFlag = true;
        }
    }
    
    deselectAllAnnotationsForEditing(windowIndex);
    
    if (oneDeselectFlag) {
        return;
    }
    
    std::set<Annotation*> uniqueAnnotationsSet;
    for (std::vector<Annotation*>::const_iterator annIter = selectedAnnotations.begin();
         annIter != selectedAnnotations.end();
         annIter++) {
        Annotation* ann = *annIter;
        CaretAssert(ann);

        const AnnotationGroupKey groupKey = ann->getAnnotationGroupKey();
        if (groupKey.getGroupType() == AnnotationGroupTypeEnum::USER) {
            EventAnnotationGroupGetWithKey getGroupEvent(groupKey);
            EventManager::get()->sendEvent(getGroupEvent.getPointer());
            
            AnnotationGroup* annotationGroup = getGroupEvent.getAnnotationGroup();
            if (annotationGroup != NULL) {
                std::vector<Annotation*> groupAnns;
                annotationGroup->getAllAnnotations(groupAnns);
                uniqueAnnotationsSet.insert(groupAnns.begin(),
                                            groupAnns.end());
            }
        }
        else {
            uniqueAnnotationsSet.insert(ann);
        }
    }
    
    for (std::set<Annotation*>::iterator annIter = uniqueAnnotationsSet.begin();
         annIter != uniqueAnnotationsSet.end();
         annIter++) {
        Annotation* ann = *annIter;
        ann->setSelectedForEditing(windowIndex,
                                   true);
    }
}


/**
 * Process extended mode annotation selection for editing.
 * 
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param shiftKeyDownFlag
 *     True if the shift key is down.
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processExtendedModeSelectionForEditing(const int32_t windowIndex,
                                                const bool shiftKeyDownFlag,
                                                Annotation* selectedAnnotation)
{
    if (selectedAnnotation != NULL) {
        if (shiftKeyDownFlag) {
            /*
             * When shift key is down, the annotation's selection status
             * is toggled
             */
            selectedAnnotation->setSelectedForEditing(windowIndex,
                                            ! selectedAnnotation->isSelectedForEditing(windowIndex));
        }
        else {
            if (selectedAnnotation->isSelectedForEditing(windowIndex)) {
                /* cannot deselect an annotation WITHOUT shift key down */
            }
            else {
                /*
                 * Clicking an annotation without shift key selects the
                 * annotation but deselects all other annotations.
                 */
                deselectAllAnnotationsForEditing(windowIndex);
                selectedAnnotation->setSelectedForEditing(windowIndex,
                                                true);
            }
        }
    }
    else {
        if (shiftKeyDownFlag) {
            /* do nothing with shift key down and no annotation clicked */
        }
        else {
            deselectAllAnnotationsForEditing(windowIndex);
        }
    }
}

/**
 * Process single mode annotation selection for editing.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectedAnnotation
 *     Annotation that was selected (NULL if no annotation selected).
 */
void
AnnotationManager::processSingleModeSelectionForEditing(const int32_t windowIndex,
                                              Annotation* selectedAnnotation)
{
    deselectAllAnnotationsForEditing(windowIndex);
    
    if (selectedAnnotation != NULL) {
        selectedAnnotation->setSelectedForEditing(windowIndex,
                                        true);
    }
}

/**
 * @return True if all of the selected annotations for editing are deletable.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 */
bool
AnnotationManager::isAnnotationSelectedForEditingDeletable(const int32_t windowIndex) const
{
    bool selectedAnnotationsDeletableFlag = false;
    
    std::vector<Annotation*> selectedAnnotations = getAnnotationsSelectedForEditing(windowIndex);
    if ( ! selectedAnnotations.empty()) {
        selectedAnnotationsDeletableFlag = true;
        for (std::vector<Annotation*>::const_iterator iter = selectedAnnotations.begin();
             iter != selectedAnnotations.end();
             iter++) {
            const Annotation* ann = *iter;
            if ( ! ann->testProperty(Annotation::Property::DELETION)) {
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
    
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
        {
            std::vector<SamplesFile*> samplesFiles(m_brain->getAllSamplesFiles());
            annotationFiles.insert(annotationFiles.end(),
                                   samplesFiles.begin(),
                                   samplesFiles.end());
        }
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            /*
             * Tabs are not in files so nothing to do
             */
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }

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
    
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
        {
            /*
             * Put color bars with annotations
             */
            EventAnnotationBarsGet barsEvent;
            EventManager::get()->sendEvent(barsEvent.getPointer());
            std::vector<AnnotationColorBar*> colorBars = barsEvent.getAnnotationColorBars();
            for (std::vector<AnnotationColorBar*>::iterator iter = colorBars.begin();
                 iter != colorBars.end();
                 iter++) {
                allAnnotations.push_back(*iter);
            }
            
            /*
             * Put scale bars with annotations
             */
            std::vector<AnnotationScaleBar*> scaleBars = barsEvent.getAnnotationScaleBars();
            for (auto sb : scaleBars) {
                allAnnotations.push_back(sb);
            }
            
            EventAnnotationChartLabelGet chartLabelEvent;
            EventManager::get()->sendEvent(chartLabelEvent.getPointer());
            std::vector<Annotation*> chartLabels = chartLabelEvent.getAnnotationChartLabels();
            allAnnotations.insert(allAnnotations.end(),
                                  chartLabels.begin(),
                                  chartLabels.end());
        }
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
        {
            EventBrowserTabGetAll allTabsEvent;
            EventManager::get()->sendEvent(allTabsEvent.getPointer());
            std::vector<BrowserTabContent*> allTabs = allTabsEvent.getAllBrowserTabs();
            for (auto tab : allTabs) {
                allAnnotations.push_back(tab->getManualLayoutBrowserTabAnnotation());
            }
        }
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }

    return allAnnotations;
}

/**
 * Get all annotations that were drawn in the window with the given index and are in the same
 * coordinate space (for tab/window also same tab/window).
 *
 * @param annotation
 * Annotation for space comparison
 * @param windowIndex
 * Index of window
 */
std::vector<Annotation*>
AnnotationManager::getAnnotationsDrawnInSameWindowAndSpace(const Annotation* annotation,
                                                                 const int32_t windowIndex) const
{
    CaretAssert(annotation);
    
    std::vector<Annotation*> allAnnotations = getAllAnnotations();
    
    std::vector<Annotation*> annotationsOut;
    for (auto a : allAnnotations) {
        if (a == annotation) {
            continue;
        }
        if (a->isDrawnInWindowStatus(windowIndex)) {
            if (annotation->isInSameCoordinateSpace(a)) {
                annotationsOut.push_back(a);
            }
        }
    }
    return annotationsOut;
}

/**
 * Get the annotation editing selection information for the given window.
 *
 * @param windowIndex
 *     Index of window.
 * @return 
 *     Annotation selection information.
 */
const
AnnotationEditingSelectionInformation*
AnnotationManager::getAnnotationEditingSelectionInformation(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_selectionInformation, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    
    const bool tileModeFlag = (m_userInputMode == UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING);

    AnnotationEditingSelectionInformation* asi = m_selectionInformation[windowIndex];
    
    std::vector<Annotation*> allAnnotations = getAllAnnotations();
    std::vector<Annotation*> selectedAnnotations;
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        Annotation* annotation = *annIter;        
        if (annotation->isSelectedForEditing(windowIndex)) {
            if (tileModeFlag) {
                if (annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
                    selectedAnnotations.push_back(annotation);
                }
            }
            else if (annotation->getType() != AnnotationTypeEnum::BROWSER_TAB) {
                selectedAnnotations.push_back(annotation);
            }
        }
    }
    
    asi->update(selectedAnnotations);
    
    return asi;
}

/**
 * @return A vector containing all SELECTED annotations for editing
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 */
std::vector<Annotation*>
AnnotationManager::getAnnotationsSelectedForEditing(const int32_t windowIndex) const
{
    std::vector<Annotation*> selectedAnnotations;

    const AnnotationEditingSelectionInformation* asi = getAnnotationEditingSelectionInformation(windowIndex);
    asi->getAnnotationsSelectedForEditing(selectedAnnotations);
    
    return selectedAnnotations;
}

/**
 * @return A vector containing all SELECTED annotations for editing and in the
 * given spaces.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param spaces
 *     Limit returned annotations to annotations in these spaces.
 */
std::vector<Annotation*>
AnnotationManager::getAnnotationsSelectedForEditingInSpaces(const int32_t windowIndex,
                                                  const std::vector<AnnotationCoordinateSpaceEnum::Enum>& spaces) const
{
    std::vector<Annotation*> annotations = getAnnotationsSelectedForEditing(windowIndex);
    
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
 * Get the selected annotations for editing and the files that contain them.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param annotationsAndFileOut
 *    A 'pair' containing a selected annotation and the file that contains the annotation.
 */
void
AnnotationManager::getAnnotationsAndFilesSelectedForEditing(const int32_t windowIndex,
                                                            std::vector<AnnotationAndFile>& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();

    std::vector<AnnotationFile*> annotationFiles;
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
        {
            std::vector<SamplesFile*> samplesFiles(m_brain->getAllSamplesFiles());
            annotationFiles.insert(annotationFiles.end(),
                                   samplesFiles.begin(),
                                   samplesFiles.end());
        }
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            /*
             * Tabs are not in files so nothing to do
             */
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }

    
    for (auto& af : annotationFiles) {
        CaretAssert(af);
        
        std::vector<Annotation*> annotations;
        af->getAllAnnotations(annotations);
        for (std::vector<Annotation*>::iterator annIter = annotations.begin();
             annIter != annotations.end();
             annIter++) {
            Annotation* ann = *annIter;
            if (ann->isSelectedForEditing(windowIndex)) {
                annotationsAndFileOut.emplace_back(ann, af, ann->getAnnotationGroupKey());
            }
        }
    }
}

/**
 * Get the selected annotations for editing and the files that contain them.
 * INCLUDES chart labels.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param annotationsAndFileOut
 *    A 'pair' containing a selected annotation and the file that contains the annotation.
 */
void
AnnotationManager::getAnnotationsAndFilesSelectedForEditingIncludingLabels(const int32_t windowIndex,
                                                                           std::vector<AnnotationAndFile>& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();
    
    if (m_userInputMode == UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING) {
        /* In Tile Editing mode and browser tabs are not in files */
        return;
    }

    getAnnotationsAndFilesSelectedForEditing(windowIndex,
                                     annotationsAndFileOut);
    
    EventAnnotationChartLabelGet chartLabelEvent;
    EventManager::get()->sendEvent(chartLabelEvent.getPointer());
    std::vector<Annotation*> chartLabels = chartLabelEvent.getAnnotationChartLabels();
    AnnotationFile* nullFile = NULL;
    for (auto cl : chartLabels) {
        if (cl->isSelectedForEditing(windowIndex)) {
            annotationsAndFileOut.emplace_back(cl, nullFile, cl->getAnnotationGroupKey());
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
    AnnotationArrangerExecutor arranger(m_userInputMode);
    
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
    AnnotationArrangerExecutor arranger(m_userInputMode);
    
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
    errorMessageOut.clear();
    
    const AnnotationEditingSelectionInformation* selectionInfo = getAnnotationEditingSelectionInformation(windowIndex);
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
    std::vector<Annotation*> annotations = selectionInfo->getAnnotationsSelectedForEditing();
    
    bool validFlag = false;
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
            
            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            command->setModeGroupingGroupAnnotations(annotationGroupKey,
                                                     annotations);
            validFlag = applyCommandInWindow(command,
                                             windowIndex,
                                             errorMessageOut);
        }
            break;
        case AnnotationGroupingModeEnum::REGROUP:
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
            
            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            command->setModeGroupingRegroupAnnotations(annotationGroupKey);
            
            validFlag = applyCommandInWindow(command,
                                             windowIndex,
                                             errorMessageOut);
        }
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
            

            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            command->setModeGroupingUngroupAnnotations(annotationGroupKey);
            
            validFlag = applyCommandInWindow(command,
                                             windowIndex,
                                             errorMessageOut);
        }
            break;
    }
    
    return validFlag;
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
    return getAnnotationEditingSelectionInformation(windowIndex)->isGroupingModeValid(groupingMode);
}

/**
 * Apply a stacking order change
 *
 * @param annotations
 *     The annotations that are reordered
 * @param selectedAnnotation
 *     The selected annotation that is moved in order relative to other annotations
 * @param orderType
 *     Type of ordering
 * @param windowIndex
 *     Index of window
 * @param errorMessageOut
 *     Output with error information
 * @return True is successful, else false
 */
bool
AnnotationManager::applyStackingOrder(const std::vector<Annotation*>& annotations,
                                      const Annotation* selectedAnnotation,
                                      const AnnotationStackingOrderTypeEnum::Enum orderType,
                                      const int32_t windowIndex,
                                      AString& errorMessageOut)
{
    AnnotationStackingOrderOperation operation(AnnotationStackingOrderOperation::Mode::MODE_REQUEST_NEW_ORDER_VALUES,
                                               annotations,
                                               selectedAnnotation,
                                               windowIndex);
    if ( ! operation.runOrdering(orderType,
                                 errorMessageOut)) {
        return false;
    }

    std::vector<AnnotationStackingOrderOperation::NewStackingOrder> results = operation.getNewStackingOrderResults();
    if (results.empty()) {
        return true;
    }
    
    std::vector<Annotation*> stackAnns;
    std::vector<float> stackOrders;
    for (auto& nso : results) {
        stackAnns.push_back(nso.m_annotation);
        stackOrders.push_back(nso.m_newStackOrder);
    }
    
    AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
    command->setModeStackingOrderAnnotations(stackAnns,
                                             stackOrders,
                                             orderType);

    const float resultFlag = applyCommandInWindow(command,
                                                  windowIndex,
                                                  errorMessageOut);
    return resultFlag;
}

/**
 * @return All annotations in the same space as the given annotation.  If the annotation
 * is a browser tab annotation, ALL browser tab annotations are returned.
 * @param annotation
 * The annotation for space matching and NOT in the return annotations.
 */
std::vector<Annotation*>
AnnotationManager::getAnnotationsInSameSpace(const Annotation* annotation)
{
    std::vector<Annotation*> sameSpaceAnns;
    std::vector<Annotation*> allAnns = getAllAnnotations();
    if (annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
        EventBrowserTabGetAll tabEvent;
        EventManager::get()->sendEvent(tabEvent.getPointer());
        std::vector<BrowserTabContent*> allTabs = tabEvent.getAllBrowserTabs();
        for (auto tab : allTabs) {
            AnnotationBrowserTab* abt = tab->getManualLayoutBrowserTabAnnotation();
            if (abt != annotation) {
                sameSpaceAnns.push_back(abt);
            }
        }
    }
    else {
        switch (annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            {
                const AString mediaFileName = annotation->getCoordinate(0)->getMediaFileName();
                for (auto a : allAnns) {
                    if (a == annotation) {
                        continue;
                    }
                    if (a->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL) {
                        if (a->getCoordinate(0)->getMediaFileName() == mediaFileName) {
                            sameSpaceAnns.push_back(a);
                        }
                    }
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::CHART:
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            case AnnotationCoordinateSpaceEnum::SPACER:
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            case AnnotationCoordinateSpaceEnum::SURFACE:
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssert("Supports only annotations in Tab or Window Space");
                return sameSpaceAnns;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
            {
                const int32_t tabIndex = annotation->getTabIndex();
                for (auto a : allAnns) {
                    if (a == annotation) {
                        continue;
                    }
                    if (a->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
                        if (a->getTabIndex() == tabIndex) {
                            sameSpaceAnns.push_back(a);
                        }
                    }
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
            {
                const int32_t windowIndex = annotation->getWindowIndex();
                for (auto a : allAnns) {
                    if (a == annotation) {
                        continue;
                    }
                    if (a->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW) {
                        if (a->getWindowIndex() == windowIndex) {
                            sameSpaceAnns.push_back(a);
                        }
                    }
                }
            }
                break;
        }
    }

    return sameSpaceAnns;
}

/**
 * Move the given window or tab annotation so that it is in front of all other annotations in the same tab or window.
 * The stack ordering algorithm uses the undo system and we do want this operation to be undoable.
 *
 * @param annotation
 *     The  annotation that is moved in front
 * @param errorMessageOut
 *     Output with error information
 * @return True is successful, else false
 */
bool
AnnotationManager::moveTabOrWindowAnnotationToFront(Annotation* annotation,
                                                    AString& errorMessageOut)
{
    CaretAssert(annotation);
    errorMessageOut.clear();
    
    std::vector<Annotation*> sameSpaceAnns = getAnnotationsInSameSpace(annotation);
    if (sameSpaceAnns.empty()) {
        return true;
    }
    
    std::map<float, Annotation*> stackOrderAnnotationMap;
    for (auto a : sameSpaceAnns) {
        AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(a);
        if (abt != NULL) {
            stackOrderAnnotationMap.emplace(static_cast<float>(abt->getStackingOrder()),
                                            a);
        }
        else {
            float z(1.0);
            AnnotationTwoCoordinateShape* oneDim = a->castToTwoCoordinateShape();
            if (oneDim != NULL) {
                float xyz[3];
                oneDim->getStartCoordinate()->getXYZ(xyz);
                z = xyz[2];
            }
            else {
                AnnotationOneCoordinateShape* twoDim = a->castToOneCoordinateShape();
                if (twoDim != NULL) {
                    float xyz[3];
                    twoDim->getCoordinate()->getXYZ(xyz);
                    z = xyz[2];
                }
            }
            stackOrderAnnotationMap.emplace(z,
                                            a);
        }
    }
    
    stackOrderAnnotationMap.emplace(-1,
                                    annotation);
    int32_t orderIndex(1);
    for (auto& soa : stackOrderAnnotationMap) {
        Annotation* a(soa.second);
        AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(a);
        if (abt != NULL) {
            abt->setStackingOrder(orderIndex);
        }
        else {
            AnnotationTwoCoordinateShape* oneDim = a->castToTwoCoordinateShape();
            if (oneDim != NULL) {
                float xyz[3];
                oneDim->getStartCoordinate()->getXYZ(xyz);
                xyz[2] = orderIndex;
                oneDim->getStartCoordinate()->setXYZ(xyz);
                oneDim->getEndCoordinate()->getXYZ(xyz);
                xyz[2] = orderIndex;
                oneDim->getEndCoordinate()->setXYZ(xyz);
            }
            else {
                AnnotationOneCoordinateShape* twoDim = a->castToOneCoordinateShape();
                if (twoDim != NULL) {
                    float xyz[3];
                    twoDim->getCoordinate()->getXYZ(xyz);
                    xyz[2] = orderIndex;
                    twoDim->getCoordinate()->setXYZ(xyz);
                }
            }
        }

        orderIndex++;
    }
    
    return true;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_VALIDATE) {
        EventAnnotationValidate* annEvent = dynamic_cast<EventAnnotationValidate*>(event);
        CaretAssert(annEvent);
        const Annotation* validateAnnotation = annEvent->getAnnotation();
        
        std::vector<Annotation*> allAnns = getAllAnnotations();
        for (const auto ann : allAnns) {
            if (ann == validateAnnotation) {
                annEvent->setAnnotationValid();
                annEvent->setEventProcessed();
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_IDS) {
        EventAnnotationTextSubstitutionGetAllGroupIDs* idEvent(dynamic_cast<EventAnnotationTextSubstitutionGetAllGroupIDs*>(event));
        CaretAssert(idEvent);
        idEvent->setEventProcessed();
        
        std::vector<Annotation*> allAnns = getAllAnnotations();
        for (const auto ann : allAnns) {
            if (ann->getType() == AnnotationTypeEnum::TEXT) {
                const AnnotationText* textAnn(ann->castToTextAnnotation());
                if (textAnn != NULL) {
                    const std::set<AString> groupIDs(textAnn->getTextSubstitutionGroupIDs());
                    for (const auto& gid : groupIDs) {
                        idEvent->addGroupID(gid);
                    }
                }
            }
        }
    }
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
 * @return The annotation clipboard
 */
AnnotationClipboard*
AnnotationManager::getClipboard()
{
    return m_clipboard.get();
}

/**
 * @return The annotation clipboard
 */
const AnnotationClipboard*
AnnotationManager::getClipboard() const
{
    return m_clipboard.get();
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
    
    std::vector<AnnotationFile*> annotationFiles;
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
        {
            std::vector<SamplesFile*> samplesFiles(m_brain->getAllSamplesFiles());
            annotationFiles.insert(annotationFiles.end(),
                                   samplesFiles.begin(),
                                   samplesFiles.end());
        }
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            /*
             * Tabs are not in files so nothing to do
             */
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }

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
                case AnnotationCoordinateSpaceEnum::CHART:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    displayedFlag = true;
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    displayedFlag = true;
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
    CaretUndoStack* undoStackOut(m_annotationsExceptBrowserTabsRedoUndoStack);
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            undoStackOut = m_annotationsExceptBrowserTabsRedoUndoStack;
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            undoStackOut = m_samplesAnnotationsRedoUndoStack;
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            undoStackOut = m_browserTabAnnotationsRedoUndoStack;
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }
    
    return undoStackOut;
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

/**
 * Shrink and expand selected browser tab to fill available space in the window. MANUAL TILE LAYOUT MODE ONLY !
 *
 * @param tabsInWindow
 *     Tabs displayed in the window (may or may not include selected tab)
 * @param windowIndex
 *     Index of window
 * @param errorMessageOut
 *     Contains error information if expansion has error (inability to expand is NOT an error)
 * @return
 *     True no error, else false.
 */
bool
AnnotationManager::shrinkAndExpandSelectedBrowserTabAnnotation(const std::vector<BrowserTabContent*>& tabsInWindow,
                                                               const int32_t windowIndex,
                                                               AString& errorMessageOut)
{
    if (m_userInputMode != UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING) {
        const AString txt("This method should only be called for TILE TABS LAYOUT EDITING");
        CaretLogSevere(txt);
        CaretAssertMessage(0, txt);
        return false;
    }

    errorMessageOut.clear();
    
    AnnotationBrowserTab* selectedTabAnnotation(NULL);
    std::vector<Annotation*> selectedAnnotations = getAnnotationsSelectedForEditing(windowIndex);
    if (selectedAnnotations.size() == 1) {
        CaretAssertVectorIndex(selectedAnnotations, 0);
        selectedTabAnnotation = dynamic_cast<AnnotationBrowserTab*>(selectedAnnotations[0]);
    }
    
    CaretAssert(selectedTabAnnotation);
    
    if (selectedTabAnnotation != NULL) {
        std::vector<const AnnotationBrowserTab*> tabAnnotations;
        for (auto btc : tabsInWindow) {
            const AnnotationBrowserTab* ta = btc->getManualLayoutBrowserTabAnnotation();
            CaretAssert(ta);
            if (ta != selectedTabAnnotation) {
                tabAnnotations.push_back(ta);
            }
        }
        
        std::array<float, 4> newBounds;
        if (AnnotationBrowserTab::shrinkAndExpandToFillEmptySpace(tabAnnotations,
                                                                  selectedTabAnnotation,
                                                                  newBounds)) {
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            undoCommand->setBoundsAll(newBounds[0],
                                      newBounds[1],
                                      newBounds[2],
                                      newBounds[3],
                                      selectedTabAnnotation);
            if (applyCommand(undoCommand,
                             errorMessageOut)) {
                return true;
            }
        }
        else {
            errorMessageOut = "Unable to move/resize tab";
        }
    }
    else {
        errorMessageOut = "Either no annotation is selected or selected annotation is not a browser tab";
    }
    
    return false;
}

/**
 * Export all samples to a surfaces
 * @param samplesFile
 *    Samples files containing the samples
 * @return
 *    FunctionResult containing surfaces
 */
FunctionResultValue<std::vector<Surface*>>
AnnotationManager::exportAllSamplesToSurfaces(const SamplesFile* samplesFile)
{
    CaretAssert(samplesFile);
    
    AString errorMessage;
    std::vector<Surface*> allSurfaces;

    /*
     * All files use the same number of coordinates so that they
     * can be in the same structure even though the extra coordinates
     * are not used in any triangles.
     */
    const int32_t surfaceFileNumCoords(2000);
    const Vector3D wayOutXYZ(999999.0, 999999.0, 999999.0);
    
    std::vector<Annotation*> allAnnotations;
    samplesFile->getAllAnnotations(allAnnotations);
    
    for (Annotation* ann : allAnnotations) {
        AnnotationPolyhedron* polyhedron(ann->castToPolyhedron());
        if (polyhedron != NULL) {
            const int32_t numCoords(polyhedron->getNumberOfCoordinates());
            if (numCoords >= 6) {
                std::vector<Vector3D> coordinates;
                std::vector<AnnotationPolyhedron::Triangle> triangles;
                polyhedron->getCoordinatesAndTriangles(coordinates, triangles);
                const int32_t sampleNumCoords(coordinates.size());
                const int32_t numTriangles(triangles.size());
                
                if (numTriangles >= 5) {
                    std::unique_ptr<Surface> surface(new Surface());
                    surface->setNumberOfNodesAndTriangles(surfaceFileNumCoords, numTriangles);
                    for (int32_t i = 0; i < sampleNumCoords; i++) {
                        surface->setCoordinate(i, polyhedron->getCoordinate(i)->getXYZ());
                    }
                    for (int32_t i = sampleNumCoords; i < surfaceFileNumCoords; i++) {
                        surface->setCoordinate(i, wayOutXYZ);
                    }
                    bool validFlag(true);
                    for (int32_t i = 0; i < numTriangles; i++) {
                        CaretAssertVectorIndex(triangles, i);
                        const int32_t n1(surface->closestNode(triangles[i].m_v1));
                        const int32_t n2(surface->closestNode(triangles[i].m_v2));
                        const int32_t n3(surface->closestNode(triangles[i].m_v3));
                        if ((n1 >= 0)
                            && (n2 >= 0)
                            && (n3 >= 0)) {
                            surface->setTriangle(i,
                                                     n1, n2, n3);
                        }
                        else {
                            errorMessage = "Failed to find a closest node";
                            validFlag = false;
                        }
                    }
                    if (validFlag) {
                        surface->computeNormals();
                        surface->setStructure(StructureEnum::CEREBELLUM);
                        const AString filename(FileInformation::assembleFileComponents(surface->getFilePath(),
                                                                                       polyhedron->getName(),
                                                                                       DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SURFACE)));
                        surface->setFileName(filename);
                        surface->setSurfaceType(SurfaceTypeEnum::ANATOMICAL);
                        surface->setModified();
                        allSurfaces.push_back(surface.release());
                    }
                }
                else {
                    errorMessage = ("A Polyhedron must have at least 5 triangles but has "
                                    + AString::number(numTriangles));
                }
            }
            else {
                errorMessage = ("A Polyhedron must have at least 6 coordintes but has "
                                + AString::number(numCoords));
            }
        }
    }

    return FunctionResultValue<std::vector<Surface*>>(allSurfaces,
                                                      errorMessage,
                                                      errorMessage.isEmpty());
}

