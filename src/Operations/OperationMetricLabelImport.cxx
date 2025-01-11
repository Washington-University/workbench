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

#include "OperationMetricLabelImport.h"
#include "OperationException.h"

#include "CaretHierarchy.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"

#include <cmath>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricLabelImport::getCommandSwitch()
{
    return "-metric-label-import";
}

AString OperationMetricLabelImport::getShortDescription()
{
    return "IMPORT A GIFTI LABEL FILE FROM A METRIC FILE";
}

OperationParameters* OperationMetricLabelImport::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "input", "the input metric file");
    
    ret->addStringParameter(2, "label-list-file", "text file containing the values and names for labels");
    
    ret->addLabelOutputParameter(3, "output", "the output gifti label file");
    
    ret->createOptionalParameter(4, "-discard-others", "set any values not mentioned in the label list to the ??? label");
    
    OptionalParameter* unlabeledOption = ret->createOptionalParameter(5, "-unlabeled-value", "set the value that will be interpreted as unlabeled");
    unlabeledOption->addIntegerParameter(1, "value", "the numeric value for unlabeled (default 0)");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column to import");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->createOptionalParameter(7, "-drop-unused-labels", "remove any unused label values from the label table");
    
    OptionalParameter* hierOpt = ret->createOptionalParameter(8, "-hierarchy", "read label name hierarchy from a json file");
    hierOpt->addStringParameter(1, "file", "the input json file");
    
    ret->setHelpText(
        AString("Creates a gifti label file from a metric file with label-like values.  ") +
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

void OperationMetricLabelImport::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    MetricFile* myMetric = myParams->getMetric(1);
    AString listfileName = myParams->getString(2);
    LabelFile* myLabelOut = myParams->getOutputLabel(3);
    bool discardOthers = myParams->getOptionalParameter(4)->m_present;
    int32_t unlabeledValue = 0;
    OptionalParameter* unlabeledOption = myParams->getOptionalParameter(5);
    if (unlabeledOption->m_present)
    {
        unlabeledValue = (int32_t)unlabeledOption->getInteger(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(6);
    int columnNum = -1;
    if (columnSelect->m_present)
    {
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0 || columnNum >= myMetric->getNumberOfMaps())
        {
            throw OperationException("invalid column specified");
        }
    }
    bool dropUnused = myParams->getOptionalParameter(7)->m_present;
    map<int32_t, int32_t> translate;
    GiftiLabelTable myTable;
    if (listfileName != "")//maybe this should be a function of GiftiLabelTable
    {
        AString temp;
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
    int32_t unusedLabel = myTable.getUnassignedLabelKey();
    OptionalParameter* hierOpt = myParams->getOptionalParameter(8);
    if (hierOpt->m_present)
    {
        AString hierfileName = hierOpt->getString(1);
        CaretHierarchy myHier;
        myHier.readJsonFile(hierfileName);
        set<AString> hierNames = myHier.getAllNames(); //might consider sticking the below warning check loop into GiftiLabelTable::setHierarchy()...
        map<int32_t, AString> tableMap; //keys aren't needed, but API only exposes names as a map
        myTable.getKeysAndNames(tableMap);
        for (auto iter : tableMap)
        {
            if (iter.first != unusedLabel && hierNames.find(iter.second) == hierNames.end())
            {
                CaretLogWarning("label name '" + iter.second + "' not found in specified hierarchy");
            }
        }
        myTable.setHierarchy(myHier);
    }
    translate[unlabeledValue] = unusedLabel;
    const int numNodes = myMetric->getNumberOfNodes();
    vector<int32_t> colScratch(numNodes);
    if (columnNum == -1)
    {
        const int numCols = myMetric->getNumberOfColumns();
        myLabelOut->setNumberOfNodesAndColumns(numNodes, numCols);
        myLabelOut->setStructure(myMetric->getStructure());
        set<int32_t> usedValues;
        for (int col = 0; col < numCols; ++col)
        {
            translateLabels(myMetric->getValuePointerForColumn(col), colScratch.data(), numNodes, myTable, translate, usedValues, dropUnused, discardOthers, unusedLabel);
            myLabelOut->setLabelKeysForColumn(col, colScratch.data());
        }
        if (dropUnused)
        {
            myTable.deleteUnusedLabels(usedValues);
        }
        *(myLabelOut->getLabelTable()) = myTable;
    } else {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, 1);
        myLabelOut->setStructure(myMetric->getStructure());
        set<int32_t> usedValues;
        translateLabels(myMetric->getValuePointerForColumn(columnNum), colScratch.data(), numNodes, myTable, translate, usedValues, dropUnused, discardOthers, unusedLabel);
        myLabelOut->setLabelKeysForColumn(0, colScratch.data());
        if (dropUnused)
        {
            myTable.deleteUnusedLabels(usedValues);
        }
        *(myLabelOut->getLabelTable()) = myTable;
    }
}

void OperationMetricLabelImport::translateLabels(const float* valuesIn, int32_t* labelsOut, const int& numNodes, GiftiLabelTable& myTable, map<int32_t, int32_t>& translate,
                                                 set<int32_t>& usedValues, const bool& dropUnused, const bool& discardOthers, const int32_t& unusedLabel)
{
    set<AString> hierNames = myTable.getHierarchy().getAllNames();
    for (int node = 0; node < numNodes; ++node)
    {
        int32_t labelval = (int32_t)floor(valuesIn[node] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
        if (dropUnused)
        {
            usedValues.insert(labelval);
        }
        map<int32_t, int32_t>::iterator myiter = translate.find(labelval);
        if (myiter == translate.end())
        {
            if (discardOthers)
            {
                labelsOut[node] = unusedLabel;
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
                if (!hierNames.empty() && hierNames.find(myLabel.getName()) == hierNames.end())
                {
                    CaretLogWarning("creating label " + myLabel.getName() + ", which does not exist in the hierarchy (note, using -discard-others would de-label vertices with that value instead)");
                }
                int32_t newValue = myTable.addLabel(&myLabel);//don't overwrite any values in the table
                translate[labelval] = newValue;
                labelsOut[node] = newValue;
            }
        } else {
            labelsOut[node] = myiter->second;
        }
    }
}
