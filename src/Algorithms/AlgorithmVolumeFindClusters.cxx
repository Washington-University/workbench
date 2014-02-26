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

#include "AlgorithmVolumeFindClusters.h"
#include "AlgorithmException.h"

#include "VolumeFile.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeFindClusters::getCommandSwitch()
{
    return "-volume-find-clusters";
}

AString AlgorithmVolumeFindClusters::getShortDescription()
{
    return "FILTER CLUSTERS BY VOLUME";
}

OperationParameters* AlgorithmVolumeFindClusters::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    ret->addDoubleParameter(2, "minimum-value", "threshold for data values");
    
    ret->addDoubleParameter(3, "minimum-volume", "threshold for cluster volume, in mm^3");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(5, "-roi", "select a region of interest");
    roiOption->addMetricParameter(1, "roi-volume", "the roi, as a volume file");
    
    OptionalParameter* subvolSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to take the gradient of");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Outputs a volume with ones for all voxels within a large enough cluster, and zeros elsewhere.  ") +
        "To apply this as a mask to the data, or to do more complicated thresholding, see -volume-math."
    );
    return ret;
}

void AlgorithmVolumeFindClusters::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    float minValue = (float)myParams->getDouble(2);
    float minVolume = (float)myParams->getDouble(3);
    VolumeFile* volOut = myParams->getOutputVolume(4);
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOption = myParams->getOptionalParameter(5);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getVolume(1);
    }
    OptionalParameter* subvolSelect = myParams->getOptionalParameter(6);
    int subvolNum = -1;
    if (subvolSelect->m_present)
    {
        subvolNum = (int)volIn->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
        if (subvolNum < 0)
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    AlgorithmVolumeFindClusters(myProgObj, volIn, minValue, minVolume, volOut, myRoi, subvolNum);
}

AlgorithmVolumeFindClusters::AlgorithmVolumeFindClusters(ProgressObject* myProgObj, const VolumeFile* volIn, const float& minValue, const float& minVolume,
                                                         VolumeFile* volOut, const VolumeFile* myRoi, const int& subvolNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const VolumeSpace& mySpace = volIn->getVolumeSpace();
    if (myRoi != NULL && !mySpace.matchesVolumeSpace(myRoi->getVolumeSpace())) throw AlgorithmException("roi volume space does not match input");
    Vector3D ivec, jvec, kvec, origin;
    mySpace.getSpacingVectors(ivec, jvec, kvec, origin);
    float voxelVolume = abs(ivec.dot(jvec.cross(kvec)));
}

float AlgorithmVolumeFindClusters::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeFindClusters::getSubAlgorithmWeight()
{
    return 0.0f;
}
