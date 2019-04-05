
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

#define __WB_MACRO_CUSTOM_OPERATION_INCREMENT_ROTATION_DECLARE__
#include "WbMacroCustomOperationIncrementRotation.h"
#undef __WB_MACRO_CUSTOM_OPERATION_INCREMENT_ROTATION_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "SystemUtilities.h"
#include "ViewingTransformations.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationIncrementRotation
 * \brief Macro custom operation for incremental rotation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationIncrementRotation::WbMacroCustomOperationIncrementRotation()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::INCREMENTAL_ROTATION)
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationIncrementRotation::~WbMacroCustomOperationIncrementRotation()
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
WbMacroCustomOperationIncrementRotation::createCommand()
{
    const int32_t versionOne(1);
    
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         WbMacroCustomOperationTypeEnum::toGuiName(getOperationType()),
                                                                         "Incremental Rotation (degrees) About Screen Axis",
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(WuQMacroDataValueTypeEnum::AXIS,
                              "Screen Axis",
                              "Y");
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Rotation (Degrees)",
                              (float)360.0);
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
WbMacroCustomOperationIncrementRotation::executeCommand(QWidget* parent,
                                                    const WuQMacroExecutorMonitor* /*executorMonitor*/,
                                                    const WuQMacroExecutorOptions* /*executorOptions*/,
                                                    const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 2)) {
        return false;
    }
    const QString axisName(macroCommand->getParameterAtIndex(0)->getValue().toString().toUpper());
    const float incrementalRotation = macroCommand->getParameterAtIndex(1)->getValue().toFloat();

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
    
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }
    Model* model = tabContent->getModelForDisplay();
    if (model == NULL) {
        return false;
    }
    
    

    int32_t mousePressX(0);
    int32_t mousePressY(0);
    int32_t mouseX(0);
    int32_t mouseY(0);
    int32_t mouseDeltaX(0);
    int32_t mouseDeltaY(0);
    bool doMouseFlag(false);
    bool doZFlag(false);
    switch (axis) {
        case Axis::X:
            doMouseFlag = true;
            mousePressX = 50;
            mouseX      = 50;
            mouseDeltaY = incrementalRotation;
            if (mouseDeltaY >= 0.0) {
                mousePressY = 1;
                mouseY = mouseDeltaY + mousePressY;
            }
            else {
                mouseY = 1;
                mousePressY = 1 - mouseDeltaY;
            }
            break;
        case Axis::Y:
            doMouseFlag = true;
            mousePressY = 50;
            mouseY      = 50;
            mouseDeltaX = incrementalRotation;
            if (mouseDeltaX >= 0.0) {
                mousePressX = 1;
                mouseX = mouseDeltaX + mousePressX;
            }
            else {
                mouseX = 1;
                mousePressX = 1 - mouseDeltaX;
            }
            break;
        case Axis::Z:
            if (incrementalRotation != 0.0) {
                doZFlag = true;
            }
            break;
    }

    if (doMouseFlag) {
        BrainOpenGLViewportContent* viewportContent(NULL);
        tabContent->applyMouseRotation(viewportContent,
                                       mousePressX,
                                       mousePressY,
                                       mouseX,
                                       mouseY,
                                       mouseDeltaX,
                                       mouseDeltaY);
    }
    else if (doZFlag) {
        ViewingTransformations* viewingTransform = tabContent->getViewingTransformation();
        Matrix4x4 rotationMatrix = viewingTransform->getRotationMatrix();
        rotationMatrix.rotateZ(incrementalRotation);
        viewingTransform->setRotationMatrix(rotationMatrix);
    }
    
    updateGraphics();
    
    return true;
}
