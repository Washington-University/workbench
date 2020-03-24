/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
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

#include "AffineSeriesFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "NiftiIO.h"

#include <QFile>

#include <fstream>
#include <sstream>
#include <string>

using namespace caret;
using namespace std;

namespace
{
    void getFSLQuirks(const AString& niftiName, FloatMatrix& outSform, FloatMatrix& outScale)
    {
        NiftiIO myIO;
        myIO.openRead(niftiName);
        outSform = FloatMatrix(myIO.getHeader().getSForm());//NOTE: this is expected to return a 4x4 matrix with the 0 0 0 1 row intact
        outScale = FloatMatrix(myIO.getHeader().getFSLSpace());
    }

    vector<FloatMatrix> readFileRaw(const AString& filename, const bool inverse = false)
    {
        vector<FloatMatrix> ret;
        FileInformation fileInfo(filename);
        if (!fileInfo.exists()) throw DataFileException("affine series file '" + filename + "' does not exist");
        ifstream affineSeriesFile(filename.toStdString());
        if (!affineSeriesFile.good()) throw DataFileException("error opening file '" + filename + "' for reading");
        string myline;
        int whatline = 0;//will be 1-based, but this way we can increment at top of loop
        bool emptyLine = false;//track if we have found an empty line, so it can be an error when in the middle of the file
        FloatMatrix tempMatrix = FloatMatrix::identity(4);
        while (getline(affineSeriesFile, myline))//this needs to be a function for reuse
        {
            ++whatline;
            stringstream mystr(myline);
            vector<float> linevals;
            float tempf;
            while (linevals.size() < 17 && (mystr >> tempf))
            {
                linevals.push_back(tempf);
            }
            switch (linevals.size())
            {
                case 16:
                    if (linevals[12] != 0.0f || linevals[13] != 0.0f || linevals[14] != 0.0f || linevals[15] != 1.0f)
                    {
                        throw DataFileException("values 13 through 16 in file '" + filename + "', line " + AString::number(whatline) + " are not equal to '0 0 0 1'");
                    }//intentional fallthrough
                case 12:
                    if (emptyLine)
                    {//we have seen an empty line, consider it an error even if next content is valid
                        throw DataFileException("empty line in file '" + filename + "', line " + AString::number(whatline - 1));
                    }
                    for (int i = 0; i < 3; ++i)
                    {
                        for (int j = 0; j < 4; ++j)
                        {
                            tempMatrix[i][j] = linevals[j + i * 4];
                        }
                    }
                    if (inverse)
                    {
                        ret.push_back(tempMatrix.inverse());
                    } else {
                        ret.push_back(tempMatrix);
                    }
                    break;
                case 0:
                    if (AString(myline).trimmed().size() == 0)
                    {
                        emptyLine = true;//accept empty lines on end of file, but not in middle
                    }
                    break;
                default:
                    throw DataFileException("malformed line in file '" + filename + "', line " + AString::number(whatline));
            }
        }
        return ret;
    }

    //write as 12-parameter, because there may be nobody else using this format
    void writeFileRaw(const AString& filename, const vector<FloatMatrix> matrixList, const bool inverse = false)
    {
        QFile::remove(filename);
        ofstream outFile(filename.toStdString());
        for (auto matrix : matrixList)
        {
            if (inverse) matrix = matrix.inverse();
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    if (i != 0 && j != 0) outFile << "\t";
                    outFile << matrix[i][j];
                }
            }
            outFile << endl;
        }
    }
}

void AffineSeriesFile::readWorld(const AString& filename, const bool inverse)
{
    m_matrixList = readFileRaw(filename, inverse);
}

void AffineSeriesFile::writeWorld(const AString& filename, const bool inverse) const
{
    writeFileRaw(filename, m_matrixList, inverse);
}

void AffineSeriesFile::readFlirt(const AString& filename, const AString& sourceName, const AString& targetName)
{
    vector<FloatMatrix> flirtMatList = readFileRaw(filename);
    FloatMatrix sourceMat, sourceScale, targetMat, targetScale;
    getFSLQuirks(sourceName, sourceMat, sourceScale);
    getFSLQuirks(targetName, targetMat, targetScale);
    m_matrixList.resize(flirtMatList.size());
    //via aff_conv : world = targmat * trgscale^-1 * input * srcscale * sourcemat^-1
    //m_matrix = targetMat * targetScale.inverse() * flirtMat * sourceScale * sourceMat.inverse();
    for (size_t i = 0; i < flirtMatList.size(); ++i)
    {
        m_matrixList[i] = targetMat * targetScale.inverse() * flirtMatList[i] * sourceScale * sourceMat.inverse();
    }
}

void AffineSeriesFile::writeFlirt(const AString& filename, const AString& sourceName, const AString& targetName) const
{
    vector<FloatMatrix> flirtListOut(m_matrixList.size());
    FloatMatrix sourceMat, sourceScale, targetMat, targetScale;
    getFSLQuirks(sourceName, sourceMat, sourceScale);
    getFSLQuirks(targetName, targetMat, targetScale);
    //FloatMatrix flirtMat = targetScale * targetMat.inverse() * m_matrix * sourceMat * sourceScale.inverse();
    for (size_t i = 0; i < m_matrixList.size(); ++i)
    {
        flirtListOut[i] = targetScale * targetMat.inverse() * m_matrixList[i] * sourceMat * sourceScale.inverse();
    }
    writeFileRaw(filename, flirtListOut);
}

vector<FloatMatrix> AffineSeriesFile::getInverseMatrixList() const
{
    vector<FloatMatrix> ret(m_matrixList.size());
    for (size_t i = 0; i < m_matrixList.size(); ++i)
    {
        ret[i] = m_matrixList[i].inverse();
    }
    return ret;
}

void AffineSeriesFile::setMatrixList(const vector<FloatMatrix>& matrixList)
{
    for (auto& matrix : matrixList)
    {
        if (matrix.getNumberOfColumns() != 4) throw DataFileException("all matrices must have 4 columns");
        if (matrix.getNumberOfRows() != 4) throw DataFileException("all matrices must have 4 rows");
    }
    m_matrixList = matrixList;
}
