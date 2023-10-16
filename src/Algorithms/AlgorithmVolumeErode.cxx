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

#include "AlgorithmVolumeErode.h"
#include "AlgorithmException.h"

#include "CaretPointLocator.h"
#include "VolumeFile.h"

#include <algorithm>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeErode::getCommandSwitch()
{
    return "-volume-erode";
}

AString AlgorithmVolumeErode::getShortDescription()
{
    return "ERODE A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeErode::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume", "the volume to erode");
    
    ret->addDoubleParameter(2, "distance", "distance in mm to erode");
    
    ret->addVolumeOutputParameter(3, "volume-out", "the output volume");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(4, "-roi", "assume voxels outside this roi are nonzero");
    roiOpt->addVolumeParameter(1, "roi-volume", "volume file, positive values denote voxels that have data");
    
    OptionalParameter* subvolOpt = ret->createOptionalParameter(5, "-subvolume", "select a single subvolume to dilate");
    subvolOpt->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Around each voxel with a value of zero, set surrounding voxels to zero.  ") +
        "The surrounding voxels are all face neighbors and all voxels within the specified distance (center to center)."
    );
    return ret;
}

void AlgorithmVolumeErode::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* volIn = myParams->getVolume(1);
    float distance = (float)myParams->getDouble(2);
    VolumeFile* volOut = myParams->getOutputVolume(3);
    VolumeFile* roiVol = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(4);
    if (roiOpt->m_present)
    {
        roiVol = roiOpt->getVolume(1);
    }
    int subvol = -1;
    OptionalParameter* subvolOpt = myParams->getOptionalParameter(5);
    if (subvolOpt->m_present)
    {
        subvol = volIn->getMapIndexFromNameOrNumber(subvolOpt->getString(1));
        if (subvol < 0) throw AlgorithmException("invalid subvolume specified");
    }
    AlgorithmVolumeErode(myProgObj, volIn, distance, volOut, roiVol, subvol);
}

namespace
{
    void erodeFrame(const VolumeFile* volIn, const int& inFrame, const int& component, const float& distance, VolumeFile* volOut, const int& outFrame, const VolumeFile* roiVol)
    {
        int neighbors[] = { 0, 0, -1,
                            0, -1, 0,
                            -1, 0, 0,
                            1, 0, 0,
                            0, 1, 0,
                            0, 0, 1 };
        Vector3D ivec, jvec, kvec, origin;
        volIn->getVolumeSpace().getSpacingVectors(ivec, jvec, kvec, origin);
        float minSpacing = min(min(ivec.length(), jvec.length()), kvec.length());
        bool checkNeighbors = (minSpacing > distance);
        vector<int64_t> myDims;
        volIn->getDimensions(myDims);
        const float* inData = volIn->getFrame(inFrame, component), *roiData = NULL;
        float emptyVal = 0.0f;
        bool labelData = false;
        if (volIn->getType() == SubvolumeAttributes::LABEL)
        {
            emptyVal = volIn->getMapLabelTable(inFrame)->getUnassignedLabelKey();
            labelData = true;
        }
        if (roiVol != NULL) roiData = roiVol->getFrame();
        vector<float> scratchFrame(inData, inData + myDims[0] * myDims[1] * myDims[2]);//start with a copy, then zero what we don't need
        vector<float> coordList;
        for (int64_t k = 0; k < myDims[2]; ++k)
        {
            for (int64_t j = 0; j < myDims[1]; ++j)
            {
                for (int64_t i = 0; i < myDims[0]; ++i)
                {
                    int64_t flatIndex = volIn->getIndex(i, j, k);
                    if (roiData == NULL || roiData[flatIndex] > 0.0f)
                    {
                        if (labelData)
                        {
                            if (floor(inData[flatIndex] + 0.5f) == emptyVal)
                            {
                                float coord[3];
                                volIn->indexToSpace(i, j, k, coord);
                                coordList.insert(coordList.end(), coord, coord + 3);
                            }
                        } else {
                            if (inData[flatIndex] == 0.0f)
                            {
                                float coord[3];
                                volIn->indexToSpace(i, j, k, coord);
                                coordList.insert(coordList.end(), coord, coord + 3);
                            }
                        }
                    }
                }
            }
        }
        CaretPointLocator myLocator(coordList.data(), coordList.size() / 3);
        for (int64_t k = 0; k < myDims[2]; ++k)
        {
            for (int64_t j = 0; j < myDims[1]; ++j)
            {
                for (int64_t i = 0; i < myDims[0]; ++i)
                {
                    float coord[3];
                    volIn->indexToSpace(i, j, k, coord);
                    if (myLocator.anyInRange(coord, distance))
                    {
                        scratchFrame[volIn->getIndex(i, j, k)] = emptyVal;//this is used for both label and normal data, use the variable
                    } else if (checkNeighbors) {
                        for (int neigh = 0; neigh < 18; neigh += 3)
                        {
                            if (volIn->indexValid(i + neighbors[neigh], j + neighbors[neigh + 1], k + neighbors[neigh + 2]))
                            {
                                int64_t flatIndex = volIn->getIndex(i + neighbors[neigh], j + neighbors[neigh + 1], k + neighbors[neigh + 2]);
                                if (roiData == NULL || roiData[flatIndex] > 0.0f)
                                {
                                    if (labelData)
                                    {
                                        if (floor(inData[flatIndex] + 0.5f) == emptyVal)
                                        {
                                            scratchFrame[volIn->getIndex(i, j, k)] = emptyVal;
                                        }
                                    } else {
                                        if (inData[flatIndex] == 0.0f)
                                        {
                                            scratchFrame[volIn->getIndex(i, j, k)] = 0.0f;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        volOut->setFrame(scratchFrame.data(), outFrame, component);
    }
}

AlgorithmVolumeErode::AlgorithmVolumeErode(ProgressObject* myProgObj, const VolumeFile* volIn, const float& distance, VolumeFile* volOut, VolumeFile* roiVol, const int& subvol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> myDims;
    volIn->getDimensions(myDims);
    if (subvol < -1 || subvol >= myDims[3])
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (distance < 0.0f)
    {
        throw AlgorithmException("distance cannot be negative");
    }
    if (roiVol != NULL && !volIn->matchesVolumeSpace(roiVol))
    {
        throw AlgorithmException("roi volume space does not match input volume");
    }
    if (subvol == -1)
    {
        volOut->reinitialize(volIn->getOriginalDimensions(), volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType(), volIn->m_header);
        for (int i = 0; i < myDims[3]; ++i)
        {
            if (volIn->getType() == SubvolumeAttributes::LABEL)
            {
                *(volOut->getMapLabelTable(i)) = *(volIn->getMapLabelTable(i));
            } else {
                *(volOut->getMapPaletteColorMapping(i)) = *(volIn->getMapPaletteColorMapping(i));
            }
            volOut->setMapName(i, volIn->getMapName(i) + " erode " + AString::number(distance));
        }
        for (int s = 0; s < myDims[3]; ++s)
        {
            for (int c = 0; c < myDims[4]; ++c)
            {
                erodeFrame(volIn, s, c, distance, volOut, s, roiVol);
            }
        }
    } else {
        vector<int64_t> outDims = myDims;
        outDims.resize(3);
        volOut->reinitialize(outDims, volIn->getSform(), volIn->getNumberOfComponents(), volIn->getType(), volIn->m_header);
        if (volIn->getType() == SubvolumeAttributes::LABEL)
        {
            *(volOut->getMapLabelTable(0)) = *(volIn->getMapLabelTable(subvol));
        } else {
            *(volOut->getMapPaletteColorMapping(0)) = *(volIn->getMapPaletteColorMapping(subvol));
        }
        volOut->setMapName(0, volIn->getMapName(subvol) + " erode " + AString::number(distance));
        for (int c = 0; c < myDims[4]; ++c)
        {
            erodeFrame(volIn, subvol, c, distance, volOut, 0, roiVol);
        }
    }
}

float AlgorithmVolumeErode::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeErode::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
