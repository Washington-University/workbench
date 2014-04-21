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

#include "NiftiTest.h"

#include "MultiDimIterator.h"
#include "NiftiIO.h"

#include <vector>

using namespace std;
using namespace caret;

NiftiFileTest::NiftiFileTest(const AString &identifier) : TestInterface(identifier)
{

}

void NiftiFileTest::execute()
{
    if(this->failed()) return;
    testNiftiReadWrite();
    if(this->failed()) return;
}

void NiftiFileTest::testNiftiReadWrite()
{
    std::cout << "Testing Nifti1 reader/writer." << std::endl;
    NiftiIO reader;
    AString inputFile = this->m_default_path + "/nifti/fcMRI1_nonlin_Subcortical_Smoothed_s6.nii";
    reader.openRead(inputFile);
    const NiftiHeader& header = reader.getHeader();
    //hack TODO, this gives it a name to write to, change to write and cleanup temp
    //files if necessary
    AString outFile = this->m_default_path + "/nifti/Nifti1TestOut.nii";
    NiftiIO writer;
    writer.writeNew(outFile, header);
    const vector<int64_t>& dims = reader.getDimensions();
    if (dims.size() < 3)
    {
        setFailed("this test requires nifti files with 3 or more dimensions");
        return;
    }
    int64_t frameLength = dims[0] * dims[1] * dims[2] * reader.getNumComponents();
    vector<float> frame(frameLength);
    vector<int64_t> extraDims(dims.begin() + 3, dims.end());
    for(MultiDimIterator<int64_t> iter(extraDims); !iter.atEnd(); ++iter)
    {
        reader.readData(frame.data(), 3, *iter);
        writer.writeData(frame.data(), 3, *iter);
    }
    writer.close();
    reader.close();
    reader.openRead(inputFile);//so that it can work on .gz files, which you can't seek backwards in

    //reopen output file, and check that frames agree
    NiftiIO test;
    
    vector<float> frameTest(frameLength);
    for(MultiDimIterator<int64_t> iter(extraDims); !iter.atEnd(); ++iter)
    {
        reader.readData(frame.data(), 3, *iter);
        test.readData(frameTest.data(), 3, *iter);
        for(int i=0;i<frameLength;i++)
        {
            if(frame[i]>frameTest[i]+0.0001 || frame[i]<frameTest[i]-0.0001)//because if NaNs, or if original datatype was different and/or scaling was done, slightly different results are possible
            {
                this->setFailed("Input and output nifti file frames are not the same.");
                return;
            }
        }
    }
    std::cout << "Reading and writing of Nifti was successful for all frames." << std::endl;
}

//Tests for reading and writing Nifti Headers

NiftiHeaderTest::NiftiHeaderTest(const AString &identifier) : TestInterface(identifier)
{
}

void NiftiHeaderTest::execute()
{
    NiftiHeader n1header;
    NiftiHeader n2header;
    readNiftiHeader("../../../wb_files/nifti/fcMRI1_nonlin_Subcortical_Smoothed_s6.nii", n1header);
    readNiftiHeader("../../../wb_files/nifti/test.cii", n2header);
    writeNifti1Header("../../../wb_files/nifti/n1.nii",n1header);
    writeNifti2Header("../../../wb_files/nifti/n2.cii",n2header);
    NiftiHeader n1headercomp;
    NiftiHeader n2headercomp;
    readNiftiHeader("../../../wb_files/nifti/n1.nii", n1headercomp);
    readNiftiHeader("../../../wb_files/nifti/n2.cii", n2headercomp);

    if(n1header != n1headercomp)
    {
        //report error
        AString message;
        message = AString("Nifti1 Header Reader/Writer error, files do not match.");
        setFailed(message);
    }
    else
    {
        std::cout << "Nifti1 Header Reader/Writer passes basic tests" << std::endl;
    }

    if(n2header != n2headercomp)
    {
        //report error
        AString message;
        message = AString("Nifti2 Header Reader/Writer error, files do not match.");
        setFailed(message);
    }
    else
    {
        std::cout << "Nifti2 Header Reader/Writer passes basic tests" << std::endl;
    }
}





void NiftiHeaderTest::readNiftiHeader(AString filename, NiftiHeader &header)
{
    CaretBinaryFile myFile;
    myFile.open(filename);
    header.read(myFile);
}

void NiftiHeaderTest::writeNifti1Header(AString filename, NiftiHeader &header)
{
    CaretBinaryFile myFile;
    myFile.open(filename, CaretBinaryFile::WRITE_TRUNCATE);
    header.write(myFile, 1);
}


void NiftiHeaderTest::writeNifti2Header(AString filename, NiftiHeader &header)
{
    CaretBinaryFile myFile;
    myFile.open(filename, CaretBinaryFile::WRITE_TRUNCATE);
    header.write(myFile, 2);
}
