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

#include "AlgorithmCiftiSeparate.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "OperationCiftiSeparateAll.h"
#include "OperationException.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationCiftiSeparateAll::getCommandSwitch()
{
    return "-cifti-separate-all";
}

AString OperationCiftiSeparateAll::getShortDescription()
{
    return "DEPRECATED: use -cifti-separate";
}

OperationParameters* OperationCiftiSeparateAll::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti to split");

    OptionalParameter* leftOpt = ret->createOptionalParameter(2, "-left", "output the left surface data");
    leftOpt->addMetricOutputParameter(1, "left-metric", "the output metric for the left surface");
    OptionalParameter* leftRoiOpt = leftOpt->createOptionalParameter(2, "-left-roi", "output the ROI for the left surface data");
    leftRoiOpt->addMetricOutputParameter(1, "left-roi-metric", "output metric for the left ROI");
    
    OptionalParameter* rightOpt = ret->createOptionalParameter(3, "-right", "output the right surface data");
    rightOpt->addMetricOutputParameter(1, "right-metric", "the output metric for the right surface");
    OptionalParameter* rightRoiOpt = rightOpt->createOptionalParameter(2, "-right-roi", "output the ROI for the right surface data");
    rightRoiOpt->addMetricOutputParameter(1, "right-roi-metric", "output metric for the right ROI");
    
    OptionalParameter* cerebOpt = ret->createOptionalParameter(4, "-cerebellum", "output the cerebellum surface data");
    cerebOpt->addMetricOutputParameter(1, "cerebellum-metric", "the output metric for the cerebellum surface");
    OptionalParameter* cerebRoiOpt = cerebOpt->createOptionalParameter(2, "-cerebellum-roi", "output the ROI for the cerebellum surface data");
    cerebRoiOpt->addMetricOutputParameter(1, "cerebellum-roi-metric", "output metric for the cerebellum ROI");
    
    OptionalParameter* volOpt = ret->createOptionalParameter(5, "-volume", "output the voxel data");
    volOpt->addVolumeOutputParameter(1, "volume-out", "output volume file");
    OptionalParameter* volRoiOpt = volOpt->createOptionalParameter(2, "-volume-roi", "output the combined ROI for the volume data");
    volRoiOpt->addVolumeOutputParameter(1, "volume-roi-out", "output volume for ROI");
    
    OptionalParameter* dirOpt = ret->createOptionalParameter(6, "-direction", "choose the direction to separate (default COLUMN)");
    dirOpt->addStringParameter(1, "direction", "which direction to separate into components, ROW or COLUMN");
    
    ret->setHelpText(
        AString("DEPRECATED: this command may be removed in a future release, use -cifti-separate.\n\n") +
        "All volume components are put together into one volume, the boundaries between volume components are not output by this command.  " +
        "The COLUMN direction (default) is usually what you want, ROW will only work for dconn.  " +
        "Using this command with -volume will usually take (much) more memory than the cifti file, since it must create the whole volume, rather than just the included voxels."
    );
    return ret;
}

void OperationCiftiSeparateAll::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCifti = myParams->getCifti(1);
    int myDir = CiftiXMLOld::ALONG_COLUMN;
    OptionalParameter* dirOpt = myParams->getOptionalParameter(6);
    if (dirOpt->m_present)
    {
        AString dirName = dirOpt->getString(1);
        if (dirName == "ROW")
        {
            myDir = CiftiXMLOld::ALONG_ROW;
        } else {
            if (dirName != "COLUMN")
            {
                throw OperationException("direction must be 'ROW' or 'COLUMN'");
            }
        }
    }
    OptionalParameter* leftOpt = myParams->getOptionalParameter(2);
    if (leftOpt->m_present)
    {
        MetricFile* outData = leftOpt->getOutputMetric(1);
        MetricFile* outRoi = NULL;
        OptionalParameter* roiOpt = leftOpt->getOptionalParameter(2);
        if (roiOpt->m_present)
        {
            outRoi = roiOpt->getOutputMetric(1);
        }
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_LEFT, myDir, outData, outRoi);
    }
    OptionalParameter* rightOpt = myParams->getOptionalParameter(3);
    if (rightOpt->m_present)
    {
        MetricFile* outData = rightOpt->getOutputMetric(1);
        MetricFile* outRoi = NULL;
        OptionalParameter* roiOpt = rightOpt->getOptionalParameter(2);
        if (roiOpt->m_present)
        {
            outRoi = roiOpt->getOutputMetric(1);
        }
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_RIGHT, myDir, outData, outRoi);
    }
    OptionalParameter* cerebOpt = myParams->getOptionalParameter(4);
    if (cerebOpt->m_present)
    {
        MetricFile* outData = cerebOpt->getOutputMetric(1);
        MetricFile* outRoi = NULL;
        OptionalParameter* roiOpt = cerebOpt->getOptionalParameter(2);
        if (roiOpt->m_present)
        {
            outRoi = roiOpt->getOutputMetric(1);
        }
        processSurfaceComponent(myCifti, StructureEnum::CEREBELLUM, myDir, outData, outRoi);
    }
    OptionalParameter* volOpt = myParams->getOptionalParameter(5);
    if (volOpt->m_present)
    {
        VolumeFile* outData = volOpt->getOutputVolume(1);
        VolumeFile* outRoi = NULL;
        OptionalParameter* roiOpt = volOpt->getOptionalParameter(2);
        if (roiOpt->m_present)
        {
            outRoi = roiOpt->getOutputVolume(1);
        }
        processVolume(myCifti, myDir, outData, outRoi);
    }
}

void OperationCiftiSeparateAll::processSurfaceComponent(const CiftiFile* myCifti, const StructureEnum::Enum& myStruct,
                                                        const int& myDir, MetricFile* outData, MetricFile* outROI)
{
    AlgorithmCiftiSeparate(NULL, myCifti, myDir, myStruct, outData, outROI);
}

void OperationCiftiSeparateAll::processVolume(const CiftiFile* myCifti, const int& myDir, VolumeFile* outData, VolumeFile* outROI)
{
    int64_t offset[3];
    AlgorithmCiftiSeparate(NULL, myCifti, myDir, outData, offset, outROI, false);
}
