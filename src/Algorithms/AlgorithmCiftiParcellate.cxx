/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmException.h"
#include "GiftiLabel.h"
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmCiftiParcellate::getCommandSwitch()
{
    return "-cifti-parcellate";
}

AString AlgorithmCiftiParcellate::getShortDescription()
{
    return "PARCELLATE A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiParcellate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to parcellate");
    
    ret->addCiftiParameter(2, "cifti-label", "a cifti label file to use for the parcellation");
    
    ret->addStringParameter(3, "direction", "which mapping to parcellate, ROW or COLUMN");
    
    ret->addCiftiOutputParameter(4, "cifti-out", "output cifti file");
    
    ret->setHelpText(
        AString("Each label in the cifti label file will be treated as a parcel, and all rows or columns within the parcel are averaged together to form the output ") +
        "row or column.  " +
        "If ROW is specified, then the input mapping along rows must be brainordinates, and the output mapping along rows will be parcels, meaning columns will be averaged together.  " +
        "For dtseries or dscalar, use COLUMN."
    );
    return ret;
}

void AlgorithmCiftiParcellate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiIn = myParams->getCifti(1);
    CiftiFile* myCiftiLabel = myParams->getCifti(2);
    AString dirString = myParams->getString(3);
    int direction = -1;
    if (dirString == "ROW")
    {
        direction = CiftiXML::ALONG_ROW;
    } else {
        if (dirString == "COLUMN")
        {
            direction = CiftiXML::ALONG_COLUMN;
        } else {
            throw AlgorithmException("unrecognized direction string, use ROW or COLUMN");
        }
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(4);
    AlgorithmCiftiParcellate(myProgObj, myCiftiIn, myCiftiLabel, direction, myCiftiOut);
}

AlgorithmCiftiParcellate::AlgorithmCiftiParcellate(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const CiftiFile* myCiftiLabel, const int& direction, CiftiFile* myCiftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    if (direction != CiftiXML::ALONG_COLUMN && direction != CiftiXML::ALONG_ROW)
    {
        throw AlgorithmException("AlgorithmCiftiParcellate doesn't support this direction");
    }
    if (myInputXML.getMappingType(direction) != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti file does not have brain models mapping type in specified direction");
    }
    if (myLabelXML.getRowMappingType() != CIFTI_INDEX_TYPE_LABELS || myLabelXML.getColumnMappingType() != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti label file has the wrong mapping types");
    }
    if ((direction == CiftiXML::ALONG_ROW && myCiftiIn->hasRowVolumeData()) || (direction == CiftiXML::ALONG_COLUMN && myCiftiIn->hasColumnVolumeData()))
    {//don't check volume space if direction doesn't have volume data
        if (myLabelXML.hasColumnVolumeData() && !myInputXML.matchesVolumeSpace(myLabelXML))
        {
            throw AlgorithmException("input cifti files must have the same volume space");
        }
    }
    vector<int> indexToParcel;
    CiftiXML myOutXML = myInputXML;
    int numParcels = -1;
    parcellateMapping(myCiftiLabel, direction, myOutXML, numParcels, indexToParcel);
    if (numParcels == 0)
    {
        throw AlgorithmException("no parcels found, output file would be empty, aborting");
    }
    myCiftiOut->setCiftiXML(myOutXML);
    int64_t numCols = myInputXML.getNumberOfColumns(), numRows = myInputXML.getNumberOfRows();
    vector<float> scratchRow(numCols);
    vector<int64_t> parcelCounts(numParcels, 0);
    for (int64_t j = 0; j < numCols; ++j)
    {
        int parcel = indexToParcel[j];
        CaretAssert(parcel > -2 && parcel < numParcels);
        if (parcel != -1)
        {
            ++parcelCounts[parcel];
        }
    }
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> scratchOutRow(numParcels);
        for (int64_t i = 0; i < numRows; ++i)
        {
            vector<double> scratchAccum(numParcels, 0.0);
            myCiftiIn->getRow(scratchRow.data(), i);
            for (int64_t j = 0; j < numCols; ++j)
            {
                int parcel = indexToParcel[j];
                if (parcel != -1)
                {
                    scratchAccum[parcel] += scratchRow[j];
                }
            }
            for (int j = 0; j < numParcels; ++j)
            {
                if (parcelCounts[j] > 0)
                {
                    scratchOutRow[j] = scratchAccum[j] / parcelCounts[j];
                } else {
                    scratchOutRow[j] = 0.0f;
                }
            }
            myCiftiOut->setRow(scratchOutRow.data(), i);
        }
    } else if (direction == CiftiXML::ALONG_COLUMN) {
        vector<vector<double> > accumRows(numParcels, vector<double>(numCols, 0.0f));
        for (int64_t i = 0; i < numRows; ++i)
        {
            int parcel = indexToParcel[i];
            if (parcel != -1)
            {
                myCiftiIn->getRow(scratchRow.data(), i);
                vector<double>& parcelRowRef = accumRows[parcel];
                for (int64_t j = 0; j < numCols; ++j)
                {
                    parcelRowRef[j] += scratchRow[j];
                }
            }
        }
        vector<float> scratchOutRow(numCols);
        for (int i = 0; i < numParcels; ++i)
        {
            int64_t count = parcelCounts[i];
            if (count > 0)
            {
                vector<double>& parcelRowRef = accumRows[i];
                for (int64_t j = 0; j < numCols; ++j)
                {
                    scratchOutRow[j] = parcelRowRef[j] / count;
                }
            } else {
                for (int64_t j = 0; j < numCols; ++j)
                {
                    scratchOutRow[j] = 0.0f;
                }
            }
            myCiftiOut->setRow(scratchOutRow.data(), i);
        }
    } else {
        throw AlgorithmException("AlgorithmCiftiParcellate doesn't support this direction");
    }
}

void AlgorithmCiftiParcellate::parcellateMapping(const CiftiFile* myCiftiLabel, const int& direction, CiftiXML& myOutXML, int& numParcels, vector<int>& indexToParcel)
{
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    const GiftiLabelTable* myLabelTable = myLabelXML.getLabelTableForRowIndex(0);
    vector<float> labelData(myLabelXML.getNumberOfRows());
    int unusedKey = myLabelTable->getUnassignedLabelKey();
    myCiftiLabel->getColumn(labelData.data(), 0);
    CiftiXML newOutXML = myOutXML;
    map<int, pair<CiftiParcelElement, int> > usedKeys;//the keys from the label table that actually overlap with data in the input file
    vector<StructureEnum::Enum> surfList, volList;
    indexToParcel.clear();
    if (direction == CiftiXML::ALONG_ROW)
    {
        indexToParcel.resize(myOutXML.getNumberOfColumns(), -1);
        newOutXML.resetRowsToParcels();
        myOutXML.getStructureListsForRows(surfList, volList);
    } else if (direction == CiftiXML::ALONG_COLUMN) {
        indexToParcel.resize(myOutXML.getNumberOfRows(), -1);
        newOutXML.resetColumnsToParcels();
        myOutXML.getStructureListsForColumns(surfList, volList);
    } else {
        throw AlgorithmException("AlgorithmCiftiParcellate doesn't support this direction");
    }
    for (int i = 0; i < (int)surfList.size(); ++i)
    {
        StructureEnum::Enum myStruct = surfList[i];
        if (myLabelXML.hasSurfaceData(CiftiXML::ALONG_COLUMN, myStruct) && myOutXML.hasSurfaceData(direction, myStruct))
        {
            if (myOutXML.getSurfaceNumberOfNodes(direction, myStruct) != myLabelXML.getSurfaceNumberOfNodes(CiftiXML::ALONG_COLUMN, myStruct))
            {
                throw AlgorithmException("mismatch in number of surface vertices between input and label for structure " + StructureEnum::toName(myStruct));
            }
            newOutXML.addParcelSurface(direction, myOutXML.getSurfaceNumberOfNodes(direction, myStruct), myStruct);
            vector<CiftiSurfaceMap> surfMap;
            myOutXML.getSurfaceMap(direction, surfMap, myStruct);
            int64_t mapSize = (int64_t)surfMap.size();
            for (int64_t j = 0; j < mapSize; ++j)
            {
                int64_t labelIndex = myLabelXML.getRowIndexForNode(surfMap[j].m_surfaceNode, myStruct);//because "row index" in this case means "along column" - we must fix this
                if (labelIndex != -1)
                {
                    int labelKey = (int)floor(labelData[labelIndex] + 0.5f);
                    if (labelKey != unusedKey)
                    {
                        int tempVal;
                        map<int, pair<CiftiParcelElement, int> >::iterator iter = usedKeys.find(labelKey);
                        if (iter == usedKeys.end())
                        {
                            const GiftiLabel* myLabel = myLabelTable->getLabel(labelKey);
                            if (myLabel != NULL)//ignore values that aren't in the label table
                            {
                                tempVal = usedKeys.size();
                                CiftiParcelElement tempParcel;
                                tempParcel.m_parcelName = myLabel->getName();
                                tempParcel.m_nodeElements.resize(1);
                                tempParcel.m_nodeElements[0].m_structure = myStruct;
                                tempParcel.m_nodeElements[0].m_nodes.push_back(surfMap[j].m_surfaceNode);
                                usedKeys[labelKey] = pair<CiftiParcelElement, int>(tempParcel, tempVal);
                            }
                        } else {
                            tempVal = iter->second.second;
                            CiftiParcelElement& tempParcel = iter->second.first;
                            int loc = -1;
                            for (int k = 0; k < (int)tempParcel.m_nodeElements.size(); ++k)
                            {
                                if (tempParcel.m_nodeElements[k].m_structure == myStruct)
                                {
                                    loc = k;
                                    break;
                                }
                            }
                            if (loc == -1)
                            {
                                CiftiParcelNodesElement tempNodeElem;
                                tempNodeElem.m_structure = myStruct;
                                tempNodeElem.m_nodes.push_back(surfMap[j].m_surfaceNode);
                                tempParcel.m_nodeElements.push_back(tempNodeElem);
                            } else {
                                tempParcel.m_nodeElements[loc].m_nodes.push_back(surfMap[j].m_surfaceNode);
                            }
                        }
                        indexToParcel[surfMap[j].m_ciftiIndex] = tempVal;//we will remap these to be in order of label keys later
                    }
                }
            }
        }
    }
    vector<CiftiVolumeMap> volMap;
    myOutXML.getVolumeMap(direction, volMap);
    int64_t mapSize = (int64_t)volMap.size();
    for (int64_t i = 0; i < mapSize; ++i)
    {
        int64_t labelIndex = myLabelXML.getRowIndexForVoxel(volMap[i].m_ijk);
        if (labelIndex != -1)
        {
            int labelKey = (int)floor(labelData[labelIndex] + 0.5f);
            if (labelKey != unusedKey)
            {
                int tempVal;
                map<int, pair<CiftiParcelElement, int> >::iterator iter = usedKeys.find(labelKey);
                if (iter == usedKeys.end())
                {
                    const GiftiLabel* myLabel = myLabelTable->getLabel(labelKey);
                    if (myLabel != NULL)//ignore values that aren't in the label table
                    {
                        tempVal = usedKeys.size();
                        CiftiParcelElement tempParcel;
                        tempParcel.m_parcelName = myLabel->getName();
                        tempParcel.m_voxelIndicesIJK.resize(3);
                        tempParcel.m_voxelIndicesIJK[0] = volMap[i].m_ijk[0];
                        tempParcel.m_voxelIndicesIJK[1] = volMap[i].m_ijk[1];
                        tempParcel.m_voxelIndicesIJK[2] = volMap[i].m_ijk[2];
                        usedKeys[labelKey] = pair<CiftiParcelElement, int>(tempParcel, tempVal);
                    }
                } else {
                    tempVal = iter->second.second;
                    CiftiParcelElement& tempParcel = iter->second.first;
                    tempParcel.m_voxelIndicesIJK.push_back(volMap[i].m_ijk[0]);
                    tempParcel.m_voxelIndicesIJK.push_back(volMap[i].m_ijk[1]);
                    tempParcel.m_voxelIndicesIJK.push_back(volMap[i].m_ijk[2]);
                }
                indexToParcel[volMap[i].m_ciftiIndex] = tempVal;//we will remap these to be in order of label keys later
            }
        }
    }
    numParcels = (int)usedKeys.size();
    vector<int> valRemap(numParcels, -1);
    int count = 0;
    for (map<int, pair<CiftiParcelElement, int> >::const_iterator iter = usedKeys.begin(); iter != usedKeys.end(); ++iter)
    {
        valRemap[iter->second.second] = count;//build a lookup from temp values to label key rank
        newOutXML.addParcel(direction, iter->second.first);
        ++count;
    }
    int64_t lookupSize = (int64_t)indexToParcel.size();
    for (int64_t i = 0; i < lookupSize; ++i)//finally, remap the temporary values to the key order of the labels
    {
        if (indexToParcel[i] != -1)
        {
            indexToParcel[i] = valRemap[indexToParcel[i]];
        }
    }
    myOutXML = newOutXML;
}

float AlgorithmCiftiParcellate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiParcellate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
