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

#include "OperationBorderExportColorTable.h"
#include "OperationException.h"

#include "Border.h"
#include "BorderFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString OperationBorderExportColorTable::getCommandSwitch()
{
    return "-border-export-color-table";
}

AString OperationBorderExportColorTable::getShortDescription()
{
    return "WRITE BORDER NAMES AND COLORS AS TEXT";
}

OperationParameters* OperationBorderExportColorTable::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addBorderParameter(1, "border-file", "the input border file");
    
    ret->addStringParameter(2, "table-out", "output - the output text file");//fake output formatting
    
    ret->createOptionalParameter(3, "-class-colors", "use class colors instead of the name colors");
    
    ret->setHelpText(
        AString("Takes the names and colors of each border, and writes it to the same format as -metric-label-import expects.  ") +
        "By default, the borders are colored by border name, specify -class-colors to color them by class instead.  " +
        "The key values start at 1 and follow the order of the borders in the file."
    );
    return ret;
}

int OperationBorderExportColorTable::floatTo255(const float& in)
{
    return (int)floor(in * 255.0f + 0.5f);
}

void OperationBorderExportColorTable::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    BorderFile* myFile = myParams->getBorder(1);
    AString outfileName = myParams->getString(2);
    bool useClassColors = myParams->getOptionalParameter(3)->m_present;
    ofstream outFile(outfileName.toLocal8Bit().constData());
    if (!outFile) throw OperationException("failed to open output text file");
    const GiftiLabelTable* tempTable = NULL;
    if (useClassColors)
    {
        tempTable = myFile->getClassColorTable();
    } else {
        tempTable = myFile->getNameColorTable();
    }
    int numBorders = myFile->getNumberOfBorders();
    for (int i = 0; i < numBorders; ++i)
    {
        const Border* thisBorder = myFile->getBorder(i);
        outFile << thisBorder->getName() << endl;
        outFile << i + 1 << " ";//NOTE: NEVER use the label key
        const GiftiLabel* tempLabel = NULL;
        if (useClassColors)
        {
            tempLabel = tempTable->getLabelBestMatching(thisBorder->getClassName());//see BrainOpenGLFixedPipeline::drawSurfaceBorders(Surface* surface)
        } else {
            tempLabel = tempTable->getLabelBestMatching(thisBorder->getName());
        }
        if (tempLabel == NULL)
        {
            outFile << "0 0 0 255" << endl;
        } else {
            outFile << floatTo255(tempLabel->getRed()) << " "
                    << floatTo255(tempLabel->getGreen()) << " "
                    << floatTo255(tempLabel->getBlue()) << " "
                    << floatTo255(tempLabel->getAlpha()) << endl;
        }
        if (!outFile) throw OperationException("error writing to output text file");
    }
}
