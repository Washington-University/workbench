/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "AffineFile.h"
#include "CaretAssert.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "NiftiHeaderIO.h"

#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AffineFile::AffineFile()
{
    m_matrix = FloatMatrix::identity(4);//use 4x4 convention for in-memory
}

void AffineFile::setMatrix(const FloatMatrix& matrix)
{
    int64_t inRows, inCols;
    matrix.getDimensions(inRows, inCols);
    CaretAssert(inCols == 4 && inRows > 2 && inRows < 5);
    m_matrix = FloatMatrix::identity(4);//set the final 0 0 0 1 row
    m_matrix[0] = matrix[0];//copy ONLY the rows that should change
    m_matrix[1] = matrix[1];
    m_matrix[2] = matrix[2];
}

void AffineFile::read34(const AString& filename)
{
    FileInformation affineInfo(filename);
    if (!affineInfo.exists()) throw DataFileException("affine file '" + filename + "' does not exist");
    fstream affineFile(filename.toLocal8Bit().constData(), fstream::in);
    if (!affineFile.good()) throw DataFileException("error opening file '" + filename + "' for reading");
    m_matrix = FloatMatrix::identity(4);//to ensure the right size and the fourth 0 0 0 1 row
    for (int i = 0; i < 3; ++i)//DO NOT read the fourth row from the file into the matrix
    {
        for (int j = 0; j < 4; ++j)
        {
            affineFile >> m_matrix[i][j];
            if (!affineFile)
            {
                m_matrix = FloatMatrix::identity(4);
                throw DataFileException("error while reading file '" + filename + "'");
            }
        }
    }
}

void AffineFile::readWorld(const AString& filename)
{
    read34(filename);//and that is it, no quirks
}

void AffineFile::readFlirt(const AString& filename, const AString& sourceName, const AString& targetName)
{
    read34(filename);
    float sourcePixDim[3], targetPixDim[3];//don't look at me, blame analyze and flirt
    vector<int64_t> sourceDims, targetDims;
    vector<vector<float> > sourceSform, targetSform;
    NiftiHeaderIO myIO;
    myIO.readFile(sourceName);
    switch (myIO.getNiftiVersion())
    {
        case 1:
        {
            Nifti1Header header1;
            myIO.getHeader(header1);
            header1.getSForm(sourceSform);
            header1.getDimensions(sourceDims);
            nifti_1_header mystruct1;
            header1.getHeaderStruct(mystruct1);
            sourcePixDim[0] = mystruct1.pixdim[1];//yes, that is really what they use, despite checking the SFORM/QFORM for flipping - ask them, not me
            sourcePixDim[1] = mystruct1.pixdim[2];
            sourcePixDim[2] = mystruct1.pixdim[3];
            break;
        }
        case 2:
        {
            Nifti2Header header2;
            myIO.getHeader(header2);
            header2.getSForm(sourceSform);
            header2.getDimensions(sourceDims);
            nifti_2_header mystruct2;
            header2.getHeaderStruct(mystruct2);
            sourcePixDim[0] = mystruct2.pixdim[1];
            sourcePixDim[1] = mystruct2.pixdim[2];
            sourcePixDim[2] = mystruct2.pixdim[3];
            break;
        }
        default:
            throw DataFileException("AffineFile doesn't know how to handle nifti version " + AString::number(myIO.getNiftiVersion()));
    }
    myIO.readFile(targetName);
    switch (myIO.getNiftiVersion())
    {
        case 1:
        {
            Nifti1Header header1;
            myIO.getHeader(header1);
            header1.getSForm(targetSform);
            header1.getDimensions(targetDims);
            nifti_1_header mystruct1;
            header1.getHeaderStruct(mystruct1);
            targetPixDim[0] = mystruct1.pixdim[1];
            targetPixDim[1] = mystruct1.pixdim[2];
            targetPixDim[2] = mystruct1.pixdim[3];
            break;
        }
        case 2:
        {
            Nifti2Header header2;
            myIO.getHeader(header2);
            header2.getSForm(targetSform);
            header2.getDimensions(targetDims);
            nifti_2_header mystruct2;
            header2.getHeaderStruct(mystruct2);
            targetPixDim[0] = mystruct2.pixdim[1];
            targetPixDim[1] = mystruct2.pixdim[2];
            targetPixDim[2] = mystruct2.pixdim[3];
            break;
        }
        default:
            throw DataFileException("AffineFile doesn't know how to handle nifti version " + AString::number(myIO.getNiftiVersion()));
    }
    if (sourceDims.size() < 3 || targetDims.size() < 3) throw DataFileException("AffineFile needs nifti files with 3 or more dimensions to interpret a flirt affine");
    float sourceDet = sourceSform[0][0] * sourceSform[1][1] * sourceSform[2][2] +
                      sourceSform[0][1] * sourceSform[1][2] * sourceSform[2][0] +
                      sourceSform[0][2] * sourceSform[1][0] * sourceSform[2][1] -
                      sourceSform[0][2] * sourceSform[1][1] * sourceSform[2][0] -
                      sourceSform[0][0] * sourceSform[1][2] * sourceSform[2][1] -
                      sourceSform[0][1] * sourceSform[1][0] * sourceSform[2][2];//just write out the 3x3 determinant rather than packing it into a FloatMatrix first - and I haven't put a determinant function in yet
    float targetDet = targetSform[0][0] * targetSform[1][1] * targetSform[2][2] +
                      targetSform[0][1] * targetSform[1][2] * targetSform[2][0] +
                      targetSform[0][2] * targetSform[1][0] * targetSform[2][1] -
                      targetSform[0][2] * targetSform[1][1] * targetSform[2][0] -
                      targetSform[0][0] * targetSform[1][2] * targetSform[2][1] -
                      targetSform[0][1] * targetSform[1][0] * targetSform[2][2];//just write out the 3x3 determinant rather than packing it into a FloatMatrix first - and I haven't put a determinant function in yet
    //via aff_conv : world = targmat * trgscale^-1 * input * srcscale * sourcemat^-1
    FloatMatrix sourceScale = FloatMatrix::identity(4), targetScale = FloatMatrix::identity(4);
    if (sourceDet > 0.0f)
    {
        sourceScale[0][0] = -sourcePixDim[0];
        sourceScale[0][3] = (sourceDims[0] - 1) * sourcePixDim[0];
    } else {
        sourceScale[0][0] = sourcePixDim[0];
    }
    sourceScale[1][1] = sourcePixDim[1];
    sourceScale[2][2] = sourcePixDim[2];
    if (targetDet > 0.0f)
    {
        targetScale[0][0] = -targetPixDim[0];
        targetScale[0][3] = (sourceDims[0] - 1) * targetPixDim[0];
    } else {
        targetScale[0][0] = targetPixDim[0];
    }
    targetScale[1][1] = targetPixDim[1];
    targetScale[2][2] = targetPixDim[2];
    FloatMatrix sourceMat(sourceSform), targetMat(targetSform);//NOTE: these are expected to return 4x4 matrices with the 0 0 0 1 row intact
    FloatMatrix world = targetMat * targetScale.inverse() * m_matrix * sourceScale * sourceMat.inverse();//store it in another variable temporarily so we can debug easier
    m_matrix = world;
}
