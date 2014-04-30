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
    testCiftiRead();
    if(this->failed()) return;
    testCiftiReadWriteInMemory();
    if(this->failed()) return;
    testCiftiReadWriteOnDisk();
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

void CiftiFileTest::testCiftiRead()
{
    //warning the data set for this is HUGE, and it will take a few minutes to run
    //this test is not recommended for day to day testing.  The test of CiftiReadWrite
    //should be more than sufficient once writing has been implemented
    std::cout << "Testing Cifti reader." << std::endl;
    if(!QFile::exists(this->m_default_path + "/cifti/DenseConnectome.dconn.nii"))
    {
        setFailed("Test file "+this->m_default_path + "/cifti/DenseConnectome.dconn.nii"+" does not exist!");
        return;
    }
    CiftiFile reader(this->m_default_path + "/cifti/DenseConnectome.dconn.nii");

    std::vector <int64_t> dim;
    int64_t columnSize = reader.getNumberOfRows();
    int64_t rowSize = reader.getNumberOfColumns();

    if(rowSize != columnSize)
    {
        setFailed("The Cifti Reader test must be run on symmetric, dense connectivity matrices");
        return;
    }
    float * row = new float[rowSize];
    float * column = new float [columnSize];
    for(int64_t i = 0;i<rowSize;i++)
    {
        reader.getRow(row,i);
        reader.getColumn(column,i);
        //if(memcmp((char *)row,(char *)column,rowSize*sizeof(float))), apparently roundoff errors keep this from working...
        for(int64_t j=0;j<columnSize;j++)
        {
            if((row[j]>(column[j]+0.0001))||
                    (row[j]<(column[j]-0.0001)))
            {
                setFailed("Row and Column " + AString::number(i) + " are not the same.");
                //std::cout << "Row " + AString::number(i) + ":" + AString::fromNumbers(row,rowSize,",") << std::endl;
                //std::cout << "Column " + AString::number(i) + ":" + AString::fromNumbers(column,columnSize,",") << std::endl;
                goto cleanup;
            }
        }
    }
    std::cout << "Files are the same, Cifti reader test successful." << std::endl;
    cleanup:
    delete [] row;
    delete [] column;
}

void CiftiFileTest::testCiftiReadWriteInMemory()
{
    std::cout << "Testing Cifti reader/writer." << std::endl;

    // read header
    CiftiFile reader(this->m_default_path + "/cifti/DenseTimeSeries.dtseries.nii");


    // read XML
    const CiftiXMLOld& root = reader.getCiftiXMLOld();

    //hack TODO, this gives it a name to write to, change to write and cleanup temp
    //files if necessary
    AString outFile = this->m_default_path + "/cifti/testOut.dtseries.nii";
    if(QFile::exists(outFile)) QFile::remove(outFile);
    CiftiFile writer;
    writer.setCiftiXML(root);

    std::vector <int64_t> dim = reader.getDimensions();
    if (dim.size() != 2) setFailed("input file must have 2 dimensions");
    int64_t rowSize = dim[0];
    int64_t columnSize = dim[1];
    float *row = new float [rowSize];

    
    for(int64_t i = 0;i<columnSize;i++)
    {
        reader.getRow(row,i);
        writer.setRow(row,i);
    }

    writer.writeFile(outFile);

    //reopen output file, and check that frames agree
    CiftiFile test(outFile);

    float *testRow = new float [rowSize];
    for(int64_t i = 0;i<columnSize;i++)
    {
        reader.getRow(row,i);
        test.getRow(testRow,i);
        if(memcmp((void *)row,(void *)testRow,rowSize*sizeof(float)))
        {
            this->setFailed("Input and output Cifti file rows are not the same.");
            return;
        }
    }
    std::cout << "Reading and writing of Cifti was successful for all frames." << std::endl;
    delete [] row;
    delete [] testRow;
}

void CiftiFileTest::testCiftiReadWriteOnDisk()
{
    std::cout << "Testing Cifti reader/writer." << std::endl;

    // read header
    CiftiFile reader(this->m_default_path + "/cifti/DenseTimeSeries.dtseries.nii");


    // read XML
    const CiftiXMLOld& root = reader.getCiftiXMLOld();

    AString outFile = this->m_default_path + "/cifti/testOut.dtseries.nii";
    if(QFile::exists(outFile)) QFile::remove(outFile);
    CiftiFile writer;
    writer.setWritingFile(outFile);
    writer.setCiftiXML(root);

    std::vector <int64_t> dim = reader.getDimensions();
    if (dim.size() != 2) setFailed("input file must have 2 dimensions");
    int64_t rowSize = dim[0];
    int64_t columnSize = dim[1];
    float *row = new float [rowSize];

    
    for(int64_t i = 0;i<columnSize;i++)
    {
        reader.getRow(row,i);
        writer.setRow(row,i);
    }

    writer.writeFile(outFile);

    //reopen output file, and check that frames agree
    CiftiFile test(outFile);

    float *testRow = new float [rowSize];
    for(int64_t i = 0;i<columnSize;i++)
    {
        reader.getRow(row,i);
        test.getRow(testRow,i);
        if(memcmp((void *)row,(void *)testRow,rowSize*sizeof(float)))
        {
            this->setFailed("Input and output Cifti file rows are not the same.");
            return;
        }
    }
    std::cout << "Reading and writing of Cifti was successful for all frames." << std::endl;
    delete [] row;
    delete [] testRow;
}

