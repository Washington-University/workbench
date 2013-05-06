/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "CommandNiftiInformation.h"
#include "CiftiFile.h"
#include "NiftiFile.h"
#include "ProgramParameters.h"
#include "CiftiXML.h"
#include "DataFileTypeEnum.h"

using namespace caret;
using namespace std;
CommandNiftiInformation::CommandNiftiInformation() : CommandOperation("-nifti-information",
                                                                        "Display information about a Nifti/Cifti File.")
{
}

/**
 * @return The help information.
 */
AString
CommandNiftiInformation::getHelpInformation(const AString& /*programName*/)
{
    AString helpInfo = ("\n"
                        "Print information about a NIFTI file.\n"
                        "\n"
                        "Usage:  <nifti-file>\n"
                        "        [-print-header]\n"
                        "        [-print-matrix]\n"
                        "        [-print-xml]\n"
                        "\n"
                        "    nifti-file\n"
                        "        Required NIFTI file name.\n"
                        "    \n"
                        "    -print-header\n"
                        "        Option to show header contents.\n"
                        "    \n"
                        "    -print-matrix\n"
                        "        Option to print the matrix (CIFTI Intents only).\n"
                        "    \n"
                        "    -print-xml\n"
                        "        Option to print the XML (CIFTI Intents only).\n"
                        );
    return helpInfo;
}

void CommandNiftiInformation::executeOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException)
{
    try
    {
        const AString fileName = parameters.nextString("C/Nifti file name");
        bool printHeader = false;
        bool printXml = false;
        bool printMatrix = false;
        while (parameters.hasNext())
        {
            AString param = parameters.nextString("Get parameter.");
            if(param== "-print-header")
            {
                printHeader = true;
            }
            else if(param == "-print-xml")
            {
                printXml = true;
            }
            else if(param == "-print-matrix")
            {
                printMatrix = true;
            }
        }

        if(!QFile::exists(fileName)) throw CommandException("File "+fileName+" does not exist.");
        else if(!fileName.endsWith(".nii") && !fileName.endsWith(".nii.gz")) throw CommandException("File doesn't end with .nii extension, is this really a Nifti file?");

        bool validDataFileType = false;
        const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(fileName,
                                                                                  &validDataFileType);
        bool isCiftiFile = false;
        if (validDataFileType) {
            if (DataFileTypeEnum::isConnectivityDataType(dataFileType)) {
                isCiftiFile = true;
            }
        }
        //if(fileName.endsWith(".dtseries.nii") || fileName.endsWith(".dconn.nii"))
        if (isCiftiFile)
        {
            CiftiFile cf(fileName, ON_DISK);
            if(printHeader) {
                CiftiHeader header;
                cf.getHeader(header);
                AString headerString;
                header.getHeaderAsString(headerString);
                cout << headerString << endl;
            }
            if(printXml) {
                const CiftiXML& xml = cf.getCiftiXML();
                AString xmlString;
                xml.writeXML(xmlString);
                cout << xmlString << endl;
            }
            if(printMatrix) {
                CiftiHeader header;
                cf.getHeader(header);
                std::vector <int64_t> dim;
                header.getDimensions(dim);
                int64_t dim0 = dim[4];
                int64_t dim1 = dim[5];
                float *row = new float[dim1];
                AString rowString;
                for(int64_t i = 0;i<dim0;i++)
                {

                    cf.getRow(row,i);

                    rowString = AString::fromNumbers(row,dim1,",");
                    cout << "Row " << i << ": " << rowString << endl;
                }
                delete [] row;
            }
        }
        else
        {
            //print out nifti file
            NiftiFile nf(fileName);
            if(printHeader) {
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
                else throw CommandException("Unrecognized Nifti Version.");
            }
            if(printXml) {
                cerr << "Printing Xml is only supported for cifti files." << endl;
            }
            if(printMatrix) {
                cerr << "Printing Matrix/Volume information is only supported for Cifti." << endl;
            }
        }
    } catch (CaretException& e) {
        throw CommandException(e);
    }
}
