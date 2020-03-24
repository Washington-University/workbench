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

#include "OperationCiftiEstimateFWHM.h"
#include "OperationException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmMetricEstimateFWHM.h"
#include "AlgorithmVolumeEstimateFWHM.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationCiftiEstimateFWHM::getCommandSwitch()
{
    return "-cifti-estimate-fwhm";
}

AString OperationCiftiEstimateFWHM::getShortDescription()
{
    return "ESTIMATE FWHM SMOOTHNESS OF A CIFTI FILE";
}

OperationParameters* OperationCiftiEstimateFWHM::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti file");
    
    ret->createOptionalParameter(2, "-merged-volume", "treat volume components as if they were a single component");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(3, "-column", "only output estimates for one column");
    columnOpt->addIntegerParameter(1, "column", "the column number");
    
    ParameterComponent* surfOpt = ret->createRepeatableParameter(4, "-surface", "specify an input surface");
    surfOpt->addStringParameter(1, "structure", "what structure to use this surface for");
    surfOpt->addSurfaceParameter(2, "surface", "the surface file");
    
    OptionalParameter* wholeFileOpt = ret->createOptionalParameter(5, "-whole-file", "estimate for the whole file at once, not each column separately");
    wholeFileOpt->createOptionalParameter(1, "-demean", "subtract the mean image before estimating smoothness");
    
    AString myText =
        AString("Estimate the smoothness of the components of the cifti file, printing the estimates to standard output.  ") +
        "If -merged-volume is used, all voxels are used as a single component, rather than separated by structure.\n\n" +
        "<structure> must be one of the following:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

struct VolParams
{
    AString name;
    CaretPointer<VolumeFile> data, roi;//prevent copy in vector expansion
};

struct SurfParams
{
    AString name;
    SurfaceFile* surf;
    CaretPointer<MetricFile> data, roi;
};

void OperationCiftiEstimateFWHM::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCifti = myParams->getCifti(1);
    bool mergedVol = myParams->getOptionalParameter(2)->m_present;
    int column = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(3);
    if (columnOpt->m_present)
    {
        column = columnOpt->getInteger(1) - 1;//compensate for 1-based UI indices
        if (column < 0 || column >= myCifti->getNumberOfColumns()) throw OperationException("invalid column index");
    }
    const vector<ParameterComponent*>& surfInstances = myParams->getRepeatableParameterInstances(4);
    int numInstances = (int)surfInstances.size();
    for (int i = 0; i < numInstances; ++i)
    {
        bool ok = false;
        StructureEnum::fromName(surfInstances[i]->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized structure name: " + surfInstances[i]->getString(1));
    }
    bool wholeFile = false, demean = false;
    OptionalParameter* wholeFileOpt = myParams->getOptionalParameter(5);
    if (wholeFileOpt->m_present)
    {
        if (columnOpt->m_present) throw OperationException("specifying both -column and -whole-file is not allowed");
        wholeFile = true;
        demean = wholeFileOpt->getOptionalParameter(1)->m_present;
    }
    vector<VolParams> volProcess;
    vector<SurfParams> surfProcess;
    const CiftiXML& myXML = myCifti->getCiftiXML();
    if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw OperationException("mapping type along column must be brain models");
    }
    const CiftiBrainModelsMap& myMap = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    vector<StructureEnum::Enum> surfStructs = myMap.getSurfaceStructureList(), volStructs = myMap.getVolumeStructureList();
    int numSurf = (int)surfStructs.size();
    surfProcess.resize(numSurf);
    for (int i = 0; i < numSurf; ++i)
    {
        surfProcess[i].surf = NULL;
        for (int j = 0; j < numInstances; ++j)
        {
            StructureEnum::Enum myStruct = StructureEnum::fromName(surfInstances[j]->getString(1), NULL);//we already checked that this is a structure name
            if (myStruct == surfStructs[i])
            {
                surfProcess[i].surf = surfInstances[j]->getSurface(2);
                break;
            }
        }
        if (surfProcess[i].surf == NULL) throw OperationException("missing surface for structure '" + StructureEnum::toName(surfStructs[i]) + "'");
        surfProcess[i].name = StructureEnum::toName(surfStructs[i]);
        surfProcess[i].data.grabNew(new MetricFile());
        surfProcess[i].roi.grabNew(new MetricFile());
        AlgorithmCiftiSeparate(NULL, myCifti, CiftiXML::ALONG_COLUMN, surfStructs[i], surfProcess[i].data, surfProcess[i].roi);
        if (surfProcess[i].surf->getNumberOfNodes() != surfProcess[i].data->getNumberOfNodes())
        {
            throw OperationException("input surface for structure '" + StructureEnum::toName(surfStructs[i]) + "' has different number of nodes than the cifti file");
        }
    }
    if (mergedVol)
    {
        volProcess.resize(1);
        volProcess[0].name = "Voxels";
        volProcess[0].data.grabNew(new VolumeFile());
        volProcess[0].roi.grabNew(new VolumeFile());
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, myCifti, CiftiXML::ALONG_COLUMN, volProcess[0].data, offset, volProcess[0].roi, true);
    } else {
        int numVol = (int)volStructs.size();
        volProcess.resize(numVol);
        for (int i = 0; i < numVol; ++i)
        {
            volProcess[i].name = StructureEnum::toName(volStructs[i]);
            volProcess[i].data.grabNew(new VolumeFile());
            volProcess[i].roi.grabNew(new VolumeFile());
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, CiftiXML::ALONG_COLUMN, volStructs[i], volProcess[i].data, offset, volProcess[i].roi, true);
        }
    }
    if (wholeFile)
    {
        for (int j = 0; j < (int)surfProcess.size(); ++j)
        {
            float fwhm = AlgorithmMetricEstimateFWHM::estimateFWHMAllColumns(surfProcess[j].surf, surfProcess[j].data, surfProcess[j].roi, demean);
            cout << surfProcess[j].name << " FWHM: " << fwhm << endl;
        }
        for (int j = 0; j < (int)volProcess.size(); ++j)
        {
            Vector3D fwhm = AlgorithmVolumeEstimateFWHM::estimateFWHMAllFrames(volProcess[j].data, volProcess[j].roi, demean);
            cout << volProcess[j].name << " FWHM: " << fwhm[0] << ", " << fwhm[1] << ", " << fwhm[2] << endl;
        }
    } else {
        if (column == -1)
        {
            int rowLength = (int)myCifti->getNumberOfColumns();
            for (int i = 0; i < rowLength; ++i)
            {
                if (rowLength > 1) cout << "Column " << i + 1 << ":" << endl;
                for (int j = 0; j < (int)surfProcess.size(); ++j)
                {
                    float fwhm = AlgorithmMetricEstimateFWHM::estimateFWHM(surfProcess[j].surf, surfProcess[j].data, surfProcess[j].roi, i);
                    cout << surfProcess[j].name << " FWHM: " << fwhm << endl;
                }
                for (int j = 0; j < (int)volProcess.size(); ++j)
                {
                    Vector3D fwhm = AlgorithmVolumeEstimateFWHM::estimateFWHM(volProcess[j].data, volProcess[j].roi, i);
                    cout << volProcess[j].name << " FWHM: " << fwhm[0] << ", " << fwhm[1] << ", " << fwhm[2] << endl;
                }
            }
        } else {
            cout << "Column " << column + 1 << ":" << endl;
            for (int j = 0; j < (int)surfProcess.size(); ++j)
            {
                float fwhm = AlgorithmMetricEstimateFWHM::estimateFWHM(surfProcess[j].surf, surfProcess[j].data, surfProcess[j].roi, column);
                cout << surfProcess[j].name << " FWHM: " << fwhm << endl;
            }
            for (int j = 0; j < (int)volProcess.size(); ++j)
            {
                Vector3D fwhm = AlgorithmVolumeEstimateFWHM::estimateFWHM(volProcess[j].data, volProcess[j].roi, column);
                cout << volProcess[j].name << " FWHM: " << fwhm[0] << ", " << fwhm[1] << ", " << fwhm[2] << endl;
            }
        }
    }
}
