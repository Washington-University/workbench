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

#include "VolumeFile.h"
#include "FloatMatrix.h"
#include <cmath>
#include "NiftiHeaderIO.h"

using namespace caret;
using namespace std;

void VolumeFile::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    freeMemory();
    CaretAssert(dimensionsIn.size() >= 3);
    CaretAssert(indexToSpace.size() >= 3);//support using the 3x4 part of a 4x4 matrix
    CaretAssert(indexToSpace[0].size() == 4);
    CaretAssert(indexToSpace[1].size() == 4);
    CaretAssert(indexToSpace[2].size() == 4);
    m_indexToSpace = indexToSpace;
    m_indexToSpace.resize(3);//drop the fourth row if it exists
    FloatMatrix temp(m_indexToSpace);
    FloatMatrix temp2 = temp.getRange(0, 0, 3, 3);//get the multiplicative part
    FloatMatrix temp3 = temp.getRange(0, 3, 3, 4);//get the additive part
    temp = temp2.inverse();//invert multiplicative part
    temp2 = temp * -temp3;//multiply the reversed vector by the inverse of the spacing to get the reverse origin
    temp3 = temp.concatHoriz(temp2);//concatenate reverse origin with inverse spacing to get inverted affine
    m_spaceToIndex = temp3.getMatrix();
    m_dimensions[0] = dimensionsIn[0];
    m_dimensions[1] = dimensionsIn[1];
    m_dimensions[2] = dimensionsIn[2];
    m_dimensions[3] = 1;
    for (int i = 3; i < (int)dimensionsIn.size(); ++i)
    {
        if (dimensionsIn[i] != 0)
        {
            m_dimensions[3] *= dimensionsIn[i];
        }
    }
    m_dimensions[4] = numComponents;
    int64_t totalSize = m_dimensions[0] * m_dimensions[1] * m_dimensions[2] * m_dimensions[3] * m_dimensions[4];
    m_data = new float[totalSize];
    CaretAssert(m_data != NULL);
    setupIndexing();
    //TODO: adjust any existing nifti header to match, or remove nifti header?
}

VolumeFile::VolumeFile()
{
    m_data = NULL;
    m_headerType = NONE;
    m_indexRef = NULL;
    m_dimensions[0] = 0;
    m_dimensions[1] = 0;
    m_dimensions[2] = 0;
    m_dimensions[3] = 0;
    m_dimensions[4] = 0;
    m_indexToSpace.resize(3);
    for (int i = 0; i < 3; ++i)
    {
        m_indexToSpace[i].resize(4);
        for (int j = 0; j < 4; ++j)
        {
            m_indexToSpace[i][j] = ((i == j) ? 1.0f : 0.0f);//default 1mm spacing, no origin
        }
    }
    m_spaceToIndex = m_indexToSpace;
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    m_data = NULL;
    m_headerType = NONE;
    m_indexRef = NULL;
    reinitialize(dimensionsIn, indexToSpace, numComponents);
}

void VolumeFile::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* centerOut)
{
    CaretAssert(isPlumb());//this will fail MISERABLY on non-plumb volumes, so assert plumb
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_indexToSpace[i][j] != 0.0f)
            {
                spacingOut[j] = m_indexToSpace[i][j];
                centerOut[j] = m_indexToSpace[i][3];
                bool negative;
                if (m_indexToSpace[i][j] > 0.0f)
                {
                    negative = true;
                } else {
                    negative = false;
                }
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

void VolumeFile::closestVoxel(const float* coordIn, int64_t* indexOut)
{
    closestVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t* indexOut)
{
    closestVoxel(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::closestVoxel(const float* coordIn, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3)
{
    closestVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeFile::closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3)
{
    float tempInd1, tempInd2, tempInd3;
    spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
    indexOut1 = (int32_t)floor(0.5f + tempInd1);
    indexOut2 = (int32_t)floor(0.5f + tempInd2);
    indexOut3 = (int32_t)floor(0.5f + tempInd3);
}

void VolumeFile::getDimensions(vector<int64_t>& dimOut)
{
    dimOut.resize(5);
    getDimensions(dimOut[0], dimOut[1], dimOut[2], dimOut[3], dimOut[4]);
}

void VolumeFile::getDimensions(int64_t& dimOut1, int64_t& dimOut2, int64_t& dimOut3, int64_t& dimBricksOut, int64_t& numComponents)
{
    dimOut1 = m_dimensions[0];
    dimOut2 = m_dimensions[1];
    dimOut3 = m_dimensions[2];
    dimBricksOut = m_dimensions[3];
    numComponents = m_dimensions[4];
}

int64_t VolumeFile::getIndex(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t brickIndex, const int64_t component)
{//the component is split out, so you have the entire R volume timeseries, then entire G, then entire B, as stored in memory, this will make indexing tricks less memory intensive
    //return indexIn1 + m_jMult[indexIn2] + m_kMult[indexIn3] + m_bMult[brickIndex] + m_cMult[component];
    //HACK: use pointer math and the indexing array to get the index
    return (m_indexRef[component][brickIndex][indexIn3][indexIn2] + indexIn1) - m_data;
}//doesn't seem to have a performance drawback to order them in memory this way

void VolumeFile::indexToSpace(const int64_t* indexIn, float* coordOut)
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float* coordOut)
{
    indexToSpace(indexIn1, indexIn2, indexIn3, coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const int64_t* indexIn, float& coordOut1, float& coordOut2, float& coordOut3)
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeFile::indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3)
{
    //do we want an assert here?  I think it is okay to find the theoretical coordinates of an undefined voxel
    coordOut1 = m_indexToSpace[0][0] * indexIn1 + m_indexToSpace[0][1] * indexIn2 + m_indexToSpace[0][2] * indexIn3 + m_indexToSpace[0][3];
    coordOut2 = m_indexToSpace[1][0] * indexIn1 + m_indexToSpace[1][1] * indexIn2 + m_indexToSpace[1][2] * indexIn3 + m_indexToSpace[1][3];
    coordOut3 = m_indexToSpace[2][0] * indexIn1 + m_indexToSpace[2][1] * indexIn2 + m_indexToSpace[2][2] * indexIn3 + m_indexToSpace[2][3];
}

void VolumeFile::indexToSpace(const float* indexIn, float* coordOut)
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float* coordOut)
{
    indexToSpace(indexIn1, indexIn2, indexIn3, coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const float* indexIn, float& coordOut1, float& coordOut2, float& coordOut3)
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeFile::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3)
{
    coordOut1 = m_indexToSpace[0][0] * indexIn1 + m_indexToSpace[0][1] * indexIn2 + m_indexToSpace[0][2] * indexIn3 + m_indexToSpace[0][3];
    coordOut2 = m_indexToSpace[1][0] * indexIn1 + m_indexToSpace[1][1] * indexIn2 + m_indexToSpace[1][2] * indexIn3 + m_indexToSpace[1][3];
    coordOut3 = m_indexToSpace[2][0] * indexIn1 + m_indexToSpace[2][1] * indexIn2 + m_indexToSpace[2][2] * indexIn3 + m_indexToSpace[2][3];
}

bool VolumeFile::isPlumb()
{
    char axisUsed = 0;
    char indexUsed = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (m_indexToSpace[i][j] != 0.0f)
            {
                if (axisUsed & (1<<i))
                {
                    return false;
                }
                if (indexUsed & (1<<j))
                {
                    return false;
                }
                axisUsed &= (1<<i);
                indexUsed &= (1<<j);
            }
        }
    }
    return true;
}

void VolumeFile::spaceToIndex(const float* coordIn, float* indexOut)
{
    spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float* indexOut)
{
    spaceToIndex(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::spaceToIndex(const float* coordIn, float& indexOut1, float& indexOut2, float& indexOut3)
{
    spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeFile::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3)
{
    indexOut1 = m_spaceToIndex[0][0] * coordIn1 + m_spaceToIndex[0][1] * coordIn2 + m_spaceToIndex[0][2] * coordIn3 + m_spaceToIndex[0][3];
    indexOut2 = m_spaceToIndex[1][0] * coordIn1 + m_spaceToIndex[1][1] * coordIn2 + m_spaceToIndex[1][2] * coordIn3 + m_spaceToIndex[1][3];
    indexOut3 = m_spaceToIndex[2][0] * coordIn1 + m_spaceToIndex[2][1] * coordIn2 + m_spaceToIndex[2][2] * coordIn3 + m_spaceToIndex[2][3];
}

void VolumeFile::freeMemory()
{
    if (m_data != NULL)
    {
        delete[] m_data;
        m_data = NULL;
    }
    if (m_indexRef != NULL)
    {//assume the entire thing exists
        delete[] m_indexRef[0][0][0];//they were actually allocated as only 4 flat arrays
        delete[] m_indexRef[0][0];
        delete[] m_indexRef[0];
        delete[] m_indexRef;
        m_indexRef = NULL;
    }
}

void VolumeFile::setupIndexing()
{//must have valid m_dimensions and m_data before calling this, and already have the previous indexing freed
    int64_t dim43 = m_dimensions[4] * m_dimensions[3];//sizes for the reverse indexing lookup arrays
    int64_t dim432 = dim43 * m_dimensions[2];
    int64_t dim4321 = dim432 * m_dimensions[1];
    int64_t dim01 = m_dimensions[0] * m_dimensions[1];//size of an xy slice
    /*int64_t dim012 = dim01 * m_dimensions[2];//size of a frame
    int64_t dim0123 = dim012 * m_dimensions[3];//*/ //size of a timeseries (single component)
    m_indexRef = new float****[m_dimensions[4]];//do dimensions in reverse order, since dim[0] moves by one float at a time
    m_indexRef[0] = new float***[dim43];//this way, you can use m_indexRef[c][t][z][y][x] to get the value with only lookups
    m_indexRef[0][0] = new float**[dim432];
    m_indexRef[0][0][0] = new float*[dim4321];
    /*m_cMult.resize(m_dimensions[4]);//these aren't the size of the lookup arrays because we can do the math manually and take less memory
    m_bMult.resize(m_dimensions[3]);//it is probably slightly slower to do the math manually than to do lookups, so have both
    m_kMult.resize(m_dimensions[2]);//its possible we could hack getIndex to use the indexing array and pointer math from m_data, but that would be a brutal hack...though it would make them the same...
    m_jMult.resize(m_dimensions[1]);//m_iMult doesn't exist because the first index isn't multiplied by anthing, so can be added directly
    for (int64_t i = 0; i < m_dimensions[1]; ++i)
    {
       m_jMult[i] = i * m_dimensions[0];
    }
    for (int64_t i = 0; i < m_dimensions[2]; ++i)
    {
       m_kMult[i] = i * dim01;
    }
    for (int64_t i = 0; i < m_dimensions[3]; ++i)
    {
       m_bMult[i] = i * dim012;
    }//*/ //TSC: commented out but preserved in case the pointer math getIndex hack is frowned on
    //
    //EXPLANATION TIME
    //
    //Apologies for the oddity below, it is highly obtuse due to the effort of avoiding a large number of multiplies
    //what it actually does is set up m_indexRef to be an array of references into m_indexRef[0], with a skip size equal to dim[3], and each m_indexRef[i] indexes into m_indexRef[0][0] with a skip of dim[2], etc
    //at the final level, it indexes into m_data with a skip of dim[0]
    //what this accomplishes is that the lookup m_indexRef[component][brick[k][j][i] will be the data value at the index (i, j, k, brick, component), with no multiplications whatsoever
    //this allows getVoxel and setVoxel to be faster than a standard index calculating flat array scheme, and actually makes it simpler to get a value from the array at an index
    //as long as the dimension that steps by 1 in m_data is large, it takes relatively little memory to accomplish, compared to the entire volume
    //
    //if this is too much of a hassle, the trick of precalculating the multiples of dim[0], dim[0] * dim[1], etc, for all values of each of the dimensions is nearly as fast and easy to use, while being more intuitive
    //the code for this is above, commented out, plus a line in the outer loop below
    int64_t cbase = 0;
    for (int64_t c = 0; c < m_dimensions[4]; ++c)
    {
        //m_cMult[c] = c * dim0123;//NOTE: this line is for precalculating multiples for a different way of calculating indexes than the simple formula
        m_indexRef[c] = m_indexRef[0] + cbase;//pointer math, redundant for [0], but [0][1], etc needs to get set, so it is easier to loop including 0
        int64_t bbase = cbase * m_dimensions[2];
        for (int64_t b = 0; b < m_dimensions[3]; ++b)
        {
            m_indexRef[c][b] = m_indexRef[0][0] + bbase;
            int64_t kbase = bbase * m_dimensions[1];
            int64_t jbase = kbase * m_dimensions[0];//treat this one specially to avoid multiplies at the slice level
            for (int64_t k = 0; k < m_dimensions[2]; ++k)
            {
                m_indexRef[c][b][k] = m_indexRef[0][0][0] + kbase;
                for (int64_t j = 0; j < m_dimensions[1]; ++j)
                {//because l looks like 1
                    m_indexRef[c][b][k][j] = m_data + jbase;//NOTE: this last pointer math is into m_data on purpose! this is why you can access the data through m_indexRef
                    jbase += m_dimensions[0];
                }
                kbase += m_dimensions[1];
                jbase += dim01;//increment by this to avoid multiply by dim[0] inside k loop
            }
            bbase += m_dimensions[2];
        }
        cbase += m_dimensions[3];
    }
}

VolumeFile::~VolumeFile()
{
    freeMemory();
}

void VolumeFile::readFile(const AString& filename) throw (DataFileException)
{
    throw DataFileException("Reading of volume files not implemented.");
    NiftiHeaderIO myHeadIO;
    myHeadIO.readFile(filename);
    int myver = myHeadIO.getNiftiVersion();
    switch (myver)
    {
        case 1:
            m_headerType = NIFTI_1;
            myHeadIO.getHeader(m_N1Header);
            break;
        case 2:
            m_headerType = NIFTI_2;
            myHeadIO.getHeader(m_N2Header);
            break;
        default:
            break;
    };
}

/**
 * Is the file empty (contains no data)?
 *
 * @return 
 *    true if the file is empty, else false.
 */
bool
VolumeFile::isEmpty() const
{
    return false;
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void 
VolumeFile::writeFile(const AString& filename) throw (DataFileException)
{
    throw DataFileException("Writing of volume files not implemented.");
}

void VolumeFile::getFrame(float* frameOut, const int64_t brickIndex, const int64_t component)
{
    int64_t startIndex = getIndex(0, 0, 0, brickIndex, component);
    int64_t endIndex = startIndex + m_dimensions[0] * m_dimensions[1] * m_dimensions[2];
    int64_t outIndex = 0;//could use memcpy, but this is more obvious and should get optimized
    for (int64_t myIndex = startIndex; myIndex < endIndex; ++myIndex)
    {
        frameOut[outIndex] = m_data[myIndex];
        ++outIndex;
    }
}

void VolumeFile::setFrame(const float* frameIn, const int64_t brickIndex, const int64_t component)
{
    int64_t startIndex = getIndex(0, 0, 0, brickIndex, component);
    int64_t endIndex = startIndex + m_dimensions[0] * m_dimensions[1] * m_dimensions[2];
    int64_t inIndex = 0;//could use memcpy, but this is more obvious and should get optimized
    for (int64_t myIndex = startIndex; myIndex < endIndex; ++myIndex)
    {
        m_data[myIndex] = frameIn[inIndex];
        ++inIndex;
    }
}
