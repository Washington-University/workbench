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

    void getFrame(NiftiMatrix &matrix,int64_t &timeSlice, float *frame);
    void printFrame(NiftiMatrix &matrix, float *frame);
    bool compareMatrices(std::vector <NiftiMatrix *> &matrices);
    void writeFrame(NiftiMatrix &matrix, int64_t &timeSlice, float *frame);
    void copyMatrices(std::vector< NiftiMatrix *> &matricesOut, std::vector< NiftiMatrix *> &matrices);


    //printfloat(
    //printdouble(AString &matrixfile);
    //printfloatbe(AString &matrixfile);//big endian
    //printdoublebe(AString &matrixfile);//big endian
    virtual void execute();
};

}
#endif // NIFTIMATRIXTEST_H
