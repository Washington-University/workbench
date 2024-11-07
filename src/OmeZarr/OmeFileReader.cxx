
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

/*=========================================================================
 *  Code for reading OME-ZARR is adapted/copied from
 *  https://github.com/InsightSoftwareConsortium/ITKIOOMEZarrNGFF/blob/main/src/itkOMEZarrNGFFImageIO.cxx#L328
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#define __OME_FILE_READER_DECLARE__
#include "OmeFileReader.h"
#undef __OME_FILE_READER_DECLARE__

#include <QFile>
#include <QImage>
#include <QImageWriter>

#include "CaretAssert.h"
#include "OmeImage.h"
#include "OmeAttrsV0p4JsonFile.h"
#include "ZarrHelper.h"
#include "ZarrV2ArrayJsonFile.h"
#include "ZarrV2GroupJsonFile.h"
#include "ZarrImageReader.h"

using namespace caret;


    
/**
 * \class caret::OmeFileReader 
 * \brief Helps with reading an OME-ZARR file.
 * \ingroup OmeZarr
 */

/**
 * Constructor.
 */
OmeFileReader::OmeFileReader()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
OmeFileReader::~OmeFileReader()
{
}

/**
 * @return The ZAttrs
 */
const OmeAttrsV0p4JsonFile*
OmeFileReader::getZAttrs() const
{
    return m_omeZAttrs.get();
}

/**
 * Initialize by reading metadata from the OME-ZARR file
 * @param omeZarrPath
 *    Path to OME-ZARR file
 * @return OK if able to read data from file or Error if there is a problem.
 */
FunctionResult
OmeFileReader::initialize(const AString& omeZarrPath)
{
    const bool debugFlag(false);
    
    /*
     * Name of driver is derived from the name of the path
     * (ie: a zip file uses the ZIP driver)
     */
    const FunctionResultValue<ZarrDriverTypeEnum::Enum> driverTypeResult(ZarrHelper::getDriverTypeFromFilename(omeZarrPath));
    if ( ! driverTypeResult.isOk()) {
        return driverTypeResult;
    }
    
    m_driverType = driverTypeResult.getValue();
    
    /*
     * Get zarr_format from .group file
     */
    FunctionResultValue<ZarrV2GroupJsonFile*> zarrGroupResultV2(ZarrHelper::readZarrV2GroupJsonFile(m_driverType,
                                                                                                         omeZarrPath));
    if (zarrGroupResultV2.isError()) {
        return FunctionResult(zarrGroupResultV2.getErrorMessage(),
                              zarrGroupResultV2.isOk());
    }
    m_zarrGroupFile.reset(zarrGroupResultV2.getValue());
    CaretAssert(m_zarrGroupFile);
    m_zarrFormatNumber = m_zarrGroupFile->getZarrFormat();
    if (m_zarrFormatNumber != 2) {
        return FunctionResult::error("ZARR format 2 is the only format supported.  Trying to read Zarr format "
                                     + AString::number(m_zarrFormatNumber));
    }
    
    /*
     * Content of .zattrs file describes OME usage of ZARR file
     */
    FunctionResultValue<OmeAttrsV0p4JsonFile*> zattrsResult(ZarrHelper::readOmeAttrsV0p4JsonFile(m_driverType,
                                                                                                              omeZarrPath));
    if (zattrsResult.isError()) {
        return FunctionResult(zattrsResult.getErrorMessage(),
                              zattrsResult.isOk());
    }
    m_omeZAttrs.reset(zattrsResult.getValue());
    CaretAssert(m_omeZAttrs);

    if (debugFlag) {
        std::cout << "ZATTRS" << std::endl;
        std::cout << m_omeZAttrs->toString() << std::endl;
    }
    
    m_dimensionIndices = m_omeZAttrs->getDimensionIndices();
    
     /*
     * Number of data sets in the OME-ZARR file
     */
    const int32_t numDataSets(m_omeZAttrs->getNumberOfDataSets());
    for (int32_t i = 0; i < numDataSets; i++) {
        OmeDataSet* dataSet(m_omeZAttrs->getDataSet(i));
        const AString relativePath(dataSet->getRelativePath());
                
        FunctionResult initializeResult(dataSet->initializeForReading(m_driverType,
                                                                      omeZarrPath));
        if (initializeResult.isError()) {
            return initializeResult;
        }
        dataSet->setDimensionIndices(m_dimensionIndices);

        const bool loadTestFlag(false);
        if (loadTestFlag
            && (i == 0)) {
            {
                /*
                 * Loads one Z-Slice
                 */
                const std::vector<int64_t> dims(dataSet->getDimensions());
                const int32_t numDims(dims.size());
                std::vector<int64_t> dimOffset(numDims, 0);
                std::vector<int64_t> dimLengths(numDims, 0);
                dimLengths[m_dimensionIndices.getIndexForX()] = dims[m_dimensionIndices.getIndexForX()];
                dimLengths[m_dimensionIndices.getIndexForY()] = dims[m_dimensionIndices.getIndexForY()];
                dimLengths[m_dimensionIndices.getIndexForZ()] = dims[m_dimensionIndices.getIndexForZ()];
                dimLengths[m_dimensionIndices.getIndexForChannel()] = dims[m_dimensionIndices.getIndexForChannel()];
                
                dimOffset[m_dimensionIndices.getIndexForZ()] = 0;
                dimLengths[m_dimensionIndices.getIndexForZ()] = 1;

                /* START 05 Nov 2024*/
                FunctionResultValue<OmeImage*> readResult(dataSet->readSlice(0));
                if (readResult.isOk()) {
                    std::unique_ptr<OmeImage> omeImage(readResult.getValue());
                    CaretAssert(omeImage);
                    const AString filename("/Users/john/images/write_test_"
                                           + AString::number(i)
                                           + ".png");
                    FunctionResult writeResult(omeImage->writeAsQImage(filename));
                    if (writeResult.isError()) {
                        std::cerr << "Error writing test image: " << filename << std::endl;
                    }
                }
                else {
                    std::cout << "LOAD TEST FAILED: " << readResult.getErrorMessage() << std::endl;
                }
                /* END 05 Nov 2024 */
            }
        }
    }

    return FunctionResult::ok();
}

/**
 * @return an image, possibly subset, from the ZARR image reader
 * @param dimOffsets
 *    Offsets for each dimension
 * @param dimLengths
 *    Lengths for each dimension
 */
FunctionResultValue<OmeImage*>
OmeFileReader::readImageData(ZarrImageReader* zarrImageReader,
                             const std::vector<int64_t>& dimOffsets,
                             const std::vector<int64_t>& dimLengths)
{
    CaretAssert(zarrImageReader);
    
    FunctionResultValue<xt::xarray<uint8_t>*> readResult(zarrImageReader->readData(dimOffsets,
                                                                                   dimLengths));
    if (readResult.isOk()) {
        OmeImage* image(new OmeImage(readResult.getValue(),
                                             m_dimensionIndices));
        return FunctionResultValue<OmeImage*>(image,
                                                  "",
                                                  true);
    }
    
    return FunctionResultValue<OmeImage*>(NULL,
                                              readResult.getErrorMessage(),
                                              readResult.isOk());

}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OmeFileReader::toString() const
{
    AString s("OmeFileReader");
    s.appendWithNewLine("   Version: "
                        + OmeVersionEnum::toGuiName(m_omeVersion));
    s.appendWithNewLine("   Indices: "
                        + m_dimensionIndices.toString());
    return s;
}
