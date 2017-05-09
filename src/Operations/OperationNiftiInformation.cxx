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

#include "CaretBinaryFile.h"
#include "CiftiFile.h"
#include "NiftiIO.h"
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
    
    OptionalParameter* headerOpt = ret->createOptionalParameter(2, "-print-header", "display the header contents");
    headerOpt->createOptionalParameter(1, "-allow-truncated", "print the header even if the data is truncated");
    
    ret->createOptionalParameter(3, "-print-matrix", "output the values in the matrix (cifti only)");
    
    OptionalParameter* printXmlOpt = ret->createOptionalParameter(4, "-print-xml", "print the cifti XML (cifti only)");
    OptionalParameter* pxVersionOpt = printXmlOpt->createOptionalParameter(1, "-version", "convert the XML to a specific CIFTI version (default is the file's cifti version)");
    pxVersionOpt->addStringParameter(1, "version", "the CIFTI version to use");
    
    ret->setHelpText(
        AString("You must specify at least one -print-* option.")
    );
    return ret;
}

void OperationNiftiInformation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const AString fileName = myParams->getString(1);
    OptionalParameter* headerOpt = myParams->getOptionalParameter(2);
    bool printHeader = headerOpt->m_present;
    bool printMatrix = myParams->getOptionalParameter(3)->m_present;
    OptionalParameter* printXmlOpt = myParams->getOptionalParameter(4);
    bool printXml = printXmlOpt->m_present;
    if (!printHeader && !printMatrix && !printXml) throw OperationException("you must specify a -print-* option");
    if(!QFile::exists(fileName)) throw OperationException("File '" + fileName + "' does not exist.");

    if(printHeader)
    {
        if (!headerOpt->getOptionalParameter(1)->m_present)
        {//NiftiIO contains the file length check
            NiftiIO myIO;
            myIO.openRead(fileName);
            cout << myIO.getHeader().toString() << endl;
            cout << getMemorySizeAsString(myIO.getDimensions()) << endl;
        } else {//NiftiHeader doesn't
            CaretBinaryFile myFile(fileName);
            NiftiHeader myHeader;
            myHeader.read(myFile);
            cout << myHeader.toString() << endl;
            cout << getMemorySizeAsString(myHeader.getDimensions()) << endl;
        }
    }
    if(printXml)
    {
        CiftiFile cf(fileName);
        const CiftiXML& xml = cf.getCiftiXML();
        CiftiVersion printVersion = xml.getParsedVersion();//by default, rewrite with the same version that it was read with
        OptionalParameter* pxVersionOpt = printXmlOpt->getOptionalParameter(1);
        if (pxVersionOpt->m_present)
        {
            printVersion = CiftiVersion(pxVersionOpt->getString(1));
        }
        AString xmlString = xml.writeXMLToString(printVersion);
        cout << xmlString << endl;
    }
    if(printMatrix)
    {
        CiftiFile cf(fileName);
        if (cf.getCiftiXML().getNumberOfDimensions() != 2) throw OperationException("-print-matrix only supports 2D cifti");
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

