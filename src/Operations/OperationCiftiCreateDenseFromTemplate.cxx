/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "OperationCiftiCreateDenseFromTemplate.h"
#include "OperationException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "CaretAssert.h"
#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <iostream>

using namespace caret;
using namespace std;

AString OperationCiftiCreateDenseFromTemplate::getCommandSwitch()
{
    return "-cifti-create-dense-from-template";
}

AString OperationCiftiCreateDenseFromTemplate::getShortDescription()
{
    return "CREATE CIFTI WITH MATCHING DENSE MAP";
}

OperationParameters* OperationCiftiCreateDenseFromTemplate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "template-cifti", "file to match brainordinates of");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    
    OptionalParameter* seriesOpt = ret->createOptionalParameter(3, "-series", "make a dtseries file instead of a dscalar");
    seriesOpt->addDoubleParameter(1, "step", "increment between series points");
    seriesOpt->addDoubleParameter(2, "start", "start value of the series");
    OptionalParameter* sUnitOpt = seriesOpt->createOptionalParameter(3, "-unit", "select unit for series (default SECOND)");
    sUnitOpt->addStringParameter(1, "unit", "unit identifier");

    OptionalParameter* volAllOpt = ret->createOptionalParameter(4, "-volume-all", "specify an input volume file for all voxel data");
    volAllOpt->addVolumeParameter(1, "volume-in", "the input volume file");
    volAllOpt->createOptionalParameter(2, "-from-cropped", "the input is cropped to the size of the voxel data in the template file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(5, "-cifti", "use input data from a cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "cifti file containing input data");
    
    ParameterComponent* metricOpt = ret->createRepeatableParameter(6, "-metric", "use input data from a metric file");
    metricOpt->addStringParameter(1, "structure", "which structure to put the metric file into");
    metricOpt->addMetricParameter(2, "metric-in", "input metric file");
    
    ParameterComponent* labelOpt = ret->createRepeatableParameter(7, "-label", "use input data from surface label files");
    labelOpt->addStringParameter(1, "structure", "which structure to put the label file into");
    labelOpt->addLabelParameter(2, "label-in", "input label file");
    
    ParameterComponent* volOpt = ret->createRepeatableParameter(8, "-volume", "use a volume file for a single volume structure's data");
    volOpt->addStringParameter(1, "structure", "which structure to put the volume file into");
    volOpt->addVolumeParameter(2, "volume-in", "the input volume file");
    volOpt->createOptionalParameter(3, "-from-cropped", "the input is cropped to the size of the volume structure");
    
    OptionalParameter* collideOpt = ret->createOptionalParameter(9, "-label-collision", "how to handle conflicts between label keys");
    collideOpt->addStringParameter(1, "action", "'ERROR', 'SURFACES_FIRST', or 'LEGACY', default 'ERROR', use 'LEGACY' to match v1.4.2 and earlier");

    AString helpText = AString("This command helps you make a new dscalar, dtseries, or dlabel cifti file that matches the brainordinate space used in another cifti file.  ") +
        "The template file must have the desired brainordinate space in the mapping along the column direction (for dtseries, dscalar, dlabel, and symmetric dconn this is always the case).  " +
        "All input cifti files must have a brain models mapping along column and use the same volume space and/or surface vertex count as the template for structures that they contain.  " +
        "If any input files contain label data, then input files with non-label data are not allowed, and the -series option may not be used.\n\n" +
        "Any structure that isn't covered by an input is filled with zeros or the unlabeled key.\n\n" +
        "The <structure> argument of -metric, -label or -volume must be one of the following:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    helpText += "\n\nThe argument to -unit must be one of the following:\n";
    vector<CiftiSeriesMap::Unit> unitList = CiftiSeriesMap::getAllUnits();
    for (int i = 0; i < (int)unitList.size(); ++i)
    {
        helpText += "\n" + CiftiSeriesMap::unitToString(unitList[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

namespace
{
    enum DataSourceType
    {
        NONE,//fill with zeros
        CIFTI,
        LABEL,
        METRIC,
        VOLUME,
        VOLUME_ALL
    };
    
    enum LabelConflictLogic
    {
        ERROR,
        SURFACES_FIRST,
        LEGACY
    };

    struct DataSourceInfo
    {
        DataSourceType type;
        int64_t index;//for repeatable options
        
        DataSourceInfo(const DataSourceType myType, const int64_t myIndex)
        {
            type = myType;
            index = myIndex;
        };
        DataSourceInfo()
        {
            type = NONE;
            index = -1;
        };
    };
}

void OperationCiftiCreateDenseFromTemplate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiFile* templateCifti = myParams->getCifti(1);
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    LabelConflictLogic conflictLogic = ERROR;
    OptionalParameter* collideOpt = myParams->getOptionalParameter(9);
    if (collideOpt->m_present)
    {
        AString collideStr = collideOpt->getString(1);
        if (collideStr == "ERROR")
        {
            conflictLogic = ERROR;
        } else if (collideStr == "SURFACES_FIRST") {
            conflictLogic = SURFACES_FIRST;
        } else if (collideStr == "LEGACY") {
            conflictLogic = LEGACY;
        } else {
            throw OperationException("incorrect string for label collision option");
        }
    }
    const CiftiXML& templateXML = templateCifti->getCiftiXML();
    if (templateXML.getNumberOfDimensions() != 2)
    {
        throw OperationException("template cifti file must have exactly 2 dimensions");
    }
    if (templateXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw OperationException("template cifti file does not have brain models along column");
    }
    CiftiXML outXML;//don't inherit any metadata from the template file, we want only the brainordinates
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_COLUMN, *(templateXML.getMap(CiftiXML::ALONG_COLUMN)));
    int labelMode = -1;//-1 not set, 0 set to false, 1 set to true
    int64_t numMaps = -1;
    const CaretMappableDataFile* nameFile = NULL;
    const CiftiFile* ciftiNameFile = NULL;//cifti doesn't inherit from CaretMappableDataFile, it is too different
    const CiftiBrainModelsMap& templateMap = templateXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    vector<StructureEnum::Enum> surfStructures = templateMap.getSurfaceStructureList(), volStructures = templateMap.getVolumeStructureList();
    vector<DataSourceInfo> surfInfo(surfStructures.size()), volInfo(volStructures.size());
    OptionalParameter* volAllOpt = myParams->getOptionalParameter(4);
    if (volAllOpt->m_present)
    {
        if (!templateMap.hasVolumeData())
        {
            throw OperationException("-volume-all specified, but template cifti file does use any voxels");
        }
        VolumeFile* allVolume = volAllOpt->getVolume(1);
        bool fromCropped = volAllOpt->getOptionalParameter(2)->m_present;
        const VolumeSpace& volAllSpace = allVolume->getVolumeSpace();
        VolumeSpace templateSpace;
        if (fromCropped)
        {
            int64_t tempDims[3], tempOffset[3];
            vector<vector<float> > tempSform;
            AlgorithmCiftiSeparate::getCroppedVolSpaceAll(templateCifti, CiftiXML::ALONG_COLUMN, tempDims, tempSform, tempOffset);
            templateSpace.setSpace(tempDims, tempSform);
        } else {
            templateSpace = templateMap.getVolumeSpace();
        }
        if (!templateSpace.matches(volAllSpace))
        {
            throw OperationException("-volume-all specifies a volume file that doesn't match the volume space of the template cifti file");
        }
        numMaps = allVolume->getNumberOfMaps();
        nameFile = allVolume;
        if (allVolume->getType() == SubvolumeAttributes::LABEL)
        {
            labelMode = 1;
        } else {
            labelMode = 0;
        }
        for (int i = 0; i < (int)volInfo.size(); ++i)
        {
            volInfo[i].type = VOLUME_ALL;
        }
    }
    const vector<ParameterComponent*>& ciftiInstances = myParams->getRepeatableParameterInstances(5);
    for (int instance = 0; instance < (int)ciftiInstances.size(); ++instance)
    {
        const CiftiFile* thisCifti = ciftiInstances[instance]->getCifti(1);
        const CiftiXML& thisXML = thisCifti->getCiftiXML();
        if (thisXML.getNumberOfDimensions() != 2)
        {
            throw OperationException("input cifti files must have exactly 2 dimensions");
        }
        if (thisXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
        {
            throw OperationException("input cifti files must have a brain models mapping along column");
        }
        const CiftiBrainModelsMap& thisDenseMap = thisXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        bool isLabel = (thisXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::LABELS);
        if (labelMode == -1)
        {
            labelMode = isLabel ? 1 : 0;
        } else {
            if (isLabel)
            {
                if (labelMode == 0)
                {
                    throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains label data, but -volume-all contains non-label data");
                }
            } else {
                if (labelMode == 1)
                {
                    throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains non-label data, but -volume-all contains label data");
                }
            }
        }
        int64_t thisNumMaps = thisXML.getDimensionLength(CiftiXML::ALONG_ROW);
        if (numMaps == -1)
        {
            numMaps = thisNumMaps;
        } else {
            if (numMaps != thisNumMaps)
            {
                throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains " +
                                         AString::number(thisNumMaps) + " maps, but -volume-all contains " + AString::number(numMaps) + " maps");
            }
        }
        if (thisDenseMap.hasVolumeData() && templateMap.hasVolumeData() && !templateMap.getVolumeSpace().matches(thisDenseMap.getVolumeSpace()))
        {
            throw OperationException("cifti file '" + thisCifti->getFileName() + "' uses a different volume space than the template file");
        }
        vector<StructureEnum::Enum> thisSurfStructs = thisDenseMap.getSurfaceStructureList(), thisVolStructs = thisDenseMap.getVolumeStructureList();
        for (int whichStruct = 0; whichStruct < (int)thisSurfStructs.size(); ++whichStruct)
        {
            vector<StructureEnum::Enum>::const_iterator iter = std::find(surfStructures.begin(), surfStructures.end(), thisSurfStructs[whichStruct]);
            if (iter == surfStructures.end())
            {
                cout << ("discarding surface structure " + StructureEnum::toName(thisSurfStructs[whichStruct]) + " from file '" + thisCifti->getFileName() + "'") << endl;
            } else {
                int outIndex = (int)(iter - surfStructures.begin());
                int64_t templateNumNodes = templateMap.getSurfaceNumberOfNodes(thisSurfStructs[whichStruct]);
                int64_t thisNumNodes = thisDenseMap.getSurfaceNumberOfNodes(thisSurfStructs[whichStruct]);
                if (thisNumNodes != templateNumNodes)
                {
                    throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains surface data for " + StructureEnum::toName(thisSurfStructs[whichStruct]) +
                                             ", but uses a surface with " + AString::number(thisNumNodes) +
                                             " vertices, while template needs " + AString::number(templateNumNodes));
                }
                if (surfInfo[outIndex].type != NONE)
                {
                    throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains data for structure " + StructureEnum::toName(thisSurfStructs[whichStruct]) +
                                             ", but data for this structure also exists in an another -cifti option's file");
                }
                surfInfo[outIndex].type = CIFTI;
                surfInfo[outIndex].index = instance;
            }
        }
        if (templateMap.hasVolumeData())
        {
            for (int whichStruct = 0; whichStruct < (int)thisVolStructs.size(); ++whichStruct)
            {
                vector<StructureEnum::Enum>::const_iterator iter = std::find(volStructures.begin(), volStructures.end(), thisVolStructs[whichStruct]);
                if (iter == volStructures.end())
                {
                    cout << ("discarding volume structure " + StructureEnum::toName(thisVolStructs[whichStruct]) + " from file '" + thisCifti->getFileName() + "'") << endl;
                } else {
                    int outIndex = (int)(iter - volStructures.begin());
                    if (volInfo[outIndex].type != NONE)
                    {
                        throw OperationException("cifti file '" + thisCifti->getFileName() + "' contains data for structure " + StructureEnum::toName(thisVolStructs[whichStruct]) +
                                                ", but data for this structure also exists in an another option's file");
                    }
                    volInfo[outIndex].type = CIFTI;
                    volInfo[outIndex].index = instance;
                }
            }
        } else {
            if (thisDenseMap.hasVolumeData())
            {
                cout << ("discarding volume structures from file '" + thisCifti->getFileName() + "'") << endl;
            }
        }
        if (ciftiNameFile == NULL && (isLabel || thisXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SCALARS ||
                                      thisXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::PARCELS))
        {
            ciftiNameFile = thisCifti;//cifti trumps everything, if it has scalar or label or parcels
        }
    }
    const vector<ParameterComponent*>& metricInstances = myParams->getRepeatableParameterInstances(6);
    for (int instance = 0; instance < (int)metricInstances.size(); ++instance)
    {
        if (labelMode == 1) throw OperationException("-metric option specified when other inputs are label-type files");
        labelMode = 0;
        AString structString = metricInstances[instance]->getString(1);
        const MetricFile* thisMetric = metricInstances[instance]->getMetric(2);
        bool ok = false;
        StructureEnum::Enum thisStruct = StructureEnum::fromName(structString, &ok);
        if (!ok) throw OperationException("unrecognized structure string in -metric option: " + structString);
        vector<StructureEnum::Enum>::const_iterator iter = std::find(surfStructures.begin(), surfStructures.end(), thisStruct);
        if (iter == surfStructures.end())
        {
            throw OperationException("-metric option specified for structure " + structString + ", but template cifti file does not contain this surface structure");
        }
        int outIndex = (int)(iter - surfStructures.begin());
        if (thisMetric->getNumberOfNodes() != templateMap.getSurfaceNumberOfNodes(thisStruct))
        {
            throw OperationException("metric file '" + thisMetric->getFileName() + "' has " + AString::number(thisMetric->getNumberOfNodes()) + " vertices, but template cifti requires " +
                AString::number(templateMap.getSurfaceNumberOfNodes(thisStruct)) + " for structure " + structString);
        }
        if (surfInfo[outIndex].type != NONE)
        {
            throw OperationException("-metric specified with structure " + structString +
                                        ", but data for this structure also exists in an another option");
        }
        int64_t thisNumMaps = thisMetric->getNumberOfMaps();
        if (numMaps == -1)
        {
            numMaps = thisNumMaps;
        } else {
            if (numMaps != thisNumMaps)
            {
                throw OperationException("metric file '" + thisMetric->getFileName() + "' contains " +
                                         AString::number(thisNumMaps) + " maps, but other file(s) contain " + AString::number(numMaps) + " maps");
            }
        }
        checkStructureMatch(thisMetric, thisStruct, "metric file '" + thisMetric->getFileName() + "'", "the -metric option specified");
        surfInfo[outIndex].type = METRIC;
        surfInfo[outIndex].index = instance;
        if (nameFile == NULL) nameFile = thisMetric;
    }
    const vector<ParameterComponent*>& labelInstances = myParams->getRepeatableParameterInstances(7);
    for (int instance = 0; instance < (int)labelInstances.size(); ++instance)
    {
        if (labelMode == 0) throw OperationException("-label option specified when other inputs are real-valued files");
        labelMode = 1;
        AString structString = labelInstances[instance]->getString(1);
        const LabelFile* thisLabel = labelInstances[instance]->getLabel(2);
        bool ok = false;
        StructureEnum::Enum thisStruct = StructureEnum::fromName(structString, &ok);
        if (!ok) throw OperationException("unrecognized structure string in -label option: " + structString);
        vector<StructureEnum::Enum>::const_iterator iter = std::find(surfStructures.begin(), surfStructures.end(), thisStruct);
        if (iter == surfStructures.end())
        {
            throw OperationException("-label option specified for structure " + structString + ", but template cifti file does not contain this surface structure");
        }
        int outIndex = (int)(iter - surfStructures.begin());
        if (thisLabel->getNumberOfNodes() != templateMap.getSurfaceNumberOfNodes(thisStruct))
        {
            throw OperationException("label file '" + thisLabel->getFileName() + "' has " + AString::number(thisLabel->getNumberOfNodes()) + " vertices, but template cifti requires " +
                AString::number(templateMap.getSurfaceNumberOfNodes(thisStruct)) + " for structure " + structString);
        }
        if (surfInfo[outIndex].type != NONE)
        {
            throw OperationException("-label specified with structure " + structString +
                                        ", but data for this structure also exists in an another option");
        }
        int64_t thisNumMaps = thisLabel->getNumberOfMaps();
        if (numMaps == -1)
        {
            numMaps = thisNumMaps;
        } else {
            if (numMaps != thisNumMaps)
            {
                throw OperationException("label file '" + thisLabel->getFileName() + "' contains " +
                                         AString::number(thisNumMaps) + " maps, but other file(s) contain " + AString::number(numMaps) + " maps");
            }
        }
        checkStructureMatch(thisLabel, thisStruct, "label file '" + thisLabel->getFileName() + "'", "the -label option specified");
        surfInfo[outIndex].type = LABEL;
        surfInfo[outIndex].index = instance;
        if (nameFile == NULL) nameFile = thisLabel;
    }
    const vector<ParameterComponent*>& volumeInstances = myParams->getRepeatableParameterInstances(8);
    for (int instance = 0; instance < (int)volumeInstances.size(); ++instance)
    {
        AString structString = volumeInstances[instance]->getString(1);
        const VolumeFile* thisVol = volumeInstances[instance]->getVolume(2);
        bool fromCropped = volumeInstances[instance]->getOptionalParameter(3)->m_present;
        bool ok = false;
        StructureEnum::Enum thisStruct = StructureEnum::fromName(structString, &ok);
        if (!ok) throw OperationException("unrecognized structure string in -volume option: " + structString);
        vector<StructureEnum::Enum>::const_iterator iter = std::find(volStructures.begin(), volStructures.end(), thisStruct);
        if (iter == volStructures.end())
        {
            throw OperationException("-volume option specified for structure " + structString + ", but template cifti file does not contain this volume structure");
        }
        int outIndex = (int)(iter - volStructures.begin());
        bool isLabel = (thisVol->getType() == SubvolumeAttributes::LABEL);
        if (labelMode == -1)
        {
            labelMode = isLabel ? 1 : 0;
        } else {
            if (isLabel)
            {
                if (labelMode == 0)
                {
                    throw OperationException("volume file '" + thisVol->getFileName() + "' contains label data, but other file(s) contain non-label data");
                }
            } else {
                if (labelMode == 1)
                {
                    throw OperationException("volume file '" + thisVol->getFileName() + "' contains non-label data, but other file(s) contain label data");
                }
            }
        }
        VolumeSpace templateSpace;
        if (fromCropped)
        {
            int64_t tempDims[3], tempOffset[3];
            vector<vector<float> > tempSform;
            AlgorithmCiftiSeparate::getCroppedVolSpace(templateCifti, CiftiXML::ALONG_COLUMN, thisStruct, tempDims, tempSform, tempOffset);
            templateSpace.setSpace(tempDims, tempSform);
        } else {
            templateSpace = templateMap.getVolumeSpace();
        }
        if (!thisVol->matchesVolumeSpace(templateSpace))
        {
            throw OperationException("volume file '" + thisVol->getFileName() + "' does not match volume space of template cifti file");
        }
        if (volInfo[outIndex].type != NONE)
        {
            throw OperationException("-volume specified with structure " + structString +
                                        ", but data for this structure also exists in an another option");
        }
        int64_t thisNumMaps = thisVol->getNumberOfMaps();
        if (numMaps == -1)
        {
            numMaps = thisNumMaps;
        } else {
            if (numMaps != thisNumMaps)
            {
                throw OperationException("volume file '" + thisVol->getFileName() + "' contains " +
                                         AString::number(thisNumMaps) + " maps, but other file(s) contain " + AString::number(numMaps) + " maps");
            }
        }
        volInfo[outIndex].type = VOLUME;
        volInfo[outIndex].index = instance;
        if (nameFile == NULL) nameFile = thisVol;//-volume is the lowest priority for names
    }
    if (numMaps == -1)
    {
        throw OperationException("you must specify at least one input option");
    }
    OptionalParameter* seriesOpt = myParams->getOptionalParameter(3);
    if (seriesOpt->m_present)
    {
        if (labelMode == 1)
        {
            throw OperationException("-series option cannot be used when input is label data");
        }
        CiftiSeriesMap outMap;
        outMap.setLength(numMaps);
        outMap.setStep(seriesOpt->getDouble(1));
        outMap.setStart(seriesOpt->getDouble(2));
        OptionalParameter* unitOpt = seriesOpt->getOptionalParameter(3);
        if (unitOpt->m_present)
        {
            AString unitString = unitOpt->getString(1);
            bool ok = false;
            CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::stringToUnit(unitString, ok);
            if (!ok) throw OperationException("unrecognized unit string '" + unitString + "'");
            outMap.setUnit(myUnit);
        }
        outXML.setMap(CiftiXML::ALONG_ROW, outMap);
    } else {
        if (labelMode == 1)
        {
            CiftiLabelsMap outMap;
            outMap.setLength(numMaps);
            if (ciftiNameFile != NULL)
            {
                const CiftiMappingType& nameMap = *(ciftiNameFile->getCiftiXML().getMap(CiftiXML::ALONG_ROW));
                for (int64_t i = 0; i < numMaps; ++i)
                {
                    outMap.setMapName(i, nameMap.getIndexName(i));
                }
            } else {
                if (nameFile != NULL)//if only input is cifti dtseries, there are no names to use
                {
                    for (int64_t i = 0; i < numMaps; ++i)
                    {
                        outMap.setMapName(i, nameFile->getMapName(i));
                    }
                }
            }
            outXML.setMap(CiftiXML::ALONG_ROW, outMap);
        } else {
            CiftiScalarsMap outMap;
            outMap.setLength(numMaps);
            if (ciftiNameFile != NULL)
            {
                const CiftiMappingType& nameMap = *(ciftiNameFile->getCiftiXML().getMap(CiftiXML::ALONG_ROW));
                for (int64_t i = 0; i < numMaps; ++i)
                {
                    outMap.setMapName(i, nameMap.getIndexName(i));
                }
            } else {
                if (nameFile != NULL)
                {
                    for (int64_t i = 0; i < numMaps; ++i)
                    {
                        outMap.setMapName(i, nameFile->getMapName(i));
                    }
                }
            }
            outXML.setMap(CiftiXML::ALONG_ROW, outMap);
        }
    }
    ciftiOut->setCiftiXML(outXML);
    vector<DataSourceInfo> legacyOrder = surfInfo;
    vector<bool> legacyIsSurface(surfInfo.size(), true);//need to separately track what the type of model is
    vector<StructureEnum::Enum> legacyStructure = surfStructures;//and its structure
    if (volStructures.size() > 0 && volInfo[0].type == VOLUME_ALL)
    {
        legacyOrder.push_back(volInfo[0]);//record order of things to do, not one per every structure
        legacyIsSurface.push_back(false);
        legacyStructure.push_back(StructureEnum::ALL);//doesn't matter
    } else {
        legacyOrder.insert(legacyOrder.end(), volInfo.begin(), volInfo.end());//there doesn't appear to be an append function
        legacyIsSurface.resize(legacyOrder.size(), false);
        legacyStructure.insert(legacyStructure.end(), volStructures.begin(), volStructures.end());
    }
    bool errorOnLabelConflict = (conflictLogic == ERROR);
    auto useOrder = legacyOrder;
    auto useIsSurface = legacyIsSurface;
    auto useStructure = legacyStructure;
    switch (conflictLogic)
    {
        case ERROR:
        case LEGACY:
            break;
        case SURFACES_FIRST:
            useOrder.assign(legacyOrder.rbegin(), legacyOrder.rend());//basically, legacy order had the right idea, but the label conflict logic reversed it
            useIsSurface.assign(legacyIsSurface.rbegin(), legacyIsSurface.rend());
            useStructure.assign(legacyStructure.rbegin(), legacyStructure.rend());
    }
    for (int i = 0; i < int(useOrder.size()); ++i)
    {
        switch(useOrder[i].type)
        {
            case CIFTI:
            {
                if (useIsSurface[i])
                {
                    const CiftiFile* toUse = ciftiInstances[useOrder[i].index]->getCifti(1);
                    if (labelMode == 1)
                    {
                        LabelFile tempLabel;
                        AlgorithmCiftiSeparate(NULL, toUse, CiftiXML::ALONG_COLUMN, useStructure[i], &tempLabel);
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempLabel, false, errorOnLabelConflict);
                    } else {
                        MetricFile tempMetric;
                        AlgorithmCiftiSeparate(NULL, toUse, CiftiXML::ALONG_COLUMN, useStructure[i], &tempMetric);
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempMetric);
                    }
                } else {
                    const CiftiFile* toUse = ciftiInstances[useOrder[i].index]->getCifti(1);
                    VolumeFile tempVol;
                    int64_t dims1[3], dims2[3], off1[3], off2[3];//check if the cropped space matches, if so we can save memory easily by using the crop argument (and this is also the common case)
                    vector<vector<float> > sform1, sform2;
                    AlgorithmCiftiSeparate::getCroppedVolSpace(toUse, CiftiXML::ALONG_COLUMN, useStructure[i], dims1, sform1, off1);
                    AlgorithmCiftiSeparate::getCroppedVolSpace(templateCifti, CiftiXML::ALONG_COLUMN, useStructure[i], dims2, sform2, off2);
                    VolumeSpace space1(dims1, sform1), space2(dims2, sform2);
                    if (space1.matches(space2))
                    {
                        AlgorithmCiftiSeparate(NULL, toUse, CiftiXML::ALONG_COLUMN, useStructure[i], &tempVol, off1, NULL, true);
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempVol, true, false, errorOnLabelConflict);
                    } else {
                        AlgorithmCiftiSeparate(NULL, toUse, CiftiXML::ALONG_COLUMN, useStructure[i], &tempVol, off1, NULL, false);
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempVol, false, false, errorOnLabelConflict);
                    }
                }
                break;
            }
            case METRIC:
            {
                const MetricFile* toUse = metricInstances[useOrder[i].index]->getMetric(2);
                AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], toUse);
                break;
            }
            case LABEL:
            {
                const LabelFile* toUse = labelInstances[useOrder[i].index]->getLabel(2);
                AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], toUse, false, errorOnLabelConflict);
                break;
            }
            case VOLUME:
            {
                const VolumeFile* toUse = volumeInstances[useOrder[i].index]->getVolume(2);
                bool fromCropped = volumeInstances[useOrder[i].index]->getOptionalParameter(3)->m_present;
                AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], toUse, fromCropped, false, errorOnLabelConflict);
                break;
            }
            case VOLUME_ALL:
            {//this only exists once in order vectors
                const VolumeFile* toUse = volAllOpt->getVolume(1);
                bool fromCropped = volAllOpt->getOptionalParameter(2)->m_present;
                AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, toUse, fromCropped, false, errorOnLabelConflict);
                break;
            }
            case NONE:
            {
                if (useIsSurface[i])
                {
                    int numNodes = templateMap.getSurfaceNumberOfNodes(useStructure[i]);
                    if (labelMode == 1)
                    {
                        LabelFile tempLabel;
                        tempLabel.setNumberOfNodesAndColumns(numNodes, numMaps);
                        int32_t unlabeledKey = tempLabel.getLabelTable()->getUnassignedLabelKey();
                        vector<int32_t> scratchCol(numNodes, unlabeledKey);
                        for (int64_t i = 0; i < numMaps; ++i)
                        {
                            tempLabel.setLabelKeysForColumn(i, scratchCol.data());
                        }
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempLabel, false, errorOnLabelConflict);
                    } else {
                        MetricFile tempMetric;
                        tempMetric.setNumberOfNodesAndColumns(numNodes, numMaps);
                        vector<float> scratchCol(numNodes, 0.0f);
                        for (int64_t i = 0; i < numMaps; ++i)
                        {
                            tempMetric.setValuesForColumn(i, scratchCol.data());
                        }
                        AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempMetric);
                    }
                } else {
                    VolumeFile tempVol;
                    int64_t offset[3];
                    vector<int64_t> dims(3);
                    vector<vector<float> > sform;
                    AlgorithmCiftiSeparate::getCroppedVolSpace(templateCifti, CiftiXML::ALONG_COLUMN, useStructure[i], dims.data(), sform, offset);
                    dims.push_back(numMaps);
                    if (labelMode == 1)
                    {
                        int64_t frameSize = dims[0] * dims[1] * dims[2];
                        tempVol.reinitialize(dims, sform, 1, SubvolumeAttributes::LABEL);
                        for (int64_t i = 0; i < numMaps; ++i)
                        {
                            int32_t unlabeledKey = tempVol.getMapLabelTable(i)->getUnassignedLabelKey();
                            vector<float> scratchFrame(frameSize, unlabeledKey);
                            tempVol.setFrame(scratchFrame.data(), i);
                        }
                    } else {
                        tempVol.reinitialize(dims, sform);
                        tempVol.setValueAllVoxels(0.0f);
                    }
                    AlgorithmCiftiReplaceStructure(NULL, ciftiOut, CiftiXML::ALONG_COLUMN, useStructure[i], &tempVol, true, false, errorOnLabelConflict);
                }
                break;
            }
        }
    }
}
