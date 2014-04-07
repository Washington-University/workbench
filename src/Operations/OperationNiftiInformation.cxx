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

#include "OperationNiftiInformation.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "NiftiFile.h"
#include "CiftiXML.h"
#include "DataFileTypeEnum.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationNiftiInformation::getCommandSwitch()
{
    return "-nifti-information";
}

AString OperationNiftiInformation::getShortDescription()
{
    return "DISPLAY INFORMATION ABOUT A NIFTI/CIFTI FILE";
}

OperationParameters* OperationNiftiInformation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "nifti-file", "the nifti/cifti file to examine");
    
    ret->createOptionalParameter(2, "-print-header", "display the header contents");
    
    ret->createOptionalParameter(3, "-print-matrix", "output the values in the matrix (cifti only)");
    
    ret->createOptionalParameter(4, "-print-xml", "print the cifti XML (cifti only)");
    
    ret->setHelpText(
        AString("You must specify at least one -print-* option.")
    );
    return ret;
}

void OperationNiftiInformation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const AString fileName = myParams->getString(1);
    bool printHeader = myParams->getOptionalParameter(2)->m_present;
    bool printMatrix = myParams->getOptionalParameter(3)->m_present;
    bool printXml = myParams->getOptionalParameter(4)->m_present;
    if (!printHeader && !printMatrix && !printXml) throw OperationException("you must specify a -print-* option");
    if(!QFile::exists(fileName)) throw OperationException("File '" + fileName + "' does not exist.");
    if(!fileName.endsWith(".nii") && !fileName.endsWith(".nii.gz")) cout << "File doesn't end with an expected extension, is this really a Nifti/Cifti file?" << endl;

    bool validDataFileType = false;
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(fileName,
                                                                                &validDataFileType);
    bool isCiftiFile = false;
    if (validDataFileType)
    {
        if (DataFileTypeEnum::isConnectivityDataType(dataFileType))
        {
            isCiftiFile = true;
        }
    }
    if(printHeader)
    {
        const bool readWithHeaderFlag = true;
        
        if (readWithHeaderFlag) {
            /*
             * Reads header only so should be fast for any size file
             */
            try {
                std::vector<int64_t> dimensions;
                
                NiftiHeaderIO niftiHeaderIO(fileName);
                switch (niftiHeaderIO.getNiftiVersion()) {
                    case 1:
                    {
                        Nifti1Header header;
                        niftiHeaderIO.getHeader(header);
                        AString headerString;
                        header.getHeaderAsString(headerString);
                        header.getDimensions(dimensions);
                        cout << headerString << endl;
                        cout << getMemorySizeAsString(dimensions) << endl;
                    }
                        break;
                    case 2:
                    {
                        Nifti2Header header;
                        niftiHeaderIO.getHeader(header);
                        AString headerString;
                        header.getHeaderAsString(headerString);
                        header.getDimensions(dimensions);
                        cout << headerString << endl;
                        cout << getMemorySizeAsString(dimensions) << endl;
                    }
                        break;
                    default:
                        throw NiftiException("Unrecognized NIFTI version: "
                                             + AString::number(niftiHeaderIO.getNiftiVersion()));
                        
                }
            }
            catch (const NiftiException& e) {
                throw OperationException(e);
            }
        }
        else {
            /*
             * Reads entire file so may be very slow for large files
             */
            NiftiFile nf(fileName);
            if(nf.getNiftiVersion()==1)
            {
                Nifti1Header header;
                nf.getHeader(header);
                AString headerString;
                header.getHeaderAsString(headerString);
                cout << headerString << endl;
            }
            else if(nf.getNiftiVersion()==2)
            {
                Nifti2Header header;
                nf.getHeader(header);
                AString headerString;
                header.getHeaderAsString(headerString);
                cout << headerString << endl;
            }
            else throw OperationException("Unrecognized Nifti Version.");
        }
    }
    if(printXml)
    {
        if (!isCiftiFile) cout << "attempting -print-xml on something that doesn't seem to be a cifti file" << endl;
        CiftiFile cf(fileName, ON_DISK);
        const CiftiXML& xml = cf.getCiftiXML();
        AString xmlString = xml.writeXMLToString(xml.getParsedVersion());//rewrite with the same version that it was read with
        cout << xmlString << endl;
    }
    if(printMatrix)
    {
        if (!isCiftiFile) cout << "attempting -print-matrix on something that doesn't seem to be a cifti file" << endl;
        CiftiFile cf(fileName, ON_DISK);
        int64_t dim0 = cf.getNumberOfRows();
        int64_t dim1 = cf.getNumberOfColumns();
        vector<float> row(dim1);
        AString rowString;
        for(int64_t i = 0;i<dim0;i++)
        {

            cf.getRow(row.data(), i);

            rowString = AString::fromNumbers(row, ",");
            cout << "Row " << i << ": " << rowString << endl;
        }
    }
}

/**
 * Get the memory usage based upon the given volume dimensions.
 *
 * @param dimensions
 *    The volumes dimensions.
 */
AString
OperationNiftiInformation::getMemorySizeAsString(const std::vector<int64_t>& dimensions)
{

    AString str;
    
    int64_t numVoxelComponents = 0;
    
    const int64_t numDims = static_cast<int64_t>(dimensions.size());
    if (numDims > 0) {
        numVoxelComponents = 1;
        for (int64_t i = 0; i < numDims; i++) {
            CaretAssertVectorIndex(dimensions, i);
            numVoxelComponents *= dimensions[i];
        }
    }
    
    const int64_t numberOfBytes = sizeof(float) * numVoxelComponents;
    
    str += ("Data Size in (float) memory, Bytes: "
            + AString::number(numberOfBytes));
    
    const double oneKilobyte = 1024.0;
    const double kilobytes = numberOfBytes / oneKilobyte;
    
    const double oneMegabyte = 1048576.0;
    const double megabytes   = numberOfBytes / oneMegabyte;
    
    const double oneGigabyte = 1073741824.0;
    const double gigabytes   = numberOfBytes / oneGigabyte;
    
    const double oneTerabyte = 1099511627776.0;
    const double terabytes   = numberOfBytes / oneTerabyte;
    
    if (terabytes >= 1.0) {
        str += ("  Terabytes: "
                + AString::number(terabytes));
    }
    else if (gigabytes >= 1.0) {
        str += ("  Gigabytes: "
                + AString::number(gigabytes));
    }
    else if (megabytes >= 1.0) {
        str += ("  Megabytes: "
                + AString::number(megabytes));
    }
    else if (kilobytes >= 1.0) {
        str += ("  Kilobytes: "
                + AString::number(kilobytes));
    }
    
    return str;
}

