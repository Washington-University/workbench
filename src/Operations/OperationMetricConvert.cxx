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

#include "OperationMetricConvert.h"
#include "OperationException.h"
#include "CaretLogger.h"
#include "FloatMatrix.h"
#include "MetricFile.h"
#include "NiftiFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricConvert::getCommandSwitch()
{
    return "-metric-convert";
}

AString OperationMetricConvert::getShortDescription()
{
    return "CONVERT TO OR FROM METRIC FILE";
}

OperationParameters* OperationMetricConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    OptionalParameter* toNifti = ret->createOptionalParameter(1, "-to-nifti", "convert metric to nifti");
    toNifti->addMetricParameter(1, "metric-in", "the metric to convert");
    toNifti->addVolumeOutputParameter(2, "nifti-out", "the output nifti file");//we can use VolumeFile because it is 4D, though some spatial dimensions may be singular
    
    OptionalParameter* fromNifti = ret->createOptionalParameter(2, "-from-nifti", "convert nifti to metric");
    fromNifti->addStringParameter(1, "nifti-in", "the nifti file to convert");
    fromNifti->addSurfaceParameter(2, "surface-in", "surface file to use number of nodes and structure from");
    fromNifti->addMetricOutputParameter(3, "metric-out", "the output metric file");
    
    ret->setHelpText(
        AString("The purpose of this command is to convert between metric files and nifti1 so that gifti-unaware programs can operate on the data.  ") +
        "You must specify exactly one of the options."
    );
    return ret;
}

void OperationMetricConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int modes = 0;
    OptionalParameter* toNifti = myParams->getOptionalParameter(1);
    if (toNifti->m_present) ++modes;
    OptionalParameter* fromNifti = myParams->getOptionalParameter(2);
    if (fromNifti->m_present) ++modes;
    if (modes != 1)
    {
        throw OperationException("you must specify exactly one conversion mode");
    }
    if (toNifti->m_present)
    {
        MetricFile* myMetric = toNifti->getMetric(1);
        VolumeFile* outVolume = toNifti->getOutputVolume(2);
        int numNodes = myMetric->getNumberOfNodes(), numCols = myMetric->getNumberOfColumns();
        if (numCols > 32767)//max of short
        {
            throw OperationException("number of metric columns exceeds nifti1 limit, failing");
        }
        vector<int64_t> myDims(4, 1);
        myDims[3] = numCols;
        if (numNodes > 32767)
        {
            myDims[0] = 32767;
            int temp = (numNodes - 1) / 32767 + 1;//round up instead of down
            if (temp > 32767)//should be rare to the point of nonexistence, but technically possible
            {
                myDims[1] = 32767;
                myDims[2] = (temp - 1) / 32767 + 1;
            } else {
                myDims[1] = temp;
            }
        } else {
            myDims[0] = numNodes;
        }
        int frameSize = myDims[0] * myDims[1] * myDims[2];
        vector<float> scratchFrame(frameSize, 0.0f);
        FloatMatrix mySpace = FloatMatrix::identity(4);
        mySpace[0][0] = -1;//use radiological sform so FSL doesn't do stupid stuff
        outVolume->reinitialize(myDims, mySpace.getMatrix());
        for (int i = 0; i < numCols; ++i)
        {
            const float* myCol = myMetric->getValuePointerForColumn(i);
            for (int j = 0; j < numNodes; ++j)
            {
                scratchFrame[j] = myCol[j];
            }
            outVolume->setFrame(scratchFrame.data(), i);
        }
    }
    if (fromNifti->m_present)
    {
        AString myNiftiName = fromNifti->getString(1);
        SurfaceFile* mySurf = fromNifti->getSurface(2);
        MetricFile* outMetric = fromNifti->getOutputMetric(3);
        vector<int64_t> myDims;
        NiftiFile myNifti(myNiftiName);
        Nifti2Header myHeader;
        myNifti.getHeader(myHeader);
        myHeader.getDimensions(myDims);
        if (myDims.size() == 0) throw OperationException("nifti file has no dimensions");
        while (myDims.size() < 4)
        {
            myDims.push_back(1);
        }
        int numNodes = mySurf->getNumberOfNodes();
        int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
        if (frameSize < numNodes)
        {
            throw OperationException("nifti file does not have dimensions large enough to satisfy specified number of nodes");
        }
        int numCols = (int)myDims[3];//if someone manages over 2 billion timepoints, reward them with truncation
        outMetric->setNumberOfNodesAndColumns(numNodes, numCols);
        outMetric->setStructure(mySurf->getStructure());
        int64_t matrixSize = myNifti.getMatrixLength();
        vector<float> matrix(matrixSize);
        myNifti.getMatrix(matrix.data());
        for (int i = 0; i < numCols; ++i)
        {
            outMetric->setValuesForColumn(i, matrix.data() + i * frameSize);
        }
    }
}
