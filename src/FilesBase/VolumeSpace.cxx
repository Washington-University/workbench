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

#include "VolumeSpace.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
#include "FloatMatrix.h"

#include <QRegExp>
#include <QStringList>

#include <algorithm>
#include <cmath>

using namespace std;
using namespace caret;

VolumeSpace::VolumeSpace()
{
    m_dims[0] = 0;
    m_dims[1] = 0;
    m_dims[2] = 0;
    m_sform = FloatMatrix::identity(4).getMatrix();
    computeInverse();
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    setSpace(dims, sform);
}

VolumeSpace::VolumeSpace(const int64_t dims[3], const float sform[12])
{
    setSpace(dims, sform);
}

void VolumeSpace::setSpace(const int64_t dims[3], const vector<vector<float> >& sform)
{
    if (sform.size() < 3 || sform.size() > 4)
    {
        CaretAssert(false);
        throw CaretException("VolumeSpace initialized with wrong size sform");
    }
    for (int i = 0; i < (int)sform.size(); ++i)
    {
        if (sform[i].size() != 4)
        {
            CaretAssert(false);
            throw CaretException("VolumeSpace initialized with wrong size sform");
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    m_sform = sform;
    m_sform.resize(4);//make sure its 4x4
    m_sform[3].resize(4);
    m_sform[3][0] = 0.0f;//force the fourth row to be correct
    m_sform[3][1] = 0.0f;
    m_sform[3][2] = 0.0f;
    m_sform[3][3] = 1.0f;
    computeInverse();
}

void VolumeSpace::setSpace(const int64_t dims[3], const float sform[12])
{
    m_sform = FloatMatrix::identity(4).getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            m_sform[i][j] = sform[i * 4 + j];
        }
    }
    m_dims[0] = dims[0];
    m_dims[1] = dims[1];
    m_dims[2] = dims[2];
    computeInverse();
}

void VolumeSpace::computeInverse()
{
    m_inverse = FloatMatrix(m_sform).inverse().getMatrix();
}

void VolumeSpace::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3) const
{
    indexOut1 = coordIn1 * m_inverse[0][0] + coordIn2 * m_inverse[0][1] + coordIn3 * m_inverse[0][2] + m_inverse[0][3];
    indexOut2 = coordIn1 * m_inverse[1][0] + coordIn2 * m_inverse[1][1] + coordIn3 * m_inverse[1][2] + m_inverse[1][3];
    indexOut3 = coordIn1 * m_inverse[2][0] + coordIn2 * m_inverse[2][1] + coordIn3 * m_inverse[2][2] + m_inverse[2][3];
}

void VolumeSpace::enclosingVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const
{
    float tempInd1, tempInd2, tempInd3;
    spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
    indexOut1 = (int64_t)floor(0.5f + tempInd1);
    indexOut2 = (int64_t)floor(0.5f + tempInd2);
    indexOut3 = (int64_t)floor(0.5f + tempInd3);
}

bool VolumeSpace::matches(const VolumeSpace& right) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_dims[i] != right.m_dims[i])
        {
            return false;
        }
    }
    const float TOLER_RATIO = 0.999f;//ratio a spacing element can mismatch by
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float leftelem = m_sform[i][j];
            float rightelem = right.m_sform[i][j];
            if ((leftelem != rightelem) && (leftelem == 0.0f || rightelem == 0.0f || (leftelem / rightelem < TOLER_RATIO || rightelem / leftelem < TOLER_RATIO)))
            {
                return false;
            }
        }
    }
    return true;
}

bool VolumeSpace::operator==(const VolumeSpace& right) const
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_dims[i] != right.m_dims[i])
        {
            return false;
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (m_sform[i][j] != right.m_sform[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

void VolumeSpace::getSpacingVectors(Vector3D& iStep, Vector3D& jStep, Vector3D& kStep, Vector3D& origin) const
{
    FloatMatrix(m_sform).getAffineVectors(iStep, jStep, kStep, origin);
}

float VolumeSpace::getVoxelVolume() const
{
    Vector3D spacingVecs[4];
    getSpacingVectors(spacingVecs[0], spacingVecs[1], spacingVecs[2], spacingVecs[3]);
    return abs(spacingVecs[0].dot(spacingVecs[1].cross(spacingVecs[2])));
}

void VolumeSpace::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* originOut) const
{
    CaretAssert(isPlumb());
    if (!isPlumb())
    {
        throw CaretException("orientation and spacing asked for on non-plumb volume space");//this will fail MISERABLY on non-plumb volumes, so throw otherwise
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                spacingOut[j] = m_sform[i][j];
                originOut[j] = m_sform[i][3];
                bool negative = (m_sform[i][j] < 0.0f);
                switch (i)
                {
                case 0:
                    //left/right
                    orientOut[j] = (negative ? RIGHT_TO_LEFT : LEFT_TO_RIGHT);
                    break;
                case 1:
                    //forward/back
                    orientOut[j] = (negative ? ANTERIOR_TO_POSTERIOR : POSTERIOR_TO_ANTERIOR);
                    break;
                case 2:
                    //up/down
                    orientOut[j] = (negative ? SUPERIOR_TO_INFERIOR : INFERIOR_TO_SUPERIOR);
                    break;
                default:
                    //will never get called
                    break;
                };
            }
        }
    }
}

void VolumeSpace::getOrientation(OrientTypes orientOut[3]) const
{
    Vector3D ivec, jvec, kvec, origin;
    getSpacingVectors(ivec, jvec, kvec, origin);
    int next = 1, bestarray[3] = {0, 0, 0};
    float bestVal = -1.0f;//make sure at least the first test trips true, if there is a zero spacing vector it will default to report LPI
    for (int first = 0; first < 3; ++first)//brute force search for best fit - only 6 to try
    {
        int third = 3 - first - next;
        float testVal = abs(ivec[first] * jvec[next] * kvec[third]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = next;
        }
        testVal = abs(ivec[first] * jvec[third] * kvec[next]);
        if (testVal > bestVal)
        {
            bestVal = testVal;
            bestarray[0] = first;
            bestarray[1] = third;
        }
        next = 0;
    }
    bestarray[2] = 3 - bestarray[0] - bestarray[1];
    Vector3D spaceHats[3];//to translate into enums without casting
    spaceHats[0] = ivec;
    spaceHats[1] = jvec;
    spaceHats[2] = kvec;
    for (int i = 0; i < 3; ++i)
    {
        bool neg = (spaceHats[i][bestarray[i]] < 0.0f);
        switch (bestarray[i])
        {
            case 0:
                if (neg)
                {
                    orientOut[i] = RIGHT_TO_LEFT;
                } else {
                    orientOut[i] = LEFT_TO_RIGHT;
                }
                break;
            case 1:
                if (neg)
                {
                    orientOut[i] = ANTERIOR_TO_POSTERIOR;
                } else {
                    orientOut[i] = POSTERIOR_TO_ANTERIOR;
                }
                break;
            case 2:
                if (neg)
                {
                    orientOut[i] = SUPERIOR_TO_INFERIOR;
                } else {
                    orientOut[i] = INFERIOR_TO_SUPERIOR;
                }
                break;
            default:
                CaretAssert(0);
        }
    }
}

bool VolumeSpace::isPlumb() const
{
    char axisUsed = 0;
    char indexUsed = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_sform[i][j] != 0.0f)
            {
                if (axisUsed & (1<<i))
                {
                    return false;
                }
                if (indexUsed & (1<<j))
                {
                    return false;
                }
                axisUsed |= (1<<i);
                indexUsed |= (1<<j);
            }
        }
    }
    return true;
}

void VolumeSpace::readCiftiXML1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("VolumeDimensions"))
    {
        throw CaretException("Volume element missing VolumeDimensions attribute");
    }
    QStringList dimStrings = attributes.value("VolumeDimensions").toString().split(',');
    if (dimStrings.size() != 3)
    {
        throw CaretException("VolumeDimensions attribute of Volume must contain exactly two commas");
    }
    int64_t newDims[3];//don't parse directly into the internal variables
    bool ok = false;
    for (int i = 0; i < 3; ++i)
    {
        newDims[i] = dimStrings[i].toLongLong(&ok);
        if (!ok)
        {
            throw CaretException("noninteger found in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
        if (newDims[i] < 1)
        {
            throw CaretException("found bad value in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
    }
    if (!xml.readNextStartElement())
    {
        throw CaretException("failed to find TransformationMatrixVoxelIndicesIJKtoXYZ element in Volume");
    }
    if (xml.name() != "TransformationMatrixVoxelIndicesIJKtoXYZ")
    {
        throw CaretException("unexpected element in Volume: " + xml.name().toString());
    }
    QXmlStreamAttributes transattribs = xml.attributes();
    if (!transattribs.hasAttribute("UnitsXYZ"))//ignore the space attributes
    {
        throw CaretException("missing UnitsXYZ attribute in TransformationMatrixVoxelIndicesIJKtoXYZ");
    }
    float mult = 0.0f;
    QStringRef unitstring = transattribs.value("UnitsXYZ");
    if (unitstring == "NIFTI_UNITS_MM")
    {
        mult = 1.0f;
    } else if (unitstring == "NIFTI_UNITS_MICRON") {
        mult = 0.001f;
    } else {
        throw CaretException("unrecognized value for UnitsXYZ in TransformationMatrixVoxelIndicesIJKtoXYZ: " + unitstring.toString());
    }
    QString accum = xml.readElementText();
    if (xml.hasError()) return;
    QStringList matrixStrings = accum.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (matrixStrings.size() != 16)
    {
        throw CaretException("text content of TransformationMatrixVoxelIndicesIJKtoXYZ must have exactly 16 numbers separated by whitespace");
    }
    FloatMatrix newsform = FloatMatrix::zeros(4, 4);
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            newsform[j][i] = matrixStrings[i + j * 4].toFloat(&ok);
            if (!ok)
            {
                throw CaretException("non-number in text of TransformationMatrixVoxelIndicesIJKtoXYZ: " + matrixStrings[i + j * 4]);
            }
        }
    }
    if (newsform[3][0] != 0.0f || newsform[3][1] != 0.0f || newsform[3][2] != 0.0f || newsform[3][3] != 1.0f)//yes, using equals, because those are all exact in float
    {
        CaretLogFiner("last row of matrix in TransformationMatrixVoxelIndicesIJKtoXYZ is not 0 0 0 1");//not error, because apparently we screwed this up in the previous cifti-1 writer
    }
    if (xml.readNextStartElement())//find Volume end element
    {
        throw CaretException("unexpected element in Volume: " + xml.name().toString());
    }
    newsform *= mult;//apply units
    newsform[3][3] = 1.0f;//reset [3][3], since it isn't spatial
    setSpace(newDims, newsform.getMatrix());//use public function so it can deal with invariants, and we can move this code elsewhere if needed
    CaretAssert(xml.isEndElement() && xml.name() == "Volume");
}

void VolumeSpace::readCiftiXML2(QXmlStreamReader& xml)
{//we changed stuff, so separate code
    QXmlStreamAttributes attributes = xml.attributes();
    if (!attributes.hasAttribute("VolumeDimensions"))
    {
        throw CaretException("Volume element missing VolumeDimensions attribute");
    }
    QStringList dimStrings = attributes.value("VolumeDimensions").toString().split(',');
    if (dimStrings.size() != 3)
    {
        throw CaretException("VolumeDimensions attribute of Volume must contain exactly two commas");
    }
    int64_t newDims[3];//don't parse directly into the internal variables
    bool ok = false;
    for (int i = 0; i < 3; ++i)
    {
        newDims[i] = dimStrings[i].toLongLong(&ok);
        if (!ok)
        {
            throw CaretException("noninteger found in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
        if (newDims[i] < 1)
        {
            throw CaretException("found bad value in VolumeDimensions attribute of Volume: " + dimStrings[i]);
        }
    }
    if (!xml.readNextStartElement())
    {
        throw CaretException("failed to find TransformationMatrixVoxelIndicesIJKtoXYZ element in Volume");
    }
    if (xml.name() != "TransformationMatrixVoxelIndicesIJKtoXYZ")
    {
        throw CaretException("unexpected element in Volume: " + xml.name().toString());
    }
    QXmlStreamAttributes transattribs = xml.attributes();
    if (!transattribs.hasAttribute("MeterExponent"))//ignore the space attributes
    {
        throw CaretException("missing MeterExponent attribute in TransformationMatrixVoxelIndicesIJKtoXYZ");
    }
    QStringRef exponentstring = transattribs.value("MeterExponent");
    int exponent = exponentstring.toString().toInt(&ok);
    if (!ok)
    {
        throw CaretException("noninteger value for MeterExponent in TransformationMatrixVoxelIndicesIJKtoXYZ: " + exponentstring.toString());
    }
    float mult = pow(10.0f, exponent + 3);//because our internal units are mm
    QString accum = xml.readElementText();
    if (xml.hasError()) return;
    QStringList matrixStrings = accum.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (matrixStrings.size() != 16)
    {
        throw CaretException("text content of TransformationMatrixVoxelIndicesIJKtoXYZ must have exactly 16 numbers separated by whitespace");
    }
    FloatMatrix newsform = FloatMatrix::zeros(4, 4);
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 4; ++i)
        {
            newsform[j][i] = matrixStrings[i + j * 4].toFloat(&ok);
            if (!ok)
            {
                throw CaretException("non-number in text of TransformationMatrixVoxelIndicesIJKtoXYZ: " + matrixStrings[i + j * 4]);
            }
        }
    }
    if (newsform[3][0] != 0.0f || newsform[3][1] != 0.0f || newsform[3][2] != 0.0f || newsform[3][3] != 1.0f)//yes, using equals, because those are all exact in float
    {
        throw CaretException("last row of matrix in TransformationMatrixVoxelIndicesIJKtoXYZ must be 0 0 0 1");
    }
    if (xml.readNextStartElement())//find Volume end element
    {
        throw CaretException("unexpected element in Volume: " + xml.name().toString());
    }
    newsform *= mult;//apply units
    newsform[3][3] = 1.0f;//reset [3][3], since it isn't spatial
    setSpace(newDims, newsform.getMatrix());//use public function so it can deal with invariants, and we can move this code elsewhere if needed
    CaretAssert(xml.isEndElement() && xml.name() == "Volume");
}

void VolumeSpace::writeCiftiXML1(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("Volume");
    AString dimString = AString::number(m_dims[0]) + "," + AString::number(m_dims[1]) + "," + AString::number(m_dims[2]);
    xml.writeAttribute("VolumeDimensions", dimString);
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");
    xml.writeAttribute("DataSpace", "NIFTI_XFORM_UNKNOWN");//meaningless attribute
    xml.writeAttribute("TransformedSpace", "NIFTI_XFORM_UNKNOWN");//removed in CIFTI-2, but apparently we have been writing this value for CIFTI-1
    xml.writeAttribute("UnitsXYZ", "NIFTI_UNITS_MM");//only other choice in cifti-1 is micron, which we will probably never need in cifti-1 - writing cifti-1 is something we won't need soon, hopefully
    AString matrixString;
    for (int j = 0; j < 3; ++j)
    {
        matrixString += "\n";
        for (int i = 0; i < 4; ++i)
        {
            matrixString += AString::number(m_sform[j][i], 'f', 7) + " ";
        }
    }
    matrixString += "\n";
    for (int i = 0; i < 3; ++i)
    {
        matrixString += AString::number(0.0f, 'f', 7) + " ";
    }
    matrixString += AString::number(1.0f, 'f', 7);
    xml.writeCharacters(matrixString);
    xml.writeEndElement();//Transfor...
    xml.writeEndElement();//Volume
}

void VolumeSpace::writeCiftiXML2(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("Volume");
    AString dimString = AString::number(m_dims[0]) + "," + AString::number(m_dims[1]) + "," + AString::number(m_dims[2]);
    xml.writeAttribute("VolumeDimensions", dimString);
    xml.writeStartElement("TransformationMatrixVoxelIndicesIJKtoXYZ");
    Vector3D vecs[4];
    getSpacingVectors(vecs[0], vecs[1], vecs[2], vecs[3]);
    float minLength = vecs[0].length();
    for (int i = 1; i < 3; ++i)
    {
        minLength = min(minLength, vecs[i].length());
    }
    int myExponent = -3;//if we have a singular spatial dimension somehow, just use mm
    if (minLength != 0.0f)
    {
        myExponent = 3 * (int)floor((log10(minLength) - log10(50.0f)) / 3.0f);//some magic to get the exponent that is a multiple of 3 that puts the length of the smallest spacing vector in [0.05, 50]
    }
    float multiplier = pow(10.0f, -3 - myExponent);//conversion factor from mm
    xml.writeAttribute("MeterExponent", AString::number(myExponent));
    AString matrixString;
    for (int j = 0; j < 3; ++j)
    {
        matrixString += "\n";
        for (int i = 0; i < 4; ++i)
        {
            matrixString += AString::number(m_sform[j][i] * multiplier, 'f', 7) + " ";
        }
    }
    matrixString += "\n";
    for (int i = 0; i < 3; ++i)
    {
        matrixString += AString::number(0.0f, 'f', 7) + " ";
    }
    matrixString += AString::number(1.0f, 'f', 7);//doesn't get multiplied, because it isn't spatial
    xml.writeCharacters(matrixString);
    xml.writeEndElement();//Transfor...
    xml.writeEndElement();//Volume
}
