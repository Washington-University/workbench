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

using namespace caret;



NiftiMatrixTest::NiftiMatrixTest(const AString &identifier) : TestInterface(identifier)
{



}

void NiftiMatrixTest::execute()
{
    //matrix layout is 10,8,6,4
    AString path = m_default_path + "/nifti";
    NiftiMatrix matrix(path+"/testmatrix.float");

    matrix.setDataType(NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32);
    std::vector <int32_t> dim = {4,10,8,6,4};
    matrix.setMatrixLayoutOnDisk(dim,1,sizeof(float),false);
    uint64_t index = 0;
    float result = 0.0f;
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
