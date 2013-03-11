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

#include "AlgorithmLabelResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelResample::getCommandSwitch()
{
    return "-label-resample";
}

AString AlgorithmLabelResample::getShortDescription()
{
    return "RESAMPLE A LABEL FILE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmLabelResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label-in", "the label file to resample");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the label file is currently on");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh");
    
    ret->addStringParameter(4, "method", "the method name");
    
    ret->addLabelOutputParameter(5, "label-out", "the output label file");
    
    OptionalParameter* areaSurfsOpt = ret->createOptionalParameter(6, "-area-surfs", "specify surfaces to do vertex area correction based on");
    areaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant anatomical surface with <current-sphere> mesh");
    areaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant anatomical surface with <new-sphere> mesh");
    
    ret->createOptionalParameter(7, "-largest", "use only the label of the vertex with the largest weight");
    
    AString myHelpText =
        AString("Resamples a label file, given two spherical surfaces that are in register.  ") +
        "If -area-surfs are not specified, the sphere surfaces are used for area correction, if the method used does area correction.\n\n" +
        "The -largest option results in nearest vertex behavior when used with BARYCENTRIC, it uses the value of the source vertex that has the largest weight.  " +
        "When -largest is not specified, the vertex weights are summed according to which label they correspond to, and the label with the largest sum is used.\n\n" +
        "The <method> argument must be one of the following:\n\n";
    
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    
    myHelpText += "\nThe ADAP_BARY_AREA method is recommended for label data, because it should be better at resolving vertices that are near multiple labels, or in case of downsampling.";
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmLabelResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* labelIn = myParams->getLabel(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum myMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(4), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid method name");
    }
    LabelFile* labelOut = myParams->getOutputLabel(5);
    SurfaceFile* curArea = curSphere, *newArea = newSphere;
    OptionalParameter* areaSurfsOpt = myParams->getOptionalParameter(6);
    if (areaSurfsOpt->m_present)
    {
        switch(myMethod)
        {
            case SurfaceResamplingMethodEnum::BARYCENTRIC:
                CaretLogInfo("This method does not use area correction, -area-surfs is not needed");
                break;
            default:
                break;
        }
        curArea = areaSurfsOpt->getSurface(1);
        newArea = areaSurfsOpt->getSurface(2);
    }
    bool largest = myParams->getOptionalParameter(7)->m_present;
    AlgorithmLabelResample(myProgObj, labelIn, curSphere, newSphere, myMethod, labelOut, curArea, newArea, largest);
}

AlgorithmLabelResample::AlgorithmLabelResample(ProgressObject* myProgObj, const LabelFile* labelIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                               const SurfaceResamplingMethodEnum::Enum& myMethod, LabelFile* labelOut, const SurfaceFile* curArea,
                                               const SurfaceFile* newArea, const bool& largest) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (labelIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input label file has different number of nodes than input sphere");
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            break;
        default:
            if (curArea == NULL || newArea == NULL) throw AlgorithmException("specified method does area correction, but no area surfaces given");
            if (curSphere->getNumberOfNodes() != curArea->getNumberOfNodes()) throw AlgorithmException("current area surface has different number of nodes than current sphere");
            if (newSphere->getNumberOfNodes() != newArea->getNumberOfNodes()) throw AlgorithmException("new area surface has different number of nodes than new sphere");
    }
    int numColumns = labelIn->getNumberOfColumns(), numNewNodes = newSphere->getNumberOfNodes();
    labelOut->setNumberOfNodesAndColumns(numNewNodes, numColumns);
    labelOut->setStructure(newSphere->getStructure());
    *labelOut->getLabelTable() = *labelIn->getLabelTable();
    vector<int32_t> colScratch(numNewNodes, 0.0f);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curArea, newArea);
    for (int i = 0; i < numColumns; ++i)
    {
        labelOut->setColumnName(i, labelIn->getColumnName(i));
        if (largest)
        {
            myHelp.resampleLargest(labelIn->getLabelKeyPointerForColumn(i), colScratch.data());
        } else {
            myHelp.resamplePopular(labelIn->getLabelKeyPointerForColumn(i), colScratch.data());
        }
        labelOut->setLabelKeysForColumn(i, colScratch.data());
    }
}

float AlgorithmLabelResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
