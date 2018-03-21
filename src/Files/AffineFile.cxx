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
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "NiftiIO.h"

#include <fstream>
#include <sstream>
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

void AffineFile::readWorld(const AString& filename, bool inverse)
{
    FloatMatrix temp = read34(filename);
    if (inverse) temp = temp.inverse();//support reading reverse affines
    m_matrix = temp;
}

void AffineFile::writeWorld(const AString& filename, bool inverse) const
{
    FloatMatrix temp = m_matrix;
    if (inverse) temp = temp.inverse();//and writing
    write44(temp, filename);
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

void AffineFile::readITK(const AString& filename)
{//odd text format, need special parsing
    FileInformation itkInfo(filename);
    if (!itkInfo.exists()) throw DataFileException(filename, "ITK affine file does not exist");
    fstream itkFile(filename.toLocal8Bit().constData(), fstream::in);
    if (!itkFile.good()) throw DataFileException(filename, "error opening file for reading");
    string myline;
    if (!getline(itkFile, myline)) throw DataFileException(filename, "ITK affine file is empty");
    if (myline != "#Insight Transform File V1.0") throw DataFileException(filename, "wrong header line for ITK affine file");
    bool haveParam = false, haveFixed = false;//maybe they can be reordered, so just wait for both
    vector<float> params(12, 0.0f), fixed(3, 0.0f);
    while (!(haveParam && haveFixed))
    {
        if (!getline(itkFile, myline)) throw DataFileException(filename, "ITK affine file is truncated");
        if (AString::fromStdString(myline).startsWith("Transform: "))
        {
            if (myline != "Transform: MatrixOffsetTransformBase_double_3_3") throw DataFileException(filename, "unrecognized transformation type in ITK affine file");
        } else if (!haveParam && AString::fromStdString(myline).startsWith("Parameters: ")) {//don't parse more than one Parameters line, to make sure we get the first
            stringstream parsestring(myline);
            string junk;
            parsestring >> junk;//strip "Parameters: "
            for (size_t i = 0; i < params.size(); ++i)
            {
                if (!(parsestring >> params[i])) throw DataFileException(filename, "expected 12 values in Parameters line, only parsed " + AString::number(i));
            }
            haveParam = true;
            if (parsestring >> junk) CaretLogWarning("found extra characters on end of Parameters line in file '" + filename + "'");
        } else if (!haveFixed && AString::fromStdString(myline).startsWith("FixedParameters: ")) {//ditto
            stringstream parsestring(myline);
            string junk;
            parsestring >> junk;//strip "FixedParameters: "
            for (size_t i = 0; i < fixed.size(); ++i)
            {
                if (!(parsestring >> fixed[i])) throw DataFileException(filename, "expected 3 values in FixedParameters line, only parsed " + AString::number(i));
            }
            haveFixed = true;
            if (parsestring >> junk) CaretLogWarning("found extra characters on end of FixedParameters line in file '" + filename + "'");
        }
    }
    FloatMatrix temp = FloatMatrix::identity(4);//to ensure the right size and the fourth 0 0 0 1 row
    for (int i = 0; i < 3; ++i)
    {
        temp[i][3] = params[9 + i] + fixed[i];//"ComputeOffset", https://github.com/hinerm/ITK/blob/master/Modules/Core/Transform/include/itkMatrixOffsetTransformBase.hxx#L729
        for (int j = 0; j < 3; ++j)
        {
            temp[i][j] = params[i * 3 + j];//https://afni.nimh.nih.gov/afni/community/board/read.php?1,94478
            temp[i][3] -= temp[i][j] * fixed[j];//the rest of the first link
        }
    }
    FloatMatrix flips = FloatMatrix::identity(4);
    flips[0][0] = -1;
    flips[1][1] = -1;//this is its own inverse, so we don't need to compute the inverse
    m_matrix = (flips * temp * flips).inverse();//convert from reverse, LPS transform to forwards, RAS transform
}

void AffineFile::writeITK(const AString& filename) const
{
    fstream itkFile(filename.toLocal8Bit().constData(), fstream::out);
    FloatMatrix flips = FloatMatrix::identity(4);
    flips[0][0] = -1;
    flips[1][1] = -1;//this is its own inverse, so we don't need to compute the inverse
    FloatMatrix temp = (flips * m_matrix * flips).inverse();//convert from forwards RAS to inverse LPS
    if (!itkFile.good()) throw DataFileException(filename, "error opening ITK affine file for writing");
    itkFile.setf(ios::fixed, ios::floatfield);
    itkFile.precision(16);//ITK uses 16 because it uses doubles, we only use single, but make them look similar anyway
    itkFile << "#Insight Transform File V1.0" << endl;
    itkFile << "#Transform 0" << endl;
    itkFile << "Transform: MatrixOffsetTransformBase_double_3_3" << endl;
    itkFile << "Parameters: ";
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            itkFile << " " << temp[i][j];
        }
    }
    for (int i = 0; i < 3; ++i)//use 0, 0, 0 center to eliminate translation/offset conversion silliness
    {
        itkFile << " " << temp[i][3];
    }
    itkFile << endl;
    itkFile << "FixedParameters: 0 0 0" << endl;
}
