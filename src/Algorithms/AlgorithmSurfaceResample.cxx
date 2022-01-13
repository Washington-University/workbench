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

#include "AlgorithmSurfaceResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "GiftiMetaData.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceResample::getCommandSwitch()
{
    return "-surface-resample";
}

AString AlgorithmSurfaceResample::getShortDescription()
{
    return "RESAMPLE A SURFACE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmSurfaceResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-in", "the surface file to resample");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the input surface is currently on");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh");
    
    ret->addStringParameter(4, "method", "the method name");
    
    ret->addSurfaceOutputParameter(5, "surface-out", "the output surface file");
    
    OptionalParameter* areaSurfsOpt = ret->createOptionalParameter(6, "-area-surfs", "specify surfaces to do vertex area correction based on");
    areaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant surface with <current-sphere> mesh");
    areaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant surface with <new-sphere> mesh");
    
    OptionalParameter* areaMetricsOpt = ret->createOptionalParameter(7, "-area-metrics", "specify vertex area metrics to do area correction based on");
    areaMetricsOpt->addMetricParameter(1, "current-area", "a metric file with vertex areas for <current-sphere> mesh");
    areaMetricsOpt->addMetricParameter(2, "new-area", "a metric file with vertex areas for <new-sphere> mesh");
    
    ret->createOptionalParameter(8, "-bypass-sphere-check", "ADVANCED: allow the current and new 'spheres' to have arbitrary shape as long as they follow the same contour");
    
    AString myHelpText =
        AString("Resamples a surface file, given two spherical surfaces that are in register.  ") +
        "If ADAP_BARY_AREA is used, exactly one of -area-surfs or -area-metrics must be specified.  " +
        "This method is not generally recommended for surface resampling, but is provided for completeness.\n\n" +
        "The BARYCENTRIC method is generally recommended for anatomical surfaces, in order to minimize smoothing.\n\n" +
        "For cut surfaces (including flatmaps), use -surface-cut-resample.\n\n" +
        "Instead of resampling a spherical surface, the -surface-sphere-project-unproject command is recommended.\n\n" +
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

void AlgorithmSurfaceResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* surfaceIn = myParams->getSurface(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum myMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(4), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid method name");
    }
    SurfaceFile* surfaceOut = myParams->getOutputSurface(5);
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
    bool allowNonSphere = myParams->getOptionalParameter(8)->m_present;
    AlgorithmSurfaceResample(myProgObj, surfaceIn, curSphere, newSphere, myMethod, surfaceOut, curAreas, newAreas, allowNonSphere);
}

AlgorithmSurfaceResample::AlgorithmSurfaceResample(ProgressObject* myProgObj, const SurfaceFile* surfaceIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                   const SurfaceResamplingMethodEnum::Enum& myMethod, SurfaceFile* surfaceOut,
                                                   const MetricFile* curAreas, const MetricFile* newAreas, const bool allowNonSphere) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (surfaceIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input surface has different number of nodes than input sphere");
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
    int numNewNodes = newSphere->getNumberOfNodes();
    GiftiMetaData savedMD;
    GiftiMetaData* provenanceMD = surfaceOut->getFileMetaData();//save provenance, since we are using operator= on surface as a hack around setting the topology
    if (provenanceMD != NULL)
    {
        savedMD = *provenanceMD;//put it into a local instance, because operator= will probably change the one the pointer refers to
    }
    *surfaceOut = *newSphere;
    provenanceMD = surfaceOut->getFileMetaData();//in case the pointer changes anyway
    if (provenanceMD != NULL)
    {
        *provenanceMD = savedMD;//put it back
    }
    surfaceOut->setStructure(surfaceIn->getStructure());
    surfaceOut->setSecondaryType(surfaceIn->getSecondaryType());
    surfaceOut->setSurfaceType(surfaceIn->getSurfaceType());
    vector<float> coordScratch(numNewNodes * 3, 0.0f);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curAreaData, newAreaData, NULL, allowNonSphere);
    myHelp.resample3DCoord(surfaceIn->getCoordinateData(), coordScratch.data());
    surfaceOut->setCoordinates(coordScratch.data());
}

float AlgorithmSurfaceResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
