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

FloatMatrix AffineFile::read34(const AString& filename)
{
    FileInformation affineInfo(filename);
    if (!affineInfo.exists()) throw DataFileException("affine file '" + filename + "' does not exist");
    fstream affineFile(filename.toLocal8Bit().constData(), fstream::in);
    if (!affineFile.good()) throw DataFileException("error opening file '" + filename + "' for reading");
    FloatMatrix ret = FloatMatrix::identity(4);//to ensure the right size and the fourth 0 0 0 1 row
    for (int i = 0; i < 3; ++i)//DO NOT read the fourth row from the file into the matrix
    {
        for (int j = 0; j < 4; ++j)
        {
            affineFile >> ret[i][j];
            if (!affineFile) throw DataFileException("error while reading file '" + filename + "'");
        }
    }
    return ret;
}

void AffineFile::write44(const FloatMatrix& out, const AString& filename)
{
    fstream affineFile(filename.toLocal8Bit().constData(), fstream::out);
    if (!affineFile.good())
    {
        throw DataFileException("error opening file '" + filename + "' for writing");
    }
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            affineFile << out[i][j];
            if (!affineFile) throw DataFileException("error while writing file '" + filename + "'");
        }
    }
}

void AffineFile::readWorld(const AString& filename)
{
    m_matrix = read34(filename);//and that is it, no quirks
}

void AffineFile::writeWorld(const AString& filename)
{
    write44(m_matrix, filename);//ditto
}

void AffineFile::readFlirt(const AString& filename, const AString& sourceName, const AString& targetName)
{
    FloatMatrix flirtMat = read34(filename);
    FloatMatrix sourceMat, sourceScale, targetMat, targetScale;
    getFlirtInfo(sourceName, sourceMat, sourceScale);
    getFlirtInfo(targetName, targetMat, targetScale);
    //via aff_conv : world = targmat * trgscale^-1 * input * srcscale * sourcemat^-1
    m_matrix = targetMat * targetScale.inverse() * flirtMat * sourceScale * sourceMat.inverse();
}

void AffineFile::writeFlirt(const AString& filename, const AString& sourceName, const AString& targetName) const
{
    FloatMatrix sourceMat, sourceScale, targetMat, targetScale;
    getFlirtInfo(sourceName, sourceMat, sourceScale);
    getFlirtInfo(targetName, targetMat, targetScale);
    FloatMatrix flirtMat = targetScale * targetMat.inverse() * m_matrix * sourceMat * sourceScale.inverse();
    write44(flirtMat, filename);
}

void AffineFile::getFlirtInfo(const AString& niftiName, FloatMatrix& outSform, FloatMatrix& outScale) const
{
    float pixDim[3];//don't look at me, blame analyze and flirt
    vector<int64_t> dimensions;
    vector<vector<float> > sform;
    NiftiHeaderIO myIO;
    myIO.readFile(niftiName);
    switch (myIO.getNiftiVersion())
    {
        case 1:
        {
            Nifti1Header header1;
            myIO.getHeader(header1);
            header1.getSForm(sform);
            header1.getDimensions(dimensions);
            nifti_1_header mystruct1;
            header1.getHeaderStruct(mystruct1);
            pixDim[0] = mystruct1.pixdim[1];//yes, that is really what they use, despite checking the SFORM/QFORM for flipping - ask them, not me
            pixDim[1] = mystruct1.pixdim[2];
            pixDim[2] = mystruct1.pixdim[3];
            break;
        }
        case 2:
        {
            Nifti2Header header2;
            myIO.getHeader(header2);
            header2.getSForm(sform);
            header2.getDimensions(dimensions);
            nifti_2_header mystruct2;
            header2.getHeaderStruct(mystruct2);
            pixDim[0] = mystruct2.pixdim[1];
            pixDim[1] = mystruct2.pixdim[2];
            pixDim[2] = mystruct2.pixdim[3];
            break;
        }
        default:
            throw DataFileException("AffineFile doesn't know how to handle nifti version " + AString::number(myIO.getNiftiVersion()));
    }
    if (dimensions.size() < 3) throw DataFileException("Nifti file '" + niftiName + "' has less than 3 dimensions, can't be used to interpret a flirt affine");
    float determinant = sform[0][0] * sform[1][1] * sform[2][2] +
                        sform[0][1] * sform[1][2] * sform[2][0] +
                        sform[0][2] * sform[1][0] * sform[2][1] -
                        sform[0][2] * sform[1][1] * sform[2][0] -
                        sform[0][0] * sform[1][2] * sform[2][1] -
                        sform[0][1] * sform[1][0] * sform[2][2];//just write out the 3x3 determinant rather than packing it into a FloatMatrix first - and I haven't put a determinant function in yet
    outScale = FloatMatrix::identity(4);
    if (determinant > 0.0f)
    {
        outScale[0][0] = -pixDim[0];
        outScale[0][3] = (dimensions[0] - 1) * pixDim[0];
    } else {
        outScale[0][0] = pixDim[0];
    }
    outScale[1][1] = pixDim[1];
    outScale[2][2] = pixDim[2];
    outSform = FloatMatrix(sform);//NOTE: this is expected to return a 4x4 matrix with the 0 0 0 1 row intact
}
