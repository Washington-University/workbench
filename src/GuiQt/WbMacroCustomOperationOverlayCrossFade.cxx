
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

#define __WB_MACRO_CUSTOM_OPERATION_OVERLAY_CROSS_FADE_DECLARE__
#include "WbMacroCustomOperationOverlayCrossFade.h"
#undef __WB_MACRO_CUSTOM_OPERATION_OVERLAY_CROSS_FADE_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "GuiManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationOverlayCrossFade
 * \brief Custom Macro Command for Surface Interpolation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationOverlayCrossFade::WbMacroCustomOperationOverlayCrossFade()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::OVERLAY_CROSS_FADE)
{
}

/**
 * Destructor.
 */
WbMacroCustomOperationOverlayCrossFade::~WbMacroCustomOperationOverlayCrossFade()
{
}

/**
 * Get a new instance of the macro command
 *
 * @return
 *     Pointer to command or NULL if not valid
 *     Use getErrorMessage() for error information if NULL returned
 */
WuQMacroCommand*
WbMacroCustomOperationOverlayCrossFade::createCommand()
{
    WuQMacroCommand* command(NULL);
    const int32_t versionNumber(2);
    switch (versionNumber) {
        case 1:
            command = createCommandVersionOne();
            break;
        case 2:
            command = createCommandVersionTwo();
            break;
    }
    CaretAssert(command);
    
    return command;
}

/**
 * Get a new instance of the macro command for version one
 *
 * @return
 *     Pointer to command or NULL if not valid
 *     Use getErrorMessage() for error information if NULL returned
 */
WuQMacroCommand*
WbMacroCustomOperationOverlayCrossFade::createCommandVersionOne()
{
    const int32_t versionOne(1);
    
    const QString description("Crossfade (blend) from one overlay to another:\n"
                              "(1) The opacity of the \"Fade to Overlay\" is set to zero;\n"
                              "(2) The opacity of the \"Fade from Overlay\" is set to one;\n"
                              "(3) The opacity of the \"Fade to Overlay\" increases until it is one\n"
                              "    and simultaneously, the opacity of the \"Face from Overlay\"\n"
                              "    decreases until it reaches zero.\n");
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         "Overlay CrossFade",
                                                                         description,
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Fade to Overlay",
                              (int)1);
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Fade from Overlay",
                              (int)2);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)10.0);
    }
    else {
        appendToErrorMessage(errorMessage);
    }
    
    return command;
}

/**
 * Get a new instance of the macro command for version two
 *
 * @return
 *     Pointer to command or NULL if not valid
 *     Use getErrorMessage() for error information if NULL returned
 */
WuQMacroCommand*
WbMacroCustomOperationOverlayCrossFade::createCommandVersionTwo()
{
    const int32_t versionTwo(2);
    
    const QString description("Crossfade (blend) from file in overlay to selected file/map:\n"
                              "(1) A copy of the selected overlay is inserted below it; \n"
                              "(2) Selected file/map is placed in new the overlay;\n"
                              "(3) The opacity of selected overlay is decreased until it\n"
                              "    becomes 0.0 revealing the selected file/map;\n"
                              "(4) The selected overlay is removed;\n"
                              "(5) The selected file/map remains displayed in its overlay\n");

    WuQMacroCommandParameter* paramSurfaceOne = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Overlay Number",
                                                                             1);
    paramSurfaceOne->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::OVERLAY_INDEX));
    
    WuQMacroCommandParameter* paramSurfaceTwo = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Fade to File",
                                                                             "");
    paramSurfaceTwo->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::OVERLAY_FILE_NAME_OR_FILE_INDEX));
    
    WuQMacroCommandParameter* paramSurfaceThree = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Fade to Map",
                                                                             "");
    paramSurfaceThree->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::OVERLAY_MAP_NAME_OR_MAP_INDEX));
    

    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionTwo,
                                                                         "none",
                                                                         "Overlay CrossFade",
                                                                         description,
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(paramSurfaceOne);
        command->addParameter(paramSurfaceTwo);
        command->addParameter(paramSurfaceThree);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)2.0);
    }
    else {
        appendToErrorMessage(errorMessage);
    }
    
    return command;

}


/**
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     the macro executor monitor
 * @param executorOptions
 *     the executor options
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationOverlayCrossFade::executeCommand(QWidget* parent,
                                                       const WuQMacroExecutorMonitor* executorMonitor,
                                                       const WuQMacroExecutorOptions* executorOptions,
                                                       const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    bool resultFlag(false);
    switch (macroCommand->getVersion()) {
        case 1:
            resultFlag = executeCommandVersionOne(parent,
                                                  executorOptions,
                                                  macroCommand);
            break;
        case 2:
            resultFlag = executeCommandVersionTwo(parent,
                                                  executorMonitor,
                                                  executorOptions,
                                                  macroCommand);
            break;
        default:
            appendUnsupportedVersionToErrorMessage(macroCommand->getVersion());
            break;
    }
    
    return resultFlag;
}

/**
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorMonitor
 *     The macro executor's monitor
 * @param executorOptions
 *     The executor options,
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationOverlayCrossFade::executeCommandVersionTwo(QWidget* parent,
                                                                 const WuQMacroExecutorMonitor* executorMonitor,
                                                                 const WuQMacroExecutorOptions* executorOptions,
                                                                 const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 4)) {
        return false;
    }
    /*
     * Get overlay indices and subtract one from them since they stat at 1
     */
    const int32_t overlayIndex(macroCommand->getParameterAtIndex(0)->getValue().toInt() - 1);
    const QString mapFileName(macroCommand->getParameterAtIndex(1)->getValue().toString());
    const QString mapName(macroCommand->getParameterAtIndex(2)->getValue().toString());
    const float durationSeconds(macroCommand->getParameterAtIndex(3)->getValue().toFloat());

    BrainBrowserWindow* bbw = qobject_cast<BrainBrowserWindow*>(parent);
    if (bbw == NULL) {
        appendToErrorMessage("Parent for running surface macro is not a browser window");
        return false;
    }
    
    BrowserTabContent* tabContent = bbw->getBrowserTabContent();
    if (tabContent == NULL) {
        appendToErrorMessage("No tab is selected in browser window");
        return false;
    }
    
    Model* model = tabContent->getModelForDisplay();
    if (model == NULL) {
        appendToErrorMessage("No model is displayed");
        return false;
    }
    
    bool validModelTypeFlag(false);
    switch (model->getModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            validModelTypeFlag = true;
            break;
    }
    if ( ! validModelTypeFlag) {
        appendToErrorMessage("For Overlay CrossFace, model must be a brain model");
        return false;
    }
    
    OverlaySet* overlaySet = tabContent->getOverlaySet();
    CaretAssert(overlaySet);
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    if ((overlayIndex < 0)
        || (overlayIndex >= numberOfOverlays)) {
        appendToErrorMessage("Overlay index is invalid.");
    }
    if (overlayIndex == (BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS - 1)) {
        appendToErrorMessage("Selected overlay cannot be the last (bottom-most) overlay");
    }
    if (mapFileName.isEmpty()) {
        appendToErrorMessage("Map File Name is empty.");
    }
    if (mapName.isEmpty()) {
        appendToErrorMessage("Map Name is empty.");
    }
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }
    
    Overlay* overlay = overlaySet->getOverlay(overlayIndex);
    if ( ! overlay->isEnabled()) {
        overlay->setEnabled(true);
    }

    std::vector<CaretMappableDataFile*> mapFiles;
    CaretMappableDataFile* selectedMapFile(NULL);
    int32_t selectedMapIndex(-1);
    overlay->getSelectionData(mapFiles,
                              selectedMapFile,
                              selectedMapIndex);
    
    CaretMappableDataFile* fadeToMapFile(NULL);
    for (auto mf : mapFiles) {
        if (mf->getFileName().endsWith(mapFileName)) {
            fadeToMapFile = mf;
            break;
        }
    }
    if (fadeToMapFile == NULL) {
        appendToErrorMessage("Unable to find data file with name "
                             + mapFileName);
        return false;
    }
    
    int32_t fadeToMapIndex(-1);
    const int32_t numMaps = fadeToMapFile->getNumberOfMaps();
    for (int32_t i = 0; i < numMaps; i++) {
        if (mapName == fadeToMapFile->getMapName(i)) {
            fadeToMapIndex = i;
            break;
        }
    }
    if (fadeToMapIndex < 0) {
        appendToErrorMessage("Unable to find map named "
                             + mapName);
        return false;
    }

    const bool successFlag = performCrossFadeVersionTwo(executorMonitor,
                                                        executorOptions,
                                                        overlaySet,
                                                        overlayIndex,
                                                        fadeToMapFile,
                                                        fadeToMapIndex,
                                                        durationSeconds);

    return successFlag;
}

/**
 * Perform crossfade version two
 *
 * @param executorMonitor
 *     The macro executor's monitor
 * @param executorOptions
 *     The executor options
 * @param overlaySet
 *     OverlaySet for the in the tab
 * @param overlayIndex
 *     Index of the overlay that fades off
 * @param fadeToMapFile
 *     File that is fades on
 * @param fadeToMapIndex
 *     Index of map in file that fades on
 * @param durationSeconds
 *     Total duration for cross fade
 * @return
 *     True if successful, else false
 */
bool
WbMacroCustomOperationOverlayCrossFade::performCrossFadeVersionTwo(const WuQMacroExecutorMonitor* executorMonitor,
                                                                   const WuQMacroExecutorOptions* executorOptions,
                                                                   OverlaySet* overlaySet,
                                                                   const int32_t overlayIndex,
                                                                   CaretMappableDataFile* fadeToMapFile,
                                                                   const int32_t fadeToMapIndex,
                                                                   const float durationSeconds)
{
    CaretAssert(overlaySet);
    CaretAssert(fadeToMapFile);
    CaretAssert((fadeToMapIndex >= 0)
                && (fadeToMapIndex < fadeToMapFile->getNumberOfMaps()));
    
    /*
     * Insert a new overlay below the fade-from overlay
     */
    overlaySet->insertOverlayBelow(overlayIndex);
    
    /*
     * Copy content of fade-from overlay to the new fade-to overlay
     */
    Overlay* fadeFromOverlay = overlaySet->getOverlay(overlayIndex);
    const int32_t nextOverlayIndex = overlayIndex + 1;
    Overlay* fadeToOverlay = overlaySet->getOverlay(nextOverlayIndex);
    fadeToOverlay->copyData(fadeFromOverlay);
    
    /*
     * Load fade-to file and map into the new overlay
     */
    fadeToOverlay->setSelectionData(fadeToMapFile,
                                    fadeToMapIndex);
    
    const float defaultNumberOfSteps(25.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(executorOptions,
                                 defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);
    
    float fadeFromOpacity(fadeFromOverlay->getOpacity());
    const float opacityDelta = fadeFromOpacity / numberOfSteps;
    
    fadeToOverlay->setEnabled(true);
    fadeFromOverlay->setEnabled(true);
    
    /*
     * Iterate while decreasing the opacity of the fade-from overlay
     */
    for (int iStep = 0; iStep < numberOfSteps; iStep++) {
        fadeFromOverlay->setOpacity(fadeFromOpacity);
        updateSurfaceColoring();
        updateUserInterface();
        updateGraphics();
        
        fadeFromOpacity -= opacityDelta;
        if (fadeFromOpacity < 0.0) {
            fadeFromOpacity = 0.0;
        }
        
        if (executorMonitor->testForStop()) {
            appendToErrorMessage(executorMonitor->getStoppedByUserMessage());
            return false;
        }
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }
    
    fadeFromOverlay->setOpacity(0.0);

    /*
     * Remove the fade-from overlay so that the fade-to overlay is visible
     */
    overlaySet->removeDisplayedOverlay(overlayIndex);
    
    updateSurfaceColoring();
    updateUserInterface();
    updateGraphics();
    
    return true;

}



/**
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param executorOptions
 *     The executor options
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationOverlayCrossFade::executeCommandVersionOne(QWidget* parent,
                                                                 const WuQMacroExecutorOptions* executorOptions,
                                                                 const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 3)) {
        return false;
    }
    /*
     * Get overlay indices and subtract one from them since they stat at 1
     */
    const int32_t fadeToOverlayIndex(macroCommand->getParameterAtIndex(0)->getValue().toInt() - 1);
    const int32_t fadeFromOverlayIndex(macroCommand->getParameterAtIndex(1)->getValue().toInt() - 1);
    const float durationSeconds(macroCommand->getParameterAtIndex(2)->getValue().toFloat());

    BrainBrowserWindow* bbw = qobject_cast<BrainBrowserWindow*>(parent);
    if (bbw == NULL) {
        appendToErrorMessage("Parent for running surface macro is not a browser window");
        return false;
    }
    
    BrowserTabContent* tabContent = bbw->getBrowserTabContent();
    if (tabContent == NULL) {
        appendToErrorMessage("No tab is selected in browser window");
        return false;
    }
    
    Model* model = tabContent->getModelForDisplay();
    if (model == NULL) {
        appendToErrorMessage("No model is displayed");
        return false;
    }
    
    bool validModelTypeFlag(false);
    switch (model->getModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            validModelTypeFlag = true;
            break;
    }
    if ( ! validModelTypeFlag) {
        appendToErrorMessage("For Overlay CrossFace, model must be a brain model");
        return false;
    }
    
    OverlaySet* overlaySet = tabContent->getOverlaySet();
    CaretAssert(overlaySet);
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    
    if ((fadeToOverlayIndex < 0)
        || (fadeToOverlayIndex >= numberOfOverlays)) {
        appendToErrorMessage("Fade To Overlay Index is not a valid overlay");
    }
    if ((fadeFromOverlayIndex < 0)
        || (fadeFromOverlayIndex >= numberOfOverlays)) {
        appendToErrorMessage("Fade From Overlay Index is not a valid overlay");
    }
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }
    
    Overlay* fadeToOverlay = overlaySet->getOverlay(fadeToOverlayIndex);
    if ( ! fadeToOverlay->isEnabled()) {
        appendToErrorMessage("Fade To Overlay is not enabled");
    }
    Overlay* fadeFromOverlay = overlaySet->getOverlay(fadeFromOverlayIndex);
    if ( ! fadeFromOverlay->isEnabled()) {
        appendToErrorMessage("Fade From Overlay is not enabled");
    }
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }

    CaretMappableDataFile* fadeToMapFile(NULL);
    int32_t fadeToMapIndex(-1);
    fadeToOverlay->getSelectionData(fadeToMapFile, fadeToMapIndex);
    if (fadeToMapFile == NULL) {
        appendToErrorMessage("Fade To Overlay does not contain a valid data file");
    }
    
    CaretMappableDataFile* fadeFromMapFile(NULL);
    int32_t fadeFromMapIndex(-1);
    fadeToOverlay->getSelectionData(fadeFromMapFile, fadeFromMapIndex);
    if (fadeFromMapFile == NULL) {
        appendToErrorMessage("Fade From Overlay does not contain a valid data file");
    }

    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }

    bool successFlag = performCrossFadeVersionOne(executorOptions,
                                                  fadeToOverlay,
                                                  fadeFromOverlay,
                                                  durationSeconds);

    return successFlag;
}

/**
 * Perform crossfade version one
 *
 * @param executorOptions
 *     The executor options
 * @param fadeToOverlay
 *     Overlay that starts with opacity zero and increases to one
 * @param fadeFromOverlay
 *     Overlay that starts with opacity one and decreases to zero
 * @param durationSeconds
 *     Total duration for cross fade
 * @return
 *     True if successful, else false
 */
bool
WbMacroCustomOperationOverlayCrossFade::performCrossFadeVersionOne(const WuQMacroExecutorOptions* executorOptions,
                                                                   Overlay* fadeToOverlay,
                                                                   Overlay* fadeFromOverlay,
                                                                   const float durationSeconds)
{
    CaretAssert(fadeToOverlay);
    CaretAssert(fadeFromOverlay);
    
    const float defaultNumberOfSteps(25.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(executorOptions,
                                 defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);

    const float opacityDelta = 1.0 / numberOfSteps;
    float fadeToOpacity(0.0);
    float fadeFromOpacity(1.0);

    /*
     * Initialize the opacities
     */

    for (int iStep = 0; iStep < numberOfSteps; iStep++) {
        fadeToOverlay->setOpacity(fadeToOpacity);
        fadeFromOverlay->setOpacity(fadeFromOpacity);
        updateSurfaceColoring();
        updateUserInterface();
        updateGraphics();
        
        fadeToOpacity   += opacityDelta;
        if (fadeToOpacity > 1.0) {
            fadeToOpacity = 1.0;
        }
        fadeFromOpacity -= opacityDelta;
        if (fadeFromOpacity < 0.0) {
            fadeFromOpacity = 0.0;
        }
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }
    
    fadeToOverlay->setOpacity(1.0);
    fadeFromOverlay->setOpacity(0.0);

    updateSurfaceColoring();
    updateUserInterface();
    updateGraphics();

    return true;
}

