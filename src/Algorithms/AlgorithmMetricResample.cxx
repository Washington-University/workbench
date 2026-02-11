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

#include "AlgorithmMetricResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmMetricResample::getCommandSwitch()
{
    return "-metric-resample";
}

AString AlgorithmMetricResample::getShortDescription()
{
    return "RESAMPLE A METRIC FILE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmMetricResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric-in", "the metric file to resample, typically a native-mesh metric");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface that is in register with <new-sphere> and has the mesh that the metric is currently on, typically a registered native-mesh sphere, such as {subject}.L.sphere.MSMAll.native.surf.gii");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that the desired output mesh, typically a standard sphere, such as L.sphere.32k_fs_LR.surf.gii");
    
    ret->addStringParameter(4, "method", "the method name");
    
    ret->addMetricOutputParameter(5, "metric-out", "the output metric");
    
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
    
    ret->createOptionalParameter(10, "-largest", "use only the value of the vertex with the largest weight");
    
    ret->createOptionalParameter(11, "-bypass-sphere-check", "ADVANCED: allow the current and new 'spheres' to have arbitrary shape as long as they follow the same contour");
    
    AString myHelpText =
        AString("Resamples a metric file, given two spherical surfaces that are in register.  ") +
        "If ADAP_BARY_AREA is used, exactly one of -area-surfs or -area-metrics must be specified.\n\n" +
        "The ADAP_BARY_AREA method is recommended for ordinary metric data, because it should use all data while downsampling, unlike BARYCENTRIC.  " +
        "The recommended areas option for most data is individual midthicknesses for individual data, and averaged vertex area metrics from individual midthicknesses for group average data.\n\n"
        "The -current-roi option only masks the input, the output may be slightly dilated in comparison, consider using -metric-mask on the output " +
        "when using -current-roi.\n\n" +
        "The -largest option results in nearest vertex behavior when used with BARYCENTRIC.  " +
        "When resampling a binary metric, consider thresholding at 0.5 after resampling rather than using -largest.\n\n" +
        "The <method> argument must be one of the following:\n\n";
    
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmMetricResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* metricIn = myParams->getMetric(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum myMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(4), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid method name");
    }
    MetricFile* metricOut = myParams->getOutputMetric(5);
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
    bool allowNonSphere = myParams->getOptionalParameter(11)->m_present;
    AlgorithmMetricResample(myProgObj, metricIn, curSphere, newSphere, myMethod, metricOut, curAreas, newAreas, currentRoi, validRoiOut, largest, allowNonSphere);
}

AlgorithmMetricResample::AlgorithmMetricResample(ProgressObject* myProgObj, const MetricFile* metricIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                 const SurfaceResamplingMethodEnum::Enum& myMethod, MetricFile* metricOut, const MetricFile* curAreas, const MetricFile* newAreas,
                                                 const MetricFile* currentRoi, MetricFile* validRoiOut, const bool& largest, const bool& allowNonSphere) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (metricIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input metric has different number of nodes than input sphere");
    if (currentRoi != NULL && currentRoi->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("roi metric has different number of nodes than input sphere");
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
    int numColumns = metricIn->getNumberOfColumns(), numNewNodes = newSphere->getNumberOfNodes();
    metricOut->setNumberOfNodesAndColumns(numNewNodes, numColumns);
    metricOut->setStructure(metricIn->getStructure());
    vector<float> colScratch(numNewNodes, 0.0f);
    const float* roiCol = NULL;
    if (currentRoi != NULL) roiCol = currentRoi->getValuePointerForColumn(0);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curAreaData, newAreaData, roiCol, allowNonSphere);
    if (validRoiOut != NULL)
    {
        validRoiOut->setNumberOfNodesAndColumns(numNewNodes, 1);
        validRoiOut->setStructure(metricIn->getStructure());
        vector<float> scratch(numNewNodes);
        myHelp.getResampleValidROI(scratch.data());
        validRoiOut->setValuesForColumn(0, scratch.data());
    }
    for (int i = 0; i < numColumns; ++i)
    {
        metricOut->setColumnName(i, metricIn->getColumnName(i));
        *metricOut->getPaletteColorMapping(i) = *metricIn->getPaletteColorMapping(i);
        if (largest)
        {
            myHelp.resampleLargest(metricIn->getValuePointerForColumn(i), colScratch.data());
        } else {
            myHelp.resampleNormal(metricIn->getValuePointerForColumn(i), colScratch.data());
        }
        metricOut->setValuesForColumn(i, colScratch.data());
    }
}

float AlgorithmMetricResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
