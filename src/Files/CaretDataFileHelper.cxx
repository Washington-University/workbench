
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

#define __CARET_DATA_FILE_HELPER_DECLARE__
#include "CaretDataFileHelper.h"
#undef __CARET_DATA_FILE_HELPER_DECLARE__

#include "AnnotationFile.h"
#include "AnnotationTextSubstitutionFile.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "CiftiConnectivityMatrixDenseParcelFile.h"
#include "CiftiConnectivityMatrixParcelDenseFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFiberTrajectoryMapFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "CziImageFile.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "HistologySlicesFile.h"
#include "ImageFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "OmeZarrImageFile.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SamplesFile.h"
#include "SceneFile.h"
#include "SpecFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include "nifti2.h"

using namespace caret;


    
/**
 * \class caret::CaretDataFileHelper 
 * \brief Contains static methods to help out with CaretDataFile subclasses.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CaretDataFileHelper::CaretDataFileHelper()
{
    
}

/**
 * Destructor.
 */
CaretDataFileHelper::~CaretDataFileHelper()
{
}

/**
 * Read any CaretDataFile subclass.
 *
 * @param filename
 *    Name of file.
 * @return
 *    Pointer to the data file after reading the file.
 * @throw
 *    DataFileException if unable to read the file for any reason.
 */
CaretDataFile*
CaretDataFileHelper::readAnyCaretDataFile(const AString& filename, const bool& preferOnDisk)
{
    bool isValid = false;
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(filename,
                                                                                    &isValid);
    
    CaretDataFile* caretDataFile = NULL;
    
    DataFileException dataFileException;
    bool dataFileExceptionValid = false;

    /*
     * If filename's extension is valid, try to read
     * the file using the matching file type.
     */
    if (isValid
        && (dataFileType != DataFileTypeEnum::UNKNOWN)) {
        caretDataFile = createCaretDataFileForFileType(dataFileType);
        CaretAssert(caretDataFile);
        
        try {
            readCaretDataFile(caretDataFile,
                              filename,
                              preferOnDisk);
        }
        catch (const DataFileException& dfe) {
            delete caretDataFile;
            caretDataFile = NULL;
            
            dataFileException = dfe;
            dataFileExceptionValid = true;
        }
    }
    else {
        dataFileException = DataFileException(filename,
                                        "Filename extension does not match any supported data file.");
        dataFileExceptionValid = true;
    }
    
    if (caretDataFile == NULL) {
        /*
         * Examine contents of file to determine file type.
         * Filename could have wrong extension.
         */
        const DataFileTypeEnum::Enum dataFileType = getDataFileTypeFromFileContent(filename);
        if (dataFileType != DataFileTypeEnum::UNKNOWN) {
            caretDataFile = createCaretDataFileForFileType(dataFileType);
            CaretAssert(dataFileType);
            
            CaretLogInfo("File "
                         + filename
                         + " appears to be of type "
                         + DataFileTypeEnum::toGuiName(dataFileType)
                         + " from examination of file content.");
            
            try {
                readCaretDataFile(caretDataFile,
                                  filename,
                                  preferOnDisk);
                
                /*
                 * File was read successfully, exception no longer valid
                 */
                dataFileExceptionValid = false;
            }
            catch (const DataFileException& dfe) {
                delete caretDataFile;
                caretDataFile = NULL;
                
                /*
                 * Do not override exception from reading
                 * using the file's extension
                 */
                if ( ! dataFileExceptionValid) {
                    dataFileException = dfe;
                    dataFileExceptionValid = true;
                }
            }
        }
        else {
            CaretAssert(dataFileExceptionValid);
        }
    }

    if (dataFileExceptionValid) {
        throw dataFileException;
    }
    
    CaretAssert(caretDataFile);
    
//    if (( ! isValid)
//        || (dataFileType == DataFileTypeEnum::UNKNOWN)) {
//        throw DataFileException(filename,
//                                "Filename extension does not match any supported data file.");
//    }
    
    
    
//    try {
//        if (preferOnDisk) caretDataFile->setPreferOnDiskReading(true);//NOTE: because Dense Connectivity also pays attention to this, never change default behaviors away from on disk
//        try {
//            caretDataFile->readFile(filename);
//        }
//        catch (const std::bad_alloc& badAlloc) {
//            /*
//             * This DataFileException will be caught
//             * in the outer try/catch and it will
//             * clean up to avoid memory leaks.
//             */
//            throw DataFileException(filename,
//                                    createBadAllocExceptionMessage(filename));
//        }
//    }
//    catch (const DataFileException& dfe) {
//        delete caretDataFile;
//        caretDataFile = NULL;
//
//        DataFileTypeEnum::Enum dft = getDataFileTypeFromFileContent(filename);
//        std::cout << "File " << filename << " may be " << DataFileTypeEnum::toGuiName(dft) << std::endl;
//        
//        throw dfe;
//        
//    }
    
    return caretDataFile;
}

/**
 * Read the given data file using the given filename.
 *
 * @param caretDataFile
 *     Caret data file instance into which data is read.
 * @param filename
 *     Name of the file.
 * @throw
 *    DataFileException if unable to read the file for any reason.
 */
void
CaretDataFileHelper::readCaretDataFile(CaretDataFile* caretDataFile,
                                       const AString& filename,
                                       const bool& preferOnDisk)
{
    try {
        if (preferOnDisk) caretDataFile->setPreferOnDiskReading(true);//NOTE: because Dense Connectivity also pays attention to this, never change default behaviors away from on disk
        try {
            caretDataFile->readFile(filename);
        }
        catch (const std::bad_alloc&) {
            /*
             * This DataFileException will be caught
             * in the outer try/catch and it will
             * clean up to avoid memory leaks.
             */
            throw DataFileException(filename,
                                    createBadAllocExceptionMessage(filename));
        }
    }
    catch (const DataFileException& dfe) {
        throw dfe;
    }
}

/**
 * Creates a useful error message when a std::bad_alloc exception occurs.
 *
 * @param filename
 *     Name of file that caused the std::bad_alloc exception.
 * @return
 *     Message with info about the file.
 */
AString
CaretDataFileHelper::createBadAllocExceptionMessage(const AString& filename)
{
    FileInformation fileInfo(filename);
    
    AString message("Unable to allocate memory for reading the file.");
    if (fileInfo.exists()) {
//        float bytes = (float)fileInfo.size();
//        short index = 0;
//        static const char *labels[9] = {" Bytes", " Kilobytes", " Megabytes", " Gigabytes", " Terabytes", " Petabytes", " Exabytes", " Zettabytes", " Yottabytes"};
//        while (index < 8 && bytes > 1000.0f)
//        {
//            ++index;
//            bytes = bytes / 1000.0f;//using 1024 would make it Kibibytes, etc
//        }
//        AString sizeString = AString::number(bytes, 'f', 2) + labels[index];//2 digits after decimal point
        
        const AString sizeString = FileInformation::fileSizeToStandardUnits(fileInfo.size());
        message.appendWithNewLine("File Size: " + sizeString);
        message.appendWithNewLine("");
        message.appendWithNewLine("Note: The amount of memory required to read a data file may be "
                                  "substantially larger than the size of the file due to the way the "
                                  "file's data is organized in memory or compression of data within the file.");
    }
    
    return message;
}

/**
 * Create an instance of a caret data file matching the given data file type.
 *
 * @param dataFileType
 *     Type of data file.
 * @return
 *     New instance of file except if type is UNKNOWN which returns NULL.
 */
CaretDataFile*
CaretDataFileHelper::createCaretDataFileForFileType(const DataFileTypeEnum::Enum dataFileType)
{
    CaretDataFile* caretDataFile = NULL;
    
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            caretDataFile = new AnnotationFile();
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            caretDataFile = new AnnotationTextSubstitutionFile();
            break;
        case DataFileTypeEnum::BORDER:
            caretDataFile = new BorderFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            caretDataFile = new CiftiConnectivityMatrixDenseFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            CaretAssertMessage(0, "Never create a dense dynamic file.");
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            caretDataFile = new CiftiBrainordinateLabelFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            caretDataFile = new CiftiConnectivityMatrixDenseParcelFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            caretDataFile = new CiftiBrainordinateScalarFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            caretDataFile = new CiftiBrainordinateDataSeriesFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            caretDataFile = new CiftiFiberOrientationFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            caretDataFile = new CiftiFiberTrajectoryFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
            caretDataFile = new CiftiFiberTrajectoryMapFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            caretDataFile = new CiftiConnectivityMatrixParcelFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            caretDataFile = new CiftiConnectivityMatrixParcelDenseFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            CaretAssertMessage(0, "Never create a parcel dynamic file");
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            caretDataFile = new CiftiParcelLabelFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            caretDataFile = new CiftiParcelScalarFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            caretDataFile = new CiftiParcelSeriesFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            caretDataFile = new CiftiScalarDataSeriesFile();
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            caretDataFile = new CziImageFile();
            break;
        case DataFileTypeEnum::FOCI:
            caretDataFile = new FociFile();
            break;
        case DataFileTypeEnum::HISTOLOGY_SLICES:
            caretDataFile = new HistologySlicesFile();
            break;
        case DataFileTypeEnum::IMAGE:
            caretDataFile = new ImageFile();
            break;
        case DataFileTypeEnum::LABEL:
            caretDataFile = new LabelFile();
            break;
        case DataFileTypeEnum::METRIC:
            caretDataFile = new MetricFile();
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            CaretAssertMessage(0, "Never create a metric dynamic file");
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            caretDataFile = new OmeZarrImageFile();
            break;
        case DataFileTypeEnum::PALETTE:
            caretDataFile = new PaletteFile();
            break;
        case DataFileTypeEnum::RGBA:
            caretDataFile = new RgbaFile();
            break;
        case DataFileTypeEnum::SAMPLES:
            caretDataFile = new SamplesFile();
            break;
        case DataFileTypeEnum::SCENE:
            caretDataFile = new SceneFile();
            break;
        case DataFileTypeEnum::SPECIFICATION:
            caretDataFile = new SpecFile();
            break;
        case DataFileTypeEnum::SURFACE:
            caretDataFile = new SurfaceFile();
            break;
        case DataFileTypeEnum::UNKNOWN:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::VOLUME:
            caretDataFile = new VolumeFile();
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            CaretAssertMessage(0, "Never create a Volume Dynamic file");
            break;
    }
    
    return caretDataFile;
}


/**
 * Examine the content of a file to determine the file type.
 *
 * @param filename
 *     Name of file.
 * @return 
 *     Type of file if it can be determined else the unknown file type.
 */
DataFileTypeEnum::Enum
CaretDataFileHelper::getDataFileTypeFromFileContent(const AString& filename)
{
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    
    if (DataFile::isFileOnNetwork(filename)) {
        return dataFileType;
    }
    
    if (filename.endsWith(".nii")) {
        try {
            CiftiFile ciftiFile;
            ciftiFile.openFile(filename);
            
            const CiftiXML ciftiXML = ciftiFile.getCiftiXML();
            
            char intentName[16];
            const int32_t intentType = ciftiXML.getIntentInfo(CiftiVersion(),
                                                           intentName);
            
            switch (intentType) {
                case NIFTI_INTENT_CONNECTIVITY_UNKNOWN:  // 3000;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE:  // 3001;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE_SERIES:  // 3002;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED:  // 3003;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED_SERIES:  // 3004;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE_TRAJECTORY:  // 3005;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE_SCALARS:  // 3006;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE_LABELS:  // 3007;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED_SCALAR:  // 3008;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED_DENSE:  // 3009;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE;
                    break;
                case NIFTI_INTENT_CONNECTIVITY_DENSE_PARCELLATED:  // 3010;
                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL;
                    break;
//                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED_PARCELLATED_SERIES:  // 3011;
//                    dataFileType = DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES
//                    break;
//                case NIFTI_INTENT_CONNECTIVITY_PARCELLATED_PARCELLATED_SCALAR:  // 3012;
//                    dataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE;
//                    break;
                default:
                    CaretLogInfo(filename
                                 + " has unrecognized CIFTI intent type="
                                 + AString::number(intentType));
                    break;
            }
        }
        catch (const DataFileException&) {
            CaretLogInfo(filename
                         + " could not be read as a CIFTI file");
        }
    }
    return dataFileType;
}


