
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiFiberTrajectoryFile.h"
#include "FociFile.h"
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
CaretDataFileHelper::readAnyCaretDataFile(const AString& filename) throw (DataFileException)
{
    bool isValid = false;
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(filename,
                                                                                    &isValid);
    
    if (( ! isValid)
        || (dataFileType == DataFileTypeEnum::UNKNOWN)) {
        throw DataFileException("Filename extension does not match any supported data file.");
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
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            caretDataFile = new CiftiParcelScalarFile();
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            caretDataFile = new CiftiParcelSeriesFile();
            break;
        case DataFileTypeEnum::FOCI:
            caretDataFile = new FociFile();
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
        caretDataFile->readFile(filename);
    }
    catch (const DataFileException& dfe) {
        delete caretDataFile;
        caretDataFile = NULL;
        throw dfe;
    }
    
    return caretDataFile;
}


