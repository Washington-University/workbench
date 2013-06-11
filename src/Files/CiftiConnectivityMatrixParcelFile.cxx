
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

#define __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__
#include "CiftiConnectivityMatrixParcelFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_PARCEL_FILE_DECLARE__

using namespace caret;


    
/**
 * \class caret::CiftiConnectivityMatrixParcelFile 
 * \brief Connectivity Parcel x Parcel File
 * \ingroup Files
 *
 * Contains connectivity matrix that measures connectivity from parcels
 * to parcels.
 */

/**
 * Constructor.
 */
CiftiConnectivityMatrixParcelFile::CiftiConnectivityMatrixParcelFile()
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL,
                                          CiftiMappableDataFile::FILE_READ_DATA_ALL,
                                          CIFTI_INDEX_TYPE_PARCELS,
                                          CIFTI_INDEX_TYPE_PARCELS,
                                          CiftiMappableDataFile::DATA_ACCESS_WITH_ROW_METHODS,
                                          CiftiMappableDataFile::DATA_ACCESS_INVALID),ChartableInterface()
{
    
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixParcelFile::~CiftiConnectivityMatrixParcelFile()
{
    
}


/**
 * @return Is charting enabled for this file?
 */
bool
CiftiConnectivityMatrixParcelFile::isChartingEnabled() const
{
    return m_chartingEnabled;
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiConnectivityMatrixParcelFile::isChartingSupported() const
{
    return true;    
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiConnectivityMatrixParcelFile::setChartingEnabled(const bool enabled)
{
    m_chartingEnabled = enabled;
}

ChartTypeEnum::Enum CiftiConnectivityMatrixParcelFile::getDefaultChartType() const
{
    return ChartTypeEnum::MATRIX;
}

void CiftiConnectivityMatrixParcelFile::getSupportedChartTypes(std::vector<ChartTypeEnum::Enum> &list) const
{
    list.clear();
    list.push_back(ChartTypeEnum::MATRIX);    
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 */
CaretMappableDataFile*
CiftiConnectivityMatrixParcelFile::getCaretMappableDataFile()
{
    return dynamic_cast<CaretMappableDataFile*>(this);
}

/**
 * @return The CaretMappableDataFile that implements this interface.
 */
const CaretMappableDataFile*
CiftiConnectivityMatrixParcelFile::getCaretMappableDataFile() const
{
    return dynamic_cast<const CaretMappableDataFile*>(this);
}

