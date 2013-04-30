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
#include "AlgorithmCiftiCrossCorrelation.h"
#include "AlgorithmCiftiExtrema.h"
#include "AlgorithmCiftiGradient.h"
#include "AlgorithmCiftiMerge.h"
#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmCiftiReduce.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiResample.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmCiftiTranspose.h"
#include "AlgorithmCreateSignedDistanceVolume.h"
#include "AlgorithmFiberDotProducts.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmLabelResample.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmMetricExtrema.h"
#include "AlgorithmMetricFillHoles.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricReduce.h"
#include "AlgorithmMetricRegression.h"
#include "AlgorithmMetricRemoveIslands.h"
#include "AlgorithmMetricResample.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmSignedDistanceToSurface.h"
#include "AlgorithmSurfaceAffineRegression.h"
#include "AlgorithmSurfaceApplyAffine.h"
#include "AlgorithmSurfaceApplyWarpfield.h"
#include "AlgorithmSurfaceAverage.h"
#include "AlgorithmSurfaceCortexLayer.h"
#include "AlgorithmSurfaceDistortion.h"
#include "AlgorithmSurfaceGenerateInflated.h"
#include "AlgorithmSurfaceInflation.h"
#include "AlgorithmSurfaceMatch.h"
#include "AlgorithmSurfaceModifySphere.h"
#include "AlgorithmSurfaceResample.h"
#include "AlgorithmSurfaceSmoothing.h"
#include "AlgorithmSurfaceSphereProjectUnproject.h"
#include "AlgorithmSurfaceWedgeVolume.h"
#include "AlgorithmVolumeAffineResample.h"
#include "AlgorithmVolumeDilate.h"
#include "AlgorithmVolumeExtrema.h"
#include "AlgorithmVolumeFillHoles.h"
#include "AlgorithmVolumeGradient.h"
#include "AlgorithmVolumeMerge.h"
#include "AlgorithmVolumeParcelResampling.h"
#include "AlgorithmVolumeParcelResamplingGeneric.h"
#include "AlgorithmVolumeParcelSmoothing.h"
#include "AlgorithmVolumeReduce.h"
#include "AlgorithmVolumeRemoveIslands.h"
#include "AlgorithmVolumeSmoothing.h"
#include "AlgorithmVolumeToSurfaceMapping.h"
#include "AlgorithmVolumeWarpfieldResample.h"

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
#include "OperationConvertAffine.h"
#include "OperationConvertFiberOrientations.h"
#include "OperationConvertMatrix4ToMatrix2.h"
#include "OperationConvertMatrix4ToWorkbenchSparse.h"
#include "OperationConvertWarpfield.h"
#include "OperationEstimateFiberBinghams.h"
#include "OperationFociListCoords.h"
#include "OperationLabelMask.h"
#include "OperationMetricConvert.h"
#include "OperationMetricMask.h"
#include "OperationMetricMath.h"
#include "OperationMetricMerge.h"
#include "OperationMetricPalette.h"
#include "OperationMetricVertexSum.h"
#include "OperationProbtrackXDotConvert.h"
#include "OperationSetMapName.h"
#include "OperationSetMapNames.h"
#include "OperationSetStructure.h"
#include "OperationShowScene.h"
#include "OperationSpecFileMerge.h"
#include "OperationSurfaceClosestVertex.h"
#include "OperationSurfaceGeodesicROIs.h"
#include "OperationSurfaceInformation.h"
#include "OperationSurfaceVertexAreas.h"
#include "OperationVolumeLabelImport.h"
#include "OperationVolumeMath.h"
#include "OperationVolumePalette.h"
#include "OperationVolumeReorient.h"
#include "OperationZipSceneFile.h"
#include "OperationZipSpecFile.h"

#include "AlgorithmException.h"
#include "ApplicationInformation.h"
#include "CommandParser.h"
#include "OperationException.h"

#include "CommandClassAddMember.h"
#include "CommandClassCreate.h"
#include "CommandClassCreateAlgorithm.h"
#include "CommandClassCreateEnum.h"
#include "CommandClassCreateOperation.h"
#include "CommandC11xTesting.h"
#include "CommandGiftiConvert.h"
#include "CommandNiftiConvert.h"
#include "CommandNiftiInformation.h"
#include "CommandUnitTest.h"
#include "ProgramParameters.h"

#include "CaretLogger.h"

#include <iostream>

using namespace caret;
using namespace std;

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
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCrossCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiParcellate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReplaceStructure()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSeparate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiTranspose()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCreateSignedDistanceVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmFiberDotProducts()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricFillHoles()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricRegression()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricRemoveIslands()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSignedDistanceToSurface()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceAffineRegression()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceApplyAffine()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceApplyWarpfield()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceAverage()));

#ifdef VELAB_INTERNAL_RELEASE_ONLY
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceCortexLayer()));
#endif // VELAB_INTERNAL_RELEASE_ONLY

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceDistortion()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceGenerateInflated()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceInflation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceMatch()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceModifySphere()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceSphereProjectUnproject()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceWedgeVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeAffineResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeFillHoles()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResampling()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResamplingGeneric()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeRemoveIslands()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeToSurfaceMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeWarpfieldResample()));
    
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
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertAffine()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertFiberOrientations()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToMatrix2()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToWorkbenchSparse()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertWarpfield()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationEstimateFiberBinghams()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFociListCoords()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricPalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricVertexSum()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationProbtrackXDotConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetMapName()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetMapNames()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetStructure()));
    if (OperationShowScene::isShowSceneCommandAvailable()) {
        this->commandOperations.push_back(new CommandParser(new AutoOperationShowScene()));
    }
    this->commandOperations.push_back(new CommandParser(new AutoOperationSpecFileMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceClosestVertex()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceGeodesicROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceInformation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceVertexAreas()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeLabelImport()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumePalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeReorient()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSceneFile()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSpecFile()));
    
    this->commandOperations.push_back(new CommandNiftiInformation());
    this->commandOperations.push_back(new CommandClassAddMember());
    this->commandOperations.push_back(new CommandClassCreate());
    this->commandOperations.push_back(new CommandClassCreateAlgorithm());
    this->commandOperations.push_back(new CommandClassCreateEnum());
    this->commandOperations.push_back(new CommandClassCreateOperation());
#ifdef WORKBENCH_HAVE_C11X
    this->commandOperations.push_back(new CommandC11xTesting());
#endif // WORKBENCH_HAVE_C11X
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
        printHelpInfo();
        return;
    }
    
    AString commandSwitch;
    try {
        commandSwitch = parameters.nextString("Command Name");
        
        if (commandSwitch == "-help")
        {
            printHelpInfo();
        } else if (commandSwitch == "-arguments-help") {
            printArgumentsHelp(parameters.getProgramName());
        } else if (commandSwitch == "-version") {
            printVersionInfo();
        } else if (commandSwitch == "-list-commands") {
            printAllCommands();
        } else if (commandSwitch == "-all-commands-help") {
            printAllCommandsHelpInfo(parameters.getProgramName());
        } else {
        
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
                cout << operation->getHelpInformation(parameters.getProgramName()) << endl;
            } else {
                operation->execute(parameters);
            }
        }
    }
    catch (ProgramParametersException& e) {
        cerr << "caught PPE" << endl;
        throw CommandException(e);
    }
}

/**
 * Print all of the commands.
 */
void
CommandOperationManager::printAllCommands()
{
    map<AString, AString> cmdMap;
    
    int64_t longestSwitch = 0;
    
    const uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        CommandOperation* op = this->commandOperations[i];
        
        const AString cmdSwitch = op->getCommandLineSwitch();
        const int64_t switchLength = cmdSwitch.length();
        if (switchLength > longestSwitch) {
            longestSwitch = switchLength;
        }
        
        cmdMap.insert(make_pair(cmdSwitch,
                                     op->getOperationShortDescription()));
        
        const AString helpInfo = op->getHelpInformation("");
        if (helpInfo.isEmpty()) {
            CaretLogSevere("Command has no help info: " + cmdSwitch);
        }
    }

    for (map<AString, AString>::iterator iter = cmdMap.begin();
         iter != cmdMap.end();
         iter++) {
        AString cmdSwitch = iter->first;
        cmdSwitch = cmdSwitch.leftJustified(longestSwitch + 2, ' ');
        AString description = iter->second;
        
        cout << qPrintable(cmdSwitch) << qPrintable(description) << endl;
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

void CommandOperationManager::printHelpInfo()
{
    printVersionInfo();
    cout << endl << "Information options:" << endl;
    cout << "   -help                 print this help info" << endl;
    cout << "   -arguments-help       explain how to read the help info for subcommands" << endl;
    cout << "   -version              print version information only" << endl;
    cout << "   -list-commands        print all non-information (processing) subcommands" << endl;
    cout << "   -all-commands-help    print all non-information (processing) subcommands and" << endl;
    cout << "                            their help info - VERY LONG" << endl;
    cout << endl;
}

void CommandOperationManager::printArgumentsHelp(const AString& programName)
{
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   To get the help information on a subcommand, run it without any additional" << endl;
    cout << "   arguments.  Options can occur in any position within the correct scope, and" << endl;
    cout << "   can have suboptions, which must occur within the scope of the option.  The" << endl;
    cout << "   easiest way to get this right is to specify options and arguments in the" << endl;
    cout << "   order they are listed.  As an example, consider this help information:" << endl;
    cout << "$ " << programName << " -volume-math" << endl;
    cout << "EVALUATE EXPRESSION ON VOLUME FILES" << endl;
    cout << "   " << programName << " -volume-math" << endl;
    cout << "      <expression>" << endl;
    cout << "      <volume-out>" << endl;
    cout << "      [-fixnan]" << endl;
    cout << "         <replace>" << endl;
    cout << "      [-var] (repeatable)" << endl;
    cout << "         <name>" << endl;
    cout << "         <volume>" << endl;
    cout << "         [-subvolume]" << endl;
    cout << "            <subvol>" << endl;
    cout << "..." << endl;
    cout << endl;
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   '<expression>' and '<volume-out>' denote mandatory parameters, '[-fixnan]'" << endl;
    cout << "   denotes an option taking one mandatory parameter '<replace>', and" << endl;
    cout << "   '[-var] (repeatable)' denotes a repeatable option with mandatory parameters" << endl;
    cout << "   '<name>' and '<volume>', and a suboption '[-subvolume]', which has a" << endl;
    cout << "   mandatory parameter '<subvol>'.  Commands also provide additional help info" << endl;
    cout << "   along with descriptions of options and parameters below the section in the" << endl;
    cout << "   example.  Each option starts a new scope, and ends any scope that it is not" << endl;
    cout << "   valid in.  For example, this command is correct:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' -fixnan 0 -var x x.nii.gz -subvolume 1" << endl;
    cout << endl;
    cout << "   as is this one (though less intuitive):" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math -fixnan 0 -var x -subvolume 1 x.nii.gz 'sin(x)'" << endl;
    cout << endl;
    cout << "   while this one is not, because the -fixnan argument ends the scope of -var" << endl;
    cout << "   before all of its mandatory arguments are given:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' -var x -fixnan 0 x.nii.gz -subvolume 1" << endl;
    cout << endl;
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   and this one is incorrect because the -subvolume option occurs after the" << endl;
    cout << "   scope of -var has ended due to -fixnan:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' -var x x.nii.gz -fixnan 0 -subvolume 1" << endl;
    cout << endl;
}

void CommandOperationManager::printVersionInfo()
{
    ApplicationInformation myInfo;
    vector<AString> myLines;
    myInfo.getAllInformation(myLines);
    for (int i = 0; i < (int)myLines.size(); ++i)
    {
        cout << myLines[i] << endl;
    }
}

void CommandOperationManager::printAllCommandsHelpInfo(const AString& programName)
{
    map<AString, CommandOperation*> cmdMap;
    const uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        CommandOperation* op = this->commandOperations[i];
        cmdMap[op->getCommandLineSwitch()] = op;
    }
    for (map<AString, CommandOperation*>::iterator iter = cmdMap.begin();
         iter != cmdMap.end();
         iter++) {
        cout << iter->first << endl;
        cout << iter->second->getHelpInformation(programName) << endl << endl;
    }
}
