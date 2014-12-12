
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

#include "CaretAssert.h"
#include "DataFileTypeEnum.h"

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

#include "BorderFile.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"
#include "CiftiConnectivityMatrixDenseParcelFile.h"
#include "CiftiConnectivityMatrixParcelDenseFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiScalarDataSeriesFile.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "ImageFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "PaletteFile.h"
#include "RgbaFile.h"
#include "SceneFile.h"
#include "SpecFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

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
    
    if (( ! isValid)
        || (dataFileType == DataFileTypeEnum::UNKNOWN)) {
        throw DataFileException(filename,
                                "Filename extension does not match any supported data file.");
    }
    
    CaretDataFile* caretDataFile = NULL;
    
    switch (dataFileType) {
        case DataFileTypeEnum::BORDER:
            caretDataFile = new BorderFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            caretDataFile = new CiftiConnectivityMatrixDenseFile();
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
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            caretDataFile = new CiftiConnectivityMatrixParcelFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            caretDataFile = new CiftiConnectivityMatrixParcelDenseFile();
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
        case DataFileTypeEnum::FOCI:
            caretDataFile = new FociFile();
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
        case DataFileTypeEnum::PALETTE:
            caretDataFile = new PaletteFile();
            break;
        case DataFileTypeEnum::RGBA:
            caretDataFile = new RgbaFile();
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
    }
    
    CaretAssert(caretDataFile);
    
    try {
        if (preferOnDisk) caretDataFile->setPreferOnDiskReading(true);//NOTE: because Dense Connectivity also pays attention to this, never change default behaviors away from on disk
        try {
            caretDataFile->readFile(filename);
        }
        catch (const std::bad_alloc& badAlloc) {
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
        delete caretDataFile;
        caretDataFile = NULL;
        throw dfe;
    }
    
    return caretDataFile;
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


