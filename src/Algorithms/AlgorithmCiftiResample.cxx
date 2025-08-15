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

#include "AlgorithmCiftiResample.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmLabelResample.h"
#include "AlgorithmMetricResample.h"
#include "AlgorithmVolumeAffineResample.h"
#include "AlgorithmVolumeWarpfieldResample.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"
#include "VolumeFile.h"
#include "VolumePaddingHelper.h"
#include "WarpfieldFile.h"

#include <algorithm>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiResample::getCommandSwitch()
{
    return "-cifti-resample";
}

AString AlgorithmCiftiResample::getShortDescription()
{
    return "RESAMPLE A CIFTI FILE TO A NEW CIFTI SPACE";
}

OperationParameters* AlgorithmCiftiResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to resample");
    
    ret->addStringParameter(2, "direction", "the direction of the input that should be resampled, ROW or COLUMN");
    
    ret->addCiftiParameter(3, "cifti-template", "a cifti file containing the cifti space to resample to");
    
    ret->addStringParameter(4, "template-direction", "the direction of the template to use as the resampling space, ROW or COLUMN");
    
    ret->addStringParameter(5, "surface-method", "specify a surface resampling method");
    
    ret->addStringParameter(6, "volume-method", "specify a volume interpolation method");
    
    ret->addCiftiOutputParameter(7, "cifti-out", "the output cifti file");
    
    ret->createOptionalParameter(8, "-surface-largest", "use largest weight instead of weighted average or popularity when doing surface resampling");
    
    OptionalParameter* volDilateOpt = ret->createOptionalParameter(9, "-volume-predilate", "dilate the volume components before resampling");
        volDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
        volDilateOpt->createOptionalParameter(2, "-nearest", "use nearest value dilation");
        OptionalParameter* volDilateWeightedOpt = volDilateOpt->createOptionalParameter(3, "-weighted", "use weighted dilation (default)");
            OptionalParameter* volDilateExpOpt = volDilateWeightedOpt->createOptionalParameter(1, "-exponent", "specify exponent in weighting function");
                volDilateExpOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (1 / (distance ^ n)) as the weighting function (default 7)");
            volDilateWeightedOpt->createOptionalParameter(2, "-legacy-cutoff", "use v1.3.2 logic for the kernel cutoff");
    
    OptionalParameter* surfDilateOpt = ret->createOptionalParameter(10, "-surface-postdilate", "dilate the surface components after resampling");
        surfDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
        surfDilateOpt->createOptionalParameter(2, "-nearest", "use nearest value dilation");
        surfDilateOpt->createOptionalParameter(3, "-linear", "use linear dilation");
        OptionalParameter* surfDilateWeightedOpt = surfDilateOpt->createOptionalParameter(4, "-weighted", "use weighted dilation (default for non-label data)");
            OptionalParameter* surfDilateExpOpt = surfDilateWeightedOpt->createOptionalParameter(1, "-exponent", "specify exponent in weighting function");
                surfDilateExpOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (area / (distance ^ n)) as the weighting function (default 6)");
            surfDilateWeightedOpt->createOptionalParameter(2, "-legacy-cutoff", "use v1.3.2 logic for the kernel cutoff");
    
    OptionalParameter* affineOpt = ret->createOptionalParameter(11, "-affine", "use an affine transformation on the volume components");
        affineOpt->addStringParameter(1, "affine-file", "the affine file to use");
        OptionalParameter* flirtOpt = affineOpt->createOptionalParameter(2, "-flirt", "MUST be used if affine is a flirt affine");
            flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
            flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    
    OptionalParameter* warpfieldOpt = ret->createOptionalParameter(12, "-warpfield", "use a warpfield on the volume components");
        warpfieldOpt->addStringParameter(1, "warpfield", "the warpfield to use");
        OptionalParameter* fnirtOpt = warpfieldOpt->createOptionalParameter(2, "-fnirt", "MUST be used if using a fnirt warpfield");
            fnirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the warpfield");
    
    OptionalParameter* leftSpheresOpt = ret->createOptionalParameter(13, "-left-spheres", "specify spheres for left surface resampling");
        leftSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current left surface");
        leftSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new left mesh that is in register with the current sphere");
        OptionalParameter* leftAreaSurfsOpt = leftSpheresOpt->createOptionalParameter(3, "-left-area-surfs", "specify left surfaces to do vertex area correction based on");
            leftAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant left anatomical surface with current mesh");
            leftAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant left anatomical surface with new mesh");
        OptionalParameter* leftAreaMetricsOpt = leftSpheresOpt->createOptionalParameter(4, "-left-area-metrics", "specify left vertex area metrics to do area correction based on");
            leftAreaMetricsOpt->addMetricParameter(1, "current-area", "a metric file with vertex areas for the current mesh");
            leftAreaMetricsOpt->addMetricParameter(2, "new-area", "a metric file with vertex areas for the new mesh");
    
    OptionalParameter* rightSpheresOpt = ret->createOptionalParameter(14, "-right-spheres", "specify spheres for right surface resampling");
        rightSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current right surface");
        rightSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new right mesh that is in register with the current sphere");
        OptionalParameter* rightAreaSurfsOpt = rightSpheresOpt->createOptionalParameter(3, "-right-area-surfs", "specify right surfaces to do vertex area correction based on");
            rightAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant right anatomical surface with current mesh");
            rightAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant right anatomical surface with new mesh");
        OptionalParameter* rightAreaMetricsOpt = rightSpheresOpt->createOptionalParameter(4, "-right-area-metrics", "specify right vertex area metrics to do area correction based on");
            rightAreaMetricsOpt->addMetricParameter(1, "current-area", "a metric file with vertex areas for the current mesh");
            rightAreaMetricsOpt->addMetricParameter(2, "new-area", "a metric file with vertex areas for the new mesh");
    
    OptionalParameter* cerebSpheresOpt = ret->createOptionalParameter(15, "-cerebellum-spheres", "specify spheres for cerebellum surface resampling");
        cerebSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current cerebellum surface");
        cerebSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new cerebellum mesh that is in register with the current sphere");
        OptionalParameter* cerebAreaSurfsOpt = cerebSpheresOpt->createOptionalParameter(3, "-cerebellum-area-surfs", "specify cerebellum surfaces to do vertex area correction based on");
            cerebAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant cerebellum anatomical surface with current mesh");
            cerebAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant cerebellum anatomical surface with new mesh");
        OptionalParameter* cerebAreaMetricsOpt = cerebSpheresOpt->createOptionalParameter(4, "-cerebellum-area-metrics", "specify cerebellum vertex area metrics to do area correction based on");
            cerebAreaMetricsOpt->addMetricParameter(1, "current-area", "a metric file with vertex areas for the current mesh");
            cerebAreaMetricsOpt->addMetricParameter(2, "new-area", "a metric file with vertex areas for the new mesh");
    
    AString myHelpText =
        AString("Resample cifti data to a different brainordinate space.  Use COLUMN for the direction to resample dscalar, dlabel, or dtseries.  ") +
        "Resampling both dimensions of a dconn requires running this command twice, once with COLUMN and once with ROW.  " +
        "If you are resampling a dconn and your machine has a large amount of memory, you might consider using -cifti-resample-dconn-memory to avoid writing and rereading an intermediate file.  " +
        "The <template-direction> argument should usually be COLUMN, as dtseries, dscalar, and dlabel all have brainordinates on that direction.  " +
        "If spheres are not specified for a surface structure which exists in the cifti files, its data is copied without resampling or dilation.  " +
        "Dilation is done with the 'nearest' method, and is done on <new-sphere> for surface data.  " +
        "Volume components are padded before dilation so that dilation doesn't run into the edge of the component bounding box.  " +
        "If neither -affine nor -warpfield are specified, the identity transform is assumed for the volume data.\n\n" +
        "The recommended resampling methods are ADAP_BARY_AREA and CUBIC (cubic spline), except for label data which should use ADAP_BARY_AREA and ENCLOSING_VOXEL.  " +
        "Using ADAP_BARY_AREA requires specifying an area option to each used -*-spheres option.\n\n" +
        "The <volume-method> argument must be one of the following:\n\n" +
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR\n\n" +
        "The <surface-method> argument must be one of the following:\n\n";
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmCiftiResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiIn = myParams->getCifti(1);
    AString myDirString = myParams->getString(2);
    int direction = -1;
    if (myDirString == "ROW")
    {
        direction = CiftiXML::ALONG_ROW;
    } else {
        if (myDirString == "COLUMN")
        {
            direction = CiftiXML::ALONG_COLUMN;
        } else {
            throw AlgorithmException("unrecognized direction string, use ROW or COLUMN");
        }
    }
    CiftiFile* myTemplate = myParams->getCifti(3);
    AString myTemplDirString = myParams->getString(4);
    int templateDir = -1;
    if (myTemplDirString == "ROW")
    {
        templateDir = CiftiXML::ALONG_ROW;
    } else {
        if (myTemplDirString == "COLUMN")
        {
            templateDir = CiftiXML::ALONG_COLUMN;
        } else {
            throw AlgorithmException("unrecognized template direction string, use ROW or COLUMN");
        }
    }
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum mySurfMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(5), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid surface resampling method name");
    }
    AString myVolMethodString = myParams->getString(6);
    VolumeFile::InterpType myVolMethod = VolumeFile::CUBIC;
    if (myVolMethodString == "CUBIC")
    {
        myVolMethod = VolumeFile::CUBIC;
    } else if (myVolMethodString == "TRILINEAR") {
        myVolMethod = VolumeFile::TRILINEAR;
    } else if (myVolMethodString == "ENCLOSING_VOXEL") {
        myVolMethod = VolumeFile::ENCLOSING_VOXEL;
    } else {
        throw AlgorithmException("unrecognized volume interpolation method");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(7);
    bool surfLargest = myParams->getOptionalParameter(8)->m_present;
    float voldilatemm = -1.0f, surfdilatemm = -1.0f;
    bool isLabelData = false;//dilation method arguments checking needs to know if it is label data
    const CiftiXML& inputXML = myCiftiIn->getCiftiXML();
    for (int i = 0; i < inputXML.getNumberOfDimensions(); ++i)
    {
        if (inputXML.getMappingType(i) == CiftiMappingType::LABELS)
        {
            isLabelData = true;
            break;
        }
    }
    AlgorithmVolumeDilate::Method volDilateMethod = AlgorithmVolumeDilate::WEIGHTED;
    float volDilateExponent = 7.0f;
    AlgorithmMetricDilate::Method surfDilateMethod = AlgorithmMetricDilate::WEIGHTED;//label dilate doesn't support multiple methods
    float surfDilateExponent = 6.0f;//label dilate currently only supports nearest, so in order to accept a default in the algorithm, it currently ignores this on label data, no warning
    bool surfLegacyCutoff = false, volLegacyCutoff = false;
    OptionalParameter* volDilateOpt = myParams->getOptionalParameter(9);//but it does check the options on the command line for sanity
    if (volDilateOpt->m_present)
    {
        bool methodSpecified = false;
        voldilatemm = (float)volDilateOpt->getDouble(1);
        if (voldilatemm <= 0.0f) throw AlgorithmException("dilation amount must be positive");
        if (volDilateOpt->getOptionalParameter(2)->m_present)
        {
            methodSpecified = true;
            volDilateMethod = AlgorithmVolumeDilate::NEAREST;
        }
        OptionalParameter* volDilateWeightedOpt = volDilateOpt->getOptionalParameter(3);
        if (volDilateWeightedOpt->m_present)
        {
            if (methodSpecified) throw AlgorithmException("cannot specify multiple volume dilation methods");
            methodSpecified = true;
            volDilateMethod = AlgorithmVolumeDilate::WEIGHTED;
            OptionalParameter* volDilateExpOpt = volDilateWeightedOpt->getOptionalParameter(1);
            if (volDilateExpOpt->m_present)
            {
                volDilateExponent = (float)volDilateExpOpt->getDouble(1);
            }
            volLegacyCutoff = volDilateWeightedOpt->getOptionalParameter(2)->m_present;
        }
    }
    OptionalParameter* surfDilateOpt = myParams->getOptionalParameter(10);
    if (surfDilateOpt->m_present)
    {
        bool methodSpecified = false;
        surfdilatemm = (float)surfDilateOpt->getDouble(1);
        if (surfdilatemm <= 0.0f) throw AlgorithmException("dilation amount must be positive");
        if (surfDilateOpt->getOptionalParameter(2)->m_present)
        {
            methodSpecified = true;
            surfDilateMethod = AlgorithmMetricDilate::NEAREST;
        }
        if (surfDilateOpt->getOptionalParameter(3)->m_present)
        {
            if (methodSpecified) throw AlgorithmException("cannot specify multiple surface dilation methods");
            methodSpecified = true;
            if (isLabelData) throw AlgorithmException("cannot do linear surface dilation on label data");
            surfDilateMethod = AlgorithmMetricDilate::LINEAR;
        }
        OptionalParameter* surfDilateWeightedOpt = surfDilateOpt->getOptionalParameter(4);
        if (surfDilateWeightedOpt->m_present)
        {
            if (methodSpecified) throw AlgorithmException("cannot specify multiple surface dilation methods");
            methodSpecified = true;
            if (isLabelData) throw AlgorithmException("cannot do weighted surface dilation on label data");
            surfDilateMethod = AlgorithmMetricDilate::WEIGHTED;
            OptionalParameter* surfDilateExpOpt = surfDilateWeightedOpt->getOptionalParameter(1);
            if (surfDilateExpOpt->m_present)
            {
                surfDilateExponent = (float)surfDilateExpOpt->getDouble(1);
            }
        }
        surfLegacyCutoff = surfDilateWeightedOpt->getOptionalParameter(2)->m_present;
    }
    OptionalParameter* affineOpt = myParams->getOptionalParameter(11);
    OptionalParameter* warpfieldOpt = myParams->getOptionalParameter(12);
    if (affineOpt->m_present && warpfieldOpt->m_present) throw AlgorithmException("you cannot specify both -affine and -warpfield");
    AffineFile myAffine;
    WarpfieldFile myWarpfield;
    if (affineOpt->m_present)
    {
        OptionalParameter* flirtOpt = affineOpt->getOptionalParameter(2);
        if (flirtOpt->m_present)
        {
            myAffine.readFlirt(affineOpt->getString(1), flirtOpt->getString(1), flirtOpt->getString(2));
        } else {
            myAffine.readWorld(affineOpt->getString(1));
        }
    }
    if (warpfieldOpt->m_present)
    {
        OptionalParameter* fnirtOpt = warpfieldOpt->getOptionalParameter(2);
        if (fnirtOpt->m_present)
        {
            myWarpfield.readFnirt(warpfieldOpt->getString(1), fnirtOpt->getString(1));
        } else {
            myWarpfield.readWorld(warpfieldOpt->getString(1));
        }
    }
    SurfaceFile* curLeftSphere = NULL, *newLeftSphere = NULL;
    MetricFile* curLeftAreas = NULL, *newLeftAreas = NULL;
    MetricFile curLeftAreasTemp, newLeftAreasTemp;
    OptionalParameter* leftSpheresOpt = myParams->getOptionalParameter(13);
    if (leftSpheresOpt->m_present)
    {
        curLeftSphere = leftSpheresOpt->getSurface(1);
        newLeftSphere = leftSpheresOpt->getSurface(2);
        OptionalParameter* leftAreaSurfsOpt = leftSpheresOpt->getOptionalParameter(3);
        if (leftAreaSurfsOpt->m_present)
        {
            SurfaceFile* curAreaSurf = leftAreaSurfsOpt->getSurface(1);
            SurfaceFile* newAreaSurf = leftAreaSurfsOpt->getSurface(2);
            vector<float> nodeAreasTemp;
            curAreaSurf->computeNodeAreas(nodeAreasTemp);
            curLeftAreasTemp.setNumberOfNodesAndColumns(curAreaSurf->getNumberOfNodes(), 1);
            curLeftAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            curLeftAreas = &curLeftAreasTemp;
            newAreaSurf->computeNodeAreas(nodeAreasTemp);
            newLeftAreasTemp.setNumberOfNodesAndColumns(newAreaSurf->getNumberOfNodes(), 1);
            newLeftAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            newLeftAreas = &newLeftAreasTemp;
        }
        OptionalParameter* leftAreaMetricsOpt = leftSpheresOpt->getOptionalParameter(4);
        if (leftAreaMetricsOpt->m_present)
        {
            if (leftAreaSurfsOpt->m_present)
            {
                throw AlgorithmException("only one of -left-area-surfs and -left-area-metrics can be specified");
            }
            curLeftAreas = leftAreaMetricsOpt->getMetric(1);
            newLeftAreas = leftAreaMetricsOpt->getMetric(2);
        }
    }
    SurfaceFile* curRightSphere = NULL, *newRightSphere = NULL;
    MetricFile* curRightAreas = NULL, *newRightAreas = NULL;
    MetricFile curRightAreasTemp, newRightAreasTemp;
    OptionalParameter* rightSpheresOpt = myParams->getOptionalParameter(14);
    if (rightSpheresOpt->m_present)
    {
        curRightSphere = rightSpheresOpt->getSurface(1);
        newRightSphere = rightSpheresOpt->getSurface(2);
        OptionalParameter* rightAreaSurfsOpt = rightSpheresOpt->getOptionalParameter(3);
        if (rightAreaSurfsOpt->m_present)
        {
            SurfaceFile* curAreaSurf = rightAreaSurfsOpt->getSurface(1);
            SurfaceFile* newAreaSurf = rightAreaSurfsOpt->getSurface(2);
            vector<float> nodeAreasTemp;
            curAreaSurf->computeNodeAreas(nodeAreasTemp);
            curRightAreasTemp.setNumberOfNodesAndColumns(curAreaSurf->getNumberOfNodes(), 1);
            curRightAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            curRightAreas = &curRightAreasTemp;
            newAreaSurf->computeNodeAreas(nodeAreasTemp);
            newRightAreasTemp.setNumberOfNodesAndColumns(newAreaSurf->getNumberOfNodes(), 1);
            newRightAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            newRightAreas = &newRightAreasTemp;
        }
        OptionalParameter* rightAreaMetricsOpt = rightSpheresOpt->getOptionalParameter(4);
        if (rightAreaMetricsOpt->m_present)
        {
            if (rightAreaSurfsOpt->m_present)
            {
                throw AlgorithmException("only one of -right-area-surfs and -right-area-metrics can be specified");
            }
            curRightAreas = rightAreaMetricsOpt->getMetric(1);
            newRightAreas = rightAreaMetricsOpt->getMetric(2);
        }
    }
    SurfaceFile* curCerebSphere = NULL, *newCerebSphere = NULL;
    MetricFile* curCerebAreas = NULL, *newCerebAreas = NULL;
    MetricFile curCerebAreasTemp, newCerebAreasTemp;
    OptionalParameter* cerebSpheresOpt = myParams->getOptionalParameter(15);
    if (cerebSpheresOpt->m_present)
    {
        curCerebSphere = cerebSpheresOpt->getSurface(1);
        newCerebSphere = cerebSpheresOpt->getSurface(2);
        OptionalParameter* cerebAreaSurfsOpt = cerebSpheresOpt->getOptionalParameter(3);
        if (cerebAreaSurfsOpt->m_present)
        {
            SurfaceFile* curAreaSurf = cerebAreaSurfsOpt->getSurface(1);
            SurfaceFile* newAreaSurf = cerebAreaSurfsOpt->getSurface(2);
            vector<float> nodeAreasTemp;
            curAreaSurf->computeNodeAreas(nodeAreasTemp);
            curCerebAreasTemp.setNumberOfNodesAndColumns(curAreaSurf->getNumberOfNodes(), 1);
            curCerebAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            curCerebAreas = &curCerebAreasTemp;
            newAreaSurf->computeNodeAreas(nodeAreasTemp);
            newCerebAreasTemp.setNumberOfNodesAndColumns(newAreaSurf->getNumberOfNodes(), 1);
            newCerebAreasTemp.setValuesForColumn(0, nodeAreasTemp.data());
            newCerebAreas = &newCerebAreasTemp;
        }
        OptionalParameter* cerebAreaMetricsOpt = cerebSpheresOpt->getOptionalParameter(4);
        if (cerebAreaMetricsOpt->m_present)
        {
            if (cerebAreaSurfsOpt->m_present)
            {
                throw AlgorithmException("only one of -cerebellum-area-surfs and -cerebellum-area-metrics can be specified");
            }
            curCerebAreas = cerebAreaMetricsOpt->getMetric(1);
            newCerebAreas = cerebAreaMetricsOpt->getMetric(2);
        }
    }
    if (warpfieldOpt->m_present)
    {
        AlgorithmCiftiResample(myProgObj, myCiftiIn, direction, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myWarpfield.getWarpfield(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent, volLegacyCutoff, surfLegacyCutoff);
    } else {//rely on AffineFile() being the identity transform for if neither option is specified
        AlgorithmCiftiResample(myProgObj, myCiftiIn, direction, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myAffine.getMatrix(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent, volLegacyCutoff, surfLegacyCutoff);
    }
}

pair<bool, AString> AlgorithmCiftiResample::checkForErrors(const CiftiFile* myCiftiIn, const int& direction, const CiftiFile* myTemplate, const int& templateDir,
                                                           const SurfaceResamplingMethodEnum::Enum& mySurfMethod,
                                                           const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const MetricFile* curLeftAreas, const MetricFile* newLeftAreas,
                                                           const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const MetricFile* curRightAreas, const MetricFile* newRightAreas,
                                                           const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const MetricFile* curCerebAreas, const MetricFile* newCerebAreas)
{
    if (direction > 1) return make_pair(true, AString("unsupported mapping direction for cifti resample"));
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    if (myInputXML.getNumberOfDimensions() != 2) return make_pair(true, AString("cifti resample only supports 2D cifti"));
    if (myInputXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS) return make_pair(true, AString("direction for input must contain brain models"));
    const CiftiBrainModelsMap& inModels = myInputXML.getBrainModelsMap(direction);
    const CiftiXML& myTemplateXML = myTemplate->getCiftiXML();
    if (templateDir < 0 || templateDir >= myTemplateXML.getNumberOfDimensions()) return make_pair(true, AString("specified template direction does not exist in template file"));
    if (myTemplateXML.getMappingType(templateDir) != CiftiMappingType::BRAIN_MODELS) return make_pair(true, AString("direction for template must contain brain models"));
    const CiftiBrainModelsMap& outModels = myTemplate->getCiftiXML().getBrainModelsMap(templateDir);
    vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
    for (int i = 0; i < (int)surfList.size(); ++i)//ensure existence of resampling spheres before doing any computation
    {
        if (!inModels.hasSurfaceData(surfList[i])) return make_pair(true, AString("input cifti missing surface information for structure: " + StructureEnum::toGuiName(surfList[i])));
        const SurfaceFile* curSphere = NULL, *newSphere = NULL;
        const MetricFile* curAreas = NULL, *newAreas = NULL;
        AString structName;
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                curSphere = curLeftSphere;
                newSphere = newLeftSphere;
                curAreas = curLeftAreas;
                newAreas = newLeftAreas;
                structName = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                curSphere = curRightSphere;
                newSphere = newRightSphere;
                curAreas = curRightAreas;
                newAreas = newRightAreas;
                structName = "right";
                break;
            case StructureEnum::CEREBELLUM:
                curSphere = curCerebSphere;
                newSphere = newCerebSphere;
                curAreas = curCerebAreas;
                newAreas = newCerebAreas;
                structName = "cerebellum";
                break;
            default:
                return make_pair(true, AString("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i])));
                break;
        }
        if (curSphere != NULL)//resampling
        {
            if (newSphere == NULL) return make_pair(true, AString("missing " + structName + " new sphere"));
            if (curSphere->getNumberOfNodes() != inModels.getSurfaceNumberOfNodes(surfList[i])) return make_pair(true, AString(structName + " current sphere doesn't match input cifti"));
            if (newSphere->getNumberOfNodes() != outModels.getSurfaceNumberOfNodes(surfList[i])) return make_pair(true, AString(structName + " new sphere doesn't match template cifti"));
            switch (mySurfMethod)
            {
                case SurfaceResamplingMethodEnum::ADAP_BARY_AREA:
                    if (curAreas == NULL || newAreas == NULL) return make_pair(true, AString(structName + " area data is missing"));
                    if (curAreas->getNumberOfNodes() != curSphere->getNumberOfNodes()) return make_pair(true, AString(structName + " current area data has the wrong number of vertices"));
                    if (newAreas->getNumberOfNodes() != newSphere->getNumberOfNodes()) return make_pair(true, AString(structName + " new area data has the wrong number of vertices"));
                    break;
                default:
                    break;
            }
        } else {//copying
            if (inModels.getSurfaceNumberOfNodes(surfList[i]) != outModels.getSurfaceNumberOfNodes(surfList[i])) return make_pair(true, AString(structName + " structure requires resampling spheres, does not match template"));
        }
    }
    for (int i = 0; i < (int)volList.size(); ++i)
    {
        if (!inModels.hasVolumeData(volList[i]))
        {
            return make_pair(true, AString(StructureEnum::toGuiName(volList[i]) + " volume model missing from input cifti"));
        }
    }
    return make_pair(false, AString());
}

namespace
{//so that we don't need these in the header file
    struct ResampleCache
    {//a place to stuff anything that can be precomputed or reused for applying to the same structure in multiple maps
        SurfaceResamplingHelper surfResamp;
        VolumePaddingHelper volPadding;
        const SurfaceFile* curSphere, *newSphere;
        MetricFile tempMetric1, tempMetric2, surfDilateRoi;
        LabelFile tempLabel1, tempLabel2;
        CaretPointer<VolumeFile> inputVol, tempVol2, tempVol3, volDilateRoi;
        vector<CiftiBrainModelsMap::SurfaceMap> inSurfMap, outSurfMap;
        vector<CiftiBrainModelsMap::VolumeMap> inVolMap, outVolMap;
        vector<float> floatScratch1, floatScratch2;
        vector<int32_t> intScratch1, intScratch2;
        int64_t refDims[3], refOffset[3], inOffset[3];
        vector<vector<float> > refSform;
        bool copyMode;
        vector<VoxelIJK> edgeVoxelList, outsideRoiVoxels, insideResampledRoiVoxels;
    };
    
    void setupRowResampling(map<StructureEnum::Enum, ResampleCache>& surfCache, map<StructureEnum::Enum, ResampleCache>& volCache, const CiftiFile* myCiftiIn, CiftiFile* myCiftiOut,
                            const SurfaceResamplingMethodEnum::Enum& mySurfMethod, const float& voldilatemm, const FloatMatrix* affine, const VolumeFile* warpfield,
                            const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const MetricFile* curLeftAreas, const MetricFile* newLeftAreas,
                            const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const MetricFile* curRightAreas, const MetricFile* newRightAreas,
                            const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const MetricFile* curCerebAreas, const MetricFile* newCerebAreas)
    {
        const CiftiXML& myInputXML = myCiftiIn->getCiftiXML(), &myOutXML = myCiftiOut->getCiftiXML();
        bool labelMode = (myInputXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS);
        const CiftiBrainModelsMap& inModels = myInputXML.getBrainModelsMap(CiftiXML::ALONG_ROW), &outModels = myOutXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
        int numSurfStructs = (int)surfList.size(), numVolStructs = (int)volList.size();
        for (int i = 0; i < numSurfStructs; ++i)//initialize reusables
        {
            const SurfaceFile* curSphere = NULL, *newSphere = NULL;
            const MetricFile* curAreas = NULL, *newAreas = NULL;
            switch (surfList[i])
            {
                case StructureEnum::CORTEX_LEFT:
                    curSphere = curLeftSphere;
                    newSphere = newLeftSphere;
                    curAreas = curLeftAreas;
                    newAreas = newLeftAreas;
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    curSphere = curRightSphere;
                    newSphere = newRightSphere;
                    curAreas = curRightAreas;
                    newAreas = newRightAreas;
                    break;
                case StructureEnum::CEREBELLUM:
                    curSphere = curCerebSphere;
                    newSphere = newCerebSphere;
                    curAreas = curCerebAreas;
                    newAreas = newCerebAreas;
                    break;
                default:
                    throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                    break;
            }
            ResampleCache& myCache = surfCache[surfList[i]];
            myCache.inSurfMap = inModels.getSurfaceMap(surfList[i]);
            myCache.outSurfMap = outModels.getSurfaceMap(surfList[i]);
            if (curSphere == NULL)
            {
                myCache.copyMode = true;
                myCache.floatScratch1.resize(inModels.getSurfaceNumberOfNodes(surfList[i]), 0.0f);
                continue;
            }
            myCache.copyMode = false;
            myCache.curSphere = curSphere;
            myCache.newSphere = newSphere;
            const float* curAreasPtr = NULL, *newAreasPtr = NULL;
            if (curAreas != NULL && newAreas != NULL)
            {
                curAreasPtr = curAreas->getValuePointerForColumn(0);
                newAreasPtr = newAreas->getValuePointerForColumn(0);
            }
            vector<float> tempRoi(curSphere->getNumberOfNodes(), 0.0f);
            for (int j = 0; j < (int)myCache.inSurfMap.size(); ++j)
            {
                tempRoi[myCache.inSurfMap[j].m_surfaceNode] = 1.0f;
            }
            myCache.surfResamp = SurfaceResamplingHelper(mySurfMethod, curSphere, newSphere, curAreasPtr, newAreasPtr, tempRoi.data());//resampling is already a helper, so use it as such
            tempRoi.resize(newSphere->getNumberOfNodes());
            myCache.surfResamp.getResampleValidROI(tempRoi.data());
            myCache.surfDilateRoi.setNumberOfNodesAndColumns(newSphere->getNumberOfNodes(), 1);
            for (int j = 0; j < (int)tempRoi.size(); ++j)
            {
                myCache.surfDilateRoi.setValue(j, 0, (tempRoi[j] > 0.0f ? 0.0f : 1.0f));
            }
            if (labelMode)
            {
                myCache.intScratch1.resize(curSphere->getNumberOfNodes(), 0);
                myCache.intScratch2.resize(newSphere->getNumberOfNodes(), 0);
                myCache.tempLabel1.setNumberOfNodesAndColumns(newSphere->getNumberOfNodes(), 1);
            } else {
                myCache.floatScratch1.resize(curSphere->getNumberOfNodes(), 0.0f);
                myCache.floatScratch2.resize(newSphere->getNumberOfNodes(), 0.0f);
                myCache.tempMetric1.setNumberOfNodesAndColumns(newSphere->getNumberOfNodes(), 1);
            }
        }
        for (int i = 0; i < numVolStructs; ++i)
        {
            ResampleCache& myCache = volCache[volList[i]];
            myCache.inVolMap = inModels.getVolumeStructureMap(volList[i]);
            myCache.outVolMap = outModels.getVolumeStructureMap(volList[i]);
            vector<vector<float> > sform;
            vector<int64_t> inDims(3);
            myCache.floatScratch1.resize(inDims[0] * inDims[1] * inDims[2]);
            AlgorithmCiftiSeparate::getCroppedVolSpace(myCiftiIn, CiftiXML::ALONG_ROW, volList[i], inDims.data(), sform, myCache.inOffset);
            AlgorithmCiftiSeparate::getCroppedVolSpace(myCiftiOut, CiftiXML::ALONG_ROW, volList[i], myCache.refDims, myCache.refSform, myCache.refOffset);
            if (labelMode)
            {
                myCache.inputVol.grabNew(new VolumeFile(inDims, sform, 1, SubvolumeAttributes::LABEL));
            } else {
                myCache.inputVol.grabNew(new VolumeFile(inDims, sform));
                myCache.inputVol->setValueAllVoxels(0.0f);
            }
            myCache.tempVol2.grabNew(new VolumeFile(inDims, sform));//make the dilation roi, to figure out edge voxels and in case we do dilation
            myCache.tempVol2->setValueAllVoxels(0.0f);
            for (int64_t j = 0; j < (int64_t)myCache.inVolMap.size(); ++j)
            {
                myCache.tempVol2->setValue(1.0f, myCache.inVolMap[j].m_ijk[0] - myCache.inOffset[0],
                                                    myCache.inVolMap[j].m_ijk[1] - myCache.inOffset[1],
                                                    myCache.inVolMap[j].m_ijk[2] - myCache.inOffset[2]);
            }
            int neighbors[] = { 0, 0, -1,
                                0, -1, 0,
                                -1, 0, 0,
                                1, 0, 0,
                                0, 1, 0,
                                0, 0, 1 };
            VolumeFile resampledRoi;
            if (voldilatemm > 0.0f)
            {
                VolumeFile ROIinvTemp(inDims, sform);
                ROIinvTemp.setValueAllVoxels(1.0f);//make the ordinary ROI so that we can dilate it
                for (int64_t j = 0; j < (int64_t)myCache.inVolMap.size(); ++j)
                {
                    ROIinvTemp.setValue(0.0f, myCache.inVolMap[j].m_ijk[0] - myCache.inOffset[0],
                                              myCache.inVolMap[j].m_ijk[1] - myCache.inOffset[1],
                                              myCache.inVolMap[j].m_ijk[2] - myCache.inOffset[2]);
                }
                myCache.volPadding = VolumePaddingHelper::padMM(myCache.inputVol, voldilatemm);
                myCache.volDilateRoi.grabNew(new VolumeFile());
                myCache.volPadding.doPadding(&ROIinvTemp, myCache.volDilateRoi, 1.0f);//this is the correct final volDilateRoi, we don't need the unpadded form anymore
                ROIinvTemp.clear();
                myCache.tempVol3.grabNew(new VolumeFile());//this is only used in processing when dilating
                myCache.volPadding.doPadding(myCache.tempVol2, myCache.tempVol3);
                AlgorithmVolumeDilate(NULL, myCache.tempVol3, voldilatemm, AlgorithmVolumeDilate::NEAREST, myCache.tempVol2, myCache.volDilateRoi);
                vector<int64_t> paddedDims = myCache.tempVol2->getDimensions();
                for (int64_t k = 0; k < paddedDims[2]; ++k)
                {
                    for (int64_t j = 0; j < paddedDims[1]; ++j)
                    {
                        for (int64_t i = 0; i < paddedDims[0]; ++i)
                        {
                            if (myCache.tempVol2->getValue(i, j, k) > 0.0f)
                            {
                                for (int n = 0; n < 6; ++n)
                                {
                                    VoxelIJK thisVoxel = VoxelIJK(i + neighbors[j * 3 + 0],
                                                                j + neighbors[j * 3 + 1],
                                                                k + neighbors[j * 3 + 2]);
                                    if (myCache.tempVol2->indexValid(thisVoxel.m_ijk) &&
                                        myCache.tempVol2->getValue(thisVoxel.m_ijk) == 0.0f)//trick: if it is merely touching an FOV edge, cubic won't cause ringing there, so ignore that voxel
                                    {
                                        myCache.edgeVoxelList.push_back(VoxelIJK(i, j, k));
                                        break;
                                    }
                                }
                            } else {
                                myCache.outsideRoiVoxels.push_back(VoxelIJK(i, j, k));
                            }
                        }
                    }
                }
                if (warpfield != NULL)
                {
                    AlgorithmVolumeWarpfieldResample(NULL, myCache.tempVol2, warpfield, myCache.refDims, myCache.refSform, VolumeFile::TRILINEAR, &resampledRoi);
                } else {
                    AlgorithmVolumeAffineResample(NULL, myCache.tempVol2, *affine, myCache.refDims, myCache.refSform, VolumeFile::TRILINEAR, &resampledRoi);
                }
            } else {
                for (int64_t k = 0; k < inDims[2]; ++k)//we have to loop through all voxels anyway to find the ones outside the ROI
                {
                    for (int64_t j = 0; j < inDims[1]; ++j)
                    {
                        for (int64_t i = 0; i < inDims[0]; ++i)
                        {
                            if (myCache.tempVol2->getValue(i, j, k) > 0.0f)
                            {
                                for (int k = 0; k < 6; ++k)
                                {
                                    VoxelIJK thisVoxel = VoxelIJK(myCache.inVolMap[j].m_ijk[0] - myCache.inOffset[0] + neighbors[k * 3 + 0],
                                                                    myCache.inVolMap[j].m_ijk[1] - myCache.inOffset[1] + neighbors[k * 3 + 1],
                                                                    myCache.inVolMap[j].m_ijk[2] - myCache.inOffset[2] + neighbors[k * 3 + 2]);
                                    if (myCache.tempVol2->indexValid(thisVoxel.m_ijk) &&
                                        myCache.tempVol2->getValue(thisVoxel.m_ijk) == 0.0f)
                                    {
                                        myCache.edgeVoxelList.push_back(VoxelIJK(myCache.inVolMap[j].m_ijk[0] - myCache.inOffset[0],
                                                                                myCache.inVolMap[j].m_ijk[1] - myCache.inOffset[1],
                                                                                myCache.inVolMap[j].m_ijk[2] - myCache.inOffset[2]));
                                        break;
                                    }
                                }
                            } else {
                                myCache.outsideRoiVoxels.push_back(VoxelIJK(i, j, k));
                            }
                        }
                    }
                }
                if (warpfield != NULL)
                {
                    AlgorithmVolumeWarpfieldResample(NULL, myCache.tempVol2, warpfield, myCache.refDims, myCache.refSform, VolumeFile::TRILINEAR, &resampledRoi);
                } else {
                    AlgorithmVolumeAffineResample(NULL, myCache.tempVol2, *affine, myCache.refDims, myCache.refSform, VolumeFile::TRILINEAR, &resampledRoi);
                }
            }
            for (int64_t k = 0; k < myCache.refDims[2]; ++k)
            {
                for (int64_t j = 0; j < myCache.refDims[1]; ++j)
                {
                    for (int64_t i = 0; i < myCache.refDims[0]; ++i)
                    {
                        if (resampledRoi.getValue(i, j, k) > 0.5f)
                        {
                            myCache.insideResampledRoiVoxels.push_back(VoxelIJK(i, j, k));
                        }
                    }
                }
            }
        }
    }
    
    void processRowSurface(ResampleCache& myCache, const vector<float>& inRow, vector<float>& outRow, const CiftiXML& myInputXML,
                           const float& surfdilatemm, const bool& surfLargest, const int& unassignedLabelKey, const int64_t& row,
                           const AlgorithmMetricDilate::Method& surfDilateMethod, const float& surfDilateExponent, const bool surfLegacyCutoff)
    {
        bool labelMode = (myInputXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS);
        int inMapSize = (int)myCache.inSurfMap.size(), outMapSize = (int)myCache.outSurfMap.size();
        if (myCache.copyMode)//copy
        {
            for (int j = 0; j < inMapSize; ++j)
            {
                myCache.floatScratch1[myCache.inSurfMap[j].m_surfaceNode] = inRow[myCache.inSurfMap[j].m_ciftiIndex];
            }
            for (int j = 0; j < outMapSize; ++j)
            {
                outRow[myCache.outSurfMap[j].m_ciftiIndex] = myCache.floatScratch1[myCache.outSurfMap[j].m_surfaceNode];
            }
        } else {
            if (labelMode)
            {
                const CiftiLabelsMap& myLabelMap = myInputXML.getLabelsMap(CiftiXML::ALONG_COLUMN);
                for (int j = 0; j < inMapSize; ++j)
                {
                    myCache.intScratch1[myCache.inSurfMap[j].m_surfaceNode] = (int)floor(inRow[myCache.inSurfMap[j].m_ciftiIndex] + 0.5f);
                }
                if (surfLargest)
                {
                    myCache.surfResamp.resampleLargest(myCache.intScratch1.data(), myCache.intScratch2.data(), unassignedLabelKey);
                } else {
                    myCache.surfResamp.resamplePopular(myCache.intScratch1.data(), myCache.intScratch2.data(), unassignedLabelKey);
                }
                *(myCache.tempLabel1.getLabelTable()) = *(myLabelMap.getMapLabelTable(row));
                myCache.tempLabel1.setLabelKeysForColumn(0, myCache.intScratch2.data());
                LabelFile* toUse = &(myCache.tempLabel1);
                if (surfdilatemm > 0.0f)
                {
                    AlgorithmLabelDilate(NULL, toUse, myCache.newSphere, surfdilatemm, &(myCache.tempLabel2), &(myCache.surfDilateRoi), 0);
                    toUse = &(myCache.tempLabel2);
                }
                const int32_t* outData = toUse->getLabelKeyPointerForColumn(0);
                for (int j = 0; j < outMapSize; ++j)
                {
                    outRow[myCache.outSurfMap[j].m_ciftiIndex] = outData[myCache.outSurfMap[j].m_surfaceNode];
                }
            } else {
                for (int j = 0; j < inMapSize; ++j)
                {
                    myCache.floatScratch1[myCache.inSurfMap[j].m_surfaceNode] = inRow[myCache.inSurfMap[j].m_ciftiIndex];
                }
                if (surfLargest)
                {
                    myCache.surfResamp.resampleLargest(myCache.floatScratch1.data(), myCache.floatScratch2.data());
                } else {
                    myCache.surfResamp.resampleNormal(myCache.floatScratch1.data(), myCache.floatScratch2.data());
                }
                myCache.tempMetric1.setValuesForColumn(0, myCache.floatScratch2.data());
                MetricFile* toUse = &(myCache.tempMetric1);
                if (surfdilatemm > 0.0f)
                {
                    AlgorithmMetricDilate(NULL, toUse, myCache.newSphere, surfdilatemm, &(myCache.tempMetric2), &(myCache.surfDilateRoi), NULL, 0, surfDilateMethod, surfDilateExponent, NULL, surfLegacyCutoff);
                    toUse = &(myCache.tempMetric2);
                }
                const float* outData = toUse->getValuePointerForColumn(0);
                for (int j = 0; j < outMapSize; ++j)
                {
                    outRow[myCache.outSurfMap[j].m_ciftiIndex] = outData[myCache.outSurfMap[j].m_surfaceNode];
                }
            }
        }
    }
    
    void processRowVolume(ResampleCache& myCache, const vector<float>& inRow, vector<float>& outRow, const CiftiXML& myInputXML,
                          const float& voldilatemm, const AlgorithmVolumeDilate::Method& volDilateMethod, const float& volDilateExponent, const int& unassignedLabelKey,
                          const VolumeFile* warpfield, const FloatMatrix* affine, const VolumeFile::InterpType& myVolMethod, const bool volLegacyCutoff)
    {
        bool labelMode = (myInputXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS);
        bool doEdgeAdjust = (myVolMethod == VolumeFile::VolumeFile::CUBIC);//if they ask for cubic on label data, let strange things happen (there will be warnings from resample)
        if (labelMode)//gets initialized to 0 when not using labels
        {
            myCache.inputVol->setValueAllVoxels(unassignedLabelKey);
        }
        int inMapSize = (int)myCache.inVolMap.size(), outMapSize = (int)myCache.outVolMap.size();
        for (int j = 0; j < inMapSize; ++j)
        {
            myCache.inputVol->setValue(inRow[myCache.inVolMap[j].m_ciftiIndex], myCache.inVolMap[j].m_ijk[0] - myCache.inOffset[0],
                                    myCache.inVolMap[j].m_ijk[1] - myCache.inOffset[1],
                                    myCache.inVolMap[j].m_ijk[2] - myCache.inOffset[2]);
        }
        VolumeFile* toResample = myCache.inputVol;
        if (voldilatemm > 0.0f)
        {//inputVol is kept at original dimensions, don't overwrite it
            myCache.volPadding.doPadding(myCache.inputVol, myCache.tempVol2);
            AlgorithmVolumeDilate(NULL, myCache.tempVol2, voldilatemm, volDilateMethod, myCache.tempVol3, myCache.volDilateRoi, NULL, -1, volDilateExponent, volLegacyCutoff);
            toResample = myCache.tempVol3;
        }
        if (doEdgeAdjust)
        {
            double edgeSum = 0.0;//calculate the average edge value so we can use it to reduce interpolation edge effects (particularly cubic)
            for (int64_t j = 0; j < (int64_t)myCache.edgeVoxelList.size(); ++j)
            {
                edgeSum += toResample->getValue(myCache.edgeVoxelList[j]);
            }
            float edgeAverage = 0.0f;
            if (!myCache.edgeVoxelList.empty())
            {
                edgeAverage = edgeSum / myCache.edgeVoxelList.size();
            }
            for (int j = 0; j < int(myCache.outsideRoiVoxels.size()); ++j)
            {
                toResample->setValue(edgeAverage, myCache.outsideRoiVoxels[j]);
            }
        }//toResample is never 2, so we can use 2
        if (warpfield != NULL)
        {
            AlgorithmVolumeWarpfieldResample(NULL, toResample, warpfield, myCache.refDims, myCache.refSform, myVolMethod, myCache.tempVol2);
        } else {
            AlgorithmVolumeAffineResample(NULL, toResample, *affine, myCache.refDims, myCache.refSform, myVolMethod, myCache.tempVol2);
        }
        if (doEdgeAdjust)
        {
            vector<float> tempFrame(myCache.refDims[0] * myCache.refDims[1] * myCache.refDims[2], 0.0f);//for consistency with COLUMN, copy out only the selected voxels
            for (int j = 0; j < int(myCache.insideResampledRoiVoxels.size()); ++j)
            {
                tempFrame[myCache.tempVol2->getIndex(myCache.insideResampledRoiVoxels[j])] = myCache.tempVol2->getValue(myCache.insideResampledRoiVoxels[j]);
            }
            myCache.tempVol2->setFrame(tempFrame.data());
        }
        for (int j = 0; j < outMapSize; ++j)
        {
            outRow[myCache.outVolMap[j].m_ciftiIndex] = myCache.tempVol2->getValue(myCache.outVolMap[j].m_ijk[0] - myCache.refOffset[0],
                                                                                    myCache.outVolMap[j].m_ijk[1] - myCache.refOffset[1],
                                                                                    myCache.outVolMap[j].m_ijk[2] - myCache.refOffset[2]);
        }
    }
}

AlgorithmCiftiResample::AlgorithmCiftiResample(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const int& direction, const CiftiFile* myTemplate, const int& templateDir,
                                               const SurfaceResamplingMethodEnum::Enum& mySurfMethod, const VolumeFile::InterpType& myVolMethod, CiftiFile* myCiftiOut,
                                               const bool& surfLargest, const float& voldilatemm, const float& surfdilatemm,
                                               const VolumeFile* warpfield,
                                               const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const MetricFile* curLeftAreas, const MetricFile* newLeftAreas,
                                               const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const MetricFile* curRightAreas, const MetricFile* newRightAreas,
                                               const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const MetricFile* curCerebAreas, const MetricFile* newCerebAreas,
                                               const AlgorithmVolumeDilate::Method& volDilateMethod, const float& volDilateExponent,
                                               const AlgorithmMetricDilate::Method& surfDilateMethod, const float& surfDilateExponent,
                                               const bool volLegacyCutoff, const bool surfLegacyCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    pair<bool, AString> myError = checkForErrors(myCiftiIn, direction, myTemplate, templateDir, mySurfMethod,
                                                curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                                                curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                                                curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
    if (myError.first) throw AlgorithmException(myError.second);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    CiftiXML myOutXML = myInputXML;
    myOutXML.setMap(direction, *(myTemplate->getCiftiXML().getMap(templateDir)));
    const CiftiBrainModelsMap& outModels = myOutXML.getBrainModelsMap(direction);
    vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
    myCiftiOut->setCiftiXML(myOutXML);
    if (direction == CiftiXML::ALONG_COLUMN)
    {
        for (int i = 0; i < (int)surfList.size(); ++i)//and now, resampling
        {
            const SurfaceFile* curSphere = NULL, *newSphere = NULL;
            const MetricFile* curAreas = NULL, *newAreas = NULL;
            switch (surfList[i])
            {
                case StructureEnum::CORTEX_LEFT:
                    curSphere = curLeftSphere;
                    newSphere = newLeftSphere;
                    curAreas = curLeftAreas;
                    newAreas = newLeftAreas;
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    curSphere = curRightSphere;
                    newSphere = newRightSphere;
                    curAreas = curRightAreas;
                    newAreas = newRightAreas;
                    break;
                case StructureEnum::CEREBELLUM:
                    curSphere = curCerebSphere;
                    newSphere = newCerebSphere;
                    curAreas = curCerebAreas;
                    newAreas = newCerebAreas;
                    break;
                default:
                    throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                    break;
            }
            processSurfaceComponent(myCiftiIn, direction, surfList[i], mySurfMethod, myCiftiOut, surfLargest, surfdilatemm, curSphere, newSphere, curAreas, newAreas, surfDilateMethod, surfDilateExponent, surfLegacyCutoff);
        }
        for (int i = 0; i < (int)volList.size(); ++i)
        {
            processVolume(myCiftiIn, direction, volList[i], myVolMethod, myCiftiOut, voldilatemm, warpfield, NULL, volDilateMethod, volDilateExponent, volLegacyCutoff);
        }
    } else {//avoid cifti separate/replace with ALONG_ROW
        bool labelMode = (myInputXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS);
        vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
        int numSurfStructs = (int)surfList.size(), numVolStructs = (int)volList.size();
        vector<int> unassignedLabelKey;
        if (labelMode)
        {
            const CiftiLabelsMap& myLabelMap = myInputXML.getLabelsMap(CiftiXML::ALONG_COLUMN);
            unassignedLabelKey.resize(myLabelMap.getLength());
            for (int i = 0; i < myLabelMap.getLength(); ++i)
            {
                unassignedLabelKey[i] = myLabelMap.getMapLabelTable(i)->getUnassignedLabelKey();
            }
        }
        map<StructureEnum::Enum, ResampleCache> surfCache, volCache;//could make them different types, but whatever - two variables in case of structure overlap in surface and volume, as some members may get used by both
        setupRowResampling(surfCache, volCache, myCiftiIn, myCiftiOut, mySurfMethod, voldilatemm, NULL, warpfield,
                           curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                           curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                           curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
        int64_t numRows = myInputXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
        vector<float> inRow(myInputXML.getDimensionLength(CiftiXML::ALONG_ROW)), outRow(myOutXML.getDimensionLength(CiftiXML::ALONG_ROW));
        for (int64_t row = 0; row < numRows; ++row)
        {
            myCiftiIn->getRow(inRow.data(), row);
            for (int i = 0; i < numSurfStructs; ++i)
            {
                map<StructureEnum::Enum, ResampleCache>::iterator iter = surfCache.find(surfList[i]);
                CaretAssert(iter != surfCache.end());
                processRowSurface(iter->second, inRow, outRow, myInputXML, surfdilatemm, surfLargest, unassignedLabelKey[row], row, surfDilateMethod, surfDilateExponent, surfLegacyCutoff);
            }
            for (int i = 0; i < numVolStructs; ++i)
            {
                map<StructureEnum::Enum, ResampleCache>::iterator iter = volCache.find(volList[i]);
                CaretAssert(iter != volCache.end());
                processRowVolume(iter->second, inRow, outRow, myInputXML, voldilatemm, volDilateMethod, volDilateExponent, unassignedLabelKey[row], warpfield, NULL, myVolMethod, volLegacyCutoff);
            }
            myCiftiOut->setRow(outRow.data(), row);
        }
    }
}

AlgorithmCiftiResample::AlgorithmCiftiResample(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const int& direction, const CiftiFile* myTemplate, const int& templateDir,
                                               const SurfaceResamplingMethodEnum::Enum& mySurfMethod, const VolumeFile::InterpType& myVolMethod, CiftiFile* myCiftiOut,
                                               const bool& surfLargest, const float& voldilatemm, const float& surfdilatemm,
                                               const FloatMatrix& affine,
                                               const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const MetricFile* curLeftAreas, const MetricFile* newLeftAreas,
                                               const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const MetricFile* curRightAreas, const MetricFile* newRightAreas,
                                               const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const MetricFile* curCerebAreas, const MetricFile* newCerebAreas,
                                               const AlgorithmVolumeDilate::Method& volDilateMethod, const float& volDilateExponent,
                                               const AlgorithmMetricDilate::Method& surfDilateMethod, const float& surfDilateExponent,
                                               const bool volLegacyCutoff, const bool surfLegacyCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    pair<bool, AString> myError = checkForErrors(myCiftiIn, direction, myTemplate, templateDir, mySurfMethod,
                                                curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                                                curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                                                curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
    if (myError.first) throw AlgorithmException(myError.second);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    CiftiXML myOutXML = myInputXML;
    myOutXML.setMap(direction, *(myTemplate->getCiftiXML().getMap(templateDir)));
    const CiftiBrainModelsMap& outModels = myOutXML.getBrainModelsMap(direction);
    vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
    myCiftiOut->setCiftiXML(myOutXML);
    if (direction == CiftiXML::ALONG_COLUMN)
    {
        for (int i = 0; i < (int)surfList.size(); ++i)//and now, resampling
        {
            const SurfaceFile* curSphere = NULL, *newSphere = NULL;
            const MetricFile* curAreas = NULL, *newAreas = NULL;
            switch (surfList[i])
            {
                case StructureEnum::CORTEX_LEFT:
                    curSphere = curLeftSphere;
                    newSphere = newLeftSphere;
                    curAreas = curLeftAreas;
                    newAreas = newLeftAreas;
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    curSphere = curRightSphere;
                    newSphere = newRightSphere;
                    curAreas = curRightAreas;
                    newAreas = newRightAreas;
                    break;
                case StructureEnum::CEREBELLUM:
                    curSphere = curCerebSphere;
                    newSphere = newCerebSphere;
                    curAreas = curCerebAreas;
                    newAreas = newCerebAreas;
                    break;
                default:
                    throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                    break;
            }
            processSurfaceComponent(myCiftiIn, direction, surfList[i], mySurfMethod, myCiftiOut, surfLargest, surfdilatemm, curSphere, newSphere, curAreas, newAreas, surfDilateMethod, surfDilateExponent, surfLegacyCutoff);
        }
        for (int i = 0; i < (int)volList.size(); ++i)
        {
            processVolume(myCiftiIn, direction, volList[i], myVolMethod, myCiftiOut, voldilatemm, NULL, &affine, volDilateMethod, volDilateExponent, volLegacyCutoff);
        }
    } else {//avoid cifti separate/replace with ALONG_ROW
        bool labelMode = (myInputXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::LABELS);
        vector<StructureEnum::Enum> surfList = outModels.getSurfaceStructureList(), volList = outModels.getVolumeStructureList();
        int numSurfStructs = (int)surfList.size(), numVolStructs = (int)volList.size();
        vector<int> unassignedLabelKey;
        if (labelMode)
        {
            const CiftiLabelsMap& myLabelMap = myInputXML.getLabelsMap(CiftiXML::ALONG_COLUMN);
            unassignedLabelKey.resize(myLabelMap.getLength());
            for (int i = 0; i < myLabelMap.getLength(); ++i)
            {
                unassignedLabelKey[i] = myLabelMap.getMapLabelTable(i)->getUnassignedLabelKey();
            }
        }
        map<StructureEnum::Enum, ResampleCache> surfCache, volCache;//could make them different types, but whatever - two variables in case of structure overlap in surface and volume, as some members may get used by both
        setupRowResampling(surfCache, volCache, myCiftiIn, myCiftiOut, mySurfMethod, voldilatemm, &affine, NULL,
                           curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                           curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                           curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
        int64_t numRows = myInputXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
        vector<float> inRow(myInputXML.getDimensionLength(CiftiXML::ALONG_ROW)), outRow(myOutXML.getDimensionLength(CiftiXML::ALONG_ROW));
        for (int64_t row = 0; row < numRows; ++row)
        {
            myCiftiIn->getRow(inRow.data(), row);
            for (int i = 0; i < numSurfStructs; ++i)
            {
                map<StructureEnum::Enum, ResampleCache>::iterator iter = surfCache.find(surfList[i]);
                CaretAssert(iter != surfCache.end());
                processRowSurface(iter->second, inRow, outRow, myInputXML, surfdilatemm, surfLargest, unassignedLabelKey[row], row, surfDilateMethod, surfDilateExponent, surfLegacyCutoff);
            }
            for (int i = 0; i < numVolStructs; ++i)
            {
                map<StructureEnum::Enum, ResampleCache>::iterator iter = volCache.find(volList[i]);
                CaretAssert(iter != volCache.end());
                processRowVolume(iter->second, inRow, outRow, myInputXML, voldilatemm, volDilateMethod, volDilateExponent, unassignedLabelKey[row], NULL, &affine, myVolMethod, volLegacyCutoff);
            }
            myCiftiOut->setRow(outRow.data(), row);
        }
    }
}

void AlgorithmCiftiResample::processSurfaceComponent(const CiftiFile* myCiftiIn, const int& direction, const StructureEnum::Enum& myStruct, const SurfaceResamplingMethodEnum::Enum& mySurfMethod,
                                                     CiftiFile* myCiftiOut, const bool& surfLargest, const float& surfdilatemm, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                     const MetricFile* curAreas, const MetricFile* newAreas,
                                                     const AlgorithmMetricDilate::Method& surfDilateMethod, const float& surfDilateExponent, const bool surfLegacyCutoff)
{
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    if (myInputXML.getMappingType(1 - direction) == CiftiMappingType::LABELS)
    {
        LabelFile origLabel;
        MetricFile origRoi, resampleROI;
        AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origLabel, &origRoi);
        LabelFile newLabel, newDilate, *newUse = &newLabel;
        if (curSphere != NULL)
        {
            AlgorithmLabelResample(NULL, &origLabel, curSphere, newSphere, mySurfMethod, &newLabel, curAreas, newAreas, &origRoi, &resampleROI, surfLargest);
            origLabel.clear();//delete the data we no longer need to keep memory use down
            if (surfdilatemm > 0.0f)
            {
                MetricFile invertResampleROI;
                invertResampleROI.setNumberOfNodesAndColumns(resampleROI.getNumberOfNodes(), 1);
                for (int j = 0; j < resampleROI.getNumberOfNodes(); ++j)
                {
                    float tempf = (resampleROI.getValue(j, 0) > 0.0f) ? 0.0f : 1.0f;//make an inverse ROI
                    invertResampleROI.setValue(j, 0, tempf);
                }
                AlgorithmLabelDilate(NULL, &newLabel, newSphere, surfdilatemm, &newDilate, &invertResampleROI);
                newLabel.clear();//ditto
                newUse = &newDilate;
            }
        } else {
            newUse = &origLabel;
        }
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, newUse);
    } else {
        MetricFile origMetric, origROI;
        AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origMetric, &origROI);
        MetricFile newMetric, newDilate, resampleROI, *newUse = &newMetric;
        if (curSphere != NULL)
        {
            AlgorithmMetricResample(NULL, &origMetric, curSphere, newSphere, mySurfMethod, &newMetric, curAreas, newAreas, &origROI, &resampleROI, surfLargest);
            origMetric.clear();//ditto
            if (surfdilatemm > 0.0f)
            {
                MetricFile invertResampleROI;
                invertResampleROI.setNumberOfNodesAndColumns(resampleROI.getNumberOfNodes(), 1);
                for (int j = 0; j < resampleROI.getNumberOfNodes(); ++j)
                {
                    float tempf = (resampleROI.getValue(j, 0) > 0.0f) ? 0.0f : 1.0f;//make an inverse ROI
                    invertResampleROI.setValue(j, 0, tempf);
                }
                AlgorithmMetricDilate(NULL, &newMetric, newSphere, surfdilatemm, &newDilate, &invertResampleROI, NULL, -1, surfDilateMethod, surfDilateExponent, NULL, surfLegacyCutoff);//we could get the data roi from the template cifti and use it here
                newMetric.clear();
                newUse = &newDilate;
            }
        } else {
            newUse = &origMetric;
        }
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, newUse);
    }
}

void AlgorithmCiftiResample::processVolume(const CiftiFile* myCiftiIn, const int& direction, const StructureEnum::Enum& myStruct, const VolumeFile::InterpType& myVolMethod,
                                                    CiftiFile* myCiftiOut, const float& voldilatemm, const VolumeFile* warpfield, const FloatMatrix* affine,
                                                    const AlgorithmVolumeDilate::Method& volDilateMethod, const float& volDilateExponent, const bool volLegacyCutoff)
{
    VolumeFile origData, origDilate, origROI, ROIDilate, *origProcess, *ROIProcess;
    origProcess = &origData;
    ROIProcess = &origROI;
    int64_t offset[3];
    const int neighbors[] = { 0, 0, -1,
                                0, -1, 0,
                                -1, 0, 0,
                                1, 0, 0,
                                0, 1, 0,
                                0, 0, 1 };
    const bool doEdgeAdjust = (myVolMethod == VolumeFile::CUBIC);//if someone asks for cubic resampling of label data, allow strange things to happen (there will be a warning from resample)
    vector<VoxelIJK> edgeVoxelList;
    AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origData, offset, &origROI, true);
    vector<int64_t> origDims = origData.getDimensions();
    if (voldilatemm > 0.0f)
    {
        vector<int64_t> spatialDims = origDims;//make the bad voxel roi by inverting
        spatialDims.resize(3);
        int64_t framesize = origDims[0] * origDims[1] * origDims[2];
        vector<float> scratchframe(framesize);
        const float* roiframe = origROI.getFrame();
        for (int64_t i = 0; i < framesize; ++i)
        {
            scratchframe[i] = (roiframe[i] > 0.0f ? 0.0f : 1.0f);
        }
        VolumeFile origPad, invertROI, invertROIPad;
        invertROI.reinitialize(spatialDims, origData.getSform());
        invertROI.setFrame(scratchframe.data());
        VolumePaddingHelper mypadding = VolumePaddingHelper::padMM(&origData, voldilatemm);
        mypadding.doPadding(&origData, &origPad);
        origData.clear();//free data copies we no longer need
        mypadding.doPadding(&invertROI, &invertROIPad, 1.0f);//pad with ones since this is an inverted ROI
        AlgorithmVolumeDilate(NULL, &origPad, voldilatemm, volDilateMethod, &origDilate, &invertROIPad, NULL, -1, volDilateExponent, volLegacyCutoff);
        origPad.clear();//ditto
        origProcess = &origDilate;
        if (doEdgeAdjust)
        {//we need to use the dilated ROI
            VolumeFile ROIPad;
            mypadding.doPadding(&origROI, &ROIPad);
            AlgorithmVolumeDilate(NULL, &ROIPad, voldilatemm, AlgorithmVolumeDilate::NEAREST, &ROIDilate, &invertROIPad);
            ROIProcess = &ROIDilate;
            vector<int64_t> paddedDims = ROIPad.getDimensions();
            for (int64_t k = 0; k < paddedDims[2]; ++k)
            {
                for (int64_t j = 0; j < paddedDims[1]; ++j)
                {
                    for (int64_t i = 0; i < paddedDims[0]; ++i)
                    {
                        if (ROIDilate.getValue(i, j, k) > 0.0f)
                        {
                            for (int n = 0; n < 6; ++n)
                            {
                                VoxelIJK thisVoxel = VoxelIJK(i + neighbors[j * 3 + 0],
                                                              j + neighbors[j * 3 + 1],
                                                              k + neighbors[j * 3 + 2]);
                                if (ROIDilate.indexValid(thisVoxel.m_ijk) &&
                                    ROIDilate.getValue(thisVoxel.m_ijk) == 0.0f)//trick: if it is merely touching an FOV edge, cubic won't cause ringing there, so ignore that voxel
                                {
                                    edgeVoxelList.push_back(VoxelIJK(i, j, k));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {//if we don't dilate, we can use cifti to find the used voxels
        if (doEdgeAdjust)
        {
            vector<CiftiBrainModelsMap::VolumeMap> myMap = myCiftiIn->getCiftiXML().getBrainModelsMap(direction).getVolumeStructureMap(myStruct);
            for (int64_t i = 0; i < (int64_t)myMap.size(); ++i)
            {
                for (int j = 0; j < 6; ++j)
                {
                    VoxelIJK thisVoxel = VoxelIJK(myMap[i].m_ijk[0] - offset[0] + neighbors[j * 3 + 0],
                                                myMap[i].m_ijk[1] - offset[1] + neighbors[j * 3 + 1],
                                                myMap[i].m_ijk[2] - offset[2] + neighbors[j * 3 + 2]);
                    if (origROI.indexValid(thisVoxel.m_ijk) &&
                        origROI.getValue(thisVoxel.m_ijk) == 0.0f)//trick: if it is merely touching an FOV edge, cubic won't cause ringing there, so ignore that voxel
                    {
                        edgeVoxelList.push_back(VoxelIJK(myMap[i].m_ijk[0] - offset[0],
                                                         myMap[i].m_ijk[1] - offset[1],
                                                         myMap[i].m_ijk[2] - offset[2]));
                        break;
                    }
                }
            }
        }
    }
    if (doEdgeAdjust)
    {
        vector<int64_t> currentDims = origProcess->getDimensions();
        int64_t framesize = currentDims[0] * currentDims[1] * currentDims[2];
        vector<float> scratchframe(framesize);
        const float* ROIFrame = ROIProcess->getFrame();
        for (int64_t b = 0; b < origDims[3]; ++b)
        {//input is cifti, no RGB or complex types
            double edgesum = 0.0;
            for (int64_t i = 0; i < (int64_t)edgeVoxelList.size(); ++i)
            {
                edgesum += origProcess->getValue(edgeVoxelList[i].m_ijk, b);
            }
            float edgeMean = edgesum / edgeVoxelList.size();
            const float* dataFrame = origProcess->getFrame(b);
            for (int64_t i = 0; i < framesize; ++i)
            {
                scratchframe[i] = (ROIFrame[i] > 0.0f ? dataFrame[i] : edgeMean);//trick: set the background to the edge mean so we don't need to readjust data values after resampling
            }
            origProcess->setFrame(scratchframe.data(), b);
        }
    }
    VolumeFile newVolume;
    int64_t refdims[3], refoffset[3];
    vector<vector<float> > refsform;
    AlgorithmCiftiSeparate::getCroppedVolSpace(myCiftiOut, direction, myStruct, refdims, refsform, refoffset);
    if (warpfield != NULL)
    {
        AlgorithmVolumeWarpfieldResample(NULL, origProcess, warpfield, refdims, refsform, myVolMethod, &newVolume);
    } else {
        AlgorithmVolumeAffineResample(NULL, origProcess, *affine, refdims, refsform, myVolMethod, &newVolume);
    }
    origProcess->clear();
    if (doEdgeAdjust)
    {//to make it obvious when the dilation didn't cover the output ROI, mask the result with the (dilated) input ROI
        VolumeFile newROI;//trilinear masked at 0.5 should give somewhat better downsampling behavior than enclosing
        if (warpfield != NULL)
        {
            AlgorithmVolumeWarpfieldResample(NULL, ROIProcess, warpfield, refdims, refsform, VolumeFile::TRILINEAR, &newROI);
        } else {
            AlgorithmVolumeAffineResample(NULL, ROIProcess, *affine, refdims, refsform, VolumeFile::TRILINEAR, &newROI);
        }
        const float* roiFrame = newROI.getFrame();
        int64_t framesize = refdims[0] * refdims[1] * refdims[2];
        vector<float> scratchframe(framesize, 0.0f);
        for (int64_t b = 0; b < origDims[3]; ++b)
        {
            const float* dataFrame = newVolume.getFrame(b);
            for (int64_t i = 0; i < framesize; ++i)
            {
                if (roiFrame[i] > 0.5f)
                {
                    scratchframe[i] = dataFrame[i];
                }
            }
            newVolume.setFrame(scratchframe.data(), b);
        }
    }
    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, &newVolume, true);
}

float AlgorithmCiftiResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
