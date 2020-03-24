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

#include "OperationFociCreate.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "SurfaceProjector.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

using namespace caret;
using namespace std;

AString OperationFociCreate::getCommandSwitch()
{
    return "-foci-create";
}

AString OperationFociCreate::getShortDescription()
{
    return "CREATE A FOCI FILE";
}

OperationParameters* OperationFociCreate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addFociOutputParameter(1, "output", "the output foci file");
    
    ParameterComponent* fociOpt = ret->createRepeatableParameter(2, "-class", "specify class input data");
    fociOpt->addStringParameter(1, "class-name", "name of class");
    fociOpt->addStringParameter(2, "foci-list-file", "text file containing foci names, coordinates, and colors");
    fociOpt->addSurfaceParameter(3, "surface", "surface file for projection of foci list file");
    
    ret->setHelpText(
        AString("Creates a foci file from names, coordinates, and RGB values in a text file.  ") +
        "The text file must have the following format (2 lines per focus):\n\n" +
        "<focus-name>\n"
        "<red> <green> <blue> <x> <y> <z> \n...\n\n" +
        "Foci names are specified on a separate line from their coordinates and color, in order to let foci names contain spaces.  " +
        "Whitespace is trimmed from both ends of the foci name, but is kept if it is in the middle of a name.  " +
        "The values of <red>, <green>, <blue> and must be integers from 0 to 255, and will specify the color the foci is drawn as.\n\n" +
        "Foci are grouped into classes and the name for the class is specified using the <class-name> parameter.\n\n" +
        "All foci within one text file must be associated with the structure contained in the <surface> "
                     "parameter and are projected to that surface."
    );
    return ret;
}

void OperationFociCreate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    
    FociFile* outputFociFile = myParams->getOutputFoci(1);

    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) {
        throw OperationException("no inputs specified");
    }

    for (int32_t i = 0; i < numInputs; i++) {
        const AString className = myInputs[i]->getString(1).trimmed();
        if (className.isEmpty()) {
            throw OperationException("Class name may not be empty");
        }
        const AString listFileName = myInputs[i]->getString(2).trimmed();
        if (listFileName.isEmpty()) {
            throw OperationException("Foci list (text) file name may not be empty");
        }
        const SurfaceFile* surfaceFile = myInputs[i]->getSurface(3);
        
        SurfaceProjector projector(surfaceFile);
        
        FileInformation textFileInfo(listFileName);
        if (!textFileInfo.exists())
        {
            throw OperationException("foci list file doesn't exist: "
                                     + listFileName);
        }
        fstream fociListFile(listFileName.toLocal8Bit().constData(), fstream::in);
        if (!fociListFile.good())
        {
            throw OperationException("error reading foci list file:"
                                     + listFileName);
        }
        
        string focusName;
        float x, y, z;
        int32_t red, green, blue;
        int fociCount = 0;
        while (fociListFile.good())
        {
            ++fociCount;//just for error messages, so start at 1
            getline(fociListFile, focusName);
            if (fociListFile.eof() && focusName == "") break;//if end of file trying to read an int, and label name is empty, its really just end of file
            fociListFile >> red;
            fociListFile >> green;
            fociListFile >> blue;
            fociListFile >> x;
            fociListFile >> y;
            
            if (!(fociListFile >> z))//yes, that is seriously the correct way to check if input was successfully extracted...so much fail
            {
                throw OperationException("foci list file is malformed for entry #" + AString::number(fociCount) + ": " + AString(focusName.c_str()));
            }
            if (red < 0 || red > 255)
            {
                throw OperationException("bad value for red for entry #" + AString::number(fociCount) + ", " + AString(focusName.c_str()) + ": " + AString::number(red));
            }
            if (green < 0 || green > 255)
            {
                throw OperationException("bad value for green for entry #" + AString::number(fociCount) + ", " + AString(focusName.c_str()) + ": " + AString::number(green));
            }
            if (blue < 0 || blue > 255)
            {
                throw OperationException("bad value for blue for entry #" + AString::number(fociCount) + ", " + AString(focusName.c_str()) + ": " + AString::number(blue));
            }
            while (isspace(fociListFile.peek()))
            {
                fociListFile.ignore();//drop the newline, possible carriage return or other whitespace so that getline doesn't get nothing, and cause int extraction to fail
            }
            
            Focus* focus = new Focus();
            focus->setClassName(className);
            focus->setName(AString(focusName.c_str()).trimmed());
            
            const float xyz[3] = { x, y, z };
            CaretAssert(focus->getNumberOfProjections() > 0);
            focus->getProjection(0)->setStereotaxicXYZ(xyz);
            
            const int32_t dummyFocusIndex(-1);
            projector.projectFocus(dummyFocusIndex,
                                   focus);
            
            const GiftiLabel* colorLabel = outputFociFile->getNameColorTable()->getLabel(focus->getName());
            if (colorLabel != NULL) {
                if ((colorLabel->getRed() != red)
                    || (colorLabel->getGreen() != green)
                    || (colorLabel->getBlue() != blue)) {
                    CaretLogWarning("More than one color for focus named \""
                                    + focus->getName()
                                    + "\".  (All foci with same name use same color)");
                }
            }
            outputFociFile->getNameColorTable()->addLabel(focus->getName(),
                                                          red, green, blue);
            outputFociFile->addFocus(focus);
        }
    }
    
    if (outputFociFile->getNumberOfFoci() <= 0) {
        throw OperationException("No foci were successfully read/projected.");
    }
}
