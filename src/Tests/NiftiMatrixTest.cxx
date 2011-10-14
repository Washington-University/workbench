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

void initMatrix(AString &matrixfile,LayoutType &layout)
{
    NiftiMatrix matrix(matrixfile);
    matrix.setMatrixLayoutOnDisk(layout);
}

void getFrame(NiftiMatrix &matrix, int64_t timeSlice, float *&frame)
{
    matrix.readFrame(timeSlice);

    //for purposes of comparison I will be returning the entire frame
    uint64_t frameLength = matrix.getFrameLength();
    for(uint64_t i = 0;i<frameLength;i++) frame[(int)i] = matrix.getComponent(i,0);
}

void printFrame(NiftiMatrix &matrix, float * &frame)
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

/*
void printMatrix(NiftiMatrix &matrix)
{

    uint64_t index = 0;
    float result = 0.0f;


    std::cout << "start of voxel reading code." << std::endl;
    for(int t = 0;t<dim[dim[0]];t++)
    {
        std::cout << "time slice: " << t << std::endl;
    uint64_t index = 0;
    float result = 0.0f;


    std::cout << "start of voxel reading code." << std::endl;
    for(int t = 0;t<dim[dim[0]];t++)
    {
        std::cout << "time slice: " << t << std::endl;
        matrix.readFrame(t);
        for(int k = 0;k<dim[3];k++)
        {
            std::cout << "k dimension: " << k << std::endl;
            for(int j = 0;j<dim[2];j++)
            {
                for(int i = 0;i<dim[1];i++)
                {
                    matrix.translateVoxel(i,j,k,index);

                    result = matrix.getComponent(index,0);
                    std::cout << index << ":" << result << ",";
                }
                std::cout << std::endl;
            }
        }
    }
    }
}*/



/*
void NiftiMatrix::get(AString &matrixfile)
{

    NiftiMatrix matrix(filename);

    matrix.setDataType(NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32);
    //std::vector <int32_t> dim = {4,10,8,6,4}; c++0x specific
    std::vector <int32_t> dim(5,0);

    {//this get's less ugly the longer the list is...
        int32_t dim2[] = {4,10,8,6,4};
        for(uint32_t i=0;i<sizeof(dim2)/sizeof(int32_t);i++)dim.push_back(dim2[i]);
    }
    matrix.setMatrixLayoutOnDisk(dim,1,sizeof(float),false);
    uint64_t index = 0;
    float result = 0.0f;


    std::cout << "start of voxel reading code." << std::endl;
    for(int t = 0;t<dim[dim[0]];t++)
    {
        std::cout << "time slice: " << t << std::endl;
        matrix.readFrame(t);
        for(int k = 0;k<dim[3];k++)
        {
            std::cout << "k dimension: " << k << std::endl;
            for(int j = 0;j<dim[2];j++)
            {
                for(int i = 0;i<dim[1];i++)
                {
                    matrix.translateVoxel(i,j,k,index);

                    result = matrix.getComponent(index,0);
                    std::cout << index << ":" << result << ",";
                }
                std::cout << std::endl;
            }
        }
    }
}*/

void NiftiMatrixTest::execute()
{
    //matrix layout is 10,8,6,4
    AString path = m_default_path + "/nifti";
    AString filename = path+"/testmatrix.float";


}




//dead code
/*for(int t = 0;t<4;t++)
{
    std::cout << "time slice" << t << std::endl;
    matrix.readFrame(t);
    for(int i = 0;i<8*6;i++)
    {
        for(int j = i*10;j<(i+1)*10;j++)
        {
            std::cout << matrix.getComponent(j,0) << ",";
        }
        std::cout << " "<< std::endl;
    }

}*/
