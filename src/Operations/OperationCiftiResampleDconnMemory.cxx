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

#include "OperationCiftiResampleDconnMemory.h"
#include "OperationException.h"

#include "AffineFile.h"
#include "AlgorithmCiftiResample.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString OperationCiftiResampleDconnMemory::getCommandSwitch()
{
    return "-cifti-resample-dconn-memory";
}

AString OperationCiftiResampleDconnMemory::getShortDescription()
{
    return "USE LOTS OF MEMORY TO RESAMPLE DCONN";
}

OperationParameters* OperationCiftiResampleDconnMemory::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to resample");
    
    ret->addCiftiParameter(3, "cifti-template", "a cifti file containing the cifti space to resample to");
    
    ret->addStringParameter(4, "template-direction", "the direction of the template to use as the resampling space");
    
    ret->addStringParameter(5, "surface-method", "specify a surface resampling method");
    
    ret->addStringParameter(6, "volume-method", "specify a volume interpolation method");
    
    ret->addCiftiOutputParameter(7, "cifti-out", "the output cifti file");
    
    ret->createOptionalParameter(8, "-surface-largest", "use largest weight instead of weighted average when doing surface resampling");
    
    OptionalParameter* volDilateOpt = ret->createOptionalParameter(9, "-volume-predilate", "dilate the volume components before resampling");
    volDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
    volDilateOpt->createOptionalParameter(2, "-nearest", "use nearest value dilation");
    OptionalParameter* volDilateWeightedOpt = volDilateOpt->createOptionalParameter(3, "-weighted", "use weighted dilation");
    OptionalParameter* volDilateExpOpt = volDilateWeightedOpt->createOptionalParameter(1, "-exponent", "specify exponent in weighting function");
    volDilateExpOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (1 / (distance ^ n)) as the weighting function (default 2)");
    
    OptionalParameter* surfDilateOpt = ret->createOptionalParameter(10, "-surface-postdilate", "dilate the surface components after resampling");
    surfDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
    surfDilateOpt->createOptionalParameter(2, "-nearest", "use nearest value dilation");
    surfDilateOpt->createOptionalParameter(3, "-linear", "use linear dilation");
    OptionalParameter* surfDilateWeightedOpt = surfDilateOpt->createOptionalParameter(4, "-weighted", "use weighted dilation");
    OptionalParameter* surfDilateExpOpt = surfDilateWeightedOpt->createOptionalParameter(1, "-exponent", "specify exponent in weighting function");
    surfDilateExpOpt->addDoubleParameter(1, "exponent", "exponent 'n' to use in (area / (distance ^ n)) as the weighting function (default 2)");
    
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
        AString("This command does the same thing as running -cifti-resample twice, but uses memory up to approximately 2x the size that the intermediate file would be.  ") +
        "This is because the intermediate dconn is kept in memory, rather than written to disk, " +
        "and the components before and after resampling/dilation have to be in memory at the same time during the relevant computation.  " +
        "If spheres are not specified for a surface structure which exists in the cifti files, its data is copied without resampling or dilation.  " +
        "Dilation is done with the 'nearest' method, and is done on <new-sphere> for surface data.  " +
        "Volume components are padded before dilation so that dilation doesn't run into the edge of the component bounding box.\n\n" +
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

void OperationCiftiResampleDconnMemory::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCiftiIn = myParams->getCifti(1);
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
            throw OperationException("unrecognized template direction string, use ROW or COLUMN");
        }
    }
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum mySurfMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(5), &ok);
    if (!ok)
    {
        throw OperationException("invalid surface resampling method name");
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
        throw OperationException("unrecognized volume interpolation method");
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
    float volDilateExponent = 2.0f;
    AlgorithmMetricDilate::Method surfDilateMethod = AlgorithmMetricDilate::WEIGHTED;//label dilate doesn't support multiple methods - what to do there, share the enum between them?
    float surfDilateExponent = 2.0f;//label dilate currently only supports nearest, so in order to accept a default in the algorithm, it currently ignores this on label data, no warning
    OptionalParameter* volDilateOpt = myParams->getOptionalParameter(9);
    if (volDilateOpt->m_present)
    {
        bool methodSpecified = false;
        voldilatemm = (float)volDilateOpt->getDouble(1);
        if (voldilatemm <= 0.0f) throw OperationException("dilation amount must be positive");
        if (volDilateOpt->getOptionalParameter(2)->m_present)
        {
            methodSpecified = true;
            volDilateMethod = AlgorithmVolumeDilate::NEAREST;
        }
        OptionalParameter* volDilateWeightedOpt = volDilateOpt->getOptionalParameter(3);
        if (volDilateWeightedOpt->m_present)
        {
            if (methodSpecified) throw OperationException("cannot specify multiple volume dilation methods");
            methodSpecified = true;
            volDilateMethod = AlgorithmVolumeDilate::WEIGHTED;
            OptionalParameter* volDilateExpOpt = volDilateWeightedOpt->getOptionalParameter(1);
            if (volDilateExpOpt->m_present)
            {
                volDilateExponent = (float)volDilateExpOpt->getDouble(1);
            }
        }
    }
    OptionalParameter* surfDilateOpt = myParams->getOptionalParameter(10);
    if (surfDilateOpt->m_present)
    {
        bool methodSpecified = false;
        surfdilatemm = (float)surfDilateOpt->getDouble(1);
        if (surfdilatemm <= 0.0f) throw OperationException("dilation amount must be positive");
        if (surfDilateOpt->getOptionalParameter(2)->m_present)
        {
            methodSpecified = true;
            surfDilateMethod = AlgorithmMetricDilate::NEAREST;
        }
        if (surfDilateOpt->getOptionalParameter(3)->m_present)
        {
            if (methodSpecified) throw OperationException("cannot specify multiple surface dilation methods");
            methodSpecified = true;
            if (isLabelData) throw OperationException("cannot do linear surface dilation on label data");
            surfDilateMethod = AlgorithmMetricDilate::LINEAR;
        }
        OptionalParameter* surfDilateWeightedOpt = surfDilateOpt->getOptionalParameter(4);
        if (surfDilateWeightedOpt->m_present)
        {
            if (methodSpecified) throw OperationException("cannot specify multiple surface dilation methods");
            methodSpecified = true;
            if (isLabelData) throw OperationException("cannot do weighted surface dilation on label data");
            surfDilateMethod = AlgorithmMetricDilate::WEIGHTED;
            OptionalParameter* surfDilateExpOpt = surfDilateWeightedOpt->getOptionalParameter(1);
            if (surfDilateExpOpt->m_present)
            {
                surfDilateExponent = (float)surfDilateExpOpt->getDouble(1);
            }
        }
    }
    OptionalParameter* affineOpt = myParams->getOptionalParameter(11);
    OptionalParameter* warpfieldOpt = myParams->getOptionalParameter(12);
    if (affineOpt->m_present && warpfieldOpt->m_present) throw OperationException("you cannot specify both -affine and -warpfield");
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
                throw OperationException("only one of -left-area-surfs and -left-area-metrics can be specified");
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
                throw OperationException("only one of -right-area-surfs and -right-area-metrics can be specified");
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
                throw OperationException("only one of -cerebellum-area-surfs and -cerebellum-area-metrics can be specified");
            }
            curCerebAreas = cerebAreaMetricsOpt->getMetric(1);
            newCerebAreas = cerebAreaMetricsOpt->getMetric(2);
        }
    }
    pair<bool, AString> colErrors = AlgorithmCiftiResample::checkForErrors(myCiftiIn, CiftiXML::ALONG_COLUMN, myTemplate, templateDir, mySurfMethod,
                            curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                            curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                            curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
    pair<bool, AString> rowErrors = AlgorithmCiftiResample::checkForErrors(myCiftiIn, CiftiXML::ALONG_ROW, myTemplate, templateDir, mySurfMethod,
                            curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                            curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                            curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas);
    ok = true;
    AString message;
    if (rowErrors.first)
    {
        message += "Error in resampling along row: " + rowErrors.second;
        ok = false;
    }
    if (colErrors.first)
    {
        if (!ok) message += "\n";
        message = "Error in resampling along column: " + colErrors.second;
        ok = false;
    }
    if (!ok)
    {
        throw OperationException(message);
    }
    CiftiFile tempCifti;
    //TSC: resampling along column first causes it to hit peak memory usage earlier
    if (warpfieldOpt->m_present)
    {
        AlgorithmCiftiResample(myProgObj, myCiftiIn, CiftiXML::ALONG_COLUMN, myTemplate, templateDir, mySurfMethod, myVolMethod, &tempCifti, surfLargest, voldilatemm, surfdilatemm, myWarpfield.getWarpfield(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent);
        AlgorithmCiftiResample(myProgObj, &tempCifti, CiftiXML::ALONG_ROW, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myWarpfield.getWarpfield(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent);
    } else {//rely on AffineFile() being the identity transform for if neither option is specified
        AlgorithmCiftiResample(myProgObj, myCiftiIn, CiftiXML::ALONG_COLUMN, myTemplate, templateDir, mySurfMethod, myVolMethod, &tempCifti, surfLargest, voldilatemm, surfdilatemm, myAffine.getMatrix(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent);
        AlgorithmCiftiResample(myProgObj, &tempCifti, CiftiXML::ALONG_ROW, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myAffine.getMatrix(),
                               curLeftSphere, newLeftSphere, curLeftAreas, newLeftAreas,
                               curRightSphere, newRightSphere, curRightAreas, newRightAreas,
                               curCerebSphere, newCerebSphere, curCerebAreas, newCerebAreas,
                               volDilateMethod, volDilateExponent, surfDilateMethod, surfDilateExponent);
    }
}
