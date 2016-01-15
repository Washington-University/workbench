/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "AlgorithmCiftiMergeParcels.h"
#include "AlgorithmException.h"

#include "CiftiFile.h"
#include "GiftiLabelTable.h"
#include "MultiDimIterator.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiMergeParcels::getCommandSwitch()
{
    return "-cifti-merge-parcels";
}

AString AlgorithmCiftiMergeParcels::getShortDescription()
{
    return "MERGE CIFTI FILES ALONG PARCELS DIMENSION";
}

OperationParameters* AlgorithmCiftiMergeParcels::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "direction", "which dimension to merge along (integer, 'ROW', or 'COLUMN')");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(3, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to merge");
    
    ret->setHelpText(
        AString("The input cifti files must have matching mappings along the direction not specified, and the mapping along the specified direction must be parcels.  ") +
        CiftiXML::directionFromStringExplanation()
    );
    return ret;
}

void AlgorithmCiftiMergeParcels::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString directionName = myParams->getString(1);
    int myDir = CiftiXML::directionFromString(directionName);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    const vector<ParameterComponent*>& myInstances = *(myParams->getRepeatableParameterInstances(3));
    vector<const CiftiFile*> ciftiList;
    int numCifti = (int)myInstances.size();
    for (int i = 0; i < numCifti; ++i)
    {
        ciftiList.push_back(myInstances[i]->getCifti(1));
    }
    AlgorithmCiftiMergeParcels(myProgObj, myDir, ciftiList, myCiftiOut);
}

AlgorithmCiftiMergeParcels::AlgorithmCiftiMergeParcels(ProgressObject* myProgObj, const int& myDir, const vector<const CiftiFile*>& ciftiList, CiftiFile* myCiftiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(myDir >= 0);
    int listSize = (int)ciftiList.size();
    if (listSize < 1) throw AlgorithmException("no input files specified");
    CaretAssert(ciftiList[0] != NULL);
    const CiftiXML& firstXML = ciftiList[0]->getCiftiXML();
    CiftiXML outXML = firstXML;
    int numDims = -1;
    int labelDim = -1;
    vector<vector<map<int32_t, int32_t> > > remaps(listSize);
    for (int i = 0; i < listSize; ++i)
    {
        CaretAssert(ciftiList[i] != NULL);
        const CiftiXML& thisXML = ciftiList[i]->getCiftiXML();
        if (myDir >= thisXML.getNumberOfDimensions())
        {
            throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' does not have the specified dimension");
        }
        if (thisXML.getMappingType(myDir) != CiftiMappingType::PARCELS)
        {
            throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' does not have a parcels mapping on the specified dimension");
        }
        if (i != 0)
        {
            CiftiParcelsMap& outParcelMap = outXML.getParcelsMap(myDir);
            const CiftiParcelsMap& thisParcelMap = thisXML.getParcelsMap(myDir);
            if (thisParcelMap.hasVolumeData())
            {
                if (outParcelMap.hasVolumeData())//if there is no volume data yet, we don't care about the volume space, if it exists
                {
                    if (!outParcelMap.getVolumeSpace().matches(thisParcelMap.getVolumeSpace()))
                    {
                        throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' does not match the volume space of earlier input files");
                    }
                } else {
                    outParcelMap.setVolumeSpace(thisParcelMap.getVolumeSpace());
                }
            }
            vector<StructureEnum::Enum> thisSurfs = thisParcelMap.getParcelSurfaceStructures();
            for (int i = 0; i < (int)thisSurfs.size(); ++i)
            {
                if (outParcelMap.hasSurface(thisSurfs[i]))
                {
                    if (thisParcelMap.getSurfaceNumberOfNodes(thisSurfs[i]) != outParcelMap.getSurfaceNumberOfNodes(thisSurfs[i]))
                    {
                        throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' has different number of vertices than earlier input files for surface " +
                                                 StructureEnum::toName(thisSurfs[i]));
                    }
                } else {
                    outParcelMap.addSurface(thisParcelMap.getSurfaceNumberOfNodes(thisSurfs[i]), thisSurfs[i]);
                }
            }
            const vector<CiftiParcelsMap::Parcel>& thisParcels = thisParcelMap.getParcels();
            for (int64_t p = 0; p < (int64_t)thisParcels.size(); ++p)
            {
                outParcelMap.addParcel(thisParcels[p]);//does the error checking for overlap, unique name
            }
        }
        int thisNumDims = thisXML.getNumberOfDimensions();
        if (i == 0)
        {
            numDims = thisNumDims;
        } else {
            if (thisNumDims != numDims)
            {
                throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' has " + AString::number(thisNumDims) +
                " dimensions, while file '" + ciftiList[0]->getFileName() + "' has " + AString::number(numDims) + " dimensions");
            }
            AString explanation;
            for (int d = 0; d < thisNumDims; ++d)
            {
                if (d != myDir)
                {
                    if (!(firstXML.getMap(d)->approximateMatch(*(thisXML.getMap(d)), &explanation)))//expect transitive property in approximateMatch, so only check against first file
                    {
                        throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' is incompatible with file '" +
                            ciftiList[0]->getFileName() + "' on dimension " + AString::number(d + 1) + ": " + explanation);
                    }
                    if (thisXML.getMappingType(d) == CiftiMappingType::LABELS || firstXML.getMappingType(d) == CiftiMappingType::LABELS)
                    {//NOTE: relies on CiftiXML not allowing more than one label dimension
                        if (firstXML.getMappingType(d) != CiftiMappingType::LABELS)
                        {
                            throw AlgorithmException("input cifti file '" + ciftiList[i]->getFileName() + "' has a labels mapping on dimension " + AString::number(d + 1) +
                                ", but file '" + ciftiList[0]->getFileName() + "' does not");
                        }
                        if (thisXML.getMappingType(d) != CiftiMappingType::LABELS)
                        {
                            throw AlgorithmException("input cifti file '" + ciftiList[0]->getFileName() + "' has a labels mapping on dimension " + AString::number(d + 1) +
                                ", but file '" + ciftiList[i]->getFileName() + "' does not");
                        }
                        labelDim = d;
                        if (i != 0)//because we used the first file's XML as a template
                        {
                            CiftiLabelsMap& outLabelsMap = outXML.getLabelsMap(d);
                            int64_t numMaps = outLabelsMap.getLength();
                            remaps[d].resize(numMaps);
                            for (int64_t m = 0; m < numMaps; ++m)
                            {
                                remaps[d][m] = outLabelsMap.getMapLabelTable(m)->append(*(thisXML.getLabelsMap(d).getMapLabelTable(m)));
                            }
                        }
                    }
                }
            }
        }
    }
    myCiftiOut->setCiftiXML(outXML);
    vector<int64_t> outDims = outXML.getDimensions();
    vector<int32_t> unlabeledKeys;
    if (labelDim != -1)
    {
        unlabeledKeys.resize(outDims[labelDim]);
        const CiftiLabelsMap& outLabelsMap = outXML.getLabelsMap(labelDim);
        for (int64_t i = 0; i < (int64_t)unlabeledKeys.size(); ++i)
        {
            unlabeledKeys[i] = outLabelsMap.getMapLabelTable(i)->getUnassignedLabelKey();
        }
    }
    if (myDir == CiftiXML::ALONG_ROW)
    {//because unit of input/output is a row
        vector<float> rowOut(outXML.getDimensionLength(CiftiXML::ALONG_ROW)), rowIn;//adapt input row size on use, in case we add options to select parcels from files
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
        {//the + 1 is to exclude the row dimension
            int64_t startIndex = 0;
            for (int i = 0; i < listSize; ++i)
            {
                int64_t inRowLength = ciftiList[i]->getCiftiXML().getDimensionLength(CiftiXML::ALONG_ROW);
                if (inRowLength > (int64_t)rowIn.size())
                {
                    rowIn.resize(inRowLength);
                }
                ciftiList[i]->getRow(rowIn.data(), *iter);
                if (labelDim != -1)
                {
                    int64_t labelMap = (*iter)[labelDim - 1];
                    for (int64_t j = 0; j < inRowLength; ++j)
                    {
                        int32_t key = (int32_t)floor(0.5f + rowIn[j]);
                        if (i == 0)//because we used the first XML as a template
                        {
                            rowOut[startIndex + j] = key;
                        } else {
                            map<int32_t, int32_t>::iterator iter = remaps[i][labelMap].find(key);
                            if (iter == remaps[i][labelMap].end())//NOTE: GiftiLabelTable::append creates an entry for every key in the original table, even if it maps to the same value
                            {
                                rowOut[startIndex + j] = unlabeledKeys[labelMap];
                            } else {
                                rowOut[startIndex + j] = iter->second;
                            }
                        }
                    }
                } else {
                    for (int64_t j = 0; j < inRowLength; ++j)
                    {
                        rowOut[startIndex + j] = rowIn[j];
                    }
                }
                startIndex += inRowLength;
            }
            myCiftiOut->setRow(rowOut.data(), *iter);
        }
    } else {//now the fun begins...
        int64_t rowLength = outXML.getDimensionLength(CiftiXML::ALONG_ROW);
        vector<float> rowOut(rowLength), rowIn(rowLength);//input rows must be the same length if not row merging
        vector<int64_t> indexvec(outDims.size() - 1, -1);
        for (MultiDimIterator<int64_t> outeriter(vector<int64_t>(outDims.begin() + myDir + 1, outDims.end())); !outeriter.atEnd(); ++outeriter)
        {//split matching cifti dimensions by greater or less than myDir, aim for sequential output
            vector<int64_t> outerindexvec = *outeriter;
            for (int i = myDir + 1; i < (int)outDims.size(); ++i)
            {
                indexvec[i - 1] = outerindexvec[i - myDir - 1];//indexvec is missing the row dimension, outerindexvec starts one after myDir
            }
            int64_t startIndex = 0;
            for (int i = 0; i < listSize; ++i)
            {//for mydir, loop over files, and then parcels within file
                int64_t thisNumParcels = ciftiList[i]->getCiftiXML().getDimensionLength(myDir);
                for (int64_t j = 0; j < thisNumParcels; ++j)
                {
                    for (MultiDimIterator<int64_t> inneriter(vector<int64_t>(outDims.begin() + 1, outDims.begin() + myDir)); !inneriter.atEnd(); ++inneriter)
                    {//then loop over dimensions less than mydir, excluding row
                        vector<int64_t> innerindices = *inneriter;
                        for (int k = 1; k < myDir; ++k)
                        {
                            indexvec[k - 1] = innerindices[k - 1];
                        }
                        indexvec[myDir - 1] = j;//for input
                        ciftiList[i]->getRow(rowIn.data(), indexvec);
                        if (labelDim != -1)
                        {//we COULD check if labelDim is before or after myDim, and move some of the following to an outer loop, but it really doesn't seem worth it
                            for (int k = 0; k < rowLength; ++k)
                            {
                                int32_t key = (int32_t)floor(0.5f + rowIn[k]);
                                if (i == 0)//we used the first XML as a template, there is no remap
                                {
                                    rowOut[k] = key;
                                } else {
                                    int64_t labelMap = -1;
                                    if (labelDim == 0)//note that labelDim will never be myDir, so indexvec doesn't need to be adjusted
                                    {
                                        labelMap = k;
                                    } else {
                                        labelMap = indexvec[labelDim - 1];
                                    }
                                    map<int32_t, int32_t>::iterator iter = remaps[i][labelMap].find(key);
                                    if (iter == remaps[i][labelMap].end())//NOTE: GiftiLabelTable::append creates an entry for every key in the original table, even if it maps to the same value
                                    {
                                        rowOut[k] = unlabeledKeys[labelMap];
                                    } else {
                                        rowOut[k] = iter->second;
                                    }
                                }
                            }
                            indexvec[myDir - 1] = startIndex + j;//for output
                            myCiftiOut->setRow(rowOut.data(), indexvec);
                        } else {
                            indexvec[myDir - 1] = startIndex + j;//for output
                            myCiftiOut->setRow(rowIn.data(), indexvec);
                        }
                    }
                }
                startIndex += thisNumParcels;
            }
        }
    }
}

float AlgorithmCiftiMergeParcels::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiMergeParcels::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
