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

#include "OperationVolumeLabelImport.h"
#include "OperationException.h"

#include "CaretHierarchy.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "VolumeFile.h"

#include <QFile>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <cmath>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <string>

using namespace caret;
using namespace std;

AString OperationVolumeLabelImport::getCommandSwitch()
{
    return "-volume-label-import";
}

AString OperationVolumeLabelImport::getShortDescription()
{
    return "IMPORT A LABEL VOLUME TO WORKBENCH FORMAT";
}

OperationParameters* OperationVolumeLabelImport::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "input", "the input volume file");
    
    ret->addStringParameter(2, "label-list-file", "text file containing the values and names for labels");
    
    ret->addVolumeOutputParameter(3, "output", "the output workbench label volume");
    
    ret->createOptionalParameter(4, "-discard-others", "set any voxels with values not mentioned in the label list to the ??? label");
    
    OptionalParameter* unlabeledOption = ret->createOptionalParameter(5, "-unlabeled-value", "set the value that will be interpreted as unlabeled");
    unlabeledOption->addIntegerParameter(1, "value", "the numeric value for unlabeled (default 0)");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to import");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->createOptionalParameter(7, "-drop-unused-labels", "remove any unused label values from the label table");
    
    OptionalParameter* hierOpt = ret->createOptionalParameter(8, "-hierarchy", "read label name hierarchy from a json file");
    hierOpt->addStringParameter(1, "file", "the input json file");
    
    ret->setHelpText(
        AString("Creates a label volume from an integer-valued volume file.  ") +
        "The label name and color information is stored in the volume header in a nifti extension, with a similar format as in caret5, see -volume-help.  " +
        "You may specify the empty string (use \"\") for <label-list-file>, which will be treated as if it is an empty file.  " +
        "The label list file must have the following format (2 lines per label):\n\n" +
        "<labelname>\n<key> <red> <green> <blue> <alpha>\n...\n\n" +
        "Label names are specified on a separate line from their value and color, in order to let label names contain spaces.  " +
        "Whitespace is trimmed from both ends of the label name, but is kept if it is in the middle of a label.  " +
        "Do not specify the \"unlabeled\" key in the file, it is assumed that 0 means not labeled unless -unlabeled-value is specified.  " +
        "The value of <key> specifies what value in the imported file should be used as this label (these same key values are also used in the output file).  " +
        "The values of <red>, <green>, <blue> and <alpha> must be integers from 0 to 255, and will specify the color the label is drawn as " +
        "(alpha of 255 means fully opaque, which is probably what you want).\n\n" +
        "By default, it will create new label names with names like LABEL_5 for any values encountered that are not mentioned in the " +
        "list file, specify -discard-others to instead set these values to the \"unlabeled\" key."
    );
    return ret;
}

namespace
{
    void recurseJson(CaretHierarchy& hierarchyOut, const QJsonArray& elements, const AString parent = "")
    {
        for (auto iter = elements.constBegin(); iter != elements.constEnd(); ++iter)
        {
            QJsonObject thisobj = iter->toObject();
            CaretHierarchy::Item toAdd;
            toAdd.name = thisobj.value("name").toString();
            if (toAdd.name == "")
            {
                if (parent == "")
                {
                    throw OperationException("empty, non-string, or missing 'name' element in hierarchy json, in a top-level item");
                } else {
                    throw OperationException("empty, non-string, or missing 'name' element in hierarchy json, in children of '" + parent + "'");
                }
            }
            auto keys = thisobj.keys();
            for (auto iter = keys.begin(); iter != keys.end(); ++iter)
            {
                AString key = *iter;
                if (key == "name") continue; //don't put name into extraInfo, it is already handled
                auto valueobj = thisobj.value(key);
                AString value;
                bool stringish = true;
                switch (valueobj.type())
                {
                    case QJsonValue::Bool:
                        if (valueobj.toBool()) { value = "True"; } else { value = "False"; }
                        break;
                    case QJsonValue::Double:
                        value = AString::number(valueobj.toDouble(), 'g', 16); //handle stupidly large integers with g16, since json numbers are always implicitly double
                        break;
                    case QJsonValue::String:
                        value = valueobj.toString();
                        break;
                    default:
                        stringish = false;
                        break;
                }
                if (key == "children")
                {
                    if (stringish)
                    {
                        CaretLogWarning("found non-array value for 'children' member in hierarchy item '" + toAdd.name + "'");
                    }
                    continue;//treat it as reserved, don't put it in extraInfo
                } else {
                    if (!stringish)
                    {
                        CaretLogWarning("found non-stringlike value for member '" + key + "' in hierarchy item '" + toAdd.name + "'");
                    }
                    continue;//ignore rather than put an empty string for the key?
                }
                toAdd.extraInfo.set(key, value);
            }
            if (!hierarchyOut.addItem(toAdd, parent))
            {
                throw OperationException("failed to add hierarchy item '" + toAdd.name + "', check whether all 'name's are unique");
            }
            if (thisobj.contains("children"))
            {
                recurseJson(hierarchyOut, thisobj.value("children").toArray(), toAdd.name);
            }
        }
    }
}

void OperationVolumeLabelImport::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString temp;
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
    bool dropUnused = myParams->getOptionalParameter(7)->m_present;
    OptionalParameter* hierOpt = myParams->getOptionalParameter(8);
    GiftiLabelTable myTable;
    map<int32_t, int32_t> translate;
    if (listfileName != "")
    {
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
        int labelCount = 0;
        translate[unlabeledValue] = 0;//placeholder, we don't know the correct translated value yet
        while (labelListFile.good())
        {
            ++labelCount;//just for error messages, so start at 1
            getline(labelListFile, labelName);
            labelListFile >> value;
            if (labelListFile.eof() && labelName == "") break;//if end of file trying to read an int, and label name is empty, its really just end of file
            labelListFile >> red;
            labelListFile >> green;
            labelListFile >> blue;
            if (!(labelListFile >> alpha))//yes, that is seriously the correct way to check if input was successfully extracted...so much fail
            {
                throw OperationException("label list file is malformed for entry #" + AString::number(labelCount) + ": " + AString(labelName.c_str()));
            }
            if (red < 0 || red > 255)
            {
                throw OperationException("bad value for red for entry #" + AString::number(labelCount) + ", " + AString(labelName.c_str()) + ": " + AString::number(red));
            }
            if (green < 0 || green > 255)
            {
                throw OperationException("bad value for green for entry #" + AString::number(labelCount) + ", " + AString(labelName.c_str()) + ": " + AString::number(green));
            }
            if (blue < 0 || blue > 255)
            {
                throw OperationException("bad value for blue for entry #" + AString::number(labelCount) + ", " + AString(labelName.c_str()) + ": " + AString::number(blue));
            }
            if (alpha < 0 || alpha > 255)
            {
                throw OperationException("bad value for alpha for entry #" + AString::number(labelCount) + ", " + AString(labelName.c_str()) + ": " + AString::number(alpha));
            }
            if (value == GiftiLabel::getInvalidLabelKey())
            {
                throw OperationException("entry #" + AString::number(labelCount) + ", " + AString(labelName.c_str()) + " specifies unusable key value: " + AString::number(value));
            }
            while (isspace(labelListFile.peek()))
            {
                labelListFile.ignore();//drop the newline, possible carriage return or other whitespace so that getline doesn't get nothing, and cause int extraction to fail
            }
            temp = AString(labelName.c_str()).trimmed();//drop errant CR or other whitespace from beginning and end of lines
            if (translate.find(value) != translate.end())
            {
                if (value == unlabeledValue)
                {
                    throw OperationException("the unlabeled value must not be specified in label list file");
                } else {
                    throw OperationException(AString("label key ") + AString::number(value) + " specified more than once");
                }
            }
            GiftiLabel myLabel(value, temp, red, green, blue, alpha);
            if (myTable.getLabelKeyFromName(temp) != GiftiLabel::getInvalidLabelKey())
            {
                AString nameBase = temp, newName;//resolve collision by generating a name with an additional number on it
                bool success = false;
                for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                {
                    newName = nameBase + "_" + AString::number(extra);
                    if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                    {
                        success = true;
                        break;
                    }
                }
                if (success)
                {
                    CaretLogWarning("name collision in input name '" + nameBase + "', changing one to '" + newName + "'");
                } else {
                    throw OperationException("giving up on resolving name collision for input name '" + nameBase + "'");
                }
                myLabel.setName(newName);
            }
            int32_t newValue;
            if (value == 0)//because label 0 exists in the default constructed table
            {
                myTable.insertLabel(&myLabel);//but we do want to be able to overwrite the default 0 label
                newValue = 0;//if value 0 is specified twice, or once without specifying a different unlabeled value, the check versus the translate map will catch it
            } else {
                newValue = myTable.addLabel(&myLabel);//we don't want to overwrite relocated labels
            }
            translate[value] = newValue;
        }
    }
    int32_t tableUnlabeledKey = myTable.getUnassignedLabelKey();
    if (hierOpt->m_present)
    {
        AString hierfileName = hierOpt->getString(1);
        QFile jsonfile(hierfileName);
        jsonfile.open(QIODevice::ReadOnly | QIODevice::Text);
        QJsonDocument myjson = QJsonDocument::fromJson(jsonfile.readAll());
        QJsonArray myarray = myjson.array();
        CaretHierarchy myHier;
        recurseJson(myHier, myarray);
        auto hierNames = myHier.getAllNames();
        map<int32_t, AString> tableMap; //keys aren't needed, but API only exposes names as a map
        myTable.getKeysAndNames(tableMap);
        for (auto iter : tableMap)
        {
            if (iter.first != tableUnlabeledKey && hierNames.find(iter.second) == hierNames.end())
            {
                CaretLogWarning("label name '" + iter.second + "' not found in specified hierarchy");
            }
        }
        myTable.setHierarchy(myHier);
    }
    vector<int64_t> myDims;
    myVol->getDimensions(myDims);
    const int64_t FRAMESIZE = myDims[0] * myDims[1] * myDims[2];
    CaretArray<float> frameOut(FRAMESIZE);
    translate[unlabeledValue] = tableUnlabeledKey;
    if (subvol == -1)
    {
        outVol->reinitialize(myVol->getOriginalDimensions(), myVol->getSform(), myDims[4], SubvolumeAttributes::LABEL);
        for (int s = 0; s < myDims[3]; ++s)
        {
            set<int32_t> usedValues;//track used values if we have dropUnused
            for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
            {
                const float* frameIn = myVol->getFrame(s, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
                for (int i = 0; i < FRAMESIZE; ++i)
                {
                    int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                    if (dropUnused)
                    {
                        usedValues.insert(labelval);
                    }
                    map<int32_t, int32_t>::iterator myiter = translate.find(labelval);
                    if (myiter == translate.end())
                    {
                        if (discardOthers)
                        {
                            frameOut[i] = tableUnlabeledKey;
                        } else {//use a random color, but fully opaque for the label
                            GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                            if (myTable.getLabelKeyFromName(myLabel.getName()) != GiftiLabel::getInvalidLabelKey())
                            {
                                AString nameBase = myLabel.getName(), newName;//resolve collision by generating a name with an additional number on it
                                bool success = false;
                                for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                                {
                                    newName = nameBase + "_" + AString::number(extra);
                                    if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                                    {
                                        success = true;
                                        break;
                                    }
                                }
                                if (success)
                                {
                                    CaretLogWarning("name collision in auto-generated name '" + nameBase + "', changed to '" + newName + "'");
                                } else {
                                    throw OperationException("giving up on resolving name collision for auto-generated name '" + nameBase + "'");
                                }
                                myLabel.setName(newName);
                            }
                            int32_t newValue = myTable.addLabel(&myLabel);//don't overwrite any values in the table
                            translate[labelval] = newValue;
                            frameOut[i] = newValue;
                        }
                    } else {
                        frameOut[i] = myiter->second;
                    }
                }
                outVol->setFrame(frameOut, s, c);
            }
            if (dropUnused)
            {
                GiftiLabelTable frameTable = myTable;
                frameTable.deleteUnusedLabels(usedValues);
                *(outVol->getMapLabelTable(s)) = frameTable;
            } else {
                *(outVol->getMapLabelTable(s)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
            }
        }
    } else {
        vector<int64_t> newDims = myDims;
        newDims.resize(3);//spatial only
        outVol->reinitialize(newDims, myVol->getSform(), myDims[4], SubvolumeAttributes::LABEL);
        set<int32_t> usedValues;//track used values if we have dropUnused
        for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
        {
            const float* frameIn = myVol->getFrame(subvol, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
            for (int i = 0; i < FRAMESIZE; ++i)
            {
                int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                if (dropUnused)
                {
                    usedValues.insert(labelval);
                }
                map<int32_t, int32_t>::iterator myiter = translate.find(labelval);
                if (myiter == translate.end())
                {
                    if (discardOthers)
                    {
                        frameOut[i] = tableUnlabeledKey;
                    } else {//use a random color, but fully opaque for the label
                        GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                        if (myTable.getLabelKeyFromName(myLabel.getName()) != GiftiLabel::getInvalidLabelKey())
                        {
                            AString nameBase = myLabel.getName(), newName;//resolve collision by generating a name with an additional number on it
                            bool success = false;
                            for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                            {
                                newName = nameBase + "_" + AString::number(extra);
                                if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                                {
                                    success = true;
                                    break;
                                }
                            }
                            if (success)
                            {
                                CaretLogWarning("name collision in auto-generated name '" + nameBase + "', changed to '" + newName + "'");
                            } else {
                                throw OperationException("giving up on resolving name collision for auto-generated name '" + nameBase + "'");
                            }
                            myLabel.setName(newName);
                        }
                        int32_t newValue = myTable.addLabel(&myLabel);//don't overwrite any values in the table
                        translate[labelval] = newValue;
                        frameOut[i] = newValue;
                    }
                } else {
                    frameOut[i] = myiter->second;
                }
            }
            outVol->setFrame(frameOut, 0, c);
        }
        if (dropUnused)
        {
            GiftiLabelTable frameTable = myTable;
            frameTable.deleteUnusedLabels(usedValues);
            *(outVol->getMapLabelTable(0)) = frameTable;
        } else {
            *(outVol->getMapLabelTable(0)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
        }
    }
}
