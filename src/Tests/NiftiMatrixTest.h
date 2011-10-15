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

#ifndef NIFTIMATRIXTEST_H
#define NIFTIMATRIXTEST_H

#include "TestInterface.h"
#include "NiftiMatrix.h"


namespace caret {

class NiftiMatrixTest : public TestInterface
{
public:
    NiftiMatrixTest(const AString& identifier);
    void testReader();
    void setupReaderMatrices(NiftiMatrix &floatMatrix,
                      NiftiMatrix &floatMatrixBE,
                      NiftiMatrix &doubleMatrix,
                      NiftiMatrix &doubleMatrixBE);
    void setupWriterMatrices(NiftiMatrix &floatMatrix,
                      NiftiMatrix &floatMatrixBE,
                      NiftiMatrix &doubleMatrix,
                      NiftiMatrix &doubleMatrixBE);
    void setupLayouts(NiftiMatrix &floatMatrix,
                      NiftiMatrix &floatMatrixBE,
                      NiftiMatrix &doubleMatrix,
                      NiftiMatrix &doubleMatrixBE);
    void testWriter();

    void getFrame(NiftiMatrix &matrix,uint64_t &timeSlice, float *frame);
    void printFrame(NiftiMatrix &matrix, float *frame);
    void compareMatrices(std::vector <NiftiMatrix *> &matrices);


    void setFrame(NiftiMatrix &matrix, uint64_t &timeSlice, float *frame);

    //printfloat(
    //printdouble(AString &matrixfile);
    //printfloatbe(AString &matrixfile);//big endian
    //printdoublebe(AString &matrixfile);//big endian
    virtual void execute();
};

}
#endif // NIFTIMATRIXTEST_H
