
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#define __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__
#include "CiftiBrainordinateScalarFile.h"
#undef __CIFTI_BRAINORDINATE_SCALAR_FILE_DECLARE__

#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiConnectivityMatrixDenseFile.h"

#include "CiftiFacade.h"
#include "CiftiFile.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateScalarFile 
 * \brief CIFTI Brainordinate by Scalar File
 * \ingroup Files
 *
 */

/**
 * Constructor.
 */
CiftiBrainordinateScalarFile::CiftiBrainordinateScalarFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                        CiftiMappableDataFile::FILE_READ_DATA_ALL,
                        CIFTI_INDEX_TYPE_SCALARS,
                        CIFTI_INDEX_TYPE_BRAIN_MODELS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_COLUMN_METHODS,
                        CiftiMappableDataFile::DATA_ACCESS_WITH_ROW_METHODS)
{
    
}

/**
 * Destructor.
 */
CiftiBrainordinateScalarFile::~CiftiBrainordinateScalarFile()
{
    
}

/**
 * Create a Cifti Scalar File using the currently loaded row in a Cifti 
 * connectivity matrix file.
 *
 * @param ciftiMatrixFile
 *    Cifti connectivity matrix file.
 * @param errorMessageOut
 *    Will describe problem if there is an error.
 * @return
 *    Pointer to the newly created Cifti Scalar File.  If there is an error,
 *    NULL will be returned and errorMessageOut will describe the problem.
 */
CiftiBrainordinateScalarFile*
CiftiBrainordinateScalarFile::newInstanceFromRowInCiftiConnectivityMatrixFile(const CiftiMappableConnectivityMatrixDataFile* ciftiMatrixFile,
                                                                              AString& errorMessageOut)
{
    errorMessageOut.clear();
 
    const CiftiConnectivityMatrixDenseFile* denseFile =
       dynamic_cast<const CiftiConnectivityMatrixDenseFile*>(ciftiMatrixFile);
    if (denseFile == NULL) {
        errorMessageOut = "Only Cifti Dense Matrix Files are supported for conversion to Cifti Scalar Files.";
        return NULL;
    }
    
    const CiftiInterface* ciftiMatrixInterface = ciftiMatrixFile->m_ciftiInterface;
    const CiftiFacade* ciftiMatrixFacade       = ciftiMatrixFile->m_ciftiFacade;
    if (ciftiMatrixFacade == NULL) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (Facade NULL).";
        return NULL;
    }
    
    if (ciftiMatrixFacade->getNumberOfMaps() <= 0) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (No Maps).";
        return NULL;
    }
    
    std::vector<float> data;
    ciftiMatrixFile->getMapData(0, data);
    if (data.empty()) {
        errorMessageOut = "No data appears to be loaded in the Cifti Matrix File (mapData empty).";
        return NULL;
    }
    
    CiftiBrainordinateScalarFile* scalarFile = NULL;
    
    try {
        CiftiFile* ciftiFile = new CiftiFile();
        
        const CiftiXML& ciftiMatrixXML = ciftiMatrixInterface->getCiftiXML();
        
        /*
         * Copy XML from matrix file
         * and update to be a scalar file.
         */
        CiftiXML ciftiScalarXML = ciftiMatrixXML;
        ciftiScalarXML.copyMapping(CiftiXML::ALONG_COLUMN,
                                   ciftiMatrixXML,
                                   CiftiXML::ALONG_ROW);
        ciftiScalarXML.resetRowsToScalars(1);
        ciftiScalarXML.setMapNameForColumnIndex(0, ciftiMatrixFile->getMapName(0));
        
        ciftiFile->setCiftiXML(ciftiScalarXML);
        
        ciftiFile->setRow(&data[0],
                          0);
        
        CiftiBrainordinateScalarFile* scalarFile = new CiftiBrainordinateScalarFile();
        scalarFile->initializeFromCiftiInterface(ciftiFile, scalarFile->getFileName());
        return scalarFile;
    }
    catch (const CiftiFileException& ce) {
        if (scalarFile != NULL) {
            delete scalarFile;
        }
        errorMessageOut = ce.whatString();
    }
    catch (const DataFileException& de) {
        if (scalarFile != NULL) {
            delete scalarFile;
        }
        errorMessageOut = de.whatString();
    }
    
    return NULL;
}


