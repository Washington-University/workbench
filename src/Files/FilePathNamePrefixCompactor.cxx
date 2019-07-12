
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __FILE_PATH_NAME_PREFIX_COMPACTOR_DECLARE__
#include "FilePathNamePrefixCompactor.h"
#undef __FILE_PATH_NAME_PREFIX_COMPACTOR_DECLARE__

#include <algorithm>
#include <limits>
#include <QStringList>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::FilePathNamePrefixCompactor 
 * \brief Remove matching prefixes from a group of file names.
 * \ingroup Files
 *
 * In the graphical user-interface names of files with a full path
 * are frequently presented to the user.  In some instances, these
 * paths may be very long, too long to display in the user-interface.
 * One solution is to place the path after the name of the file.
 * However, there are instance in which there are file with identical
 * names but different paths and when the paths are long, the user 
 * may not see the entire path and is unable to fully identify 
 * a file.
 * 
 * The static methods in this class are used to remove the matching
 * prefixes (first <some number> of characters) from the path
 * of each file.  Do so, allows the user to see the name of the file
 * followed by the unique portion of the file's path.
 */

/**
 * Constructor.
 */
FilePathNamePrefixCompactor::FilePathNamePrefixCompactor()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
FilePathNamePrefixCompactor::~FilePathNamePrefixCompactor()
{
}

/**
 * Create names that show the filename followed by the path BUT remove
 * any matching prefix from the paths for a group of CaretDataFiles.
 *
 * Example Input File Names:
 *    /mnt/myelin/data/subject2/rsfmri/activity.dscalar.nii
 *    /mnt/myelin/data/subject1/rsfmri/activity.dscalar.nii
 * Output:
 *    actitivity.dscalar.nii (../subject2/rsfmri)
 *    actitivity.dscalar.nii (../subject1/rsfmri)
 *
 * @param caretMappableDataFiles
 *     The caret mappable data files from which names are obtained.
 * @param prefixRemovedNamesOut
 *     Names of files with matching prefixes removed.  Number of elements
 *     will match the number of elements in caretDataFiles.
 */
void
FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(const std::vector<CaretMappableDataFile*>& caretMappableDataFiles,
                                                                        std::vector<AString>& prefixRemovedNamesOut)
{
    std::vector<CaretDataFile*> caretDataFiles;
    for (std::vector<CaretMappableDataFile*>::const_iterator iter = caretMappableDataFiles.begin();
         iter != caretMappableDataFiles.end();
         iter++) {
        caretDataFiles.push_back(*iter);
    }
    removeMatchingPathPrefixFromCaretDataFiles(caretDataFiles,
                                               prefixRemovedNamesOut);
}

/**
 * Create names that show the filename followed by the path BUT remove
 * any matching prefix from the paths for a group of CaretDataFiles.
 *
 * Example Input File Names:
 *    /mnt/myelin/data/subject2/rsfmri/activity.dscalar.nii
 *    /mnt/myelin/data/subject1/rsfmri/activity.dscalar.nii
 * Output:
 *    actitivity.dscalar.nii (../subject2/rsfmri)
 *    actitivity.dscalar.nii (../subject1/rsfmri)
 *
 * @param caretDataFiles
 *     The caret data files from which names are obtained.
 * @param prefixRemovedNamesOut
 *     Names of files with matching prefixes removed.  Number of elements
 *     will match the number of elements in caretDataFiles.
 */
void
FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(const std::vector<CaretDataFile*>& caretDataFiles,
                                                                        std::vector<AString>& prefixRemovedNamesOut)
{
    std::vector<AString> filePathNames;
    std::vector<AString> fileNames;
    std::vector<AString> specialPrefixes;
    for (std::vector<CaretDataFile*>::const_iterator iter = caretDataFiles.begin();
         iter != caretDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        CaretAssert(cdf);
        
        AString fileSpecialPrefix;
        
        switch (cdf->getDataFileType()) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                fileSpecialPrefix = "dynconn - ";
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::METRIC_DYNAMIC:
                fileSpecialPrefix = "metricdynconn - ";
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
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
                fileSpecialPrefix = "voldynconn - ";
                break;
        }
        specialPrefixes.push_back(fileSpecialPrefix);

        filePathNames.push_back(cdf->getFileName());
        fileNames.push_back(cdf->getFileNameNoPath());
    }
    
    CaretAssert(filePathNames.size() == fileNames.size());
    CaretAssert(filePathNames.size() == specialPrefixes.size());
    
    const bool onlyFilesWithMatchingFileNamesFlag(true);
    if (onlyFilesWithMatchingFileNamesFlag) {
        const int32_t numFiles = static_cast<int32_t>(filePathNames.size());
        std::vector<bool> cleanedFlag(numFiles, false);
        
        /*
         * Initialize output to just file names (no paths)
         */
        prefixRemovedNamesOut = fileNames;
        
        for (int32_t i = 0; i < (numFiles - 1); i++) {
            CaretAssertVectorIndex(cleanedFlag, i);
            if (cleanedFlag[i]) {
                continue;
            }
            
            CaretAssertVectorIndex(fileNames, i);
            const AString& name = fileNames[i];
            
            std::vector<int32_t> duplicateFileIndices;
            std::vector<AString> duplicateNameFiles;
            duplicateFileIndices.push_back(i);
            CaretAssertVectorIndex(filePathNames, i);
            duplicateNameFiles.push_back(filePathNames[i]);
            
            /*
             * Find files with same name (excluding path)
             */
            for (int32_t j = i + 1; j < numFiles; j++) {
                CaretAssertVectorIndex(cleanedFlag, j);
                if (cleanedFlag[j]) {
                    continue;
                }
                
                CaretAssertVectorIndex(fileNames, j);
                if (name == fileNames[j]) {
                    duplicateFileIndices.push_back(j);
                    CaretAssertVectorIndex(filePathNames, j);
                    duplicateNameFiles.push_back(filePathNames[j]);
                }
            }
            
            /*
             * If multiple files with same same, remove the matching paths
             */
            std::vector<AString> cleanedFileNames;
            if (duplicateNameFiles.size() > 1) {
                removeMatchingPathPrefixFromFileNames(duplicateNameFiles,
                                                      cleanedFileNames);
                
                /*
                 * Update the name with prefix removed for output
                 */
                CaretAssert(duplicateFileIndices.size() == cleanedFileNames.size());
                const int32_t numCleanedFiles = static_cast<int32_t>(cleanedFileNames.size());
                for (int32_t m = 0; m < numCleanedFiles; m++) {
                    CaretAssertVectorIndex(duplicateFileIndices, m);
                    const int32_t indx = duplicateFileIndices[m];
                    CaretAssertVectorIndex(cleanedFileNames, m);
                    prefixRemovedNamesOut[indx] = cleanedFileNames[m];
                    CaretAssertVectorIndex(cleanedFlag, indx);
                    cleanedFlag[indx] = true;
                }
            }
        }
    }
    else {
        /*
         * Running this will operate on all files as if all of the them have the same file name
         */
        removeMatchingPathPrefixFromFileNames(filePathNames,
                                              prefixRemovedNamesOut);
    }

    const int32_t numFiles = static_cast<int32_t>(prefixRemovedNamesOut.size());
    CaretAssert(numFiles == static_cast<int32_t>(specialPrefixes.size()));
    for (int32_t i = 0; i < numFiles; i++) {
        prefixRemovedNamesOut[i].insert(0, specialPrefixes[i]);
    }
}

/**
 * Create names that show the filename followed by the path BUT remove
 * any matching prefix from the paths for a group of file names.
 *
 * Example Input File Names:
 *    /mnt/myelin/data/subject2/rsfmri/activity.dscalar.nii
 *    /mnt/myelin/data/subject1/rsfmri/activity.dscalar.nii
 * Output:
 *    actitivity.dscalar.nii (../subject2/rsfmri)
 *    actitivity.dscalar.nii (../subject1/rsfmri)
 *
 * @param caretDataFiles
 *     The caret data files from which names are obtained.
 * @param prefixRemovedNamesOut
 *     Names of files with matching prefixes removed.  Number of elements
 *     will match the number of elements in caretDataFiles.
 */
void
FilePathNamePrefixCompactor::removeMatchingPathPrefixFromFileNames(const std::vector<AString>& fileNames,
                                                  std::vector<AString>& prefixRemovedNamesOut)
{
    prefixRemovedNamesOut.clear();
    
    const int32_t numFiles = static_cast<int32_t>(fileNames.size());
    if (numFiles == 1) {
        FileInformation fileInfo(fileNames[0]);
        prefixRemovedNamesOut.push_back(fileInfo.getFileName());
        return;
    }
    else if (numFiles < 1){
        return;
    }
    
    std::vector<std::vector<AString> > pathComponentEachFile;
    int32_t mininumComponentCount = std::numeric_limits<int32_t>::max();
    
    /*
     * For each file, split its path components (parts between '/') and
     * place them into a vector.  Also find the minimum number from
     * all of the paths as that will be the maximum number of matching
     * components.
     */
    for (std::vector<AString>::const_iterator iter = fileNames.begin();
         iter != fileNames.end();
         iter++) {
        FileInformation fileInfo(*iter);
        const QString path = fileInfo.getPathName();
        
        QStringList pathComponentsList = path.split('/');
        mininumComponentCount = std::min(mininumComponentCount,
                                         pathComponentsList.size());
        
        
        std::vector<AString> pathComponents;
        for (int32_t ip = 0; ip < pathComponentsList.size(); ip++) {
            pathComponents.push_back(pathComponentsList[ip]);
        }
        pathComponentEachFile.push_back(pathComponents);
    }
    
    CaretAssert(static_cast<int32_t>(pathComponentEachFile.size()) == numFiles);
    
    /*
     * For each of the file names, examine and compare its path 
     * components to all of the other file path components to
     * determine the matching prefix.
     */
    int32_t numMatchingLeadingComponents = 0;
    for (int32_t iComp = 0; iComp < mininumComponentCount; iComp++) {
        CaretAssertVectorIndex(pathComponentEachFile, 0);
        const std::vector<AString>& firstFileCompontents = pathComponentEachFile[0];
        const AString component = firstFileCompontents[iComp];
        
        bool doneFlag = false;
        for (int32_t jFile = 1; jFile < numFiles; jFile++) {
            CaretAssertVectorIndex(pathComponentEachFile, jFile);
            const std::vector<AString>& fileComps = pathComponentEachFile[jFile];
            
            CaretAssertVectorIndex(fileComps, iComp);
            if (fileComps[iComp] != component) {
                doneFlag = true;
                break;
            }
        }
        
        if (doneFlag) {
            break;
        }
        else {
            numMatchingLeadingComponents++;
        }
    }
    
    /*
     * For each of the file names, create a name in the form of:
     * filename (unique-suffix-of-path).
     */
    for (int32_t iFile = 0; iFile < numFiles; iFile++) {
        CaretAssertVectorIndex(fileNames, iFile);
        FileInformation fileInfo(fileNames[iFile]);
        AString name = fileInfo.getFileName() + " ";
        
        CaretAssertVectorIndex(pathComponentEachFile, iFile);
        const std::vector<AString>& fileComps = pathComponentEachFile[iFile];
        const int32_t numComps = static_cast<int32_t>(fileComps.size());
        
        bool addedComponentsFlag = false;
        for (int32_t iComp = numMatchingLeadingComponents; iComp < numComps; iComp++) {
            if (iComp == numMatchingLeadingComponents) {
                name.append("(..../");
            }
            CaretAssertVectorIndex(fileComps, iComp);
            name.append(fileComps[iComp] + "/");
            addedComponentsFlag = true;
        }
        if (addedComponentsFlag) {
            name.append(")");
        }
        
        prefixRemovedNamesOut.push_back(name);
    }
    
    CaretAssert(fileNames.size() == prefixRemovedNamesOut.size());
}
