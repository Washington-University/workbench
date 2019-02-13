
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

#define __WB_MACRO_CUSTOM_OPERATION_SURFACE_INTERPOLATION_DECLARE__
#include "WbMacroCustomOperationSurfaceInterpolation.h"
#undef __WB_MACRO_CUSTOM_OPERATION_SURFACE_INTERPOLATION_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "Surface.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"

using namespace caret;


    
/**
 * \class caret::WbMacroCustomOperationSurfaceInterpolation 
 * \brief Custom Macro Command for Surface Interpolation
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WbMacroCustomOperationSurfaceInterpolation::WbMacroCustomOperationSurfaceInterpolation()
: WbMacroCustomOperationBase(WbMacroCustomOperationTypeEnum::SURFACE_INTERPOLATION)
{
}

/**
 * Destructor.
 */
WbMacroCustomOperationSurfaceInterpolation::~WbMacroCustomOperationSurfaceInterpolation()
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
WbMacroCustomOperationSurfaceInterpolation::createCommand()
{
    const int32_t versionOne(1);
    
    WuQMacroCommandParameter* paramSurfaceOne = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Starting Surface",
                                                                             "");
    paramSurfaceOne->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::SURFACE));
    
    WuQMacroCommandParameter* paramSurfaceTwo = new WuQMacroCommandParameter(WuQMacroDataValueTypeEnum::CUSTOM_DATA,
                                                                             "Ending Surface",
                                                                             "");
    paramSurfaceTwo->setCustomDataType(WbMacroCustomDataTypeEnum::toName(WbMacroCustomDataTypeEnum::SURFACE));
    
    QString errorMessage;
    WuQMacroCommand* command = WuQMacroCommand::newInstanceCustomCommand(WbMacroCustomOperationTypeEnum::toName(WbMacroCustomOperationTypeEnum::SURFACE_INTERPOLATION),
                                                                         versionOne,
                                                                         "none",
                                                                         "Surface Interpolation",
                                                                         "Interpolate Between Two Surface",
                                                                         1.0,
                                                                         errorMessage);
    if (command != NULL) {
        command->addParameter(paramSurfaceOne);
        command->addParameter(paramSurfaceTwo);
        command->addParameter(WuQMacroDataValueTypeEnum::FLOAT,
                              "Duration (secs)",
                              (float)5.0);
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
WbMacroCustomOperationSurfaceInterpolation::executeCommand(QWidget* parent,
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
    
    ModelWholeBrain* wholeBrainModel = tabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel == NULL) {
        appendToErrorMessage("View selected is not ALL view");
        return false;
    }
    
    const QString startSurfaceName(macroCommand->getParameterAtIndex(0)->getValue().toString());
    Surface* startSurface = findSurface(startSurfaceName,
                                        "Starting surface");
    
    const QString endSurfaceName(macroCommand->getParameterAtIndex(1)->getValue().toString());
    Surface* endSurface = findSurface(endSurfaceName,
                                      "Ending surface");
    
    const float durationSeconds = macroCommand->getParameterAtIndex(2)->getValue().toFloat();
    
    if ((startSurface != NULL)
        && (endSurface != NULL)) {
        if (startSurface == endSurface) {
            appendToErrorMessage("Starting and ending surfaces are the same surfaces");
        }
        if (startSurface->getStructure() != endSurface->getStructure()) {
            appendToErrorMessage("The surfaces' structures are different");
        }
        if (startSurface->getNumberOfNodes() != endSurface->getNumberOfNodes()) {
            appendToErrorMessage("The surfaces contain a different number of vertices");
        }
        switch (startSurface->getStructure()) {
            case StructureEnum::CEREBELLUM:
            case StructureEnum::CORTEX_LEFT:
            case StructureEnum::CORTEX_RIGHT:
                break;
            default:
                appendToErrorMessage("Supported surface structures are: "
                                     + StructureEnum::toGuiName(StructureEnum::CEREBELLUM) + ", "
                                     + StructureEnum::toGuiName(StructureEnum::CORTEX_LEFT) + ", "
                                     + StructureEnum::toGuiName(StructureEnum::CORTEX_RIGHT));
                break;
        }
    }
    
    if ( ! getErrorMessage().isEmpty()) {
        return false;
    }

    bool successFlag = interpolateSurface(tabContent->getTabNumber(),
                                          wholeBrainModel,
                                          startSurface,
                                          endSurface,
                                          durationSeconds);
    
    return successFlag;
}

/**
 * Interpolate from starting to ending surface
 *
 * @param tabIndex
 *     Index of selected tab
 * @param wholeBrainModel
 *     The whole brain model
 * @param startSurface
 *     The starting surface
 * @param endSurface
 *     The ending surface
 * @param durationSeconds
 *     Total duration for surface interpolation
 * @return
 *     True if successful, else false
 */
bool
WbMacroCustomOperationSurfaceInterpolation::interpolateSurface(const int32_t tabIndex,
                                                               ModelWholeBrain* wholeBrainModel,
                                                               Surface* startSurface,
                                                               Surface* endSurface,
                                                               const float durationSeconds)
{
    CaretAssert(wholeBrainModel);
    CaretAssert(startSurface);
    CaretAssert(endSurface);
    
    const float defaultNumberOfSteps(50.0);
    float numberOfSteps(0.0);
    float iterationSleepTime(0.0);
    getNumberOfStepsAndSleepTime(defaultNumberOfSteps,
                                 durationSeconds,
                                 numberOfSteps,
                                 iterationSleepTime);

    const StructureEnum::Enum structure = startSurface->getStructure();
    const bool endSurfaceModifiedStatus = endSurface->isModified();
    
    const float* startingXYZ = startSurface->getCoordinateData();
    const float* endingXYZ   = endSurface->getCoordinateData();

    /*
     * XYZ that will be interpolated
     */
    const int32_t numberOfVertices = endSurface->getNumberOfNodes();
    const int32_t numberOfComponents = numberOfVertices * 3;
    
    /*
     * Initialize with starting coordinates
     */
    std::vector<float> xyz(startingXYZ,
                           startingXYZ + numberOfComponents);
    
    /*
     * Amount to move each interpolation iteration
     */
    std::vector<float> deltaXYZ(numberOfComponents);
    for (int32_t i = 0; i < numberOfComponents; i++) {
        const float distance = endingXYZ[i] - startingXYZ[i];
        const float stepDistance = distance / numberOfSteps;
        CaretAssertVectorIndex(deltaXYZ, i);
        deltaXYZ[i] = stepDistance;
    }
    
    /*
     * Replace ending surface's coordinates with starting surfaces coordinates
     */
    for (int32_t i = 0; i < numberOfVertices; i++) {
        const int32_t i3 = i * 3;
        endSurface->setCoordinate(i, &startingXYZ[i3]);
    }
    endSurface->computeNormals();

    /*
     * Put ending surface into view
     */
    wholeBrainModel->setSelectedSurfaceType(tabIndex,
                                            endSurface->getSurfaceType());
    wholeBrainModel->setSelectedSurface(structure,
                                        tabIndex,
                                        endSurface);

    switch (structure) {
        case StructureEnum::CEREBELLUM:
            break;
        case StructureEnum::CORTEX_LEFT:
            break;
        case StructureEnum::CORTEX_RIGHT:
            break;
        default:
            CaretAssert(0);
            break;
    }
    updateUserInterface();
    updateGraphics();

    for (int iStep = 0; iStep < numberOfSteps; iStep++) {
        /*
         * Move coordinate components
         */
        for (int32_t i = 0; i < numberOfComponents; i++) {
            CaretAssertVectorIndex(xyz, i);
            CaretAssertVectorIndex(deltaXYZ, i);
            xyz[i] += deltaXYZ[i];
        }
        
        
        /*
         * Update surface coordinates
         */
        for (int32_t i = 0; i < numberOfVertices; i++) {
            const int32_t i3 = i * 3;
            endSurface->setCoordinate(i,
                                      &xyz[i3]);
        }
        endSurface->computeNormals();
        
        const bool debugFlag(false);
        if (debugFlag) {
            const int32_t vertexIndex = 16764;
            const float* p = endSurface->getCoordinate(vertexIndex);
            std::cout << "XYZ " << iStep << ": "
            << AString::number(p[0]) << " "
            << AString::number(p[1]) << " "
            << AString::number(p[2]) << std::endl;
        }
        
        updateGraphics();
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
//        if ( ! recordingFlag) {
//            SystemUtilities::sleepSeconds(iterationSleepTime);
//        }
    }
    
    /*
     * Restore end surface coordinates
     */
    for (int32_t i = 0; i < numberOfVertices; i++) {
        const int32_t i3 = i * 3;
        endSurface->setCoordinate(i, &endingXYZ[i3]);
    }
    if ( ! endSurfaceModifiedStatus) {
        endSurface->clearModified();
    }
    updateGraphics();
    
    return true;
}

