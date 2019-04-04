
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

#define __WB_MACRO_CUSTOM_OPERATION_ANIMATE_ROTATION_DECLARE__
#include "WbMacroCustomOperationAnimateRotation.h"
#undef __WB_MACRO_CUSTOM_OPERATION_ANIMATE_ROTATION_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "SystemUtilities.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationAnimateRotation 
 * \brief Macro custom operation for model rotation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationAnimateRotation::WbMacroCustomOperationAnimateRotation()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::ANIMATE_ROTATION)
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationAnimateRotation::~WbMacroCustomOperationAnimateRotation()
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
WbMacroCustomOperationAnimateRotation::createCommand()
{
    const int32_t versionOne(1);
    
    WuQMacroCommandParameter* paramOne = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::AXIS,
                                                                             "Screen Axis",
                                                                             "Y");
    
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(WbMacroCustomOperationTypeEnum::ANIMATE_ROTATION),
                                                                         versionOne,
                                                                         "none",
                                                                         WbMacroCustomOperationTypeEnum::toGuiName(getOperationType()),
                                                                         "Rotate the Brain Model About a Screen Axis",
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(paramOne);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Total Rotation (Degrees)",
                              (float)360.0);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)15.0);
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
 *     Options for executor
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationAnimateRotation::executeCommand(QWidget* parent,
                                                    const WuQMacroExecutorMonitor* executorMonitor,
                                                    const WuQMacroExecutorOptions* executorOptions,
                                                    const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 3)) {
        return false;
    }
    const QString axisName(macroCommand->getParameterAtIndex(0)->getValue().toString().toUpper());
    const float totalRotation = macroCommand->getParameterAtIndex(1)->getValue().toFloat();
    const float durationSeconds = macroCommand->getParameterAtIndex(2)->getValue().toFloat();

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
    
    if (totalRotation < 0.0) {
        appendToErrorMessage("Total Rotation must be greater than zero.");
    }
    if (durationSeconds < 0.0) {
        appendToErrorMessage("Duration must be greater than zero.");
    }
    
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }
    Model* model = tabContent->getModelForDisplay();
    if (model != NULL) {
    }
    
    const bool successFlag = performRotation(executorMonitor,
                                             executorOptions,
                                             tabContent,
                                             axis,
                                             totalRotation,
                                             durationSeconds);
    return successFlag;
}

/**
 * Perform the rotation
 *
 * @param executorMonitor
 *     the macro executor monitor
 * @param executorOptions
 *     executor options
 * @param tabContent
 *     Content of the tab
 * @param axis
 *     The screen axis of rotation
 * @param totalRotation
 *     Total amount of rotation
 * @param durationSeconds
 *     To time for command to run
 *
 */
bool
WbMacroCustomOperationAnimateRotation::performRotation(const WuQMacroExecutorMonitor* executorMonitor,
                                                     const WuQMacroExecutorOptions* executorOptions,
                                                     BrowserTabContent* tabContent,
                                                     const Axis axis,
                                                     const float totalRotation,
                                                     const float durationSeconds)
{
    const float defaultNumberOfSteps(60.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(executorOptions,
                                 defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);

//    const double rotationIncrement = 1.0;
//    const int32_t rotationStepCount = static_cast<int32_t>(totalRotation / rotationIncrement);
//    const float sleepTimeSeconds = durationSeconds / rotationStepCount;
    const int32_t rotationStepCount = numberOfSteps;
    const float rotationIncrement = totalRotation / numberOfSteps;

    for (int32_t i = 0; i < rotationStepCount; i++) {
        Matrix4x4 rotationMatrix = tabContent->getRotationMatrix();
        switch (axis) {
            case Axis::X:
                rotationMatrix.rotateX(rotationIncrement);
                break;
            case Axis::Y:
                rotationMatrix.rotateY(rotationIncrement);
                break;
            case Axis::Z:
                rotationMatrix.rotateZ(rotationIncrement);
                break;
        }
        
        tabContent->setRotationMatrix(rotationMatrix);
        updateGraphics();
        
        if (executorMonitor->testForStop()) {
            appendToErrorMessage(executorMonitor->getStoppedByUserMessage());
            return false;
        }
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }
    
    return true;
}
