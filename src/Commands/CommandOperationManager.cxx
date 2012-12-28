/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <map>

#define __COMMAND_OPERATION_MANAGER_DEFINE__
#include "CommandOperationManager.h"
#undef __COMMAND_OPERATION_MANAGER_DEFINE__

#include "AlgorithmCiftiAverage.h"
#include "AlgorithmCiftiAverageDenseROI.h"
#include "AlgorithmCiftiAverageROICorrelation.h"
#include "AlgorithmCiftiCorrelation.h"
#include "AlgorithmCiftiCorrelationGradient.h"
#include "AlgorithmCiftiCreateDenseTimeseries.h"
#include "AlgorithmCiftiCreateLabel.h"
#include "AlgorithmCiftiGradient.h"
#include "AlgorithmCiftiMerge.h"
#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmCiftiTranspose.h"
#include "AlgorithmCreateSignedDistanceVolume.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmMetricExtrema.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmSignedDistanceToSurface.h"
#include "AlgorithmSurfaceCortexLayer.h"
#include "AlgorithmSurfaceMatch.h"
#include "AlgorithmSurfaceWedgeVolume.h"
#include "AlgorithmVolumeExtrema.h"
#include "AlgorithmVolumeGradient.h"
#include "AlgorithmVolumeParcelResampling.h"
#include "AlgorithmVolumeParcelResamplingGeneric.h"
#include "AlgorithmVolumeParcelSmoothing.h"
#include "AlgorithmVolumeSmoothing.h"
#include "AlgorithmVolumeToSurfaceMapping.h"

#include "OperationAddToSpecFile.h"
#include "OperationBackendAverageDenseROI.h"
#include "OperationBackendAverageROICorrelation.h"
#include "OperationCiftiChangeTimestep.h"
#include "OperationCiftiConvert.h"
#include "OperationCiftiConvertToScalar.h"
#include "OperationCiftiMath.h"
#include "OperationCiftiPalette.h"
#include "OperationCiftiROIAverage.h"
#include "OperationCiftiSeparateAll.h"
#include "OperationConvertMatrix4ToMatrix2.h"
#include "OperationConvertMatrix4ToWorkbenchSparse.h"
#include "OperationEstimateFiberBinghams.h"
#include "OperationFociListCoords.h"
#include "OperationLabelMask.h"
#include "OperationMetricMask.h"
#include "OperationMetricMath.h"
#include "OperationMetricPalette.h"
#include "OperationProbtrackXDotConvert.h"
#include "OperationSetStructure.h"
#include "OperationSpecFileMerge.h"
#include "OperationSurfaceClosestVertex.h"
#include "OperationSurfaceGeodesicROIs.h"
#include "OperationSurfaceInformation.h"
#include "OperationVolumeLabelImport.h"
#include "OperationVolumeMath.h"
#include "OperationVolumePalette.h"
#include "OperationZipSceneFile.h"
#include "OperationZipSpecFile.h"

#include "CommandParser.h"
#include "AlgorithmException.h"
#include "OperationException.h"

#include "CommandClassAddMember.h"
#include "CommandClassCreate.h"
#include "CommandClassCreateAlgorithm.h"
#include "CommandClassCreateEnum.h"
#include "CommandClassCreateOperation.h"
#include "CommandGiftiConvert.h"
#include "CommandNiftiConvert.h"
#include "CommandNiftiInformation.h"
#include "CommandUnitTest.h"
#include "ProgramParameters.h"

#include "CaretLogger.h"

#include <iostream>

using namespace caret;

/**
 * Get the command operation manager.
 *
 * return 
 *   Pointer to the command operation manager.
 */
CommandOperationManager* 
CommandOperationManager::getCommandOperationManager()
{
    if (singletonCommandOperationManager == NULL) {
        singletonCommandOperationManager = 
        new CommandOperationManager();
    }
    return singletonCommandOperationManager;
}

/**
 * Delete the command operation manager.
 */
void 
CommandOperationManager::deleteCommandOperationManager()
{
    if (singletonCommandOperationManager != NULL) {
        delete singletonCommandOperationManager;
        singletonCommandOperationManager = NULL;
    }
}

/**
 * Constructor.
 */
CommandOperationManager::CommandOperationManager()
{
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverage()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverageDenseROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverageROICorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCorrelationGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateDenseTimeseries()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateLabel()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiParcellate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReplaceStructure()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSeparate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiTranspose()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCreateSignedDistanceVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSignedDistanceToSurface()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceCortexLayer()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceMatch()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceWedgeVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResampling()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResamplingGeneric()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeToSurfaceMapping()));
    
    this->commandOperations.push_back(new CommandParser(new AutoOperationAddToSpecFile()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBackendAverageDenseROI()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBackendAverageROICorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiChangeTimestep()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiConvertToScalar()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiPalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiROIAverage()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiSeparateAll()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToMatrix2()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToWorkbenchSparse()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationEstimateFiberBinghams()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFociListCoords()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricPalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationProbtrackXDotConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetStructure()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSpecFileMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceClosestVertex()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceGeodesicROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceInformation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeLabelImport()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumePalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSceneFile()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSpecFile()));
    
    this->commandOperations.push_back(new CommandNiftiInformation());
    this->commandOperations.push_back(new CommandClassAddMember());
    this->commandOperations.push_back(new CommandClassCreate());
    this->commandOperations.push_back(new CommandClassCreateAlgorithm());
    this->commandOperations.push_back(new CommandClassCreateEnum());
    this->commandOperations.push_back(new CommandClassCreateOperation());
    this->commandOperations.push_back(new CommandGiftiConvert());
    this->commandOperations.push_back(new CommandUnitTest());
    this->commandOperations.push_back(new CommandNiftiConvert());
}

/**
 * Destructor.
 */
CommandOperationManager::~CommandOperationManager()
{
    uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        delete this->commandOperations[i];
        this->commandOperations[i] = NULL;
    }
    this->commandOperations.clear();
}

/**
 * Run a command.
 * 
 * @param parameters
 *    Reference to the command's parameters.
 * @throws CommandException
 *    If the command failed.
 */
void 
CommandOperationManager::runCommand(ProgramParameters& parameters) throw (CommandException)
{
    const uint64_t numberOfCommands = this->commandOperations.size();

    if (parameters.hasNext() == false) {
        printAllCommands();
        return;
    }
    
    AString commandSwitch;
    try {
        commandSwitch = parameters.nextString("Command Name");
    
        CommandOperation* operation = NULL;
        
        for (uint64_t i = 0; i < numberOfCommands; i++) {
            if (this->commandOperations[i]->getCommandLineSwitch() == commandSwitch) {
                operation = this->commandOperations[i];
                break;
            }
        }
        
        if (operation == NULL) {
            throw CommandException("Command \"" + commandSwitch + "\" not found.");
        }

        if (!parameters.hasNext() && operation->takesParameters())
        {
            std::cout << operation->getHelpInformation(parameters.getProgramName()) << std::endl;
        } else {
            operation->execute(parameters);
        }
    }
    catch (ProgramParametersException& e) {
        std::cerr << "caught PPE" << std::endl;
        throw CommandException(e);
    }
}

/**
 * Print all of the commands.
 */
void
CommandOperationManager::printAllCommands()
{
    std::map<AString, AString> cmdMap;
    
    int64_t longestSwitch = 0;
    
    const uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        CommandOperation* op = this->commandOperations[i];
        
        const AString cmdSwitch = op->getCommandLineSwitch();
        const int64_t switchLength = cmdSwitch.length();
        if (switchLength > longestSwitch) {
            longestSwitch = switchLength;
        }
        
        cmdMap.insert(std::make_pair(cmdSwitch,
                                     op->getOperationShortDescription()));
        
        const AString helpInfo = op->getHelpInformation("");
        if (helpInfo.isEmpty()) {
            CaretLogSevere("Command has no help info: " + cmdSwitch);
        }
    }

    for (std::map<AString, AString>::iterator iter = cmdMap.begin();
         iter != cmdMap.end();
         iter++) {
        AString cmdSwitch = iter->first;
        cmdSwitch = cmdSwitch.leftJustified(longestSwitch + 2, ' ');
        AString description = iter->second;
        
        std::cout << qPrintable(cmdSwitch) << qPrintable(description) << std::endl;
    }
}

/**
 * Get the command operations.
 * 
 * @return
 *   A vector containing the command operations.
 * Do not modify the returned value.
 */
std::vector<CommandOperation*> 
CommandOperationManager::getCommandOperations()
{
    return this->commandOperations;
}



