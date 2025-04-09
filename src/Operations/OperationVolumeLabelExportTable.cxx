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

#include "OperationVolumeLabelExportTable.h"
#include "OperationException.h"

#include "CaretHierarchy.h"
#include "GiftiLabel.h" //we should rename these to not imply that they are gifti-specific
#include "GiftiLabelTable.h"
#include "VolumeFile.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString OperationVolumeLabelExportTable::getCommandSwitch()
{
    return "-volume-label-export-table";
}

AString OperationVolumeLabelExportTable::getShortDescription()
{
    return "EXPORT LABEL TABLE FROM VOLUME AS TEXT";
}

OperationParameters* OperationVolumeLabelExportTable::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "label-in", "the input volume label file");
    
    ret->addStringParameter(2, "map", "the number or name of the label map to use");
    
    ret->addStringParameter(3, "table-out", "output - the output text file"); //fake output formatting
    
    OptionalParameter* hierOpt = ret->createOptionalParameter(4, "-hierarchy", "export the hierarchy as json");
    hierOpt->addStringParameter(1, "json-out", "output - filename to write hierarchy to"); //fake the output formatting
    
    ret->setHelpText(
        AString("Takes the label table from the volume label map, and writes it to a text format matching what is expected by -volume-label-import.")
    );
    return ret;
}

int OperationVolumeLabelExportTable::floatTo255(const float& in)
{
    return (int)floor(in * 255.0f + 0.5f);
}

void OperationVolumeLabelExportTable::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVolume = myParams->getVolume(1);
    AString mapString = myParams->getString(2);
    AString outfileName = myParams->getString(3);
    if (myVolume->getType() != SubvolumeAttributes::LABEL) throw OperationException("volume file must be a label volume");
    int64_t mapIndex = myVolume->getMapIndexFromNameOrNumber(mapString);
    if (mapIndex == -1) throw OperationException("map '" + mapString + "' not found");
    ofstream outFile(outfileName.toLocal8Bit().constData());
    if (!outFile) throw OperationException("failed to open output text file");
    const GiftiLabelTable* myTable = myVolume->getMapLabelTable(mapIndex);
    set<int32_t> allKeys = myTable->getKeys();
    int32_t unassignedKey = myTable->getUnassignedLabelKey();
    for (set<int32_t>::iterator iter = allKeys.begin(); iter != allKeys.end(); ++iter)
    {
        if (*iter == unassignedKey) continue;//don't output the unused key, because import doesn't want it in the text file
        const GiftiLabel* thisLabel = myTable->getLabel(*iter);
        outFile << thisLabel->getName() << endl;
        outFile << thisLabel->getKey() << " "
            << floatTo255(thisLabel->getRed()) << " "
            << floatTo255(thisLabel->getGreen()) << " "
            << floatTo255(thisLabel->getBlue()) << " "
            << floatTo255(thisLabel->getAlpha()) << endl;
        if (!outFile) throw OperationException("error writing to output text file");
    }
    OptionalParameter* hierOpt = myParams->getOptionalParameter(4);
    if (hierOpt->m_present)
    {
        myTable->getHierarchy().writeJsonFile(hierOpt->getString(1));
    }
}
