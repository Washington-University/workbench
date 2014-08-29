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

#include "OperationFociListCoords.h"
#include "OperationException.h"

#include "FociFile.h"
#include "Focus.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationFociListCoords::getCommandSwitch()
{
    return "-foci-list-coords";
}

AString OperationFociListCoords::getShortDescription()
{
    return "OUTPUT FOCI COORDINATES IN A TEXT FILE";
}

OperationParameters* OperationFociListCoords::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addFociParameter(1, "foci-file", "input foci file");
    ret->addStringParameter(2, "coord-file-out", "output - the output coordinate text file");
    OptionalParameter* namesOpt = ret->createOptionalParameter(3, "-names-out", "output the foci names");
    namesOpt->addStringParameter(1, "names-file-out", "output - text file to put foci names in");
    ret->setHelpText(
        AString("Output the coordinates for every focus in the foci file, and optionally the focus names in a second text file.")
    );
    return ret;
}

void OperationFociListCoords::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    FociFile* myFoci = myParams->getFoci(1);//gets the surface with key 1
    AString coordFileName = myParams->getString(2);
    OptionalParameter* namesOpt = myParams->getOptionalParameter(3);
    bool outputNames = false;
    AString nameFileName;
    if (namesOpt->m_present)
    {
        outputNames = true;
        nameFileName = namesOpt->getString(1);
    }
    ofstream coordFile(coordFileName.toLocal8Bit().constData());
    if (!coordFile.good())
    {
        throw OperationException("failed to open coordinate output file for writing");
    }
    fstream nameFile;
    if (outputNames)
    {
        nameFile.open(nameFileName.toLocal8Bit().constData(), fstream::out);
        if (!nameFile.good())
        {
            throw OperationException("failed to open name output file for writing");
        }
    }
    int numFoci = myFoci->getNumberOfFoci();
    for (int i = 0; i < numFoci; ++i)
    {
        const Focus* myFocus = myFoci->getFocus(i);
        const float* coords = myFocus->getSearchXYZ();
        coordFile << coords[0] << " " << coords[1] << " " << coords[2] << endl;
        if (outputNames)
        {
            nameFile << myFocus->getName() << endl;
        }
    }
}
