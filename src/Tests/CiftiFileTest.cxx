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

#include "CiftiFileTest.h"
#include "CiftiFile.h"
using namespace caret;
CiftiFileTest::CiftiFileTest(const AString &identifier) : TestInterface(identifier)
{
}

void CiftiFileTest::execute()
{
    testObjectCreateDestroy();
    if(this->failed()) return;
    testCifti1ReadWrite();
    if(this->failed()) return;
}

void CiftiFileTest::testObjectCreateDestroy()
{
    CiftiFile *ciftiFile = new CiftiFile();
    if(ciftiFile) std::cout << "Object created successfully." << std::endl;
    else {
        setFailed("Error creating object.");
        return;
    }

    delete ciftiFile;
}

void CiftiFileTest::testCifti1ReadWrite()
{
    std::cout << "Testing Cifti1 reader/writer." << std::endl;

    // read header
    CiftiFile reader(this->m_default_path + "/cifti/DenseTimeSeries.dtseries.nii");
    CiftiHeader header;
    reader.getHeader(header);


    // read XML
    CiftiXML root;
    reader.getCiftiXML(root);

    //hack TODO, this gives it a name to write to, change to write and cleanup temp
    //files if necessary
    AString outFile = this->m_default_path + "/cifti/testOut.dtseries.nii";
    if(QFile::exists(outFile)) QFile::remove(outFile);
    CiftiFile writer(outFile);
    writer.setHeader(header);
    writer.setCiftiXML(root);

    std::vector <int64_t> dim;
    header.getDimensions(dim);
    float *row = new float [dim[0]];
    for(int64_t i = 0;i<dim[0];i++)
    {
        reader.getRow(row,i);
        writer.setRow(row,i);
    }

    writer.writeFile(outFile);

    //reopen output file, and check that frames agree
    CiftiFile test(outFile);

    float *testRow = new float [dim[0]];
    for(int64_t i = 0;i<dim[0];i++)
    {
        reader.getRow(row,i);
        test.getRow(testRow,i);
        if(!memcmp((void *)row,(void *)testRow,dim[0]*sizeof(float)))
        {
            this->setFailed("Input and output Cifti file rows are not the same.");
            return;
        }
    }
    std::cout << "Reading and writing of Cifti was successful for all frames." << std::endl;
    delete [] row;
    delete [] testRow;
}
