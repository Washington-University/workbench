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

#include "OperationEstimateFiberBinghams.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "MathFunctions.h"
#include "StructureEnum.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cmath>
#include <vector>

using namespace caret;
using namespace std;

AString OperationEstimateFiberBinghams::getCommandSwitch()
{
    return "-estimate-fiber-binghams";
}

AString OperationEstimateFiberBinghams::getShortDescription()
{
    return "ESTIMATE FIBER ORIENTATION DISTRIBUTIONS FROM BEDPOSTX SAMPLES";
}

OperationParameters* OperationEstimateFiberBinghams::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "merged_f1samples", "fiber 1 strength samples");
    ret->addVolumeParameter(2, "merged_th1samples", "fiber 1 theta samples");
    ret->addVolumeParameter(3, "merged_ph1samples", "fiber 1 phi samples");
    ret->addVolumeParameter(4, "merged_f2samples", "fiber 2 strength samples");
    ret->addVolumeParameter(5, "merged_th2samples", "fiber 2 theta samples");
    ret->addVolumeParameter(6, "merged_ph2samples", "fiber 2 phi samples");
    ret->addVolumeParameter(7, "merged_f3samples", "fiber 3 strength samples");
    ret->addVolumeParameter(8, "merged_th3samples", "fiber 3 theta samples");
    ret->addVolumeParameter(9, "merged_ph3samples", "fiber 3 phi samples");
    ret->addVolumeParameter(10, "label-volume", "volume of cifti structure labels");
    ret->addCiftiOutputParameter(11, "cifti-out", "output cifti fiber distributons file");
    AString myText = AString("This command does an estimation of a bingham distribution for each fiber orientation in each voxel which is ") +
        "labeled a structure identifier.  These labelings come from the <label-volume> argument, which must have labels that match the following strings:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationEstimateFiberBinghams::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const VolumeFile* f1_samples = myParams->getVolume(1);
    const VolumeFile* th1_samples = myParams->getVolume(2);
    const VolumeFile* ph1_samples = myParams->getVolume(3);
    const VolumeFile* f2_samples = myParams->getVolume(4);
    const VolumeFile* th2_samples = myParams->getVolume(5);
    const VolumeFile* ph2_samples = myParams->getVolume(6);
    const VolumeFile* f3_samples = myParams->getVolume(7);
    const VolumeFile* th3_samples = myParams->getVolume(8);
    const VolumeFile* ph3_samples = myParams->getVolume(9);
    const VolumeFile* myVolLabel = myParams->getVolume(10);
    if (myVolLabel->getType() != SubvolumeAttributes::LABEL)
    {
        throw OperationException("<label-volume> must have a label table, see -volume-label-import");
    }
    if (!(myVolLabel->matchesVolumeSpace(f1_samples) &&
        myVolLabel->matchesVolumeSpace(th1_samples) &&
        myVolLabel->matchesVolumeSpace(ph1_samples) &&
        myVolLabel->matchesVolumeSpace(f2_samples) &&
        myVolLabel->matchesVolumeSpace(th2_samples) &&
        myVolLabel->matchesVolumeSpace(ph2_samples) &&
        myVolLabel->matchesVolumeSpace(f3_samples) &&
        myVolLabel->matchesVolumeSpace(th3_samples) &&
        myVolLabel->matchesVolumeSpace(ph3_samples)))
    {
        throw OperationException("all inputs must be in the same volume space");
    }
    int64_t junk, numfsamp, numthsamp, numphsamp;
    f1_samples->getDimensions(junk, junk, junk, numfsamp, junk);
    th1_samples->getDimensions(junk, junk, junk, numthsamp, junk);
    ph1_samples->getDimensions(junk, junk, junk, numphsamp, junk);
    if (numfsamp != numthsamp || numfsamp != numphsamp)
    {
        throw OperationException("fiber 1 volumes have different numbers of samples");
    }
    f2_samples->getDimensions(junk, junk, junk, numfsamp, junk);
    th2_samples->getDimensions(junk, junk, junk, numthsamp, junk);
    ph2_samples->getDimensions(junk, junk, junk, numphsamp, junk);
    if (numfsamp != numthsamp || numfsamp != numphsamp)
    {
        throw OperationException("fiber 2 volumes have different numbers of samples");
    }
    f3_samples->getDimensions(junk, junk, junk, numfsamp, junk);
    th3_samples->getDimensions(junk, junk, junk, numthsamp, junk);
    ph3_samples->getDimensions(junk, junk, junk, numphsamp, junk);
    if (numfsamp != numthsamp || numfsamp != numphsamp)
    {
        throw OperationException("fiber 3 volumes have different numbers of samples");
    }
    map<int, StructureEnum::Enum> labelMap;//maps label values to structures
    vector<vector<voxelIndexType> > voxelLists;//voxel lists for each volume component
    map<StructureEnum::Enum, int> componentMap;//maps structures to indexes in voxelLists
    const GiftiLabelTable* myLabelTable = myVolLabel->getMapLabelTable(0);
    vector<int32_t> labelKeys;
    myLabelTable->getKeys(labelKeys);
    int count = 0;
    for (int i = 0; i < (int)labelKeys.size(); ++i)
    {
        bool ok = false;
        StructureEnum::Enum thisStructure = StructureEnum::fromName(myLabelTable->getLabelName(labelKeys[i]), &ok);
        if (ok)
        {
            if (componentMap.find(thisStructure) == componentMap.end())//make sure we don't already have this structure from another label
            {
                labelMap[labelKeys[i]] = thisStructure;
                componentMap[thisStructure] = count;
                ++count;
            }
        }
    }
    voxelLists.resize(count);
    vector<int64_t> mydims;
    myVolLabel->getDimensions(mydims);
    for (int64_t k = 0; k < mydims[2]; ++k)
    {
        for (int64_t j = 0; j < mydims[1]; ++j)
        {
            for (int64_t i = 0; i < mydims[0]; ++i)
            {
                int myval = (int)floor(myVolLabel->getValue(i, j, k) + 0.5f);
                map<int, StructureEnum::Enum>::iterator myiter = labelMap.find(myval);
                if (myiter != labelMap.end())
                {
                    int whichList = componentMap.find(myiter->second)->second;//this should always find one, so we don't need to check for being end
                    voxelLists[whichList].push_back(i);
                    voxelLists[whichList].push_back(j);
                    voxelLists[whichList].push_back(k);
                }
            }
        }
    }
    int64_t ciftiVolDims[3];
    ciftiVolDims[0] = mydims[0];
    ciftiVolDims[1] = mydims[1];
    ciftiVolDims[2] = mydims[2];
    CiftiXMLOld myXML;
    myXML.resetColumnsToBrainModels();
    myXML.setVolumeDimsAndSForm(ciftiVolDims, myVolLabel->getSform());
    for (map<StructureEnum::Enum, int>::iterator myiter = componentMap.begin(); myiter != componentMap.end(); ++myiter)
    {
        myXML.addVolumeModelToColumns(voxelLists[myiter->second], myiter->first);
    }
    myXML.resetRowsToScalars(24);
    myXML.setMapNameForRowIndex(0, "x coord");
    myXML.setMapNameForRowIndex(1, "y coord");
    myXML.setMapNameForRowIndex(2, "z coord");
    myXML.setMapNameForRowIndex(3, "mean f1");
    myXML.setMapNameForRowIndex(4, "stdev f1");
    myXML.setMapNameForRowIndex(5, "theta1");
    myXML.setMapNameForRowIndex(6, "phi1");
    myXML.setMapNameForRowIndex(7, "ka1");
    myXML.setMapNameForRowIndex(8, "kb1");
    myXML.setMapNameForRowIndex(9, "psi1");
    myXML.setMapNameForRowIndex(10, "mean f2");
    myXML.setMapNameForRowIndex(11, "stdev f2");
    myXML.setMapNameForRowIndex(12, "theta2");
    myXML.setMapNameForRowIndex(13, "phi2");
    myXML.setMapNameForRowIndex(14, "ka2");
    myXML.setMapNameForRowIndex(15, "kb2");
    myXML.setMapNameForRowIndex(16, "psi2");
    myXML.setMapNameForRowIndex(17, "mean f3");
    myXML.setMapNameForRowIndex(18, "stdev f3");
    myXML.setMapNameForRowIndex(19, "theta3");
    myXML.setMapNameForRowIndex(20, "phi3");
    myXML.setMapNameForRowIndex(21, "ka3");
    myXML.setMapNameForRowIndex(22, "kb3");
    myXML.setMapNameForRowIndex(23, "psi3");
    CiftiFile* myCifti = myParams->getOutputCifti(11);
    myCifti->setCiftiXML(myXML);
    vector<CiftiVolumeMap> volMap;
    CaretArray<float> temprow(24);
    myXML.getVolumeMapForColumns(volMap);//we don't need to know which voxel is from which parcel
    int64_t end = (int64_t)volMap.size();
    for (int64_t i = 0; i < end; ++i)
    {
        myVolLabel->indexToSpace(volMap[i].m_ijk, temprow);//first three elements are the coordinates
        estimateBingham(temprow.getArray() + 3, volMap[i].m_ijk, f1_samples, th1_samples, ph1_samples);
        estimateBingham(temprow.getArray() + 10, volMap[i].m_ijk, f2_samples, th2_samples, ph2_samples);
        estimateBingham(temprow.getArray() + 17, volMap[i].m_ijk, f3_samples, th3_samples, ph3_samples);
        myCifti->setRow(temprow, volMap[i].m_ciftiIndex);
    }
}

void OperationEstimateFiberBinghams::estimateBingham(float* binghamOut, const int64_t ijk[3], const VolumeFile* f_samples, const VolumeFile* theta_samples, const VolumeFile* phi_samples)
{
    vector<int64_t> fdims;
    f_samples->getDimensions(fdims);
    vector<Vector3D> samples(fdims[3]);
    vector<float> fsamplearray(fdims[3]);//to do two-pass mean/stdev without using the VolumeFile function twice per sample
    double accum = 0.0;
    Vector3D accumvec;//initializes to the zero vector
    for (int i = 0; i < fdims[3]; ++i)
    {
        fsamplearray[i] = f_samples->getValue(ijk, i);
        accum += fsamplearray[i];
        float theta = theta_samples->getValue(ijk, i);//these have already been checked to have the same number of bricks
        float phi = phi_samples->getValue(ijk, i);
        samples[i][0] = -sin(theta) * cos(phi);//NOTE: theta, phi are polar coordinates for a RADIOLOGICAL volume, so flip x so that +x = right
        samples[i][1] = sin(theta) * sin(phi);//ignore the f values for directionality testing
        samples[i][2] = cos(theta);//beware, samples may be the negative of other samples, BAS model doesn't care, and they may have restricted the samples to +z or something
        if (i != 0)
        {
            if (accumvec.dot(samples[i]) < 0.0f)//invert the ones that have a negative dot product with the current accumulated vector, and hope the first samples don't have a spread of more than 90 degrees
            {
                samples[i] = -samples[i];
            }
        }
        accumvec += samples[i];
    }
    accumvec = accumvec.normal();//normalize the average direction, trust this to make all components in [-1, 1]
    binghamOut[0] = accum / fdims[3];//the mean value of f
    accum = 0.0;
    for (int i = 0; i < fdims[3]; ++i)
    {
        float temp = fsamplearray[i] - binghamOut[0];
        accum += temp * temp;
    }
    binghamOut[1] = sqrt(accum / (fdims[3] - 1));//sample standard deviation
    float theta = acos(accumvec[2]);//[0, pi]
    float phi = atan2(accumvec[1], -accumvec[0]);//[-pi, pi] - NOTE: radiological polar strikes again
    if (phi < 0.0f) phi += 2 * 3.14159265358979;//[0, 2pi]
    binghamOut[2] = theta;
    binghamOut[3] = phi;
    Vector3D xhat, yhat;//vectors to project to the normal plane of the average direction, assuming psi=0
    xhat[0] = cos(theta) * cos(phi);//more radiological polar to neurological euclidean stuff
    xhat[1] = -cos(theta) * sin(phi);
    xhat[2] = sin(theta);
    yhat[0] = sin(phi);
    yhat[1] = cos(phi);
    yhat[2] = 0.0f;
    vector<float> x_samples(fdims[3]), y_samples(fdims[3]);
    float dist = -1.0f;
    int end1 = -1, end2 = -1;
    for (int i = 0; i < fdims[3]; ++i)//first endpoint is farthest from mean direction
    {
        x_samples[i] = samples[i].dot(xhat);
        y_samples[i] = samples[i].dot(yhat);
        float tempf = x_samples[i] * x_samples[i] + y_samples[i] * y_samples[i];
        if (tempf > dist)
        {
            end1 = i;
            dist = tempf;
        }
    }
    dist = -1.0f;
    for (int i = 0; i < fdims[3]; ++i)//second endpoint is farthest from first endpoint
    {
        float xdiff = x_samples[i] - x_samples[end1];
        float ydiff = y_samples[i] - y_samples[end1];
        float tempf = xdiff * xdiff + ydiff * ydiff;
        if (tempf > dist)
        {
            end2 = i;
            dist = tempf;
        }
    }//NOTE: the MAJOR axis of fanning is along y when psi = 0! ka > kb means kb is in the direction of more fanning
    float psi = atan((x_samples[end2] - x_samples[end1]) / (y_samples[end2] - y_samples[end1]));//[-pi/2, pi/2] - TODO: check radiological psi orientation
    if (!MathFunctions::isNumeric(psi))
    {
        const float LARGE_K = 1800.0f;//stdev of 1/60, typical maximum ka in a voxel is around 200
        binghamOut[4] = LARGE_K;
        binghamOut[5] = LARGE_K;
        binghamOut[6] = 0;
        return;
    }
    if (psi < 0) psi += 3.14159265358979;//[0, pi]
    binghamOut[6] = psi;
    double accumx = 0.0, accumy = 0.0;
    for (int i = 0; i < fdims[3]; ++i)
    {//rotate the samples through -psi on the plane to orient them to the axes
        float newx = x_samples[i] * cos(psi) + y_samples[i] * -sin(psi);//NOTE: again, radiological psi?
        float newy = x_samples[i] * sin(psi) + y_samples[i] * cos(psi);
        accumx += newx * newx;//assume mean of zero, because we already chose the center of the distribution
        accumy += newy * newy;
    }
    float ka = (fdims[3] - 1) / (2 * accumx);//convert variance to concentrations
    float kb = (fdims[3] - 1) / (2 * accumy);//but they aren't negative
    binghamOut[4] = ka;
    binghamOut[5] = kb;
}
