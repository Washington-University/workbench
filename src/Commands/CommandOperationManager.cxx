/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <map>

#define __COMMAND_OPERATION_MANAGER_DEFINE__
#include "CommandOperationManager.h"
#undef __COMMAND_OPERATION_MANAGER_DEFINE__

#include "AlgorithmBorderResample.h"
#include "AlgorithmBorderToVertices.h"
#include "AlgorithmCiftiAllLabelsToROIs.h"
#include "AlgorithmCiftiAverage.h"
#include "AlgorithmCiftiAverageDenseROI.h"
#include "AlgorithmCiftiAverageROICorrelation.h"
#include "AlgorithmCiftiCorrelation.h"
#include "AlgorithmCiftiCorrelationGradient.h"
#include "AlgorithmCiftiCreateDenseScalar.h"
#include "AlgorithmCiftiCreateDenseTimeseries.h"
#include "AlgorithmCiftiCreateLabel.h"
#include "AlgorithmCiftiCrossCorrelation.h"
#include "AlgorithmCiftiDilate.h"
#include "AlgorithmCiftiErode.h"
#include "AlgorithmCiftiExtrema.h"
#include "AlgorithmCiftiFalseCorrelation.h"
#include "AlgorithmCiftiFindClusters.h"
#include "AlgorithmCiftiGradient.h"
#include "AlgorithmCiftiLabelAdjacency.h"
#include "AlgorithmCiftiLabelProbability.h"
#include "AlgorithmCiftiLabelToBorder.h"
#include "AlgorithmCiftiLabelToROI.h"
#include "AlgorithmCiftiMergeDense.h"
#include "AlgorithmCiftiMergeParcels.h"
#include "AlgorithmCiftiPairwiseCorrelation.h"
#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmCiftiParcelMappingToLabel.h"
#include "AlgorithmCiftiReduce.h"
#include "AlgorithmCiftiReorder.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiResample.h"
#include "AlgorithmCiftiROIsFromExtrema.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmCiftiTranspose.h"
#include "AlgorithmCiftiVectorOperation.h"
#include "AlgorithmCreateSignedDistanceVolume.h"
#include "AlgorithmFiberDotProducts.h"
#include "AlgorithmFociResample.h"
#include "AlgorithmGiftiAllLabelsToROIs.h"
#include "AlgorithmGiftiLabelAddPrefix.h"
#include "AlgorithmGiftiLabelToROI.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmLabelErode.h"
#include "AlgorithmLabelModifyKeys.h"
#include "AlgorithmLabelProbability.h"
#include "AlgorithmLabelResample.h"
#include "AlgorithmLabelToBorder.h"
#include "AlgorithmLabelToVolumeMapping.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmMetricErode.h"
#include "AlgorithmMetricEstimateFWHM.h"
#include "AlgorithmMetricExtrema.h"
#include "AlgorithmMetricFalseCorrelation.h"
#include "AlgorithmMetricFillHoles.h"
#include "AlgorithmMetricFindClusters.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricReduce.h"
#include "AlgorithmMetricRegression.h"
#include "AlgorithmMetricRemoveIslands.h"
#include "AlgorithmMetricResample.h"
#include "AlgorithmMetricROIsFromExtrema.h"
#include "AlgorithmMetricROIsToBorder.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmMetricTFCE.h"
#include "AlgorithmMetricToVolumeMapping.h"
#include "AlgorithmMetricVectorOperation.h"
#include "AlgorithmMetricVectorTowardROI.h"
#include "AlgorithmNodesInsideBorder.h" //-border-to-rois
#include "AlgorithmSignedDistanceToSurface.h"
#include "AlgorithmSurfaceAffineRegression.h"
#include "AlgorithmSurfaceApplyAffine.h"
#include "AlgorithmSurfaceApplyWarpfield.h"
#include "AlgorithmSurfaceAverage.h"
#include "AlgorithmSurfaceCortexLayer.h"
#include "AlgorithmSurfaceCreateSphere.h"
#include "AlgorithmSurfaceCurvature.h"
#include "AlgorithmSurfaceDistortion.h"
#include "AlgorithmSurfaceFlipLR.h"
#include "AlgorithmSurfaceGenerateInflated.h"
#include "AlgorithmSurfaceInflation.h"
#include "AlgorithmSurfaceMatch.h"
#include "AlgorithmSurfaceModifySphere.h"
#include "AlgorithmSurfaceResample.h"
#include "AlgorithmSurfaceSmoothing.h"
#include "AlgorithmSurfaceSphereProjectUnproject.h"
#include "AlgorithmSurfaceToSurface3dDistance.h"
#include "AlgorithmSurfaceWedgeVolume.h"
#include "AlgorithmVolumeAffineResample.h"
#include "AlgorithmVolumeAllLabelsToROIs.h"
#include "AlgorithmVolumeDilate.h"
#include "AlgorithmVolumeDistortion.h"
#include "AlgorithmVolumeErode.h"
#include "AlgorithmVolumeEstimateFWHM.h"
#include "AlgorithmVolumeExtrema.h"
#include "AlgorithmVolumeFillHoles.h"
#include "AlgorithmVolumeFindClusters.h"
#include "AlgorithmVolumeGradient.h"
#include "AlgorithmVolumeLabelProbability.h"
#include "AlgorithmVolumeLabelToROI.h"
#include "AlgorithmVolumeLabelToSurfaceMapping.h"
#include "AlgorithmVolumeParcelResampling.h"
#include "AlgorithmVolumeParcelResamplingGeneric.h"
#include "AlgorithmVolumeParcelSmoothing.h"
#include "AlgorithmVolumeReduce.h"
#include "AlgorithmVolumeRemoveIslands.h"
#include "AlgorithmVolumeROIsFromExtrema.h"
#include "AlgorithmVolumeSmoothing.h"
#include "AlgorithmVolumeTFCE.h"
#include "AlgorithmVolumeToSurfaceMapping.h"
#include "AlgorithmVolumeVectorOperation.h"
#include "AlgorithmVolumeWarpfieldAffineRegression.h"
#include "AlgorithmVolumeWarpfieldResample.h"

#include "OperationAddToSpecFile.h"
#include "OperationBackendAverageDenseROI.h"
#include "OperationBackendAverageROICorrelation.h"
#include "OperationBorderExportColorTable.h"
#include "OperationBorderFileExportToCaret5.h"
#include "OperationBorderLength.h"
#include "OperationBorderMerge.h"
#include "OperationCiftiChangeMapping.h"
#include "OperationCiftiChangeTimestep.h"
#include "OperationCiftiConvert.h"
#include "OperationCiftiConvertToScalar.h"
#include "OperationCiftiCopyMapping.h"
#include "OperationCiftiCreateDenseFromTemplate.h"
#include "OperationCiftiCreateParcellatedFromTemplate.h"
#include "OperationCiftiCreateScalarSeries.h"
#include "OperationCiftiEstimateFWHM.h"
#include "OperationCiftiExportDenseMapping.h"
#include "OperationCiftiLabelExportTable.h"
#include "OperationCiftiLabelImport.h"
#include "OperationCiftiMath.h"
#include "OperationCiftiMerge.h"
#include "OperationCiftiPalette.h"
#include "OperationCiftiResampleDconnMemory.h"
#include "AlgorithmCiftiRestrictDenseMap.h"
#include "OperationCiftiROIAverage.h"
#include "OperationCiftiSeparateAll.h"
#include "OperationCiftiStats.h"
#include "OperationCiftiWeightedStats.h"
#include "OperationConvertAffine.h"
#include "OperationConvertFiberOrientations.h"
#include "OperationConvertMatrix4ToMatrix2.h"
#include "OperationConvertMatrix4ToWorkbenchSparse.h"
#include "OperationConvertWarpfield.h"
#include "OperationEstimateFiberBinghams.h"
#include "OperationFileConvert.h"
#include "OperationFileInformation.h"
#include "OperationFociGetProjectionVertex.h"
#include "OperationFociListCoords.h"
#include "OperationGiftiConvert.h"
#include "OperationLabelExportTable.h"
#include "OperationLabelMask.h"
#include "OperationLabelMerge.h"
#include "OperationMetadataRemoveProvenance.h"
#include "OperationMetadataStringReplace.h"
#include "OperationMetricConvert.h"
#include "OperationMetricLabelImport.h"
#include "OperationMetricMask.h"
#include "OperationMetricMath.h"
#include "OperationMetricMerge.h"
#include "OperationMetricPalette.h"
#include "OperationMetricStats.h"
#include "OperationMetricVertexSum.h"
#include "OperationMetricWeightedStats.h"
#include "OperationNiftiInformation.h"
#include "OperationProbtrackXDotConvert.h"
#include "OperationSceneFileMerge.h"
#include "OperationSceneFileRelocate.h"
#include "OperationSetMapName.h"
#include "OperationSetMapNames.h"
#include "OperationSetStructure.h"
#include "OperationShowScene.h"
#include "OperationSpecFileMerge.h"
#include "OperationSpecFileRelocate.h"
#include "OperationSurfaceClosestVertex.h"
#include "OperationSurfaceCoordinatesToMetric.h"
#include "OperationSurfaceCutResample.h"
#include "OperationSurfaceFlipNormals.h"
#include "OperationSurfaceGeodesicDistance.h"
#include "OperationSurfaceGeodesicROIs.h"
#include "OperationSurfaceInformation.h"
#include "OperationSurfaceNormals.h"
#include "OperationSurfaceSetCoordinates.h"
#include "OperationSurfaceVertexAreas.h"
#include "OperationVolumeCapturePlane.h"
#include "OperationVolumeCopyExtensions.h"
#include "OperationVolumeCreate.h"
#include "OperationVolumeLabelExportTable.h"
#include "OperationVolumeLabelImport.h"
#include "OperationVolumeMath.h"
#include "OperationVolumeMerge.h"
#include "OperationVolumePalette.h"
#include "OperationVolumeReorient.h"
#include "OperationVolumeSetSpace.h"
#include "OperationVolumeStats.h"
#include "OperationVolumeWeightedStats.h"
#include "OperationWbsparseMergeDense.h"
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
#include "CommandUnitTest.h"
#include "ProgramParameters.h"

#include "CaretLogger.h"
#include "dot_wrapper.h"
#include "StructureEnum.h"

#include <iostream>
#include <map>

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
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmBorderResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmBorderToVertices()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAllLabelsToROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverage()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverageDenseROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiAverageROICorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCorrelationGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateDenseScalar()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateDenseTimeseries()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCreateLabel()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCrossCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiErode()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiFalseCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiFindClusters()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelAdjacency()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelProbability()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelToBorder()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiLabelToROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiMergeDense()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiMergeParcels()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiPairwiseCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiParcellate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiParcelMappingToLabel()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReorder()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiReplaceStructure()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiRestrictDenseMap()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiROIsFromExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSeparate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiTranspose()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiVectorOperation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCreateSignedDistanceVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmFiberDotProducts()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmFociResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmGiftiAllLabelsToROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmGiftiLabelAddPrefix()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmGiftiLabelToROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelErode()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelModifyKeys()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelProbability()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelToBorder()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmLabelToVolumeMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricErode()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricEstimateFWHM()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricFalseCorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricFillHoles()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricFindClusters()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricRegression()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricRemoveIslands()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricROIsFromExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricROIsToBorder()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricTFCE()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricToVolumeMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricVectorOperation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmMetricVectorTowardROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmNodesInsideBorder()));//-border-to-rois
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSignedDistanceToSurface()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceAffineRegression()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceApplyAffine()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceApplyWarpfield()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceAverage()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceCortexLayer()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceCreateSphere()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceCurvature()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceDistortion()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceFlipLR()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceGenerateInflated()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceInflation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceMatch()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceModifySphere()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceSphereProjectUnproject()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceToSurface3dDistance()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmSurfaceWedgeVolume()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeAffineResample()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeAllLabelsToROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeDilate()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeDistortion()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeErode()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeEstimateFWHM()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeFillHoles()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeFindClusters()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeGradient()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeLabelProbability()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeLabelToROI()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeLabelToSurfaceMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResampling()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResamplingGeneric()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeReduce()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeRemoveIslands()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeROIsFromExtrema()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeSmoothing()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeTFCE()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeToSurfaceMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeVectorOperation()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeWarpfieldAffineRegression()));
    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeWarpfieldResample()));
    
    this->commandOperations.push_back(new CommandParser(new AutoOperationAddToSpecFile()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBackendAverageDenseROI()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBackendAverageROICorrelation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBorderExportColorTable()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBorderFileExportToCaret5()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBorderLength()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationBorderMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiChangeMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiCreateDenseFromTemplate()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiCreateParcellatedFromTemplate()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiCreateScalarSeries()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiEstimateFWHM()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiExportDenseMapping()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiLabelExportTable()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiLabelImport()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiPalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiResampleDconnMemory()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiROIAverage()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationCiftiWeightedStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertAffine()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertFiberOrientations()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToMatrix2()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertMatrix4ToWorkbenchSparse()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationConvertWarpfield()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationEstimateFiberBinghams()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFileConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFileInformation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFociGetProjectionVertex()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationFociListCoords()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationGiftiConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelExportTable()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationLabelMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetadataRemoveProvenance()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetadataStringReplace()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricLabelImport()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMask()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricPalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationMetricWeightedStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationNiftiInformation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationProbtrackXDotConvert()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSceneFileMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSceneFileRelocate()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetMapNames()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSetStructure()));
    if (OperationShowScene::isShowSceneCommandAvailable()) {
        this->commandOperations.push_back(new CommandParser(new AutoOperationShowScene()));
    }
    this->commandOperations.push_back(new CommandParser(new AutoOperationSpecFileMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSpecFileRelocate()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceClosestVertex()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceCoordinatesToMetric()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceCutResample()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceFlipNormals()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceGeodesicDistance()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceGeodesicROIs()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceInformation()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceNormals()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceSetCoordinates()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationSurfaceVertexAreas()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeCapturePlane()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeCopyExtensions()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeCreate()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeLabelExportTable()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeLabelImport()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeMath()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeMerge()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumePalette()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeReorient()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeSetSpace()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationVolumeWeightedStats()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationWbsparseMergeDense()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSceneFile()));
    this->commandOperations.push_back(new CommandParser(new AutoOperationZipSpecFile()));
    
    this->commandOperations.push_back(new CommandClassAddMember());
    this->commandOperations.push_back(new CommandClassCreate());
    this->commandOperations.push_back(new CommandClassCreateAlgorithm());
    this->commandOperations.push_back(new CommandClassCreateEnum());
    this->commandOperations.push_back(new CommandClassCreateOperation());
#ifdef WORKBENCH_HAVE_C11X
    this->commandOperations.push_back(new CommandC11xTesting());
#endif // WORKBENCH_HAVE_C11X
    this->commandOperations.push_back(new CommandUnitTest());
    
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationCiftiChangeTimestep()));
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationCiftiConvertToScalar()));
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationCiftiCopyMapping()));
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationCiftiSeparateAll()));
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationMetricVertexSum()));
    this->deprecatedOperations.push_back(new CommandParser(new AutoOperationSetMapName()));
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
    uint64_t numberOfDeprecated = this->deprecatedOperations.size();
    for (uint64_t i = 0; i < numberOfDeprecated; i++) {
        delete this->deprecatedOperations[i];
        this->deprecatedOperations[i] = NULL;
    }
    this->deprecatedOperations.clear();
}

namespace
{
    //quick hack to convert type argument to internal integer
    int16_t stringToCiftiType(const AString& input)
    {
        map<AString, int16_t> nameToCode;
        nameToCode["INT8"] = NIFTI_TYPE_INT8;
        nameToCode["UINT8"] = NIFTI_TYPE_UINT8;
        nameToCode["INT16"] = NIFTI_TYPE_INT16;
        nameToCode["UINT16"] = NIFTI_TYPE_UINT16;
        nameToCode["INT32"] = NIFTI_TYPE_INT32;
        nameToCode["UINT32"] = NIFTI_TYPE_UINT32;
        nameToCode["INT64"] = NIFTI_TYPE_INT64;
        nameToCode["UINT64"] = NIFTI_TYPE_UINT64;
        nameToCode["FLOAT32"] = NIFTI_TYPE_FLOAT32;
        nameToCode["FLOAT64"] = NIFTI_TYPE_FLOAT64;
        nameToCode["FLOAT128"] = NIFTI_TYPE_FLOAT128;
        map<AString, int16_t>::iterator iter = nameToCode.find(input);
        if (iter == nameToCode.end())
        {
            throw CommandException("Unrecognized cifti datatype: '" + input + "'");
        }
        return iter->second;
    }
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
CommandOperationManager::runCommand(ProgramParameters& parameters)
{
    vector<AString> globalOptionArgs;
    bool preventProvenance = getGlobalOption(parameters, "-disable-provenance", 0, globalOptionArgs);//check these BEFORE we test if we have a command switch, because they remove the switch and arguments from the ProgramParameters
    if (getGlobalOption(parameters, "-logging", 1, globalOptionArgs))
    {
        bool valid = false;
        const LogLevelEnum::Enum level = LogLevelEnum::fromName(globalOptionArgs[0], &valid);
        if (!valid) throw CommandException("unrecognized logging level: '" + globalOptionArgs[0] + "'");
        CaretLogger::getLogger()->setLevel(level);
    }
    if (getGlobalOption(parameters, "-simd", 1, globalOptionArgs))
    {
        bool valid = false;
        const DotSIMDEnum::Enum impl = DotSIMDEnum::fromName(globalOptionArgs[0], &valid);
        if (!valid) throw CommandException("unrecognized SIMD type: '" + globalOptionArgs[0] + "'");
        DotSIMDEnum::Enum retval = dot_set_impl(impl);
        if (impl != DOT_AUTO && retval != impl)
        {
            CaretLogWarning("SIMD type '" + DotSIMDEnum::toName(impl) + "' not supported (could be cpu, compiler, or build options), using '" + DotSIMDEnum::toName(retval) + "'");
        }
    }
    int16_t ciftiDType = NIFTI_TYPE_FLOAT32;
    bool ciftiScale = false;
    double ciftiMin = -1.0, ciftiMax = -1.0;
    if (getGlobalOption(parameters, "-cifti-output-datatype", 1, globalOptionArgs))
    {
        ciftiDType = stringToCiftiType(globalOptionArgs[0]);
    }
    if (getGlobalOption(parameters, "-cifti-output-range", 2, globalOptionArgs))
    {
        ciftiScale = true;
        bool valid = false;
        ciftiMin = globalOptionArgs[0].toDouble(&valid);
        if (!valid) throw CommandException("non-numeric option to -cifti-output-range: '" + globalOptionArgs[0] + "'");
        ciftiMax = globalOptionArgs[1].toDouble(&valid);
        if (!valid) throw CommandException("non-numeric option to -cifti-output-range: '" + globalOptionArgs[1] + "'");
    }

    const uint64_t numberOfCommands = this->commandOperations.size();
    const uint64_t numberOfDeprecated = this->deprecatedOperations.size();

    if (parameters.hasNext() == false) {
        printHelpInfo();
        return;
    }
    
    AString commandSwitch;
    commandSwitch = fixUnicode(parameters.nextString("Command Name"), false);
    
    //hardcode program name, instead of taking it from the parameters, so that it doesn't include path or show wrapper script details
    const AString myProgramName = "wb_command";
    if (commandSwitch == "-help")
    {
        printHelpInfo();
    } else if (commandSwitch == "-arguments-help") {
        printArgumentsHelp(myProgramName);
    } else if (commandSwitch == "-global-options") {
        printGlobalOptions();
    } else if (commandSwitch == "-cifti-help") {
        printCiftiHelp();
    } else if (commandSwitch == "-gifti-help") {
        printGiftiHelp();
    } else if (commandSwitch == "-parallel-help") {
        printParallelHelp(myProgramName);
    } else if (commandSwitch == "-version") {
        printVersionInfo();
    } else if (commandSwitch == "-list-commands") {
        printAllCommands();
    } else if (commandSwitch == "-list-deprecated-commands") {
        printDeprecatedCommands();
    } else if (commandSwitch == "-all-commands-help") {
        printAllCommandsHelpInfo(myProgramName);
    } else {
        
        CommandOperation* operation = NULL;
        
        for (uint64_t i = 0; i < numberOfCommands; i++)
        {
            if (this->commandOperations[i]->getCommandLineSwitch() == commandSwitch)
            {
                operation = this->commandOperations[i];
                break;
            }
        }
        if (operation == NULL)
        {
            for (uint64_t i = 0; i < numberOfDeprecated; i++)
            {
                if (this->deprecatedOperations[i]->getCommandLineSwitch() == commandSwitch)
                {
                    operation = this->deprecatedOperations[i];
                    break;
                }
            }
        }
        
        if (operation == NULL) {
            if (!parameters.hasNext())
            {
                printAllCommandsMatching(commandSwitch);
            } else {
                throw CommandException("Command \"" + commandSwitch + "\" not found.");
            }
        } else {
            if (!parameters.hasNext() && operation->takesParameters())
            {
                cout << operation->getHelpInformation(myProgramName) << endl;
            } else {
                if (ciftiScale)
                {
                    operation->setCiftiOutputDTypeAndScale(ciftiDType, ciftiMin, ciftiMax);
                } else {
                    operation->setCiftiOutputDTypeNoScale(ciftiDType);
                }
                operation->execute(parameters, preventProvenance);
            }
        }
    }
}

AString CommandOperationManager::doCompletion(ProgramParameters& parameters, const bool& useExtGlob)
{
    AString ret;
    vector<AString> globalOptionArgs;
    /*OptionInfo provInfo = */parseGlobalOption(parameters, "-disable-provenance", 0, globalOptionArgs, true);//we need to at least strip out the global options for other parsing to work
    OptionInfo loggingInfo = parseGlobalOption(parameters, "-logging", 1, globalOptionArgs, true);//the previous option doesn't take arguments, doesn't need completion testing
    if (loggingInfo.specified && !loggingInfo.complete)
    {//user is tab completing the logging option, and as it only takes one argument, we know what the completions are
        vector<LogLevelEnum::Enum> logLevels;
        LogLevelEnum::getAllEnums(logLevels);
        ret = "wordlist ";
        for (int i = 0; i < (int)logLevels.size(); ++i)
        {
            if (i != 0) ret += "\\ ";//backslash-escaped space to leave the list of levels as one word
            ret += LogLevelEnum::toName(logLevels[i]);
        }
        return ret;
    }
    OptionInfo simdInfo = parseGlobalOption(parameters, "-simd", 1, globalOptionArgs, true);//the previous option doesn't take arguments, doesn't need completion testing
    if (simdInfo.specified && !simdInfo.complete)
    {//user is tab completing the logging option, and as it only takes one argument, we know what the completions are
        vector<DotSIMDEnum::Enum> simdTypes = DotSIMDEnum::getAllEnums();
        ret = "wordlist ";
        for (int i = 0; i < (int)simdTypes.size(); ++i)
        {
            if (i != 0) ret += "\\ ";//backslash-escaped space to leave the list of levels as one word
            ret += DotSIMDEnum::toName(simdTypes[i]);
        }
        return ret;
    }
    OptionInfo ciftiDTypeInfo = parseGlobalOption(parameters, "-cifti-output-datatype", 1, globalOptionArgs, true);
    if (ciftiDTypeInfo.specified && !ciftiDTypeInfo.complete)
    {
        return "wordlist INT8 UINT8 INT16 UINT16 INT32 UINT32 INT64 UINT64 FLOAT32 FLOAT64 FLOAT128";
    }
    OptionInfo ciftiRangeInfo = parseGlobalOption(parameters, "-cifti-output-range", 2, globalOptionArgs, true);
    if (ciftiRangeInfo.specified && !ciftiRangeInfo.complete)
    {//can't tab complete a literal number
        return "";
    }
    ret = "wordlist -disable-provenance\\ -logging\\ -simd\\ -cifti-output-datatype\\ -cifti-output-range";//we could prevent suggesting an already-provided global option, but that would be a bit surprising
    const uint64_t numberOfCommands = this->commandOperations.size();
    const uint64_t numberOfDeprecated = this->deprecatedOperations.size();
    if (!parameters.hasNext())
    {//suggest all commands, including deprecated and informational (order doesn't matter, bash sorts them before displaying)
        ret += "\\ -help\\ -arguments-help\\ -global-options\\ -parallel-help\\ -cifti-help\\ -gifti-help\\ -parallel-help\\ -version\\ -list-commands\\ -list-deprecated-commands\\ -all-commands-help";
        for (uint64_t i = 0; i < numberOfCommands; i++)
        {
            ret += "\\ " + commandOperations[i]->getCommandLineSwitch();
        }
        for (uint64_t i = 0; i < numberOfDeprecated; i++)
        {
            ret += "\\ " + deprecatedOperations[i]->getCommandLineSwitch();
        }
        return ret;
    }
    //only processing commands take additional arguments, so we can now ignore -help and similar
    AString commandSwitch;
    commandSwitch = fixUnicode(parameters.nextString("Command Name"), true);
    for (uint64_t i = 0; i < numberOfCommands; i++)
    {
        if (commandOperations[i]->getCommandLineSwitch() == commandSwitch)
        {
            AString commandCompletion = commandOperations[i]->doCompletion(parameters, useExtGlob);
            if (commandCompletion != "")
            {
                if (ret != "") ret += " ";
                ret += commandCompletion;
            }
            return ret;
        }
    }
    for (uint64_t i = 0; i < numberOfDeprecated; i++)
    {
        if (deprecatedOperations[i]->getCommandLineSwitch() == commandSwitch)
        {
            AString commandCompletion = deprecatedOperations[i]->doCompletion(parameters, useExtGlob);
            if (commandCompletion != "")
            {
                if (ret != "") ret += " ";
                ret += commandCompletion;
            }
            return ret;
        }
    }
    //if we get here, then the operation switch is wrong, or goes to an informational option
    return ret;
}

bool CommandOperationManager::getGlobalOption(ProgramParameters& parameters, const AString& optionString, const int& numArgs, vector<AString>& arguments)
{
    OptionInfo retval = parseGlobalOption(parameters, optionString, numArgs, arguments, false);
    if (retval.specified && !retval.complete)
    {
        throw CommandException("missing argument #" + AString::number(arguments.size() + 1) + " to global option ");
    }
    return retval.specified;
}

CommandOperationManager::OptionInfo CommandOperationManager::parseGlobalOption(ProgramParameters& parameters, const AString& optionString, const int& numArgs, vector<AString>& arguments, const bool& quiet)
{
    OptionInfo ret;//initializes to false, false, -1
    int32_t initialIndex = parameters.getParameterIndex();//before returning, restore initial index - also, only search from current index onwards
    while (parameters.hasNext())
    {
        bool hyphenReplaced = false;
        AString testRaw = parameters.nextString("global option");
        AString test = testRaw.fixUnicodeHyphens(&hyphenReplaced, NULL, quiet);
        if (test == optionString)
        {
            if (!quiet && hyphenReplaced)
            {
                CaretLogWarning("replaced non-ascii hyphen/dash characters in global option '" + testRaw + "' with ascii '-'");
            }//delay parameter removal until we know we have all arguments
            if (!quiet && ret.specified)
            {
                CaretLogInfo("global option '" + optionString + "' specified multiple times");
            }
            arguments.clear();
            OptionInfo temp;//initializes to false, false, -1
            temp.specified = true;
            temp.index = parameters.getParameterIndex() - 1;//the function actually reports the index of the next parameter
            for (int i = 0; i < numArgs; ++i)
            {
                if (!parameters.hasNext())
                {
                    return temp;//complete is still false - since this is a fatal parsing error, don't reset the parameter index
                }
                arguments.push_back(parameters.nextString("global option argument"));
            }
            for (int i = 0; i < numArgs; ++i)
            {
                parameters.remove();//remove arguments
            }
            parameters.remove();//remove option switch
            temp.complete = true;
            ret = temp;
        }
    }
    parameters.setParameterIndex(initialIndex);
    return ret;
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
#ifndef NDEBUG
        const AString helpInfo = op->getHelpInformation("");//TSC: generating help info takes a little processing (populating and walking an OperationParameters tree for each command)
        if (helpInfo.isEmpty()) {//So, test the same define as for asserts and skip this check in release
            CaretLogSevere("Command has no help info: " + cmdSwitch);
        }
#endif
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

void CommandOperationManager::printDeprecatedCommands()
{
    map<AString, AString> cmdMap;
    
    int64_t longestSwitch = 0;
    
    const uint64_t numberOfDeprecated = this->deprecatedOperations.size();
    for (uint64_t i = 0; i < numberOfDeprecated; i++) {
        CommandOperation* op = this->deprecatedOperations[i];
        
        const AString cmdSwitch = op->getCommandLineSwitch();
        const int64_t switchLength = cmdSwitch.length();
        if (switchLength > longestSwitch) {
            longestSwitch = switchLength;
        }
        
        cmdMap.insert(make_pair(cmdSwitch,
                                     op->getOperationShortDescription()));
#ifndef NDEBUG
        const AString helpInfo = op->getHelpInformation("");//TSC: generating help info takes a little processing (populating and walking an OperationParameters tree for each command)
        if (helpInfo.isEmpty()) {//So, test the same define as for asserts and skip this check in release
            CaretLogSevere("Command has no help info: " + cmdSwitch);
        }
#endif
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
 * Print all of the commands matching a partial switch.
 */
void
CommandOperationManager::printAllCommandsMatching(const AString& partialSwitch)
{
    map<AString, AString> cmdMap;
    
    int64_t longestSwitch = -1;
    
    const uint64_t numberOfCommands = this->commandOperations.size();
    for (uint64_t i = 0; i < numberOfCommands; i++) {
        CommandOperation* op = this->commandOperations[i];
        
        const AString cmdSwitch = op->getCommandLineSwitch();
        if (cmdSwitch.startsWith(partialSwitch))
        {
            const int64_t switchLength = cmdSwitch.length();
            if (switchLength > longestSwitch) {
                longestSwitch = switchLength;
            }
            
            cmdMap.insert(make_pair(cmdSwitch,
                                        op->getOperationShortDescription()));
#ifndef NDEBUG
            const AString helpInfo = op->getHelpInformation("");//TSC: generating help info takes a little processing (populating and walking an OperationParameters tree for each command)
            if (helpInfo.isEmpty()) {//So, test the same define as for asserts and skip this check in release
                CaretLogSevere("Command has no help info: " + cmdSwitch);
            }
#endif
        }
    }
    if (longestSwitch == -1)//no command found
    {
        throw CommandException("the switch '" + partialSwitch + "' does not match any processing commands");
    }

    for (map<AString, AString>::iterator iter = cmdMap.begin();
         iter != cmdMap.end();
         iter++) {
        AString cmdSwitch = iter->first;
        if (cmdSwitch.startsWith(partialSwitch))
        {
            cmdSwitch = cmdSwitch.leftJustified(longestSwitch + 2, ' ');
            AString description = iter->second;
            
            cout << qPrintable(cmdSwitch) << qPrintable(description) << endl;
        }
    }
}

/**
 * Get the command operations.
 * 
 * @return
 *   A vector containing the command operations.
 * Do not modify the returned value.
 */
vector<CommandOperation*> 
CommandOperationManager::getCommandOperations()
{
    return this->commandOperations;
}

void CommandOperationManager::printHelpInfo()
{
    cout << ApplicationInformation().getSummaryInformationInString("\n");
    //guide for wrap, assuming 80 columns:                                                  |
    cout << endl << "Information options:" << endl;
    cout << "   -help                       show this help info" << endl;
    cout << "   -arguments-help             explain the format of subcommand help info" << endl;
    cout << "   -global-options             display options that can be added to any command" << endl;
    cout << "   -parallel-help              details on how wb_command uses parallelization" << endl;
    cout << "   -cifti-help                 explain the cifti file format and related terms" << endl;
    cout << "   -gifti-help                 explain the gifti file format (metric, surface)" << endl;
    cout << "   -version                    show extended version information" << endl;
    cout << "   -list-commands              list all processing subcommands" << endl;
    cout << "   -list-deprecated-commands   list deprecated subcommands" << endl;
    cout << "   -all-commands-help          show all processing subcommands and their help" << endl;
    cout << "                                  info - VERY LONG" << endl;
    cout << endl;
    cout << "To get the help information of a processing subcommand, run it without any" << endl;
    cout << "   additional arguments." << endl;
    cout << endl;
    cout << "If the first argument is not recognized, all processing commands that start" << endl;
    cout << "   with the argument are displayed" << endl;
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
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "$ " << programName << " -volume-math" << endl;
    cout << "EVALUATE EXPRESSION ON VOLUME FILES" << endl;
    cout << "   " << programName << " -volume-math" << endl;
    cout << "      <expression> - the expression to evaluate, in quotes" << endl;
    cout << "      <volume-out> - output - the output volume" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "      [-fixnan] - replace NaN results with a value" << endl;
    cout << "         <replace> - value to replace NaN with" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "      [-var] - repeatable - a volume file to use as a variable" << endl;
    cout << "         <name> - the name of the variable, as used in the expression" << endl;
    cout << "         <volume> - the volume file to use as this variable" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "         [-subvolume] - select a single subvolume" << endl;
    cout << "            <subvol> - the subvolume number or name" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "         [-repeat] - reuse a single subvolume for each subvolume of calculation" << endl;
    cout << "..." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   '<expression>' and '<volume-out>' denote mandatory parameters.  '[-fixnan]'" << endl;
    cout << "   denotes an option taking one mandatory parameter '<replace>', and" << endl;
    cout << "   '[-var] - repeatable' denotes a repeatable option with mandatory parameters" << endl;
    cout << "   '<name>' and '<volume>', and two suboptions: '[-subvolume]', which has a" << endl;
    cout << "   mandatory parameter '<subvol>', and '[-repeat]', which takes no parameters." << endl;
    cout << "   Commands also provide additional help info below the section in the example." << endl;
    cout << "   Each option starts a new scope, and all options and arguments end any scope" << endl;
    cout << "   that they are not valid in.  For example, this command is correct:" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "$ " << programName << " -volume-math 'sin(x)' sin_x.nii.gz -fixnan 0 -var x x.nii.gz -subvolume 1" << endl;
    cout << endl;
    cout << "   as is this one (though less intuitive):" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math -fixnan 0 'sin(x)' -var x -subvolume 1 x.nii.gz sin_x.nii.gz" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   while this one is not, because the -fixnan option ends the scope of the -var" << endl;
    cout << "   option before all of its mandatory arguments are given:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' sin_x.nii.gz -var x -fixnan 0 x.nii.gz -subvolume 1" << endl;
    cout << endl;
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   and this one is incorrect because the -subvolume option occurs after the" << endl;
    cout << "   scope of the -var option has ended due to -fixnan:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' sin_x.nii.gz -var x x.nii.gz -fixnan 0 -subvolume 1" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   and this one is similarly incorrect because the -subvolume option occurs" << endl;
    cout << "   after the scope of the -var option has ended due to the volume-out argument:" << endl;
    cout << endl;
    cout << "$ " << programName << " -volume-math 'sin(x)' -fixnan 0 -var x x.nii.gz sin_x.nii.gz -subvolume 1" << endl;
    cout << endl;
}

void CommandOperationManager::printGlobalOptions()
{
    cout << "Global options (can be added to any command):" << endl;
    cout << endl;
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   -disable-provenance               don't generate provenance info in output" << endl;
    cout << "                                        files" << endl;
    cout << endl;
    cout << "   -cifti-output-datatype <type>     write cifti output with the given" << endl;
    cout << "                                        datatype (default FLOAT32), note that" << endl;
    cout << "                                        calculation precision is only float32," << endl;
    cout << "                                        valid values are:" << endl;
    cout << "                          INT8" << endl;
    cout << "                          UINT8" << endl;
    cout << "                          INT16" << endl;
    cout << "                          UINT16" << endl;
    cout << "                          INT32" << endl;
    cout << "                          UINT32" << endl;
    cout << "                          INT64" << endl;
    cout << "                          UINT64" << endl;
    cout << "                          FLOAT32" << endl;
    cout << "                          FLOAT64" << endl;
    cout << "                          FLOAT128" << endl;
    cout << endl;
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   -cifti-output-range <min> <max>   write cifti output with scaling and offset" << endl;
    cout << "                                        header fields such that <min> and <max>" << endl;
    cout << "                                        are the most extreme values that can be" << endl;
    cout << "                                        represented, mostly useful with integer" << endl;
    cout << "                                        output datatypes (see above)" << endl;
    cout << endl;
    cout << "   -logging <level>                  set the logging level, valid values are:" << endl;
    vector<LogLevelEnum::Enum> logLevels;
    LogLevelEnum::getAllEnums(logLevels);
    for (vector<LogLevelEnum::Enum>::iterator iter = logLevels.begin();
         iter != logLevels.end();
         iter++) {
        cout << "            " << LogLevelEnum::toName(*iter) << endl;
    }
    cout << endl;//add a line after the logging types for readability
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   -simd <type>                      set the SIMD implementation to use" << endl;
    cout << "                                        (currently used only for correlation," << endl;
    cout << "                                        default AUTO which selects fastest" << endl;
    cout << "                                        supported), valid values are:" << endl;
    vector<DotSIMDEnum::Enum> simdTypes = DotSIMDEnum::getAllEnums();
    for (vector<DotSIMDEnum::Enum>::iterator iter = simdTypes.begin();
         iter != simdTypes.end();
         iter++) {
        cout << "         " << DotSIMDEnum::toName(*iter) << endl;
    }
    cout << endl;
}

void CommandOperationManager::printCiftiHelp()
{
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   The CIFTI format is a new data file format intended to make it easier to" << endl;
    cout << "   work with data from multiple disjoint structures at the same time - often" << endl;
    cout << "   this means both hemispheres of cortex as surface data, and other structures" << endl;
    cout << "   as voxel data (amygdala, thalamus, putamen, etc).  Additionally, it can" << endl;
    cout << "   exclude locations that are uninteresting for the task at hand (medial wall," << endl;
    cout << "   white matter, csf), preventing them from taking up room in the data of the" << endl;
    cout << "   file.  The set of structures and the locations in them that are used in a" << endl;
    cout << "   cifti file are referred to as 'brainordinates', or for the specific case of" << endl;
    cout << "   'all gray matter locations', 'grayordinates'." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   However, to explain the cifti format, it is easiest to work from the" << endl;
    cout << "   opposite direction, as it is conceptually simpler.  A single cifti file is a" << endl;
    cout << "   single rectangular data matrix (usually 2 dimensions, but supports 3, and" << endl;
    cout << "   may support more in the future), where each dimension is labeled with what" << endl;
    cout << "   we call a 'mapping', each of which uses one of (currently) five possible" << endl;
    cout << "   'mapping types'.  It is the combinations of these mapping types that give" << endl;
    cout << "   rise to the diverse types of cifti files.  A single mapping of type 'brain" << endl;
    cout << "   models' (also known as 'dense') can represent both hemispheres and all" << endl;
    cout << "   subcortical structures simultaneously, meaning that only a single matrix" << endl;
    cout << "   dimension is used to represent over a dozen structures, both surface-based" << endl;
    cout << "   and voxel-based.  Each mapping contains all information needed to figure out" << endl;
    cout << "   what every index along the matrix dimension means.  By putting a dense" << endl;
    cout << "   mapping along both dimensions in a 2D cifti file, you get a brainordinates" << endl;
    cout << "   by brainordinates matrix, which is used for connectivity measures.  Notably," << endl;
    cout << "   even if two dimensions use the same mapping *type*, they can have different" << endl;
    cout << "   information in them, for example a connectivity matrix between two different" << endl;
    cout << "   parcellations." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   The other mapping types that currently may be used in a cifti file are:" << endl;
    cout << "      Parcels: each index refers to a named subset of the brainordinates (i.e." << endl;
    cout << "         'V1', and the surface vertices in V1)" << endl;
    cout << "      Scalars: each index is simply given a name (i.e. 'Myelin')" << endl;
    cout << "      Series: each index is assigned a quantity in a linear series (i.e., a" << endl;
    cout << "         timeseries of 0 sec, 0.7 sec, 1.4 sec, ...)" << endl;
    cout << "      Labels: each index is assigned a name (i.e., 'Visual Areas'), but also a" << endl;
    cout << "         list of labels that maps integer data values to names and colors (i.e." << endl;
    cout << "         {(5, 'V1', #ff0000), (7, 'V2', #00ff00), ...}" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   The commands that operate on cifti files often require you to specify which" << endl;
    cout << "   dimension they should operate on.  Because cifti files can contain 3" << endl;
    cout << "   dimensions, we specify them as which dimension to operate along, that is, " << endl;
    cout << "   the ROW dimension refers to the mapping along the length of a row." << endl;
    cout << "   Additionally, the ROW dimension is the *first* dimension in a cifti file," << endl;
    cout << "   unlike 2D matrices in linear algebra.  This means that increasing the value" << endl;
    cout << "   of the first index moves rightwards in the matrix, not downwards." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   The common types of cifti files and the mapping types they use are:" << endl;
    cout << "      dconn: ROW is dense, COLUMN is dense" << endl;
    cout << "      dscalar: ROW is scalars, COLUMN is dense" << endl;
    cout << "      dtseries: ROW is series, COLUMN is dense" << endl;
    cout << "      dlabel: ROW is labels, COLUMN is dense" << endl;
    cout << "      pconn: ROW is parcels, COLUMN is parcels" << endl;
    cout << "      pdconn: ROW is dense, COLUMN is parcels" << endl;
    cout << "      dpconn: ROW is parcels, COLUMN is dense" << endl;
    cout << "      pscalar: ROW is scalars, COLUMN is parcels" << endl;
    cout << "      ptseries: ROW is series, COLUMN is parcels" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   For the full details of the CIFTI format, see" << endl;
    cout << "      http://www.nitrc.org/projects/cifti/" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
}

void CommandOperationManager::printGiftiHelp()
{
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   The GIFTI format is an established data file format intended for use with" << endl;
    cout << "   surface-based data.  It has subtypes for geometry (.surf.gii), continuous" << endl;
    cout << "   data (.func.gii, .shape.gii), and integer label data (.label.gii).  The" << endl;
    cout << "   files that contain data, rather than geometry, consist mainly of a 2D array," << endl;
    cout << "   with one dimension having length equal to the number of vertices in the" << endl;
    cout << "   surface.  Label files (.label.gii) also contain a list of integer values" << endl;
    cout << "   that are used in the file, plus a name and a color for each one.  In" << endl;
    cout << "   workbench, the files for continuous data are called 'metric files', and" << endl;
    cout << "   .func.gii is usually the preferred extension, but there is no difference in" << endl;
    cout << "   file format between .func.gii and .shape.gii.  Geometry files are simply" << endl;
    cout << "   called 'surface files', and must contain only the coordinate and triangle" << endl;
    cout << "   arrays.  Notably, other software may put data arrays (the equivalent of a" << endl;
    cout << "   metric file) into the same file as the geometry information.  Workbench does" << endl;
    cout << "   not support this kind of combined format, and you must use other tools to" << endl;
    cout << "   separate the data array from the geometry." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   For the full details of the GIFTI format, see" << endl;
    cout << "      http://www.nitrc.org/projects/gifti/" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
}

void CommandOperationManager::printParallelHelp(const AString& programName)
{
    //guide for wrap, assuming 80 columns:                                                  |
    cout << "   Many processing commands make use of multithreading so that scripts can" << endl;
    cout << "   finish more quickly.  By default, these commands will use all available" << endl;
    cout << "   cores on the system.  Because we use OpenMP for the parallelization, this" << endl;
    cout << "   behavior can be controlled through various environment variables.  In" << endl;
    cout << "   particular, 'OMP_NUM_THREADS' will set the maximum number of threads it will" << endl;
    cout << "   use, when exported as an environment variable.  Additionally, some shells," << endl;
    cout << "   such as bash, have syntax that allows you to set an environment variable for" << endl;
    cout << "   a single command or script, for instance:" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "$ OMP_NUM_THREADS=4 "<< programName << " -volume-smoothing input.nii.gz 4 output.nii.gz" << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   If you have a multi-socket system, be aware that the parallelization can be" << endl;
    cout << "   much slower when threads are on different sockets, and this interacts badly" << endl;
    cout << "   with the default behavior of using all available cores.  It is advisable to" << endl;
    cout << "   use other tools to restrict the entire script to execute on a single socket," << endl;
    cout << "   especially if a queueing system is involved." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
    cout << "   Also note that wb_view contains a few features that use multithreading" << endl;
    cout << "   (dynamic connectivity, border optimize), which can be controlled by setting" << endl;
    cout << "   the same environment variables before launching wb_view." << endl;
    cout << endl;//guide for wrap, assuming 80 columns:                                     |
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

AString CommandOperationManager::fixUnicode(const AString& input, const bool& quiet)
{
    bool hyphenReplaced = false, otherNonAscii = false;
    AString ret = input.fixUnicodeHyphens(&hyphenReplaced, &otherNonAscii, quiet);
    if (otherNonAscii)
    {
        throw CaretException("found non-ascii character in operation switch '" + input + "', but one that is not recognized as a dash/hyphen/minus");
    }
    if (!quiet && hyphenReplaced)
    {
        CaretLogWarning("replaced non-ascii hyphen/dash characters in operation switch '" + input + "' with ascii '-'");
    }
    return ret;
}
