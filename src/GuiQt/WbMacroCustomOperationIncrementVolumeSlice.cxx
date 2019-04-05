
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

#define __WB_MACRO_CUSTOM_OPERATION_INCREMENT_VOLUME_SLICE_DECLARE__
#include "WbMacroCustomOperationIncrementVolumeSlice.h"
#undef __WB_MACRO_CUSTOM_OPERATION_INCREMENT_VOLUME_SLICE_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "Matrix4x4.h"
#include "Model.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "SystemUtilities.h"
#include "ViewingTransformations.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationIncrementVolumeSlice
 * \brief Macro custom operation for incremental rotation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationIncrementVolumeSlice::WbMacroCustomOperationIncrementVolumeSlice()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::INCREMENTAL_VOLUME_SLICE)
{
    
}

/**
 * Destructor.
 */
WbMacroCustomOperationIncrementVolumeSlice::~WbMacroCustomOperationIncrementVolumeSlice()
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
WbMacroCustomOperationIncrementVolumeSlice::createCommand()
{
    const int32_t versionOne(1);
    
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(getOperationType()),
                                                                         versionOne,
                                                                         "none",
                                                                         WbMacroCustomOperationTypeEnum::toGuiName(getOperationType()),
                                                                         "Increment Volume Slice",
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(WuQMacroDataValueTypeEnum::INTEGER,
                              "Increment Slice Index By",
                              (int)1);
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
WbMacroCustomOperationIncrementVolumeSlice::executeCommand(QWidget* parent,
                                                    const WuQMacroExecutorMonitor* /*executorMonitor*/,
                                                    const WuQMacroExecutorOptions* /*executorOptions*/,
                                                    const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 1)) {
        return false;
    }
    
    const float incrementSlice = macroCommand->getParameterAtIndex(0)->getValue().toFloat();

    BrainBrowserWindow* bbw = qobject_cast<BrainBrowserWindow*>(parent);
    if (bbw == NULL) {
        appendToErrorMessage("Parent for running macro is not a browser window.");
        return false;
    }
    
    BrowserTabContent* tabContent = bbw->getBrowserTabContent();
    if (tabContent == NULL) {
        appendToErrorMessage("No tab is selected in browser window.");
        return false;
    }
    const int32_t tabIndex = tabContent->getTabNumber();

    VolumeMappableInterface* underlayVolumeFile(NULL);
    ModelVolume* volumeModel = tabContent->getDisplayedVolumeModel();

    if (volumeModel != NULL) {
        underlayVolumeFile = volumeModel->getUnderlayVolumeFile(tabIndex);
    }
    ModelWholeBrain* wholeBrainModel = tabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        underlayVolumeFile = wholeBrainModel->getUnderlayVolumeFile(tabIndex);
    }
    
    if (underlayVolumeFile == NULL) {
        appendToErrorMessage("Must have ALL or Volume view for slice increment or no volume is displayed.");
        return false;
    }

    int32_t sliceIndexAxial        = tabContent->getSliceIndexAxial(underlayVolumeFile);
    int32_t sliceIndexCoronal      = tabContent->getSliceIndexCoronal(underlayVolumeFile);
    int32_t sliceIndexParasagittal = tabContent->getSliceIndexParasagittal(underlayVolumeFile);
    
    VolumeSliceViewPlaneEnum::Enum slicePlane = tabContent->getSliceViewPlane();
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            sliceIndexAxial        += incrementSlice;
            sliceIndexCoronal      += incrementSlice;
            sliceIndexParasagittal += incrementSlice;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            sliceIndexAxial += incrementSlice;
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            sliceIndexCoronal += incrementSlice;
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            sliceIndexParasagittal += incrementSlice;
            break;
    }
    
    tabContent->setSliceIndexAxial(underlayVolumeFile,
                                   sliceIndexAxial);
    tabContent->setSliceIndexCoronal(underlayVolumeFile,
                                     sliceIndexCoronal);
    tabContent->setSliceIndexParasagittal(underlayVolumeFile,
                                          sliceIndexParasagittal);
    
    updateGraphics();
    updateUserInterface();

    return true;
}
