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

#include "OperationVolumeLabelImport.h"
#include "OperationException.h"
#include "VolumeFile.h"
#include "GiftiLabel.h"
#include "FileInformation.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace caret;
using namespace std;

AString OperationVolumeLabelImport::getCommandSwitch()
{
    return "-volume-label-import";
}

AString OperationVolumeLabelImport::getShortDescription()
{
    return "IMPORT A LABEL VOLUME TO CARET FORMAT";
}

OperationParameters* OperationVolumeLabelImport::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "input", "the label volume to import");
    
    ret->addStringParameter(2, "label-list-file", "text file containing the values and names for labels");
    
    ret->addVolumeOutputParameter(3, "output", "the output caret label volume");
    
    ret->createOptionalParameter(4, "-discard-others", "set any voxels with values not mentioned in the label list to the ??? label");
    
    OptionalParameter* unlabeledOption = ret->createOptionalParameter(5, "-unlabeled-value", "set the value that will be interpreted as unlabeled");
    unlabeledOption->addIntegerParameter(1, "value", "the numeric value for unlabeled (default 0)");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to import");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Creates a new volume with label information in the header in the caret nifti extension format.  The label list file should have ") +
        "lines of the following format:\n\n" + 
        "<labelname>\n<value> <red> <green> <blue> <alpha>\n\n" + 
        "Label names MUST be on a separate line, but may contain spaces or other unusual characters (but not newline).  The values of red, green, blue and alpha " +
        "should be integers from 0 to 255, and will specify the color the label is drawn as (alpha of 255 means opaque, which is probably " +
        "what you want).  By default, it will set new label names with names of LABEL_# for any values encountered that are not mentioned in the " +
        "list file, specify -discard-others to instead set these voxels to the \"???\" label.  Negative keys will get remapped to unused positive keys " +
        "(caret doesn't allow negative label keys)."
    );
    return ret;
}

void OperationVolumeLabelImport::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString temp;
    srand(time(NULL));
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVol = myParams->getVolume(1);
    AString listfileName = myParams->getString(2);
    VolumeFile* outVol = myParams->getOutputVolume(3);
    bool discardOthers = false;
    OptionalParameter* discardOption = myParams->getOptionalParameter(4);
    if (discardOption->m_present)
    {
        discardOthers = true;
    }
    int32_t unlabeledValue = 0;
    OptionalParameter* unlabeledOption = myParams->getOptionalParameter(5);
    if (unlabeledOption->m_present)
    {
        unlabeledValue = (int32_t)unlabeledOption->getInteger(1);
    }
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(6);
    int subvol = -1;
    if (subvolumeSelect->m_present)
    {//set up to use the single column
        subvol = (int)myVol->getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (subvol < 0 || subvol >= myVol->getNumberOfMaps())
        {
            throw OperationException("invalid column specified");
        }
    }
    FileInformation textFileInfo(listfileName);
    if (!textFileInfo.exists())
    {
        throw OperationException("label list file doesn't exist");
    }
    fstream labelListFile(listfileName.toLocal8Bit().constData(), fstream::in);
    if (!labelListFile.good())
    {
        throw OperationException("error reading label list file");
    }
    string labelName;
    int32_t value, red, green, blue, alpha;
    GiftiLabelTable myTable;
    const int LOOKUP_SIZE = 10000;
    vector<bool> labelUsed(LOOKUP_SIZE, false);
    CaretArray<int32_t> labelTranslate(LOOKUP_SIZE, -1);//quickly lookup labels in the expected range of values
    vector<pair<int32_t, int32_t> > outsideLookup;//but be able to handle labels with unusually large values
    if (unlabeledValue >= 0 && unlabeledValue < LOOKUP_SIZE)
    {
        labelUsed[unlabeledValue] = true;//to ensure that the label list file doesn't try to specify the unused label
    } else {
        outsideLookup.push_back(make_pair(unlabeledValue, 0));//placeholder, we don't know the correct translated value yet
    }
    while (labelListFile.good())
    {
        getline(labelListFile, labelName);
        labelListFile >> value;
        labelListFile >> red;
        labelListFile >> green;
        labelListFile >> blue;
        if (!(labelListFile >> alpha))//yes, that is seriously the correct way to check if input was successfully extracted...so much fail
        {
            break;//ignore malformed lines
        }
        labelListFile.ignore();//drop the newline so that getline doesn't explode
        temp = AString(labelName.c_str());
        GiftiLabel myLabel(value, temp, red, green, blue, alpha);
        myTable.insertLabel(&myLabel);
        int32_t newValue = myLabel.getKey();
        if (value >= 0 && value < LOOKUP_SIZE)
        {
            if (labelUsed[value])
            {
                throw OperationException(AString("label ") + AString::number(value) + " specified more than once");
            }
            labelUsed[value] = true;//this isn't really necessary with the current implementation of GiftiLabelTable, only negative keys get remapped, but it is cleaner
            labelTranslate[value] = newValue;
        } else {
            int numOutside = (int)outsideLookup.size();
            for (int i = 0; i < numOutside; ++i)
            {
                if (outsideLookup[i].first == value)
                {
                    throw OperationException(AString("label ") + AString::number(value) + " specified more than once");
                }
            }
            outsideLookup.push_back(make_pair(value, newValue));
        }
    }
    vector<int64_t> myDims;
    myVol->getDimensions(myDims);
    const int64_t FRAMESIZE = myDims[0] * myDims[1] * myDims[2];
    CaretArray<float> frameOut(FRAMESIZE);
    int32_t unusedLabel = myTable.getUnassignedLabelKey();
    if (unlabeledValue >= 0 && unlabeledValue < LOOKUP_SIZE)
    {
        labelTranslate[unlabeledValue] = unusedLabel;
    } else {
        int numOutside = (int)outsideLookup.size();
        bool found = false;
        for (int i = 0; i < numOutside; ++i)
        {
            if (outsideLookup[i].first == unlabeledValue)
            {
                outsideLookup[i].second = unusedLabel;
                found = true;
                break;
            }
        }
        if (!found) throw OperationException("internal failure constructing label table");
    }
    if (subvol == -1)
    {
        outVol->reinitialize(myVol->getOriginalDimensions(), myVol->getVolumeSpace(), myDims[4], SubvolumeAttributes::LABEL);
        for (int s = 0; s < myDims[3]; ++s)
        {
            for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
            {
                const float* frameIn = myVol->getFrame(s, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
                for (int i = 0; i < FRAMESIZE; ++i)
                {
                    int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                    if (labelval >= 0 && labelval < LOOKUP_SIZE)
                    {
                        if (labelUsed[labelval])
                        {
                            frameOut[i] = labelTranslate[labelval];
                        } else {
                            if (discardOthers)
                            {
                                frameOut[i] = unusedLabel;
                            } else {//use a random color, but fully opaque for the label
                                GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                                myTable.insertLabel(&myLabel);
                                int32_t newValue = myLabel.getKey();
                                labelUsed[labelval] = true;
                                labelTranslate[labelval] = newValue;
                                frameOut[i] = newValue;
                            }
                        }
                    } else {
                        int numOutside = (int)outsideLookup.size(), whichIndex = -1;
                        bool found = false;
                        for (int ind = 0; ind < numOutside; ++ind)
                        {
                            if (outsideLookup[ind].first == value)
                            {
                                found = true;
                                whichIndex = ind;
                                break;
                            }
                        }
                        if (found)
                        {
                            frameOut[i] = outsideLookup[whichIndex].second;
                        } else {
                            if (discardOthers)
                            {
                                frameOut[i] = unusedLabel;
                            } else {
                                GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                                myTable.insertLabel(&myLabel);
                                int32_t newValue = myLabel.getKey();
                                outsideLookup.push_back(make_pair(labelval, newValue));
                            }
                        }
                    }
                }
                outVol->setFrame(frameOut, s, c);
            }
            *(outVol->getMapLabelTable(s)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
        }
    } else {
        vector<int64_t> newDims = myDims;
        newDims.resize(3);//spatial only
        outVol->reinitialize(newDims, myVol->getVolumeSpace(), myDims[4], SubvolumeAttributes::LABEL);
        for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
        {
            const float* frameIn = myVol->getFrame(subvol, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
            for (int i = 0; i < FRAMESIZE; ++i)
            {
                int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                if (labelval >= 0 && labelval < LOOKUP_SIZE)
                {
                    if (labelUsed[labelval])
                    {
                        frameOut[i] = labelTranslate[labelval];
                    } else {
                        if (discardOthers)
                        {
                            frameOut[i] = unusedLabel;
                        } else {//use a random color, but fully opaque for the label
                            GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                            myTable.insertLabel(&myLabel);
                            int32_t newValue = myLabel.getKey();
                            labelUsed[labelval] = true;
                            labelTranslate[labelval] = newValue;
                            frameOut[i] = newValue;
                        }
                    }
                } else {
                    int numOutside = (int)outsideLookup.size(), whichIndex = -1;
                    bool found = false;
                    for (int ind = 0; ind < numOutside; ++ind)
                    {
                        if (outsideLookup[ind].first == value)
                        {
                            found = true;
                            whichIndex = ind;
                            break;
                        }
                    }
                    if (found)
                    {
                        frameOut[i] = outsideLookup[whichIndex].second;
                    } else {
                        if (discardOthers)
                        {
                            frameOut[i] = unusedLabel;
                        } else {
                            GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                            myTable.insertLabel(&myLabel);
                            int32_t newValue = myLabel.getKey();
                            outsideLookup.push_back(make_pair(labelval, newValue));
                        }
                    }
                }
            }
            outVol->setFrame(frameOut, 0, c);
        }
        *(outVol->getMapLabelTable(0)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
    }
}
