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

#include "CaretLogger.h"
#include "DataFile.h"
#include "FileInformation.h"
#include "OperationZipSceneFile.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneFile.h"
#include "SpecFile.h"

#include "quazip.h"
#include "quazipfile.h"

#include <QDir>

#include <iostream>
#include <set>

using namespace caret;
using namespace std;

AString OperationZipSceneFile::getCommandSwitch()
{
    return "-zip-scene-file";
}

AString OperationZipSceneFile::getShortDescription()
{
    return "ZIP A SCENE FILE AND ITS DATA FILES";
}

OperationParameters* OperationZipSceneFile::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "scene-file", "the scene file to make the zip file from");
    
    ret->addStringParameter(2, "extract-folder", "the name of the folder created when the zip file is unzipped");
    
    ret->addStringParameter(3, "zip-file", "out - the zip file that will be created");
    
    OptionalParameter* baseOpt = ret->createOptionalParameter(4, "-base-dir", "specify a directory that all data files are somewhere within, this will become the root of the zipfile's directory structure");
    baseOpt->addStringParameter(1, "directory", "the directory");

    ret->setHelpText("If zip-file already exists, it will be overwritten.  "
        "If -base-dir is not specified, the directory containing the scene file is used for the base directory.  "
        "The scene file must contain only relative paths, and no data files may be outside the base directory.");
    return ret;
}

void OperationZipSceneFile::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString sceneFileName = myParams->getString(1);
    AString outputSubDirectory = myParams->getString(2);
    AString zipFileName = myParams->getString(3);
    FileInformation sceneFileInfo(sceneFileName);
    if (outputSubDirectory.isEmpty()) {
        throw OperationException("extract-dir must contain characters");
    }
    OptionalParameter* baseOpt = myParams->getOptionalParameter(4);
    AString myBaseDir;
    if (baseOpt->m_present)
    {
        myBaseDir = QDir::cleanPath(QDir(baseOpt->getString(1)).absolutePath());
    } else {
        myBaseDir = QDir::cleanPath(sceneFileInfo.getAbsolutePath());
    }
    if (!myBaseDir.endsWith('/'))//root is a special case, if we didn't handle it differently it would end up looking for "//somefile"
    {//this is actually because the path function strips the final "/" from the path, but not when it is just "/"
        myBaseDir += "/";//so, add the trailing slash to the path
    }
    AString sceneFilePath = QDir::cleanPath(sceneFileInfo.getAbsoluteFilePath());//resolve filenames to open from the spec file's location, NOT from current directory
    if (!sceneFilePath.startsWith(myBaseDir))
    {
        throw OperationException("scene file lies outside the base directory");
    }
    if (FileInformation(outputSubDirectory).isAbsolute())
    {
        CaretLogWarning("You have specified that the zip file should extract to an absolute path, this is generally frowned on.  "
                        "The <extract-folder> parameter should generally be a string without '/' or '\\' in it.");
    } else {
        if (outputSubDirectory.indexOfAnyChar("/\\") != -1)//assume backslashes work too
        {
            CaretLogWarning("You have specified that the zipfile should create multiple levels of otherwise empty directories "
                            "before the file paths starting from the base directory, this is probably going to be inconvenient.  "
                            "The <extract-folder> parameter should generally be a string without '/' or '\\' in it.");
        }
    }
    set<AString> allFiles;
    allFiles.insert(sceneFilePath);
    SceneFile sceneFile;
    sceneFile.readFile(sceneFileName);
    const int numScenes = sceneFile.getNumberOfScenes();
    for (int i = 0; i < numScenes; ++i)
    {
        Scene* thisScene = sceneFile.getSceneAtIndex(i);
        SceneAttributes* myAttrs = thisScene->getAttributes();
        const SceneClass* guiMgrClass = thisScene->getClassWithName("guiManager");
        if (guiMgrClass == NULL)
        {
            throw OperationException("scene '" + thisScene->getName() + "' is missing guiManager class");
        }
        const SceneClass* sessMgrClass = guiMgrClass->getClass("m_sessionManager");
        if (sessMgrClass == NULL)
        {
            throw OperationException("scene '" + thisScene->getName() + "' is missing m_sessionManager class");
        }
        const SceneClassArray* brainArray = sessMgrClass->getClassArray("m_brains");
        if (brainArray == NULL)
        {
            throw OperationException("scene '" + thisScene->getName() + "' is missing m_brains class array");
        }
        const int numBrainClasses = brainArray->getNumberOfArrayElements();
        for (int j = 0; j < numBrainClasses; ++j)
        {
            const SceneClass* brainClass = brainArray->getClassAtIndex(j);
            const SceneClass* specClass = brainClass->getClass("specFile");
            if (specClass == NULL)
            {
                throw OperationException("scene '" + thisScene->getName() + "' is missing specFile class in m_brains element " + AString::number(j));
            }
            SpecFile tempSpec;
            tempSpec.restoreFromScene(myAttrs, specClass);
            vector<AString> tempNames = tempSpec.getAllDataFileNamesSelectedForLoading();
            int numNames = (int)tempNames.size();
            for (int k = 0; k < numNames; ++k)
            {
                if (DataFile::isFileOnNetwork(tempNames[k]))
                {
                    cout << "skipping network file '" << tempNames[k] << "'" << endl;
                    continue;
                }
                AString thisName = QDir::cleanPath(tempNames[k]);
                if (allFiles.insert(thisName).second)
                {
                    if (FileInformation(thisName).isRelative())
                    {
                        throw OperationException("scene '" + thisScene->getName() + "' contains an unresolved relative path: '" + tempNames[k] + "'");
                    }
                    if (!thisName.startsWith(myBaseDir))
                    {
                        throw OperationException("scene '" + thisScene->getName() + "' contains a file outside the base directory: '" + thisName + "', try using -base-dir");
                    }
                }
            }
        }
    }
    QFile zipFileObject(zipFileName);
    QuaZip zipFile(&zipFileObject);
    if (!zipFile.open(QuaZip::mdCreate))
    {
        throw OperationException("Unable to open ZIP File \""
                                 + zipFileName
                                 + "\" for writing.");
    }
    static const char *myUnits[9] = {" B    ", " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB"};
    for (set<AString>::iterator iter = allFiles.begin(); iter != allFiles.end(); ++iter)
    {
        AString dataFileName = *iter;
        AString unzippedDataFileName = outputSubDirectory + "/" + dataFileName.mid(myBaseDir.size());//we know the string matches to the length of myBaseDir, and is cleaned, so we can just chop the right number of characters off
        QFile dataFileIn(dataFileName);
        if (!dataFileIn.open(QFile::ReadOnly)) {
            throw OperationException("Unable to open \"" + dataFileName + "\" for reading: " + dataFileIn.errorString());
        }
        float fileSize = (float)dataFileIn.size();
        int unit = 0;
        while (unit < 8 && fileSize >= 1000.0f)//don't let there be 4 digits to the left of decimal point
        {
            ++unit;
            fileSize /= 1000.0f;//use GB and friends, not GiB
        }
        if (unit > 0)
        {
            cout << AString::number(fileSize, 'f', 2);
        } else {
            cout << AString::number(fileSize);
        }
        cout << myUnits[unit] << "     \t" << unzippedDataFileName;
        cout.flush();//don't endl until it finishes
        
        QuaZipNewInfo zipNewInfo(unzippedDataFileName,
                                 dataFileName);
        zipNewInfo.externalAttr |= (6 << 22L) | (6 << 19L) | (4 << 16L);//make permissions 664
        
        QuaZipFile dataFileOut(&zipFile);
        if (!dataFileOut.open(QIODevice::WriteOnly, zipNewInfo)) {
            throw OperationException("Unable to open zip output for \"" + dataFileName + "\"");
        }
        
        const qint64 BUFFER_SIZE = 1024 * 1024;
        vector<char> buffer(BUFFER_SIZE);
        
        while (dataFileIn.atEnd() == false) {
            const qint64 numRead = dataFileIn.read(buffer.data(), BUFFER_SIZE);
            if (numRead < 0) throw OperationException("Error reading from data file");
            if (numRead > 0) {
                qint64 result = dataFileOut.write(buffer.data(), numRead);
                if (result != numRead) throw OperationException("Error writing to zip file");
            }
        }
        
        dataFileIn.close();
        dataFileOut.close();
        cout << endl;
    }
    zipFile.close();
}
