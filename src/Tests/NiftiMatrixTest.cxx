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

#include "NiftiMatrixTest.h"
#include <stdint.h>

using namespace caret;



NiftiMatrixTest::NiftiMatrixTest(const AString &identifier) : TestInterface(identifier)
{



}



void NiftiMatrixTest::getFrame(NiftiMatrix &matrix, uint64_t &timeSlice, float *frame)
{
    matrix.readFrame(timeSlice);

    //for purposes of comparison I will be returning the entire frame
    uint64_t frameLength = matrix.getFrameLength();
    for(uint64_t i = 0;i<frameLength;i++) frame[i] = matrix.getComponent(i,0);
}

void NiftiMatrixTest::writeFrame(NiftiMatrix &matrix, uint64_t &timeSlice, float *frame)
{
    //for the purposes of testing, we tell the api to take in a null formatted matrix, in
    //the future, I will simply have it allocate a null buffer if it is the first param
    //I could just as easily handed if the frame and been done with it...
    float *temp = new float[matrix.getFrameLength()];
    memset(temp,matrix.getFrameLength(),sizeof(float));
    matrix.setFrame(temp,matrix.getFrameLength(),timeSlice);

    //set the frame using the low level api to make sure it works
    uint64_t frameLength = matrix.getFrameLength();
    for(uint64_t i = 0;i<frameLength;i++) matrix.setComponent(i,0,frame[i]);

}

void NiftiMatrixTest::testWriter()
{
    NiftiMatrix floatMatrix, floatMatrixBE, doubleMatrix, doubleMatrixBE;
    setupReaderMatrices(floatMatrix, floatMatrixBE, doubleMatrix, doubleMatrixBE);
    std::vector <NiftiMatrix *> matrices;
    matrices.push_back( &floatMatrix);
    matrices.push_back(&floatMatrixBE);
    matrices.push_back(&doubleMatrix);
    matrices.push_back(&doubleMatrixBE);
    NiftiMatrix floatMatrixOut, floatMatrixOutBE, doubleMatrixOut, doubleMatrixOutBE;
    std::vector <NiftiMatrix *> matricesOut;
    matricesOut.push_back(&floatMatrixOut);
    matricesOut.push_back(&floatMatrixOutBE);
    matricesOut.push_back(&doubleMatrixOut);
    matricesOut.push_back(&doubleMatrixOutBE);
    setupWriterMatrices(floatMatrixOut,floatMatrixOutBE, doubleMatrixOut,doubleMatrixOutBE);
    copyMatrices(matricesOut,matrices);

    compareMatrices(matrices);
}

void NiftiMatrixTest::copyMatrices(std::vector< NiftiMatrix *> &matricesOut, std::vector< NiftiMatrix *> &matrices)
{
    //below we load frames, note that all values are stored as floats,
    //the word double denotes what file on disk it was stored as.
    //need better sanity checking in the future, such as comparing layouts, not just frame lengths
    NiftiMatrix *mat = matrices[0];
    uint64_t frameLength = matrices[0]->getFrameLength();
    for(uint32_t i = 0;i<matrices.size();i++)
    {
        if(frameLength!=matrices[i]->getFrameLength()!=matricesOut[i]->getFrameLength())
        {
            setFailed("Error comparing matrices, they do not have the same frame size.");
            return;
        }

    }
    std::vector<float *> frames;
    std::vector<float *> outFrames;
    for(uint i = 0;i<matrices.size();i++)
    {
        frames.push_back(new float[frameLength]);
        outFrames.push_back(new float(frameLength));
    }
    LayoutType layout;
    mat->getMatrixLayoutOnDisk(layout);
    std::vector <int32_t> dim = layout.dimensions;


    for(uint64_t t=0;t<dim[4];t++)
    {
        for(uint i = 0;i < matrices.size();i++)
        {
            this->getFrame(*(matrices[i]),t,(float *)(frames[i]));
            this->writeFrame(*(matricesOut[i]), t, (float *)(frames[i]));
        }
    }
}
void NiftiMatrixTest::execute()
{
    testReader();
    //testWriter();


}

void NiftiMatrixTest::setupLayouts(NiftiMatrix &floatMatrix,
                               NiftiMatrix &floatMatrixBE,
                               NiftiMatrix &doubleMatrix,
                               NiftiMatrix &doubleMatrixBE
)
{
    //define layouts matrix layout is 10,8,6,4
    std::vector <int32_t> dim(5,0);
    dim[0]=4;dim[1]=10;dim[2]=8;dim[3]=6;dim[4]=4;

    // below we define four layouts
    LayoutType floatLayout;
    floatLayout.dimensions = dim;
    floatLayout.componentDimensions = 1;
    floatLayout.layoutSet = true;
    floatLayout.needsSwapping = false;
    floatLayout.niftiDataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32;
    floatLayout.valueByteSize = 4;
    LayoutType floatLayoutBE( floatLayout);
    floatLayoutBE.needsSwapping = true;
    LayoutType doubleLayout(floatLayout);
    doubleLayout.niftiDataType = NiftiDataTypeEnum::NIFTI_TYPE_FLOAT64;
    doubleLayout.valueByteSize = 8;
    LayoutType doubleLayoutBE(doubleLayout);
    doubleLayoutBE.needsSwapping = true;

    floatMatrix.setMatrixLayoutOnDisk(floatLayout);
    floatMatrixBE.setMatrixLayoutOnDisk(floatLayoutBE);
    doubleMatrix.setMatrixLayoutOnDisk(doubleLayout);
    doubleMatrixBE.setMatrixLayoutOnDisk(doubleLayoutBE);

}

//tecnically any matrix can be read/written from, however, this are ONLY read from
void NiftiMatrixTest::setupReaderMatrices(NiftiMatrix &floatMatrix,
NiftiMatrix &floatMatrixBE,
NiftiMatrix &doubleMatrix,
NiftiMatrix &doubleMatrixBE)
{
    //define file paths
    AString path = m_default_path + "/nifti";
    floatMatrix.setMatrixFile( path+"/testmatrix.float");
    floatMatrixBE.setMatrixFile( path+"/testmatrix.floatbe");
    doubleMatrix.setMatrixFile( path+"/testmatrix.double");
    doubleMatrixBE.setMatrixFile( path+"/testmatrix.doublebe");
    setupLayouts(floatMatrix,floatMatrixBE,doubleMatrix,doubleMatrixBE);
}

//we copy data from the "read" matrices using the api, then test to make sure data has
//been read/written properly
void NiftiMatrixTest::setupWriterMatrices(NiftiMatrix &floatMatrix,
NiftiMatrix &floatMatrixBE,
NiftiMatrix &doubleMatrix,
NiftiMatrix &doubleMatrixBE)
{
    //define file paths
    AString path = m_default_path + "/nifti";
    floatMatrix.setMatrixFile( path+"/testmatrix.float");
    floatMatrixBE.setMatrixFile( path+"/testmatrix.floatbe");
    doubleMatrix.setMatrixFile( path+"/testmatrix.double");
    doubleMatrixBE.setMatrixFile( path+"/testmatrix.doublebe");
    setupLayouts(floatMatrix,floatMatrixBE,doubleMatrix,doubleMatrixBE);
}

void NiftiMatrixTest::testReader()
{
    NiftiMatrix floatMatrix, floatMatrixBE, doubleMatrix, doubleMatrixBE;
    setupReaderMatrices(floatMatrix, floatMatrixBE, doubleMatrix, doubleMatrixBE);
    std::vector <NiftiMatrix *> matrices;
    matrices.push_back( &floatMatrix);
    matrices.push_back(&floatMatrixBE);
    matrices.push_back(&doubleMatrix);
    matrices.push_back(&doubleMatrixBE);
    int size = matrices.size();
    compareMatrices(matrices);
}

void NiftiMatrixTest::compareMatrices(std::vector <NiftiMatrix *> &matrices)
{
    //below we load frames, note that all values are stored as floats,
    //the word double denotes what file on disk it was stored as.
    //need better sanity checking in the future, such as comparing layouts, not just frame lengths
    NiftiMatrix *mat = matrices[0];
    uint64_t frameLength = matrices[0]->getFrameLength();
    for(uint32_t i = 0;i<matrices.size();i++)
    {
        if(frameLength!=matrices[i]->getFrameLength())
        {
            setFailed("Error comparing matrices, they do not have the same frame size.");
            return;
        }
    }
    std::vector<float *> frames;
    for(uint i = 0;i<matrices.size();i++)
    {
        frames.push_back(new float[frameLength]);
    }
    LayoutType layout;
    mat->getMatrixLayoutOnDisk(layout);
    std::vector <int32_t> dim = layout.dimensions;


    for(uint64_t t=0;t<dim[4];t++)
    {
        for(uint i = 0;i < matrices.size();i++)
        {
            this->getFrame(*(matrices[i]),t,(float *)(frames[i]));
        }

        for(uint i=0;i<(matrices.size()-1);i++)
        {
            if(memcmp(frames[i],frames[i+1],frameLength*sizeof(float)))
            {
                setFailed("Error reading frames using NiftiMatrix");
                return;
            }
        }
        std::cout << "reading time slice "<< t << " was successful for all supported formats" << std::endl;
    }
}

// below should only be used when troubleshooting test problems, as it generates a lot of output
void NiftiMatrixTest::printFrame(NiftiMatrix &matrix, float * frame)
{
    LayoutType layout;
    matrix.getMatrixLayoutOnDisk(layout);
    uint64_t index = 0;

    for(int k = 0;k<layout.dimensions[3];k++)
    {
        std::cout << "k dimension: " << k << std::endl;
        for(int j = 0;j<layout.dimensions[2];j++)
        {
            for(int i = 0;i<layout.dimensions[1];i++)
            {
                matrix.translateVoxel(i,j,k,index);
                std::cout << index << ":" << frame[index] << ",";
            }
            std::cout << std::endl;
        }
    }
}




