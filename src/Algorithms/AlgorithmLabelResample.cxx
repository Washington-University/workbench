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

#include "AlgorithmLabelResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
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
    
    OptionalParameter* areaMetricsOpt = ret->createOptionalParameter(7, "-area-metrics", "specify vertex area metrics to do area correction based on");
    areaMetricsOpt->addMetricParameter(1, "current-area", "a metric file with vertex areas for <current-sphere> mesh");
    areaMetricsOpt->addMetricParameter(2, "new-area", "a metric file with vertex areas for <new-sphere> mesh");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(8, "-current-roi", "use an input roi on the current mesh to exclude non-data vertices");
    roiOpt->addMetricParameter(1, "roi-metric", "the roi, as a metric file");
    
    OptionalParameter* validRoiOutOpt = ret->createOptionalParameter(9, "-valid-roi-out", "output the ROI of vertices that got data from valid source vertices");
    validRoiOutOpt->addMetricOutputParameter(1, "roi-out", "the output roi as a metric");
    
    ret->createOptionalParameter(10, "-largest", "use only the label of the vertex with the largest weight");
    
    AString myHelpText =
        AString("Resamples a label file, given two spherical surfaces that are in register.  ") +
        "If the method does area correction, exactly one of -area-surfs or -area-metrics must be specified.\n\n" +
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
    MetricFile* curAreas = NULL, *newAreas = NULL;
    MetricFile curAreasTemp, newAreasTemp;
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
        vector<float> nodeAreasTemp;
        SurfaceFile* curAreaSurf = areaSurfsOpt->getSurface(1);
        SurfaceFile* newAreaSurf = areaSurfsOpt->getSurface(2);
        curAreaSurf->computeNodeAreas(nodeAreasTemp);
        curAreasTemp.setNumberOfNodesAndColumns(curAreaSurf->getNumberOfNodes(), 1);
        curAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
        curAreas = &curAreasTemp;
        newAreaSurf->computeNodeAreas(nodeAreasTemp);
        newAreasTemp.setNumberOfNodesAndColumns(newAreaSurf->getNumberOfNodes(), 1);
        newAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
        newAreas = &newAreasTemp;
    }
    OptionalParameter* areaMetricsOpt = myParams->getOptionalParameter(7);
    if (areaMetricsOpt->m_present)
    {
        if (areaSurfsOpt->m_present)
        {
            throw AlgorithmException("only one of -area-surfs and -area-metrics can be specified");
        }
        switch(myMethod)
        {
            case SurfaceResamplingMethodEnum::BARYCENTRIC:
                CaretLogInfo("This method does not use area correction, -area-metrics is not needed");
                break;
            default:
                break;
        }
        curAreas = areaMetricsOpt->getMetric(1);
        newAreas = areaMetricsOpt->getMetric(2);
    }
    OptionalParameter* roiOpt = myParams->getOptionalParameter(8);
    MetricFile* currentRoi = NULL;
    if (roiOpt->m_present)
    {
        currentRoi = roiOpt->getMetric(1);
    }
    MetricFile* validRoiOut = NULL;
    OptionalParameter* validRoiOutOpt = myParams->getOptionalParameter(9);
    if (validRoiOutOpt->m_present)
    {
        validRoiOut = validRoiOutOpt->getOutputMetric(1);
    }
    bool largest = myParams->getOptionalParameter(10)->m_present;
    AlgorithmLabelResample(myProgObj, labelIn, curSphere, newSphere, myMethod, labelOut, curAreas, newAreas, currentRoi, validRoiOut, largest);
}

AlgorithmLabelResample::AlgorithmLabelResample(ProgressObject* myProgObj, const LabelFile* labelIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                               const SurfaceResamplingMethodEnum::Enum& myMethod, LabelFile* labelOut, const MetricFile* curAreas,
                                               const MetricFile* newAreas, const MetricFile* currentRoi, MetricFile* validRoiOut, const bool& largest) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (labelIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input label file has different number of nodes than input sphere");
    const float* curAreaData = NULL, *newAreaData = NULL;
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            break;
        default:
            if (curAreas == NULL || newAreas == NULL) throw AlgorithmException("specified method does area correction, but no vertex area data given");
            if (curSphere->getNumberOfNodes() != curAreas->getNumberOfNodes()) throw AlgorithmException("current vertex area data has different number of nodes than current sphere");
            if (newSphere->getNumberOfNodes() != newAreas->getNumberOfNodes()) throw AlgorithmException("new vertex area data has different number of nodes than new sphere");
            curAreaData = curAreas->getValuePointerForColumn(0);
            newAreaData = newAreas->getValuePointerForColumn(0);
    }
    int numColumns = labelIn->getNumberOfColumns(), numNewNodes = newSphere->getNumberOfNodes();
    labelOut->setNumberOfNodesAndColumns(numNewNodes, numColumns);
    labelOut->setStructure(labelIn->getStructure());
    *labelOut->getLabelTable() = *labelIn->getLabelTable();
    int32_t unusedLabel = labelIn->getLabelTable()->getUnassignedLabelKey();
    vector<int32_t> colScratch(numNewNodes, unusedLabel);
    const float* roiCol = NULL;
    if (currentRoi != NULL) roiCol = currentRoi->getValuePointerForColumn(0);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curAreaData, newAreaData, roiCol);
    if (validRoiOut != NULL)
    {
        validRoiOut->setNumberOfNodesAndColumns(numNewNodes, 1);
        validRoiOut->setStructure(labelIn->getStructure());
        vector<float> scratch(numNewNodes);
        myHelp.getResampleValidROI(scratch.data());
        validRoiOut->setValuesForColumn(0, scratch.data());
    }
    for (int i = 0; i < numColumns; ++i)
    {
        labelOut->setColumnName(i, labelIn->getColumnName(i));
        if (largest)
        {
            myHelp.resampleLargest(labelIn->getLabelKeyPointerForColumn(i), colScratch.data(), unusedLabel);
        } else {
            myHelp.resamplePopular(labelIn->getLabelKeyPointerForColumn(i), colScratch.data(), unusedLabel);
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
