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

#include "OperationLabelExportTable.h"
#include "OperationException.h"

#include "CaretHierarchy.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString OperationLabelExportTable::getCommandSwitch()
{
    return "-label-export-table";
}

AString OperationLabelExportTable::getShortDescription()
{
    return "EXPORT LABEL TABLE FROM GIFTI AS TEXT";
}

OperationParameters* OperationLabelExportTable::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label-in", "the input label file");
    
    ret->addStringParameter(2, "table-out", "output - the output text file");//fake output formatting
    
    OptionalParameter* hierOpt = ret->createOptionalParameter(3, "-hierarchy", "export the hierarchy as json");
    hierOpt->addStringParameter(1, "json-out", "output - filename to write hierarchy to"); //fake the output formatting
    
    ret->setHelpText(
        AString("Takes the label table from the gifti label file, and writes it to a text format matching what is expected by -metric-label-import.")
    );
    return ret;
}

int OperationLabelExportTable::floatTo255(const float& in)
{
    return (int)floor(in * 255.0f + 0.5f);
}

void OperationLabelExportTable::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    LabelFile* myLabel = myParams->getLabel(1);
    AString outName = myParams->getString(2);
    ofstream outFile(outName.toLocal8Bit().constData());
    if (!outFile) throw OperationException("failed to open output text file");
    const GiftiLabelTable* myTable = myLabel->getLabelTable();
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
    OptionalParameter* hierOpt = myParams->getOptionalParameter(3);
    if (hierOpt->m_present)
    {
        myTable->getHierarchy().writeJsonFile(hierOpt->getString(1));
    }
}
