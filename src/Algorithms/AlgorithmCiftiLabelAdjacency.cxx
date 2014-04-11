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

#include "AlgorithmCiftiLabelAdjacency.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmCiftiSeparate.h" //for cropped volume space
#include "CiftiFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiLabelAdjacency::getCommandSwitch()
{
    return "-cifti-label-adjacency";
}

AString AlgorithmCiftiLabelAdjacency::getShortDescription()
{
    return "MAKE ADJACENCY MATRIX OF A CIFTI LABEL FILE";
}

OperationParameters* AlgorithmCiftiLabelAdjacency::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "label-in", "the input cifti label file");
    
    ret->addCiftiOutputParameter(2, "adjacency-out", "the output cifti pconn adjacency matrix");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(3, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(4, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(5, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    ret->setHelpText(
        AString("Find face-adjacent voxels and connected vertices that have different label values, and count them for each pair.  ") +
        "Put the resulting counts into a parcellated connectivity file, with the diagonal being zero.  " +
        "This gives a rough estimate of how long or expansive the border between two labels is."
    );
    return ret;
}

void AlgorithmCiftiLabelAdjacency::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myLabelIn = myParams->getCifti(1);
    CiftiFile* myAdjOut = myParams->getOutputCifti(2);
    SurfaceFile* myLeftSurf = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(3);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    SurfaceFile* myRightSurf = NULL;
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(4);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    SurfaceFile* myCerebSurf = NULL;
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(5);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
    }
    AlgorithmCiftiLabelAdjacency(myProgObj, myLabelIn, myAdjOut, myLeftSurf, myRightSurf, myCerebSurf);
}

AlgorithmCiftiLabelAdjacency::AlgorithmCiftiLabelAdjacency(ProgressObject* myProgObj, const CiftiInterface* myLabelIn, CiftiFile* myAdjOut,
                                                           const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myLabelXML = myLabelIn->getCiftiXML();
    if (myLabelXML.getNumberOfDimensions() != 2 ||
        myLabelXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS ||
        myLabelXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti label file has the wrong mapping types");
    }
    const CiftiBrainModelsMap& myDenseMap = myLabelXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    vector<StructureEnum::Enum> surfaceList = myDenseMap.getSurfaceStructureList();
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        const SurfaceFile* mySurf = NULL;
        AString surfType;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                surfType = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                surfType = "right";
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                surfType = "cerebellum";
                break;
            default:
                throw AlgorithmException("found surface model with unexpected type: " + StructureEnum::toName(surfaceList[whichStruct]));
                break;
        }
        if (mySurf == NULL)
        {
            throw AlgorithmException(surfType + " surface required but not provided");
        }
        if (mySurf->getNumberOfNodes() != myDenseMap.getSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
    }
    vector<int> indexToParcel;
    CiftiParcelsMap myParcelMap = AlgorithmCiftiParcellate::parcellateMapping(myLabelIn, myDenseMap, indexToParcel);
    int numParcels = myParcelMap.getLength();
    if (numParcels == 0) throw AlgorithmException("no labels found, output would be empty");
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_ROW, myParcelMap);
    outXML.setMap(CiftiXML::ALONG_COLUMN, myParcelMap);
    myAdjOut->setCiftiXML(outXML);
    vector<vector<int64_t> > adjCount(numParcels, vector<int64_t>(numParcels, 0));//count as int, translate back to float when writing
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                break;
            default:
                throw AlgorithmException("found surface model with unexpected type: " + StructureEnum::toName(surfaceList[whichStruct]));
                break;
        }
        CaretPointer<TopologyHelper> myHelp = mySurf->getTopologyHelper();
        int numNodes = mySurf->getNumberOfNodes();
        for (int i = 0; i < numNodes - 1; ++i)//to avoid double counting, only count pairs that are in ascending order - this means the last node won't have any valid edges
        {
            int64_t baseIndex = myDenseMap.getIndexForNode(i, surfaceList[whichStruct]);
            if (baseIndex < 0) continue;
            int baseLabel = indexToParcel[baseIndex];//translate on the fly, to do separate we would need to put indexToParcel into a temporary CiftiFile
            if (baseLabel < 0) continue;
            const vector<int32_t>& neighbors = myHelp->getNodeNeighbors(i);
            int numNeighbors = (int)neighbors.size();
            for (int j = 0; j < numNeighbors; ++j)
            {
                if (neighbors[j] > i)
                {
                    int64_t neighIndex = myDenseMap.getIndexForNode(neighbors[j], surfaceList[whichStruct]);
                    if (neighIndex < 0) continue;
                    int neighLabel = indexToParcel[neighIndex];
                    if (neighLabel < 0) continue;
                    if (baseLabel != neighLabel)
                    {
                        ++adjCount[baseLabel][neighLabel];
                        ++adjCount[neighLabel][baseLabel];
                    }
                }
            }
        }
    }
    if (myDenseMap.hasVolumeData())
    {
        int64_t dims[3], offset[3];//all we really want is offset and dims, to avoid scanning the original FOV
        vector<vector<float> > sform;
        AlgorithmCiftiSeparate::getCroppedVolSpaceAll(myLabelIn, CiftiXML::ALONG_COLUMN, dims, sform, offset);
        int64_t stencil[9] = {1, 0, 0,
                                0, 1, 0,
                                0, 0, 1};//only forward differences, to avoid double counting
        int64_t ijk[3];
        for (ijk[2] = offset[2]; ijk[2] < offset[2] + dims[2]; ++ijk[2])
        {
            for (ijk[1] = offset[1]; ijk[1] < offset[1] + dims[1]; ++ijk[1])
            {
                for (ijk[0] = offset[0]; ijk[0] < offset[0] + dims[0]; ++ijk[0])
                {
                    int64_t baseIndex = myDenseMap.getIndexForVoxel(ijk);
                    if (baseIndex < 0) continue;
                    int baseLabel = indexToParcel[baseIndex];
                    if (baseLabel < 0) continue;
                    for (int neighbor = 0; neighbor < 9; neighbor += 3)
                    {
                        int64_t neighIndex = myDenseMap.getIndexForVoxel(ijk[0] + stencil[neighbor], ijk[1] + stencil[neighbor + 1], ijk[2] + stencil[neighbor + 2]);
                        if (neighIndex < 0) continue;
                        int neighLabel = indexToParcel[neighIndex];
                        if (neighLabel < 0) continue;
                        if (baseLabel != neighLabel)
                        {
                            ++adjCount[baseLabel][neighLabel];
                            ++adjCount[neighLabel][baseLabel];
                        }
                    }
                }
            }
        }
    }
    vector<float> tempRow(numParcels);
    for (int i = 0; i < numParcels; ++i)
    {
        for (int j = 0; j < numParcels; ++j)
        {
            tempRow[j] = adjCount[i][j];
        }
        myAdjOut->setRow(tempRow.data(), i);
    }
}

float AlgorithmCiftiLabelAdjacency::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiLabelAdjacency::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
