
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

#define __WB_MACRO_CUSTOM_OPERATION_VOLUME_SLICE_INCREMENT_DECLARE__
#include "WbMacroCustomOperationVolumeSliceIncrement.h"
#undef __WB_MACRO_CUSTOM_OPERATION_VOLUME_SLICE_INCREMENT_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SystemUtilities.h"
#include "VolumeMappableInterface.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationVolumeSliceIncrement
 * \brief Macro custom operation incrementing volume slices
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationVolumeSliceIncrement::WbMacroCustomOperationVolumeSliceIncrement()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::VOLUME_SLICE_INCREMENT)
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationVolumeSliceIncrement::~WbMacroCustomOperationVolumeSliceIncrement()
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
WbMacroCustomOperationVolumeSliceIncrement::createCommand()
{
    const int32_t versionOne(1);
    
    
    const QString description("Sequence through the volume slices: \n"
                              "(1) start at the \"selected slice\"; \n"
                              "(2) decrement the slice index to the first slice; \n"
                              "(3) increment the slice index to the last slice; \n"
                              "(4) decrement the slice index returning to the \"selected slice\"");
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         "Volume Slice Increment",
                                                                         description,
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        WuQMacroCommandParameter* paramOne = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                          "Screen Axis",
                                                                          "Z");
        paramOne->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::SCREEN_AXIS));
        command->addParameter(paramOne);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)20.0);
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
WbMacroCustomOperationVolumeSliceIncrement::executeCommand(QWidget* parent,
                                                           const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    validateCorrectNumberOfParameters(macroCommand);
    
    BrainBrowserWindow* bbw = qobject_cast<BrainBrowserWindow*>(parent);
    if (bbw == NULL) {
        appendToErrorMessage("Parent for running surface macro is not a browser window.");
        return false;
    }
    
    BrowserTabContent* tabContent = bbw->getBrowserTabContent();
    if (tabContent == NULL) {
        appendToErrorMessage("No tab is selected in browser window.");
        return false;
    }

    int32_t parameterIndex(0);
    const QString axisName(macroCommand->getParameterAtIndex(parameterIndex)->getValue().toString().toUpper());
    parameterIndex++;
    const float durationSeconds = macroCommand->getParameterAtIndex(parameterIndex)->getValue().toFloat();
    parameterIndex++;

    Axis axis = Axis::X;
    if (axisName == "X") {
        axis = Axis::X;
    }
    else if (axisName == "Y") {
        axis = Axis::Y;
    }
    else if (axisName == "Z") {
        axis = Axis::Z;
    }
    else {
        appendToErrorMessage("Axis named \""
                             + axisName
                             + "\" is invalid.  Use X, Y, or Z.");
    }
    
    if (durationSeconds < 0.0) {
        appendToErrorMessage("Duration must be greater than zero.");
    }
    
    Model* model = tabContent->getModelForDisplay();
    if (model == NULL) {
        appendToErrorMessage("No model for surface rotation");
    }

    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }

    const bool successFlag = performSliceIncrement(tabContent,
                                                   axis,
                                                   durationSeconds);
    return successFlag;
}

/**
 * @param tabContent
 *     Content in the selected tab
 * @param axis
 *     Axis for viewing
 * @param durationSecondes
 *     Duration of time for command to run
 */
bool
WbMacroCustomOperationVolumeSliceIncrement::performSliceIncrement(BrowserTabContent* tabContent,
                                                                  const Axis axis,
                                                                  const float durationSeconds)
{
    ModelVolume* volumeModel(tabContent->getDisplayedVolumeModel());
    ModelWholeBrain* wholeBrainModel(tabContent->getDisplayedWholeBrainModel());
    if ((volumeModel == NULL)
        && (wholeBrainModel == NULL)) {
        appendToErrorMessage("For slice increment, View must be All or Volume");
        return false;
    }
    
    const VolumeMappableInterface* vmi = tabContent->getOverlaySet()->getUnderlayVolume();
    if (vmi == NULL) {
        appendToErrorMessage("No volume is selected as an overlay");
        return false;
    }
    std::vector<int64_t> dims;
    vmi->getDimensions(dims);
    if (dims.size() < 3) {
        appendToErrorMessage("Dimensions are invalid for underlay volume");
    }
    
    int32_t numberOfSlices(0);
    int32_t startingSliceIndex(0);
    switch (axis) {
        case Axis::X:
            numberOfSlices = dims[0];
            startingSliceIndex = tabContent->getSliceIndexParasagittal(vmi);
            break;
        case Axis::Y:
            numberOfSlices = dims[1];
            startingSliceIndex = tabContent->getSliceIndexCoronal(vmi);
            break;
        case Axis::Z:
            numberOfSlices = dims[2];
            startingSliceIndex = tabContent->getSliceIndexAxial(vmi);
            break;
    }
    if (numberOfSlices <= 0) {
        appendToErrorMessage("No slices in underlay volume for selected dimension");
    }
    
    const float defaultNumberOfSteps(numberOfSlices);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);

    enum SliceMode {
        DECREMENT_TO_ZERO,
        INCREMENT_TO_LAST,
        DECREMENT_TO_START
    };
    SliceMode sliceMode = DECREMENT_TO_ZERO;
    
    float sliceIndex = startingSliceIndex;
    float sliceIncrement = numberOfSlices / (numberOfSteps / 2);
    
    bool doneFlag(false);
    while ( ! doneFlag) {
        switch (axis) {
            case Axis::X:
                tabContent->setSliceIndexParasagittal(vmi, sliceIndex);
                break;
            case Axis::Y:
                tabContent->setSliceIndexCoronal(vmi, sliceIndex);
                break;
            case Axis::Z:
                tabContent->setSliceIndexAxial(vmi, sliceIndex);
                break;
        }

        switch (sliceMode) {
            case DECREMENT_TO_START:
                sliceIndex -= sliceIncrement;
                if (sliceIndex <= startingSliceIndex) {
                    doneFlag = true;
                }
                break;
            case DECREMENT_TO_ZERO:
                sliceIndex -= sliceIncrement;
                if (sliceIndex <= 0.0) {
                    sliceIndex = 0;
                    sliceMode = INCREMENT_TO_LAST;
                }
                break;
            case INCREMENT_TO_LAST:
                sliceIndex += sliceIncrement;
                if (sliceIndex >= (numberOfSlices - 1)) {
                    sliceIndex = (numberOfSlices - 1);
                    sliceMode = DECREMENT_TO_START;
                }
                break;
        }
        

        updateGraphics();        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }
    
    switch (axis) {
        case Axis::X:
            tabContent->setSliceIndexParasagittal(vmi, startingSliceIndex);
            break;
        case Axis::Y:
            tabContent->setSliceIndexCoronal(vmi, startingSliceIndex);
            break;
        case Axis::Z:
            tabContent->setSliceIndexAxial(vmi, startingSliceIndex);
            break;
    }
    updateGraphics();
    
    return true;
}
