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

#include "AffineFile.h"
#include "CaretAssert.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "NiftiIO.h"

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
    affineFile.setf(ios::fixed, ios::floatfield);
    affineFile.precision(10);//flirt appears to use 10, so do the same
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            affineFile << out[i][j];
            if (j < 3) affineFile << "  ";//double space like flirt
            if (!affineFile) throw DataFileException("error while writing file '" + filename + "'");
        }
        affineFile << endl;
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
    getFSLQuirks(sourceName, sourceMat, sourceScale);
    getFSLQuirks(targetName, targetMat, targetScale);
    //via aff_conv : world = targmat * trgscale^-1 * input * srcscale * sourcemat^-1
    m_matrix = targetMat * targetScale.inverse() * flirtMat * sourceScale * sourceMat.inverse();
}

void AffineFile::writeFlirt(const AString& filename, const AString& sourceName, const AString& targetName) const
{
    FloatMatrix sourceMat, sourceScale, targetMat, targetScale;
    getFSLQuirks(sourceName, sourceMat, sourceScale);
    getFSLQuirks(targetName, targetMat, targetScale);
    FloatMatrix flirtMat = targetScale * targetMat.inverse() * m_matrix * sourceMat * sourceScale.inverse();
    write44(flirtMat, filename);
}

void AffineFile::getFSLQuirks(const AString& niftiName, FloatMatrix& outSform, FloatMatrix& outScale)
{
    NiftiIO myIO;
    myIO.openRead(niftiName);
    outSform = FloatMatrix(myIO.getHeader().getSForm());//NOTE: this is expected to return a 4x4 matrix with the 0 0 0 1 row intact
    outScale = FloatMatrix(myIO.getHeader().getFSLSpace());
}
