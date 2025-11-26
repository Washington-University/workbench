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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFile.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "FileInformation.h"
#include "HistologySlicesFile.h"
#include "MetaVolumeFile.h"
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
#include <set>

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
    
    ret->addStringParameter(2, "extract-folder", "the name of the folder created when the zip file is unzipped");
    
    ret->addStringParameter(3, "zip-file", "out - the zip file that will be created");
    
    OptionalParameter* baseOpt = ret->createOptionalParameter(4, "-base-dir", "specify a directory that all data files are somewhere within, this will become the root of the zipfile's directory structure");
    baseOpt->addStringParameter(1, "directory", "the directory");
    
    ret->createOptionalParameter(5, "-skip-missing", "any missing files will generate only warnings, and the zip file will be created anyway");
    
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
    bool skipMissing = myParams->getOptionalParameter(5)->m_present;
    
    createZipFile(myProgObj,
                  myParams->getString(1),
                  specFileName,
                  outputSubDirectory,
                  zipFileName,
                  myBaseDir,
                  PROGRESS_COMMAND_LINE,
                  skipMissing);
}

void OperationZipSpecFile::createZipFile(ProgressObject* /*myProgObj*/,
                                         const AString& inputSpecFileName,
                                         const AString& specFileName,
                                         const AString& outputSubDirectory,
                                         const AString& zipFileName,
                                         const AString& myBaseDirIn,
                                         const ProgressMode progressMode,
                                         const bool skipMissing)
{
    AString myBaseDir(myBaseDirIn);
    if (!myBaseDir.endsWith('/'))//root is a special case, if we didn't handle it differently it would end up looking for "//somefile"
    {//this is actually because the path function strips the final "/" from the path, but not when it is just "/"
        myBaseDir += "/";//so, add the trailing slash to the path
    }

    if (outputSubDirectory.isEmpty()) {
        throw OperationException("extract-dir must contain characters");
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
    
    CaretLogFine("Spec Info:"
                 "\n   myParams->getString(1): " + inputSpecFileName
                 + "\n   specFile.getFileName(): " + specFile.getFileName()
                 + "\n   specFileName: " + specFileName
                 + "\n   specPath: " + specPath);
    
    /*
     * Verify that all data files exist
     */
    std::set<AString> allChildDataFileNames;
    AString missingDataFileNames;
    AString outsideBaseDirFiles;
    /* Files in vector may change so need to check size every iteration */
    for (int32_t i = 0; i < static_cast<int32_t>(allDataFileNames.size()); i++) {
        CaretAssertVectorIndex(allDataFileNames, i);
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
            CaretAssertVectorIndex(allDataFileNames, i);
            CaretLogFine("Outside Path: "
                         "\n   myBaseDir: " + myBaseDir
                         + "\n   allDataFileNames[i]: " + allDataFileNames[i]
                         + "\n   dataFileName: " + dataFileName
                         + "\n   absFilePath: " + dataFileInfo.getAbsoluteFilePath()
                         + "\n   absName: " + absName);
            outsideBaseDirFiles += absName + "\n";
        }
        if (dataFileInfo.exists() == false) {
            missingDataFileNames += absName + "\n";
        }
        CaretAssertVectorIndex(allDataFileNames, i);
        allDataFileNames[i] = absName;//so we don't have to do this again
        
        std::vector<AString> childDataFileNames;
        addChildDataFiles(allDataFileNames[i],
                          childDataFileNames);
        allChildDataFileNames.insert(childDataFileNames.begin(),
                                     childDataFileNames.end());
    }
    if (!missingDataFileNames.isEmpty())
    {
        if (skipMissing)
        {
            CaretLogWarning("These data files do not exist and will be skipped:\n" + missingDataFileNames);
        } else {
            throw OperationException("These data files do not exist:\n" + missingDataFileNames);
        }
    }
    if (!outsideBaseDirFiles.isEmpty())
    {
        throw OperationException("These data files lie outside the base directiory:\n" + outsideBaseDirFiles + "Try using -base-dir.");
    }
    
    /*
     * Include names of any child data files so that they get zipped
     */
    if ( ! allChildDataFileNames.empty()) {
        allDataFileNames.insert(allDataFileNames.end(),
                                allChildDataFileNames.begin(),
                                allChildDataFileNames.end());
    }
    
    switch (progressMode) {
        case PROGRESS_COMMAND_LINE:
            break;
        case PROGRESS_GUI_EVENT:
            EventProgressUpdate progressEvent(0, allDataFileNames.size(), 0, "Creating ZIP File");
            EventManager::get()->sendEvent(progressEvent.getPointer());
            break;
    }

    /*
     * Create the ZIP file
     */
    QFile zipFileObject(zipFileName);
    zipFileObject.remove();//delete it if it exists, to play better with file symlinks
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
    static const char *myUnits[9] = {" B    ", " KB", " MB", " GB", " TB", " PB", " EB", " ZB", " YB"};
    for (int32_t i = 0; i < static_cast<int32_t>(allDataFileNames.size()); i++) {
        CaretAssertVectorIndex(allDataFileNames, i);
        AString dataFileName = allDataFileNames[i];
        AString unzippedDataFileName = outputSubDirectory + "/" + dataFileName.mid(myBaseDir.size());//we know the string matches to the length of myBaseDir, and is cleaned, so we can just chop the right number of characters off
        QFile dataFileIn(dataFileName);
        if (dataFileIn.open(QFile::ReadOnly) == false) {
            if (skipMissing)
            {
                continue;
            } else {
                errorMessage = "Unable to open \""
                                    + dataFileName
                                    + "\" for reading: "
                                    + dataFileIn.errorString();
                break;
            }
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
        
        switch (progressMode) {
            case PROGRESS_COMMAND_LINE:
                break;
            case PROGRESS_GUI_EVENT:
            {
                EventProgressUpdate progressEvent(0, allDataFileNames.size(), 0, "Creating ZIP File");
                progressEvent.setProgress(i,
                                          ("Adding "
                                           + (QString::number(i) + " of " + QString::number(static_cast<int32_t>(allDataFileNames.size())) + " (")
                                           + ((unit > 0) ? AString::number(fileSize, 'f', 2) : AString::number(fileSize))
                                           + myUnits[unit]
                                           + ") "
                                           + FileInformation(unzippedDataFileName).getFileName()));
                EventManager::get()->sendEvent(progressEvent.getPointer());
            }
                break;
        }

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
        vector<char> buffer(BUFFER_SIZE);
        
        while (dataFileIn.atEnd() == false) {
            const qint64 numRead = dataFileIn.read(buffer.data(), BUFFER_SIZE);
            if (numRead < 0)
            {
                errorMessage = "Error reading from data file";
                break;
            }
            if (numRead > 0) {
                qint64 result = dataFileOut.write(buffer.data(), numRead);
                if (result != numRead)
                {
                    errorMessage = "Error writing to zip file";
                    break;
                }
            }
        }
        if (!errorMessage.isEmpty()) break;
        
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

/**
 * Get names of any child data files (files not in spec file)
 * @param dataFileName
 *    Name of data file.
 * @param childDataFileNamesOut
 *    Output containing names of child data files
 */
void
OperationZipSpecFile::addChildDataFiles(const AString& dataFileName,
                                        std::vector<AString>& childDataFileNamesOut)
{
    bool validFlag(false);
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(dataFileName,
                                                                                    &validFlag);
    if (validFlag) {
        switch (dataFileType) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::CZI_IMAGE_FILE:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::HISTOLOGY_SLICES:
                try {
                    HistologySlicesFile hsf;
                    hsf.readFile(dataFileName);
                    childDataFileNamesOut = hsf.getChildDataFilePathNames();
                }
                catch (const DataFileException& dfe) {
                    CaretLogWarning("Error reading Histology Slices File to get child data files: "
                                    + dfe.whatString());
                }
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::META_VOLUME:
                try {
                    MetaVolumeFile mvf;
                    mvf.readFile(dataFileName);
                    childDataFileNamesOut = mvf.getChildDataFilePathNames();
                }
                catch (const DataFileException& dfe) {
                    CaretLogWarning("Error reading Meta-Volume File to get child data files: "
                                    + dfe.whatString());
                }
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                break;
            case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SAMPLES:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
            case DataFileTypeEnum::VOLUME:
                break;
            case DataFileTypeEnum::VOLUME_DYNAMIC:
                break;
        }
    }
}
