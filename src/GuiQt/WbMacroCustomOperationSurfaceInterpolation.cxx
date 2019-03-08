
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
#include "EventDataFileAdd.h"
#include "EventDataFileDelete.h"
#include "EventCaretDataFilesGet.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "SpecFile.h"
#include "Surface.h"
#include "WbMacroCustomDataTypeEnum.h"
#include "WbMacroCustomOperationTypeEnum.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WuQMacroExecutorMonitor.h"

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
 * @param executorMonitor
 *     the macro executor monitor
 * @param macroCommand
 *     macro command to run
 * @return
 *     True if command executed successfully, else false
 *     Use getErrorMessage() for error information if false returned
 */
bool
WbMacroCustomOperationSurfaceInterpolation::executeCommand(QWidget* parent,
                                                           const WuQMacroExecutorMonitor* executorMonitor,
                                                           const WuQMacroCommand* macroCommand)
{
    CaretAssert(parent);
    CaretAssert(macroCommand);
    
    if ( ! validateCorrectNumberOfParameters(macroCommand, 3)) {
        return false;
    }
    const QString startSurfaceName(macroCommand->getParameterAtIndex(0)->getValue().toString());
    Surface* startSurface = findSurface(startSurfaceName,
                                        "Starting surface");
    const QString endSurfaceName(macroCommand->getParameterAtIndex(1)->getValue().toString());
    Surface* endSurface = findSurface(endSurfaceName,
                                      "Ending surface");
    const float durationSeconds = macroCommand->getParameterAtIndex(2)->getValue().toFloat();

    
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

    bool successFlag = interpolateSurface(executorMonitor,
                                          tabContent->getTabNumber(),
                                          wholeBrainModel,
                                          startSurface,
                                          endSurface,
                                          durationSeconds);
    
    return successFlag;
}

/**
 * Interpolate from starting to ending surface
 *
 * @param executorMonitor
 *     The macro executor's monitor
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
WbMacroCustomOperationSurfaceInterpolation::interpolateSurface(const WuQMacroExecutorMonitor* executorMonitor,
                                                               const int32_t tabIndex,
                                                               ModelWholeBrain* wholeBrainModel,
                                                               const Surface* startSurface,
                                                               const Surface* endSurface,
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

    createInterpolationSurface(startSurface);
    CaretAssert(m_interpolationSurface);
    
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
     * Put interpolation surface into view
     */
    wholeBrainModel->setSelectedSurfaceType(tabIndex,
                                            m_interpolationSurface->getSurfaceType());
    wholeBrainModel->setSelectedSurface(structure,
                                        tabIndex,
                                        m_interpolationSurface);
    
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
            m_interpolationSurface->setCoordinate(i,
                                      &xyz[i3]);
        }
        m_interpolationSurface->invalidateNormals();
        m_interpolationSurface->computeNormals();
        
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
        
        if (executorMonitor->testForStop()) {
            appendToErrorMessage(executorMonitor->getStoppedByUserMessage());
            return false;
        }
        
        sleepForSecondsAtEndOfIteration(iterationSleepTime);
    }

    /*
     * View ending surface
     */
    /*
     * Put interpolation surface into view
     */
    wholeBrainModel->setSelectedSurfaceType(tabIndex,
                                            endSurface->getSurfaceType());
    wholeBrainModel->setSelectedSurface(structure,
                                        tabIndex,
                                        const_cast<Surface*>(endSurface));

    updateGraphics();
    updateUserInterface();
    
    deleteInterpolationSurface();
    
    return true;
}

/**
 * Create the interpolation surface
 *
 * @param surface
 *     Surface that is copied to create the interpolation surface
 */
void
WbMacroCustomOperationSurfaceInterpolation::createInterpolationSurface(const Surface* surface)
{
    CaretAssert(surface);
    
    std::vector<CaretDataFile*> specFileVector = EventCaretDataFilesGet::getCaretDataFilesForType(DataFileTypeEnum::SPECIFICATION);
    if ( ! specFileVector.empty()) {
        m_specFile = dynamic_cast<SpecFile*>(specFileVector[0]);
        m_specFileModificationStatus = m_specFile->isModified();
    }
    
    m_interpolationSurface = new Surface(*surface);
    CaretAssert(m_interpolationSurface);
    
    FileInformation fileInfo(m_interpolationSurface->getFileName());
    AString path, name, ext;
    fileInfo.getFileComponents(path, name, ext);
    const AString newName = FileInformation::assembleFileComponents(path, "Interpolation", ext);
    m_interpolationSurface->setFileName(newName);
    
    EventDataFileAdd addSurfaceEvent(m_interpolationSurface);
    EventManager::get()->sendEvent(addSurfaceEvent.getPointer());
}

/**
 * Delete the interpolation surface
 */
void
WbMacroCustomOperationSurfaceInterpolation::deleteInterpolationSurface()
{
    if (m_interpolationSurface != NULL) {
        EventDataFileDelete deleteFileEvent(m_interpolationSurface);
        EventManager::get()->sendEvent(deleteFileEvent.getPointer());
    }
    
    if (m_specFile != NULL) {
        if ( ! m_specFileModificationStatus) {
            m_specFile->clearModified();
        }
    }
}

