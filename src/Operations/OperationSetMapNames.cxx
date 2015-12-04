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

#include "OperationSetMapNames.h"
#include "OperationException.h"
#include "CaretDataFile.h"
#include "CaretDataFileHelper.h"
#include "CaretMappableDataFile.h"
#include "CaretPointer.h"
#include "CaretLogger.h"

#include <fstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationSetMapNames::getCommandSwitch()
{
    return "-set-map-names";
}

AString OperationSetMapNames::getShortDescription()
{
    return "SET THE NAME OF ONE OR MORE MAPS IN A FILE";
}

OperationParameters* OperationSetMapNames::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "data-file", "the file to set the map names of");
    
    OptionalParameter* fileOpt = ret->createOptionalParameter(2, "-name-file", "use a text file to replace all map names");
    fileOpt->addStringParameter(1, "file", "text file containing map names, one per line");
    
    ParameterComponent* mapOpt = ret->createRepeatableParameter(3, "-map", "specify a map to set the name of");
    mapOpt->addIntegerParameter(1, "index", "the map index to change the name of");
    mapOpt->addStringParameter(2, "new-name", "the name to set for the map");
    
    ret->setHelpText(
        AString("Sets the name of one or more maps for metric, shape, label, volume, cifti scalar or cifti label files.  ") +
        "If the -name-file option is not specified, the -map option must be specified at least once.  " +
        "The -map option cannot be used when -name-file is specified."
    );
    return ret;
}

void OperationSetMapNames::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString fileName = myParams->getString(1);
    OptionalParameter* fileOpt = myParams->getOptionalParameter(2);
    const vector<ParameterComponent*>& mapOpts = *(myParams->getRepeatableParameterInstances(3));
    CaretPointer<CaretDataFile> caretDataFile(CaretDataFileHelper::readAnyCaretDataFile(fileName));
    CaretMappableDataFile* mappableFile = dynamic_cast<CaretMappableDataFile*>(caretDataFile.getPointer());
    if (mappableFile == NULL) throw OperationException("cannot set map names on this file type");
    if (fileOpt->m_present)
    {
        if (!mapOpts.empty()) throw OperationException("-map may not be specified when using -name-file");
        AString listfileName = fileOpt->getString(1);
        ifstream nameListFile(listfileName.toLocal8Bit().constData());
        if (!nameListFile.good())
        {
            throw OperationException("error reading name list file");
        }
        string mapName;
        int numMaps = mappableFile->getNumberOfMaps();
        for (int i = 0; i < numMaps; ++i)
        {
            getline(nameListFile, mapName);
            if (!nameListFile)
            {
                throw OperationException("name file contained " + AString::number(i) + " names, expected " + AString::number(numMaps));
                break;
            }
            mappableFile->setMapName(i, mapName.c_str());
        }
        if (getline(nameListFile, mapName) && mapName != "")
        {//accept a blank line as not being another name
            throw OperationException("name file contains more names than can be used on the file");
        }
        if (getline(nameListFile, mapName))
        {//don't accept two or more additional lines, period
            throw OperationException("name file contains more names than can be used on the file");
        }
    } else {
        if (mapOpts.empty()) throw OperationException("you must specify at least one option that sets a map name");
        for (int i = 0; i < (int)mapOpts.size(); ++i)
        {
            int mapIndex = (int)mapOpts[i]->getInteger(1) - 1;
            if (mapIndex < 0) throw OperationException("invalid map index, indices are 1-based");
            if (mapIndex >= mappableFile->getNumberOfMaps()) throw OperationException("invalid map index, file doesn't have enough maps");
            AString newName = mapOpts[i]->getString(2);
            mappableFile->setMapName(mapIndex, newName);
        }
    }
    mappableFile->writeFile(fileName);
}
