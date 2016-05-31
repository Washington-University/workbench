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

#include "OperationSurfaceClosestVertex.h"
#include "OperationException.h"

#include "SurfaceFile.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationSurfaceClosestVertex::getCommandSwitch()
{
    return "-surface-closest-vertex";
}

AString OperationSurfaceClosestVertex::getShortDescription()
{
    return "FIND CLOSEST SURFACE VERTEX TO COORDINATES";
}

OperationParameters* OperationSurfaceClosestVertex::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use");
    ret->addStringParameter(2, "coord-list-file", "text file with coordinates");
    ret->addStringParameter(3, "vertex-list-out", "output - the output text file with vertex numbers");//HACK: we don't currently have an "output text file" parameter type, fake the formatting
    ret->setHelpText(
        AString("For each coordinate XYZ triple, find the closest vertex in the surface, and output its vertex number into a text file.  ") +
        "The input file should only use whitespace to separate coordinates (spaces, newlines, tabs), for instance:\n\n" +
        "20 30 25\n30 -20 10"
    );
    return ret;
}

void OperationSurfaceClosestVertex::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString coordFileName = myParams->getString(2);
    fstream coordFile(coordFileName.toLocal8Bit().constData(), fstream::in);
    if (!coordFile.good())
    {
        throw OperationException("error opening coordinate list file for reading");
    }
    AString nodeFileName = myParams->getString(3);
    fstream nodeFile(nodeFileName.toLocal8Bit().constData(), fstream::out);
    if (!nodeFile.good())
    {
        throw OperationException("error opening output file for writing");
    }
    vector<float> coords;
    float x, y, z;
    while (coordFile >> x)//yes, really, thats how they intended it to be used, more or less
    {
        if (!(coordFile >> y >> z))//if we fail to read the rest of the triple, error
        {
            throw OperationException("read incomplete coordinate triple, would have been coordinate number " + AString::number(coords.size() / 3 + 1));
        }
        coords.push_back(x);
        coords.push_back(y);
        coords.push_back(z);
    }//because of how we parse the file, we know that coords contains a multiple of 3
    if (coords.empty())
    {
        throw OperationException("did not find any coordinates in file, make sure you use only whitespace to separate numbers");
    }
    for (int i = 0; i < (int)coords.size(); i += 3)
    {
        int node = mySurf->closestNode(coords.data() + i);
        nodeFile << node << endl;
    }
}
