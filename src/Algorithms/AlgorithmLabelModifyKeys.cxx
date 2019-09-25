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

#include "AlgorithmLabelModifyKeys.h"
#include "AlgorithmException.h"

#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"

#include <fstream>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmLabelModifyKeys::getCommandSwitch()
{
    return "-label-modify-keys";
}

AString AlgorithmLabelModifyKeys::getShortDescription()
{
    return "CHANGE KEY VALUES IN A LABEL FILE";
}

OperationParameters* AlgorithmLabelModifyKeys::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label-in", "the input label file");
    
    ret->addStringParameter(2, "remap-file", "text file with old and new key values");
    
    ret->addLabelOutputParameter(3, "label-out", "output label file");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("<remap-file> should have lines of the form 'oldkey newkey', like so:\n\n") +
        "3 5\n5 8\n8 2\n\n" +
        "This would change the current label with key '3' to use the key '5' instead, 5 would use 8, and 8 would use 2.  " +
        "Any collision in key values results in the label that was not specified in the remap file getting remapped to an otherwise unused key.  " +
        "Remapping more than one key to the same new key, or the same key to more than one new key, results in an error.  " +
        "This will not change the appearance of the file when displayed, as it will change the key values in the data at the same time."
    );
    return ret;
}

void AlgorithmLabelModifyKeys::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* labelIn = myParams->getLabel(1);
    AString remapName = myParams->getString(2);
    LabelFile* labelOut = myParams->getOutputLabel(3);
    int column = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    if (columnOpt->m_present)
    {
        column = labelIn->getMapIndexFromNameOrNumber(columnOpt->getString(1));
        if (column < 0) throw AlgorithmException("invalid column specified");
    }
    FileInformation textFileInfo(remapName);
    if (!textFileInfo.exists())
    {
        throw AlgorithmException("label list file doesn't exist");
    }
    fstream remapFile(remapName.toLocal8Bit().constData(), fstream::in);
    if (!remapFile.good())
    {
        throw AlgorithmException("error reading label list file");
    }
    map<int32_t, int32_t> remap;
    int32_t oldkey, newkey;
    while (remapFile >> oldkey >> newkey)
    {
        if (remap.find(oldkey) != remap.end()) throw AlgorithmException("remapping tried to duplicate label " + AString::number(oldkey));
        remap[oldkey] = newkey;
    }
    AlgorithmLabelModifyKeys(myProgObj, labelIn, remap, labelOut, column);
}

AlgorithmLabelModifyKeys::AlgorithmLabelModifyKeys(ProgressObject* myProgObj, const LabelFile* labelIn, const map<int32_t, int32_t>& remap, LabelFile* labelOut, const int& column) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const GiftiLabelTable* oldTable = labelIn->getLabelTable();
    int32_t oldUnlabeled = oldTable->getUnassignedLabelKey();//because GiftiLabelTable is quirky, we need to check if the unlabeled value ends up as something other than 0
    GiftiLabelTable newTable;//beware: key 0 already has the "???" label
    bool setZero = false;//because of this, we need to track if we overwrote it, so that we can pretend it isn't there
    for (map<int32_t, int32_t>::const_iterator iter = remap.begin(); iter != remap.end(); ++iter)
    {
        const GiftiLabel* oldLabel = oldTable->getLabel(iter->first);
        if (oldLabel == NULL) throw AlgorithmException("label key " + AString::number(iter->first) + " does not exist in the input file");
        GiftiLabel newLabel(*oldLabel);
        newLabel.setKey(iter->second);
        if (iter->first == oldUnlabeled)
        {
            if (iter->second != 0)//if it isn't the default unlabeled value, then we have to do something
            {
                if (newTable.getLabel(iter->second) != NULL) throw AlgorithmException("remapping tried to set label " + AString::number(iter->second) + " more than once");
                newTable.deleteLabel(0);//delete the default, since we don't know what overwrites it, if anything
                newTable.insertLabel(&newLabel);//insert forces it to use the key in the label, even if it causes a duplicate name (which the original might theoretically have)
            } else {//otherwise, just error checking
                if (setZero) throw AlgorithmException("remapping tried to set label " + AString::number(iter->second) + " more than once");
                setZero = true;//we do have to track that zero now contains something that can't be overwritten
            }
        } else {
            if (iter->second == 0)//if it remaps to the default unlabeled key, we have to check it differently
            {
                if (setZero) throw AlgorithmException("remapping tried to set label " + AString::number(iter->second) + " more than once");
                setZero = true;
                newTable.insertLabel(&newLabel);//insert forces it to use the key in the label, so it will overwrite the existing default 0 key
            } else {//finally, the simple case
                if (newTable.getLabel(iter->second) != NULL) throw AlgorithmException("remapping tried to set label " + AString::number(iter->second) + " more than once");
                newTable.insertLabel(&newLabel);//insert forces it to use the key in the label, even if it causes a duplicate name (which the original might theoretically have)
            }
        }
    }
    set<int32_t> keys = oldTable->getKeys(), collisions;
    for (set<int32_t>::const_iterator iter = keys.begin(); iter != keys.end(); ++iter)
    {
        if (remap.find(*iter) == remap.end())//skip if it was remapped
        {
            if (*iter == 0)//again with the special default 0 key
            {
                if (setZero)//check for collision
                {
                    collisions.insert(*iter);
                } else {
                    setZero = true;
                    if (*iter != oldUnlabeled)//if its merely the unassigned label already, we can just keep the existing default
                    {
                        newTable.insertLabel(oldTable->getLabel(*iter));
                    }
                }
            } else {
                if (newTable.getLabel(*iter) == NULL)
                {
                    newTable.insertLabel(oldTable->getLabel(*iter));
                } else {//collision
                    collisions.insert(*iter);
                }
            }
        }
    }
    map<int32_t, int32_t> valueChanges = remap;//start with the specified changes, then add the collision changes
    for (set<int32_t>::const_iterator iter = collisions.begin(); iter != collisions.end(); ++iter)
    {//now deal with collisions
        int32_t newKey = newTable.generateUnusedKey();
        GiftiLabel newLabel(*(oldTable->getLabel(*iter)));
        newLabel.setKey(newKey);
        newTable.insertLabel(&newLabel);//insert forces it to use the key in the label, even if it causes a duplicate name (which the original might theoretically have)
        valueChanges[*iter] = newKey;
    }
    int numNodes = labelIn->getNumberOfNodes(), numColumns = labelIn->getNumberOfColumns();
    vector<int32_t> scratchCol(numNodes);
    if (column == -1)
    {
        labelOut->setNumberOfNodesAndColumns(numNodes, numColumns);
        labelOut->setStructure(labelIn->getStructure());
        *(labelOut->getLabelTable()) = newTable;
        for (int i = 0; i < numColumns; ++i)
        {
            labelOut->setColumnName(i, labelIn->getColumnName(i));
            const int32_t* inCol = labelIn->getLabelKeyPointerForColumn(i);
            for (int j = 0; j < numNodes; ++j)
            {
                map<int32_t, int32_t>::const_iterator iter = valueChanges.find(inCol[j]);
                if (iter == valueChanges.end())
                {
                    scratchCol[j] = inCol[j];
                } else {
                    scratchCol[j] = iter->second;
                }
            }
            labelOut->setLabelKeysForColumn(i, scratchCol.data());
        }
    } else {
        labelOut->setNumberOfNodesAndColumns(numNodes, 1);
        labelOut->setStructure(labelIn->getStructure());
        *(labelOut->getLabelTable()) = newTable;
        labelOut->setColumnName(0, labelIn->getColumnName(column));
        const int32_t* inCol = labelIn->getLabelKeyPointerForColumn(column);
        for (int j = 0; j < numNodes; ++j)
        {
            map<int32_t, int32_t>::const_iterator iter = valueChanges.find(inCol[j]);
            if (iter == valueChanges.end())
            {
                scratchCol[j] = inCol[j];
            } else {
                scratchCol[j] = iter->second;
            }
        }
        labelOut->setLabelKeysForColumn(0, scratchCol.data());
    }
}

float AlgorithmLabelModifyKeys::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelModifyKeys::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
