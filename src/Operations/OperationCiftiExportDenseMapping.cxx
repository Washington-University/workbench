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

#include "OperationCiftiExportDenseMapping.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "CiftiXML.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationCiftiExportDenseMapping::getCommandSwitch()
{
    return "-cifti-export-dense-mapping";
}

AString OperationCiftiExportDenseMapping::getShortDescription()
{
    return "WRITE INDEX TO ELEMENT MAPPING AS TEXT";
}

OperationParameters* OperationCiftiExportDenseMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the cifti file");
    
    ret->addStringParameter(2, "direction", "which direction to export the mapping from, ROW or COLUMN");//TODO: 3rd+ dimension
    
    ParameterComponent* surfOpt = ret->createRepeatableParameter(3, "-surface", "export the the mapping of one surface structure");
    surfOpt->addStringParameter(1, "structure", "the structure to output");
    surfOpt->addStringParameter(2, "text-out", "output - the output text file");//fake the output formatting
    surfOpt->createOptionalParameter(3, "-no-cifti-index", "don't write the cifti index in the output file");
    
    ParameterComponent* volOpt = ret->createRepeatableParameter(4, "-volume", "export the the mapping of one volume structure");
    volOpt->addStringParameter(1, "structure", "the structure to output");
    volOpt->addStringParameter(2, "text-out", "output - the output text file");//fake the output formatting
    volOpt->createOptionalParameter(3, "-no-cifti-index", "don't write the cifti index in the output file");
    
    OptionalParameter* volAllOpt = ret->createOptionalParameter(5, "-volume-all", "export the the mapping of all voxels");//repeatable, for different options?
    volAllOpt->addStringParameter(1, "text-out", "output - the output text file");//fake the output formatting
    volAllOpt->createOptionalParameter(2, "-no-cifti-index", "don't write the cifti index in the output file");
    volAllOpt->createOptionalParameter(3, "-structure", "write the structure each voxel belongs to in the output file");
    
    //-surface-all? -all?
    
    AString helpText =
        AString("This command produces text files that describe the mapping from cifti indices to surface vertices or voxels.  ") +
        "All indices are zero-based.  " +
        "The default format for -surface is lines of the form:\n\n" +
        "<cifti-index> <vertex>\n\n" +
        "The default format for -volume and -volume-all is lines of the form:\n\n" +
        "<cifti-index> <i> <j> <k>\n\n" + 
        "For each <structure> argument, use one of the following strings:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void OperationCiftiExportDenseMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCifti = myParams->getCifti(1);
    AString dirString = myParams->getString(2);
    int myDir;
    if (dirString == "ROW")
    {
        myDir = CiftiXML::ALONG_ROW;
    } else if (dirString == "COLUMN") {
        myDir = CiftiXML::ALONG_COLUMN;
    } else {
        throw OperationException("incorrect string for direction, use ROW or COLUMN");
    }
    const vector<ParameterComponent*>& surfOpts = *(myParams->getRepeatableParameterInstances(3));
    const vector<ParameterComponent*>& volOpts = *(myParams->getRepeatableParameterInstances(4));
    OptionalParameter* volAllOpt = myParams->getOptionalParameter(5);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS) throw OperationException("specified direction in cifti file does not have BRAIN_MODELS mapping");
    const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(myDir);
    for (int i = 0; i < (int)surfOpts.size(); ++i)
    {
        AString structName = surfOpts[i]->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok) throw OperationException("invalid structure name: '" + structName + "'");
        if (!myDenseMap.hasSurfaceData(myStruct)) throw OperationException("no surface mapping found in specified direction for structure '" + structName + "'");
        AString outName = surfOpts[i]->getString(2);
        bool writeCiftiIndex = !(surfOpts[i]->getOptionalParameter(3)->m_present);
        ofstream outFile(outName.toLocal8Bit().constData());
        if (!outFile) throw OperationException("failed to open output text file");
        vector<CiftiBrainModelsMap::SurfaceMap> myMap = myDenseMap.getSurfaceMap(myStruct);
        for (int j = 0; j < (int)myMap.size(); ++j)
        {
            if (writeCiftiIndex) outFile << myMap[j].m_ciftiIndex << " ";
            outFile << myMap[j].m_surfaceNode << "\n";//avoid endl so it doesn't constantly flush
        }
        outFile.flush();
        if (!outFile) throw OperationException("failed to write to output text file");
    }
    for (int i = 0; i < (int)volOpts.size(); ++i)
    {
        AString structName = volOpts[i]->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok) throw OperationException("invalid structure name: '" + structName + "'");
        if (!myDenseMap.hasVolumeData(myStruct)) throw OperationException("no volume mapping found in specified direction for structure '" + structName + "'");
        AString outName = volOpts[i]->getString(2);
        bool writeCiftiIndex = !(volOpts[i]->getOptionalParameter(3)->m_present);
        ofstream outFile(outName.toLocal8Bit().constData());
        if (!outFile) throw OperationException("failed to open output text file");
        vector<CiftiBrainModelsMap::VolumeMap> myMap = myDenseMap.getVolumeStructureMap(myStruct);
        for (int j = 0; j < (int)myMap.size(); ++j)
        {
            if (writeCiftiIndex) outFile << myMap[j].m_ciftiIndex << " ";
            outFile << myMap[j].m_ijk[0] << " " << myMap[j].m_ijk[1] << " " << myMap[j].m_ijk[2] << "\n";//avoid endl so it doesn't constantly flush
        }
        outFile.flush();
        if (!outFile) throw OperationException("failed to write to output text file");
    }
    if (volAllOpt->m_present)
    {
        if (!myDenseMap.hasVolumeData()) throw OperationException("no volume data found in specified direction");
        AString outName = volAllOpt->getString(1);
        bool writeCiftiIndex = !(volAllOpt->getOptionalParameter(2)->m_present);
        bool writeStructure = volAllOpt->getOptionalParameter(3)->m_present;
        ofstream outFile(outName.toLocal8Bit().constData());
        if (!outFile) throw OperationException("failed to open output text file");
        vector<StructureEnum::Enum> volStructs = myDenseMap.getVolumeStructureList();//NOTE: CiftiBrainModelsMap guarantees this is in cifti index order
        for (int j = 0; j < (int)volStructs.size(); ++j)
        {
            vector<CiftiBrainModelsMap::VolumeMap> myMap = myDenseMap.getVolumeStructureMap(volStructs[j]);
            AString structName = StructureEnum::toName(volStructs[j]);
            for (int k = 0; k < (int)myMap.size(); ++k)
            {
                if (writeCiftiIndex) outFile << myMap[k].m_ciftiIndex << " ";
                if (writeStructure) outFile << structName << " ";
                outFile << myMap[k].m_ijk[0] << " " << myMap[k].m_ijk[1] << " " << myMap[k].m_ijk[2] << "\n";//avoid endl so it doesn't constantly flush
            }
        }
        outFile.flush();
        if (!outFile) throw OperationException("failed to write to output text file");
    }
}
