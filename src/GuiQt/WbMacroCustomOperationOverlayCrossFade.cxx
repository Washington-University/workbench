
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
#include "GuiManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"

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
 * Execute the macro command
 *
 * @param parent
 *     Parent widget for any dialogs
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationOverlayCrossFade::executeCommand(QWidget* parent,
                                                           const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    validateCorrectNumberOfParameters(macroCommand);
    
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
    
    /*
     * Get overlay indices and subtract one from them since they stat at 1
     */
    const int32_t fadeToOverlayIndex(macroCommand->getParameterAtIndex(0)->getValue().toInt() - 1);
    const int32_t fadeFromOverlayIndex(macroCommand->getParameterAtIndex(1)->getValue().toInt() - 1);
    const float durationSeconds(macroCommand->getParameterAtIndex(2)->getValue().toFloat());
    
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

    bool successFlag = performCrossFade(fadeToOverlay,
                                        fadeFromOverlay,
                                        durationSeconds);
    
    return successFlag;
}

/**
 * Interpolate from starting to ending surface
 *
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
WbMacroCustomOperationOverlayCrossFade::performCrossFade(Overlay* fadeToOverlay,
                                                         Overlay* fadeFromOverlay,
                                                         const float durationSeconds)
{
    CaretAssert(fadeToOverlay);
    CaretAssert(fadeFromOverlay);
    
    const float defaultNumberOfSteps(25.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(defaultNumberOfSteps,
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

