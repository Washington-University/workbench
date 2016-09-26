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

#include "AlgorithmCiftiParcelMappingToLabel.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"

#include <cstdlib>

using namespace caret;
using namespace std;

AString AlgorithmCiftiParcelMappingToLabel::getCommandSwitch()
{
    return "-cifti-parcel-mapping-to-label";
}

AString AlgorithmCiftiParcelMappingToLabel::getShortDescription()
{
    return "CREATE DLABEL FROM PARCELLATED FILE";
}

OperationParameters* AlgorithmCiftiParcelMappingToLabel::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input parcellated file");
    
    ret->addStringParameter(2, "direction", "which dimension to take the parcel map from, ROW or COLUMN");
    
    ret->addCiftiParameter(3, "template-cifti", "a cifti file with the desired dense mapping along column");
    
    ret->addCiftiOutputParameter(4, "dlabel-out", "the output dense label file");
    
    ret->setHelpText(
        AString("This command will output a dlabel file, useful for doing the same parcellation to another dense file.\n\n") +
        "For ptseries, pscalar, plabel, pconn, and pdconn, using COLUMN for <direction> will work."
    );
    return ret;
}

void AlgorithmCiftiParcelMappingToLabel::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* ciftiIn = myParams->getCifti(1);
    AString dirString = myParams->getString(2);
    int direction = -1;
    if (dirString == "ROW")
    {
        direction = CiftiXML::ALONG_ROW;
    } else if (dirString == "COLUMN") {
        direction = CiftiXML::ALONG_COLUMN;
    } else {
        throw AlgorithmException("unrecognized direction string, use ROW or COLUMN");
    }
    CiftiFile* templateCifti = myParams->getCifti(3);
    CiftiFile* ciftiOut = myParams->getOutputCifti(4);
    const CiftiXML& parcelXML = ciftiIn->getCiftiXML();
    if (direction >= parcelXML.getNumberOfDimensions())
    {
        throw AlgorithmException("specified direction does not exist in input cifti file");
    }
    if (parcelXML.getMappingType(direction) != CiftiMappingType::PARCELS)
    {
        throw AlgorithmException("input cifti file does not have parcels mapping on specified direction");
    }
    const CiftiXML& denseXML = templateCifti->getCiftiXML();
    if (denseXML.getNumberOfDimensions() < 2)
    {
        throw AlgorithmException("template cifti file does not have an along column dimension");
    }
    if (denseXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("template cifti file does not have a dense mapping along column, try using a dscalar, dtseries, dlabel, or dconn");
    }
    AlgorithmCiftiParcelMappingToLabel(myProgObj, parcelXML.getParcelsMap(direction), denseXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN), ciftiOut);
}

AlgorithmCiftiParcelMappingToLabel::AlgorithmCiftiParcelMappingToLabel(ProgressObject* myProgObj, const CiftiParcelsMap& parcelMap, const CiftiBrainModelsMap& denseMap, CiftiFile* ciftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_COLUMN, denseMap);
    vector<StructureEnum::Enum> surfStructures = parcelMap.getParcelSurfaceStructures();
    CiftiLabelsMap outLabelMap;
    outLabelMap.setLength(1);
    GiftiLabelTable* outTable = outLabelMap.getMapLabelTable(0);
    vector<int32_t> parcelToKey(parcelMap.getLength());
    for (int i = 0; i < (int)parcelMap.getLength(); ++i)
    {
        parcelToKey[i] = outTable->addLabel(parcelMap.getIndexName(i), rand() & 255, rand() & 255, rand() & 255, 255);
    }
    outXML.setMap(CiftiXML::ALONG_ROW, outLabelMap);
    vector<float> scratchCol(denseMap.getLength(), outTable->getUnassignedLabelKey());
    vector<StructureEnum::Enum> surfStructs = denseMap.getSurfaceStructureList();
    for (int i = 0; i < (int)surfStructs.size(); ++i)
    {
        vector<CiftiBrainModelsMap::SurfaceMap> surfMap = denseMap.getSurfaceMap(surfStructs[i]);
        for (int j = 0; j < (int)surfMap.size(); ++j)
        {
            int whichParcel = parcelMap.getIndexForNode(surfMap[j].m_surfaceNode, surfStructs[i]);
            if (whichParcel >= 0)
            {
                scratchCol[surfMap[j].m_ciftiIndex] = parcelToKey[whichParcel];
            }
        }
    }
    vector<CiftiBrainModelsMap::VolumeMap> volMap = denseMap.getFullVolumeMap();
    for (int i = 0; i < (int)volMap.size(); ++i)
    {
        int whichParcel = parcelMap.getIndexForVoxel(volMap[i].m_ijk);
        if (whichParcel >= 0)
        {
            scratchCol[volMap[i].m_ciftiIndex] = parcelToKey[whichParcel];
        }
    }
    ciftiOut->setCiftiXML(outXML);
    ciftiOut->setColumn(scratchCol.data(), 0);
}

float AlgorithmCiftiParcelMappingToLabel::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiParcelMappingToLabel::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
