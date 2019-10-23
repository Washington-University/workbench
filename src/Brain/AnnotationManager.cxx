
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
#include "AnnotationColorBar.h"
#include "AnnotationFile.h"
#include "AnnotationGroup.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationEditingSelectionInformation.h"
#include "AnnotationTwoDimensionalShape.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventAnnotationChartLabelGet.h"
#include "EventAnnotationColorBarGet.h"
#include "EventAnnotationGroupGetWithKey.h"
#include "EventAnnotationValidate.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserWindowContent.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "EventUserInputModeGet.h"

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
    
    m_annotationsExceptBrowserTabsRedoUndoStack.grabNew(new CaretUndoStack());
    m_annotationsExceptBrowserTabsRedoUndoStack->setUndoLimit(500);
    
    m_browserTabAnnotationsRedoUndoStack.grabNew(new CaretUndoStack());
    m_browserTabAnnotationsRedoUndoStack->setUndoLimit(100);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_annotationBeingDrawnInWindow[i] = NULL;
        m_selectionInformation[i] = new AnnotationEditingSelectionInformation(i);
    }

    m_sceneAssistant = new SceneClassAssistant();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_VALIDATE);
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

    m_annotationsExceptBrowserTabsRedoUndoStack->clear();
    m_browserTabAnnotationsRedoUndoStack->clear();
    
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
}

/**
 * Apply a new command to the selected annotations.  After execution of
 * the command, the command is placed on the undo stack so that the
 * user can undo or redo the command.
 *
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
 * @param command
 *     Command that will be applied to the selected annotations.
 *     Annotation manager will take ownership of the command and
 *     destroy it at the appropriate time.
 * @param errorMessageOut
 *     Output with error information if command fails.
 */
bool
AnnotationManager::applyCommand(const UserInputModeEnum::Enum userInputMode,
                                AnnotationRedoUndoCommand* command,
                                AString& errorMessageOut)
{
    return applyCommandInWindow(userInputMode,
                                command,
                                -1,
                                errorMessageOut);
}

/**
 * Apply a new command to the selected annotations.  After execution of
 * the command, the command is placed on the undo stack so that the
 * user can undo or redo the command.
 *
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
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
AnnotationManager::applyCommandInWindow(const UserInputModeEnum::Enum userInputMode,
                                        AnnotationRedoUndoCommand* command,
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
    const bool result = getCommandRedoUndoStack(userInputMode)->pushAndRedo(command,
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

    EventAnnotationColorBarGet colorBarEvent;
    EventManager::get()->sendEvent(colorBarEvent.getPointer());
    std::vector<AnnotationColorBar*> colorBars = colorBarEvent.getAnnotationColorBars();

    for (std::vector<AnnotationColorBar*>::iterator iter = colorBars.begin();
         iter != colorBars.end();
         iter++) {
        AnnotationColorBar* cb = *iter;
        cb->setSelectedForEditing(windowIndex,
                                  false);
    }
    
    EventAnnotationChartLabelGet chartLabelEvent;
    EventManager::get()->sendEvent(chartLabelEvent.getPointer());
    std::vector<Annotation*> chartLabels = chartLabelEvent.getAnnotationChartLabels();
    for (auto label : chartLabels) {
        label->setSelectedForEditing(windowIndex,
                                     false);
    }
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    std::vector<BrowserTabContent*> allTabs = allTabsEvent.getAllBrowserTabs();
    for (auto tab : allTabs) {
        tab->getManualLayoutBrowserTabAnnotation()->setSelectedForEditing(windowIndex,
                                                                          false);
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
    deselectAllAnnotationsForEditing(windowIndex);
    
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

    EventAnnotationChartLabelGet chartLabelEvent;
    EventManager::get()->sendEvent(chartLabelEvent.getPointer());
    std::vector<Annotation*> chartLabels = chartLabelEvent.getAnnotationChartLabels();
    allAnnotations.insert(allAnnotations.end(),
                          chartLabels.begin(),
                          chartLabels.end());
    
    EventBrowserTabGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    std::vector<BrowserTabContent*> allTabs = allTabsEvent.getAllBrowserTabs();
    for (auto tab : allTabs) {
        allAnnotations.push_back(tab->getManualLayoutBrowserTabAnnotation());
    }
    
    return allAnnotations;
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
    
//    bool manualTileTabsModeFlag(false);
//    std::unique_ptr<EventBrowserWindowContent> windowContentEvent = EventBrowserWindowContent::getWindowContent(windowIndex);
//    if (windowContentEvent) {
//        EventManager::get()->sendEvent(windowContentEvent.get());
//        const BrowserWindowContent* windowContent = windowContentEvent->getBrowserWindowContent();
//        if (windowContent != NULL) {
//            switch (windowContent->getTileTabsConfigurationMode()) {
//                case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
//                    break;
//                case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
//                    break;
//                case TileTabsLayoutConfigurationTypeEnum::MANUAL:
//                    manualTileTabsModeFlag = true;
//                    break;
//            }
//        }
//    }
    
    EventUserInputModeGet modeEvent(windowIndex);
    EventManager::get()->sendEvent(modeEvent.getPointer());
    const bool tileModeFlag = (modeEvent.getUserInputMode() == UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING);

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
                                          std::vector<std::pair<Annotation*, AnnotationFile*> >& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();

    EventUserInputModeGet modeEvent(windowIndex);
    EventManager::get()->sendEvent(modeEvent.getPointer());
    if (modeEvent.getUserInputMode() == UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING) {
        /* In Tile Editing mode and browser tabs are not in files */
        return;
    }
    
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
            if (ann->isSelectedForEditing(windowIndex)) {
                annotationsAndFileOut.push_back(std::make_pair(ann, file));
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
                                                     std::vector<std::pair<Annotation*, AnnotationFile*> >& annotationsAndFileOut) const
{
    annotationsAndFileOut.clear();
    
    EventUserInputModeGet modeEvent(windowIndex);
    EventManager::get()->sendEvent(modeEvent.getPointer());
    if (modeEvent.getUserInputMode() == UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING) {
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
            annotationsAndFileOut.push_back(std::make_pair(cl, nullFile));
        }
    }
}

/**
 * Align annotations.
 * 
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
 * @param arrangerInputs
 *    Inputs to algorithm that aligns the annotations.
 * @param errorMessageOut
 *    Contains error message upon exit.
 * @return
 *    True if successful, false if error.
 */
bool
AnnotationManager::alignAnnotations(const UserInputModeEnum::Enum userInputMode,
                                    const AnnotationArrangerInputs& arrangerInputs,
                                    const AnnotationAlignmentEnum::Enum alignment,
                                    AString& errorMessageOut)
{
    AnnotationArrangerExecutor arranger(userInputMode);
    
    return arranger.alignAnnotations(this,
                                     arrangerInputs,
                                     alignment,
                                     errorMessageOut);
}

/**
 * Align annotations.
 *
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
 * @param arrangerInputs
 *    Inputs to algorithm that aligns the annotations.
 * @param errorMessageOut
 *    Contains error message upon exit.
 * @return
 *    True if successful, false if error.
 */
bool
AnnotationManager::distributeAnnotations(const UserInputModeEnum::Enum userInputMode,
                                         const AnnotationArrangerInputs& arrangerInputs,
                                         const AnnotationDistributeEnum::Enum distribute,
                                         AString& errorMessageOut)
{
    AnnotationArrangerExecutor arranger(userInputMode);
    
    return arranger.distributeAnnotations(this,
                                     arrangerInputs,
                                     distribute,
                                     errorMessageOut);
}

/**
 * Apply given grouping mode valid in the given window.
 *
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
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
AnnotationManager::applyGroupingMode(const UserInputModeEnum::Enum userInputMode,
                                     const int32_t windowIndex,
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
            validFlag = applyCommandInWindow(userInputMode,
                                             command,
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
            
            validFlag = applyCommandInWindow(userInputMode,
                                             command,
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
            
            validFlag = applyCommandInWindow(userInputMode,
                                             command,
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
                case AnnotationCoordinateSpaceEnum::CHART:
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
 *
 * @param userInputMode
 *     The current user input mode which MUST be ANNOTATIONS or TILE_TABS_MANUAL_LAYOUT_EDITING
 */
CaretUndoStack*
AnnotationManager::getCommandRedoUndoStack(const UserInputModeEnum::Enum userInputMode)
{
    CaretUndoStack* undoStackOut(NULL);
    switch (userInputMode) {
        case UserInputModeEnum::ANNOTATIONS:
            undoStackOut = m_annotationsExceptBrowserTabsRedoUndoStack;
            break;
        case UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING:
            undoStackOut = m_browserTabAnnotationsRedoUndoStack;
            break;
        case UserInputModeEnum::BORDERS:
        case UserInputModeEnum::FOCI:
        case UserInputModeEnum::IMAGE:
        case UserInputModeEnum::INVALID:
        case UserInputModeEnum::VIEW:
        case UserInputModeEnum::VOLUME_EDIT:
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
 * Expand selected browser tab to fill available space in the window. MANUAL MODE ONLY !
 *
 * @param tabsInWindow
 *     Tabs displayed in the window (may or may not include selected tab)
 * @param windowIndex
 *     Index of window
 * @param userInputMode
 *     The current user input mode (browser tab always)
 * @param errorMessageOut
 *     Contains error information if expansion has error (inability to expand is NOT an error)
 * @return
 *     True no error, else false.
 */
bool
AnnotationManager::expandSelectedBrowserTabAnnotation(const std::vector<BrowserTabContent*>& tabsInWindow,
                                                      const int32_t windowIndex,
                                                      const UserInputModeEnum::Enum userInputMode,
                                                      AString& errorMessageOut)
{
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
        
        float newBounds[4] { 0, 0, 0, 0 };
        if (AnnotationBrowserTab::expandTab(tabAnnotations,
                                            selectedTabAnnotation,
                                            newBounds)) {
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            undoCommand->setBoundsAll(newBounds[0],
                                      newBounds[1],
                                      newBounds[2],
                                      newBounds[3],
                                      selectedTabAnnotation);
            if (applyCommand(userInputMode,
                             undoCommand,
                             errorMessageOut)) {
                return true;
            }
        }
        else {
            errorMessageOut = "Unable to expand tab";
        }
    }
    else {
        errorMessageOut = "Either no annotation is selected or selected annotation is not a browser tab";
    }
    
    return false;
}

