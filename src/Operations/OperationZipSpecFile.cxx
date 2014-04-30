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

#include "DataFile.h"
#include "FileInformation.h"
#include "OperationZipSpecFile.h"
#include "OperationException.h"
#include "SpecFile.h"

#include "quazip.h"
#include "quazipfile.h"

//for cleanPath
#include <QDir>

//to print file sizes as it makes the zip
#include <iostream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationZipSpecFile::getCommandSwitch()
{
    return "-zip-spec-file";
}

AString OperationZipSpecFile::getShortDescription()
{
    return "ZIP A SPEC FILE AND ITS DATA FILES";
}

OperationParameters* OperationZipSpecFile::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "spec-file", "the specification file to add to zip file");
    
    ret->addStringParameter(2, "extract-dir", "the directory created when the zip file is unzipped");
    
    ret->addStringParameter(3, "zip-file", "out - the zip file that will be created");
    
    OptionalParameter* baseOpt = ret->createOptionalParameter(4, "-base-dir", "specify a directory that all data files are somewhere within");
    baseOpt->addStringParameter(1, "directory", "the directory that will become the root of the zipfile's directory structure");
    
    ret->setHelpText(AString("If zip-file already exists, it will be overwritten.  ") +
        "If -base-dir is not specified, the directory containing the spec file is used for the base directory.  " +
        "The spec file must contain only relative paths, and no data files may be outside the base directory.  " +
        "Scene files inside spec files are not checked for what files they reference, ensure that all data files referenced by the scene files are also referenced by the spec file.");
    return ret;
}

void OperationZipSpecFile::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString specFileName = FileInformation(myParams->getString(1)).getAbsoluteFilePath();
    AString outputSubDirectory = myParams->getString(2);
    AString zipFileName = FileInformation(myParams->getString(3)).getAbsoluteFilePath();
    OptionalParameter* baseOpt = myParams->getOptionalParameter(4);
    AString myBaseDir;
    if (baseOpt->m_present)
    {
        myBaseDir = QDir::cleanPath(QDir(baseOpt->getString(1)).absolutePath());
    } else {
        FileInformation specFileInfo(specFileName);
        myBaseDir = QDir::cleanPath(specFileInfo.getAbsolutePath());
    }
    if (!myBaseDir.endsWith('/'))//root is a special case, if we didn't handle it differently it would end up looking for "//somefile"
    {//this is actually because the path function strips the final "/" from the path, but not when it is just "/"
        myBaseDir += "/";//so, add the trailing slash to the path
    }

    if (outputSubDirectory.isEmpty()) {
        throw OperationException("extract-dir must contain characters");
    }
    
    /*
     * Verify that ZIP file DOES NOT exist
     */
    /*FileInformation zipFileInfo(zipFileName);
    if (zipFileInfo.exists()) {
        throw OperationException("ZIP file \""
                                 + zipFileName
                                 + "\" exists and this command will not overwrite the ZIP file.");
    }//*/ //TSC: this is annoying, and all other commands overwrite existing output files without warning
    
    /*
     * Read the spec file and get the names of its data files.
     * Look for any files that are missing (name in spec file
     * but file not found).
     */
    FileInformation specFileInfo(specFileName);
    AString specPath = QDir::cleanPath(specFileInfo.getAbsolutePath());
    if (!specPath.endsWith('/'))
    {
        specPath += "/";
    }
    SpecFile specFile;
    specFile.readFile(specFileName);
    std::vector<AString> allDataFileNames = specFile.getAllDataFileNames();
    allDataFileNames.push_back(specFileName);
    
    /*
     * Verify that all data files exist
     */
    AString missingDataFileNames;
    AString outsideBaseDirFiles;
    const int32_t numberOfDataFiles = static_cast<int32_t>(allDataFileNames.size());
    for (int32_t i = 0; i < numberOfDataFiles; i++) {
        AString dataFileName = allDataFileNames[i];
        if (DataFile::isFileOnNetwork(dataFileName))
        {
            cout << "skipping network file '" << dataFileName << "'" << endl;
            allDataFileNames.erase(allDataFileNames.begin() + i);//remove it from the list
            --i;//decrement i in order not to skip anything
            continue;
        }
        FileInformation tempInfo(dataFileName);
        if (tempInfo.isRelative())
        {
            dataFileName = specPath + dataFileName;
        }
        FileInformation dataFileInfo(dataFileName);
        AString absName = QDir::cleanPath(dataFileInfo.getAbsoluteFilePath());
        if (!absName.startsWith(myBaseDir))
        {
            outsideBaseDirFiles += absName + "\n";
        }
        if (dataFileInfo.exists() == false) {
            missingDataFileNames += absName + "\n";
        }
        allDataFileNames[i] = absName;//so we don't have to do this again
    }
    if (!missingDataFileNames.isEmpty())
    {
        throw OperationException("These data files do not exist:\n" + missingDataFileNames);
    }
    if (!outsideBaseDirFiles.isEmpty())
    {
        throw OperationException("These data files lie outside the base directiory:\n" + outsideBaseDirFiles + "Try using -base-dir.");
    }
    
    /*
     * Create the ZIP file
     */
    QFile zipFileObject(zipFileName);
    QuaZip zipFile(&zipFileObject);
    if (zipFile.open(QuaZip::mdCreate) == false) {
        throw OperationException("Unable to open ZIP File \""
                                 + zipFileName
                                 + "\" for writing.");
    }
    
    /*
     * Compress each of the files and add them to the zip file
     */
    AString errorMessage;
    static const char *myUnits[9] = {" B", " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB"};
    for (int32_t i = 0; i < numberOfDataFiles; i++) {
        AString dataFileName = allDataFileNames[i];
        AString unzippedDataFileName = outputSubDirectory + "/" + dataFileName.mid(myBaseDir.size());//we know the string matches to the length of myBaseDir, and is cleaned, so we can just chop the right number of characters off
        QFile dataFileIn(dataFileName);
        if (dataFileIn.open(QFile::ReadOnly) == false) {
            errorMessage = "Unable to open \""
                                   + dataFileName
                                   + "\" for reading: "
                                   + dataFileIn.errorString();
            break;
        }
        int64_t fileSize = (float)dataFileIn.size() * 10;//fixed point, 1 decimal place
        int unit = 0;
        int64_t divisor = 1;
        while (unit < 8 && fileSize / divisor > 9998)//don't let there be 4 digits to the left of decimal point
        {
            ++unit;
            divisor *= 1024;//don't round until we decide on a divisor
        }
        int fixedpt = (fileSize + divisor / 2) / divisor;
        int ipart = fixedpt / 10;
        int fpart = fixedpt % 10;
        cout << ipart;
        if (unit > 0) cout << "." << fpart;
        cout << myUnits[unit] << "     \t" << unzippedDataFileName;
        cout.flush();//don't endl until it finishes
        
        QuaZipNewInfo zipNewInfo(unzippedDataFileName,
                                 dataFileName);
        zipNewInfo.externalAttr |= (6 << 22L) | (6 << 19L) | (4 << 16L);//make permissions 664
        
        QuaZipFile dataFileOut(&zipFile);
        if (dataFileOut.open(QIODevice::WriteOnly,
                             zipNewInfo) == false) {
            errorMessage = "Unable to open zip output for \""
                            + dataFileName
                            + "\"";
            break;
        }
        
        const qint64 BUFFER_SIZE = 1024 * 1024;
        char buffer[BUFFER_SIZE];
        
        while (dataFileIn.QIODevice::atEnd() == false) {
            const qint64 numRead = dataFileIn.read(buffer, BUFFER_SIZE);
            if (numRead > 0) {
                dataFileOut.write(buffer, numRead);
            }
        }
        
        dataFileIn.close();
        dataFileOut.close();
        cout << endl;
    }
    
    /*
     * Close the zip file
     */
    zipFile.close();
    
    /*
     * If there are errors, remove the ZIP file and
     * indicate an error has occurred.
     */
    if (errorMessage.isEmpty() == false) {
        QFile::remove(zipFileName);
        
        throw OperationException(errorMessage);
    }
}
