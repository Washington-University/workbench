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

#include "VolumeBase.h"
#include "DataFileException.h"
#include "FloatMatrix.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AbstractHeader::~AbstractHeader()
{
}

void VolumeBase::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    CaretAssert(numComponents > 0);
    clear();
    int numDims = (int)dimensionsIn.size();
    if (numDims < 3)
    {
        throw DataFileException("volume files must have 3 or more dimensions");
    }
    m_origDims = dimensionsIn;//save the original dimensions
    int64_t storeDims[5];
    storeDims[3] = 1;
    for (int i = 0; i < numDims; ++i)
    {
        if (i > 2)
        {
            storeDims[3] *= dimensionsIn[i];
        } else {
            storeDims[i] = dimensionsIn[i];
        }
    }
    m_volSpace.setSpace(storeDims, indexToSpace);
    if (storeDims[0] == 1 && storeDims[1] == 1 && storeDims[2] == 1 && storeDims[3] > 10000)
    {//slight hack, to detect if a cifti file is loaded as a volume file, because many 1x1x1 frames could use a surprisingly large amount of memory (metadata, palette, etc)
        throw DataFileException("this file doesn't appear to be a volume file");
    }
    storeDims[4] = numComponents;
    m_storage.reinitialize(storeDims);
}

void VolumeBase::addSubvolumes(const int64_t& numToAdd)
{
    CaretAssert(numToAdd > 0);
    vector<int64_t> olddims = getDimensions();//use the already flattened dimensions to start, as the non-spatial dimensions must be flattened to add an arbitrary number of maps
    CaretAssert(olddims[3] > 0);//can't add volumes when we have no dimensions, stop the debugger here
    if (olddims[3] < 1)
    {
        throw DataFileException("cannot call addSubvolumes on an uninitialized VolumeFile");//release shouldn't allow it either
    }
    vector<int64_t> newdims = olddims;
    newdims[3] += numToAdd;//add to the flattened non-spatial dimensions
    VolumeStorage newStorage(newdims.data());
    newdims.resize(4);//drop the number of components from the dimensions array
    m_origDims = newdims;//and reset our original dimensions
    for (int64_t c = 0; c < olddims[4]; ++c)
    {
        for (int64_t b = 0; b < olddims[3]; ++b)
        {
            newStorage.setFrame(m_storage.getFrame(b, c), b, c);
        }
    }
    m_storage.swap(newStorage);
    setModified();//NOTE: will invalidate splines, can be made more efficient for certain cases when merging Base and File
}

void VolumeBase::setVolumeSpace(const vector<vector<float> >& indexToSpace)
{
    m_volSpace.setSpace(getDimensionsPtr(), indexToSpace);
    setModified();
}

VolumeBase::VolumeBase()
{
    m_origDims.push_back(0);//give original dimensions 3 elements, just because
    m_origDims.push_back(0);
    m_origDims.push_back(0);
    m_ModifiedFlag = false;
}

VolumeBase::VolumeBase(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    reinitialize(dimensionsIn, indexToSpace, numComponents);
    m_ModifiedFlag = true;
}

void VolumeBase::getOrientAndSpacingForPlumb(VolumeSpace::OrientTypes* orientOut, float* spacingOut, float* centerOut) const
{
    m_volSpace.getOrientAndSpacingForPlumb(orientOut, spacingOut, centerOut);
}

void VolumeBase::getOrientation(VolumeSpace::OrientTypes orientOut[3]) const
{
    m_volSpace.getOrientation(orientOut);
}

void VolumeBase::reorient(const VolumeSpace::OrientTypes newOrient[3])
{
    VolumeSpace::OrientTypes curOrient[3];
    getOrientation(curOrient);
    int curReverse[3];//for each spatial axis, which index currently goes that direction
    bool curReverseNeg[3];
    int doSomething = false;//check whether newOrient is any different
    for (int i = 0; i < 3; ++i)
    {
        if (curOrient[i] != newOrient[i]) doSomething = true;
        curReverse[curOrient[i] & 3] = i;//int values of the enum are crafted to make this work
        curReverseNeg[curOrient[i] & 3] = ((curOrient[i] & 4) != 0);
    }
    if (!doSomething) return;
    bool flip[3];
    int fetchFrom[3];
    for (int i = 0; i < 3; ++i)
    {
        flip[i] = curReverseNeg[newOrient[i] & 3] != ((newOrient[i] & 4) != 0);
        fetchFrom[i] = curReverse[newOrient[i] & 3];
    }
    const int64_t* dims = getDimensionsPtr();
    int64_t rowSize = dims[0];
    int64_t sliceSize = rowSize * dims[1];
    int64_t frameSize = sliceSize * dims[2];
    vector<float> scratchFrame(frameSize);
    int64_t newDims[5] = {dims[fetchFrom[0]], dims[fetchFrom[1]], dims[fetchFrom[2]], dims[3], dims[4]};
    VolumeStorage newStorage(newDims);
    for (int c = 0; c < dims[4]; ++c)
    {
        for (int b = 0; b < dims[3]; ++b)
        {
            const float* oldFrame = m_storage.getFrame(b, c);
            int64_t newIndices[3], oldIndices[3];
            for (newIndices[2] = 0; newIndices[2] < newDims[2]; ++newIndices[2])
            {
                if (flip[2])
                {
                    oldIndices[fetchFrom[2]] = newDims[2] - newIndices[2] - 1;
                } else {
                    oldIndices[fetchFrom[2]] = newIndices[2];
                }
                for (newIndices[1] = 0; newIndices[1] < newDims[1]; ++newIndices[1])
                {
                    if (flip[1])
                    {
                        oldIndices[fetchFrom[1]] = newDims[1] - newIndices[1] - 1;
                    } else {
                        oldIndices[fetchFrom[1]] = newIndices[1];
                    }
                    for (newIndices[0] = 0; newIndices[0] < newDims[0]; ++newIndices[0])
                    {
                        if (flip[0])
                        {
                            oldIndices[fetchFrom[0]] = newDims[0] - newIndices[0] - 1;
                        } else {
                            oldIndices[fetchFrom[0]] = newIndices[0];
                        }
                        scratchFrame[newStorage.getIndex(newIndices, 0, 0)] = oldFrame[getIndex(oldIndices)];
                    }
                }
            }
            newStorage.setFrame(scratchFrame.data(), b, c);
        }
    }
    const vector<vector<float> >& oldSform = m_volSpace.getSform();
    vector<vector<float> > indexToSpace = oldSform;//reorder and flip the sform - this might belong in VolumeSpace
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (flip[j])
            {
                indexToSpace[i][j] = -oldSform[i][fetchFrom[j]];
                indexToSpace[i][3] += oldSform[i][fetchFrom[j]] * (newDims[j] - 1);
            } else {
                indexToSpace[i][j] = oldSform[i][fetchFrom[j]];
            }
        }
    }//and now generate the inverse for spaceToIndex
    m_volSpace.setSpace(newDims, indexToSpace);
    m_origDims[0] = newDims[0];//update m_origDims too
    m_origDims[1] = newDims[1];
    m_origDims[2] = newDims[2];
    m_storage.swap(newStorage);
    setModified();
}

void VolumeBase::enclosingVoxel(const float* coordIn, int64_t* indexOut) const
{
    enclosingVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeBase::enclosingVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t* indexOut) const
{
    enclosingVoxel(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeBase::enclosingVoxel(const float* coordIn, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const
{
    enclosingVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeBase::enclosingVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3) const
{
    float tempInd1, tempInd2, tempInd3;
    spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
    indexOut1 = (int64_t)floor(0.5f + tempInd1);
    indexOut2 = (int64_t)floor(0.5f + tempInd2);
    indexOut3 = (int64_t)floor(0.5f + tempInd3);
}

int64_t VolumeBase::getBrickIndexFromNonSpatialIndexes(const vector<int64_t>& extraInds) const
{
    CaretAssert(extraInds.size() == m_origDims.size() - 3);
    int extraDims = (int)extraInds.size();
    if (extraDims == 0) return 0;
    CaretAssert(extraInds[extraDims - 1] >= 0 && extraInds[extraDims - 1] < m_origDims[extraDims + 2]);
    int64_t ret = extraInds[extraDims - 1];
    for (int i = extraDims - 2; i >= 0; --i)//yes, its supposed to loop starting with the second highest dimension
    {
        CaretAssert(extraInds[i] >= 0 && extraInds[i] < m_origDims[i + 3]);
        ret = ret * m_origDims[i + 3] + extraInds[i];//factored polynomial form
    }
    CaretAssert(ret < getDimensionsPtr()[3]);//otherwise, dimensions[3] and m_origDims don't match
    return ret;
}

vector<int64_t> VolumeBase::getNonSpatialIndexesFromBrickIndex(const int64_t& brickIndex) const
{
    CaretAssert(brickIndex >= 0 && brickIndex < getDimensionsPtr()[3]);
    vector<int64_t> ret;
    int extraDims = (int)m_origDims.size() - 3;
    if (extraDims <= 0) return ret;//empty vector if there are no extra-spatial dimensions, so we don't call resize(0), even though it should be safe
    ret.resize(extraDims);
    int64_t myRemaining = brickIndex, temp;
    for (int i = 0; i < extraDims; ++i)
    {
        temp = myRemaining % m_origDims[i + 3];//modulus
        myRemaining = (myRemaining - temp) / m_origDims[i + 3];//subtract the remainder even though int divide should truncate correctly, just to make it obvious
        ret[i] = temp;
    }
    CaretAssert(myRemaining == 0);//otherwise, m_dimensions[3] and m_origDims don't match
    return ret;
}

void VolumeBase::indexToSpace(const int64_t* indexIn, float* coordOut) const
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeBase::indexToSpace(const int64_t* indexIn, float& coordOut1, float& coordOut2, float& coordOut3) const
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeBase::indexToSpace(const float* indexIn, float* coordOut) const
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeBase::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float* coordOut) const
{
    indexToSpace(indexIn1, indexIn2, indexIn3, coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeBase::indexToSpace(const float* indexIn, float& coordOut1, float& coordOut2, float& coordOut3) const
{
    indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeBase::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3) const
{
    m_volSpace.indexToSpace(indexIn1, indexIn2, indexIn3, coordOut1, coordOut2, coordOut3);
}

bool VolumeBase::isPlumb() const
{
    return m_volSpace.isPlumb();
}

void VolumeBase::spaceToIndex(const float* coordIn, float* indexOut) const
{
    spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeBase::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float* indexOut) const
{
    spaceToIndex(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeBase::spaceToIndex(const float* coordIn, float& indexOut1, float& indexOut2, float& indexOut3) const
{
    spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeBase::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3) const
{
    m_volSpace.spaceToIndex(coordIn1, coordIn2, coordIn3, indexOut1, indexOut2, indexOut3);
}

void VolumeBase::clear()
{
    m_storage.clear();
    m_origDims.clear();
    m_origDims.resize(3, 0);//give original dimensions 3 elements, just because
    m_header.grabNew(NULL);
}

VolumeBase::~VolumeBase()
{
}

/**
 * Is the file empty (contains no data)?
 *
 * @return 
 *    true if the file is empty, else false.
 */
bool
VolumeBase::isEmpty() const
{
    return (getDimensionsPtr()[0] <= 0);
}

VolumeBase::VolumeStorage::VolumeStorage()
{
    for (int i = 0; i < 5; ++i)
    {
        m_dimensions[i] = 0;
        m_mult[i] = 0;
    }
}

void VolumeBase::VolumeStorage::reinitialize(int64_t dims[5])
{
    for (int i = 0; i < 5; ++i)
    {
        CaretAssert(dims[i] > 0);//stop the debugger in the right place
        if (dims[i] < 1) throw DataFileException("VolumeStorage dimensions must be positive");//release shouldn't allow it either, though
        m_dimensions[i] = dims[i];
    }
    m_mult[0] = m_dimensions[0];
    for (int i = 1; i < 5; ++i)
    {
        m_mult[i] = m_mult[i - 1] * m_dimensions[i];
    }
    m_data.resize(m_mult[4]);
}

VolumeBase::VolumeStorage::VolumeStorage(int64_t dims[5])
{
    reinitialize(dims);
}

const float* VolumeBase::VolumeStorage::getFrame(const int64_t brickIndex, const int64_t component) const
{
    return m_data.data() + brickIndex * m_mult[2] + component * m_mult[3];//NOTE: do not use [4]
}

void VolumeBase::VolumeStorage::setFrame(const float* frameIn, const int64_t brickIndex, const int64_t component)
{
    CaretAssert(brickIndex >= 0 && brickIndex < m_dimensions[3]);
    CaretAssert(component >= 0 && component < m_dimensions[4]);
    int64_t start = brickIndex * m_mult[2] + component * m_mult[3];
    for (int64_t i = 0; i < m_mult[2]; ++i)
    {
        m_data[i + start] = frameIn[i];
    }
}

void VolumeBase::VolumeStorage::setValueAllVoxels(const float value)
{
    for (int64_t i = 0; i < m_mult[4]; ++i)
    {
        m_data[i] = value;
    }
}

void VolumeBase::VolumeStorage::swap(VolumeStorage& rhs)
{
    m_data.swap(rhs.m_data);
    for (int i = 0; i < 5; ++i)
    {
        std::swap(m_dimensions[i], rhs.m_dimensions[i]);
        std::swap(m_mult[i], rhs.m_mult[i]);
    }
}

void VolumeBase::VolumeStorage::getDimensions(vector<int64_t>& dimOut) const
{
    dimOut.resize(5);
    for (int i = 0; i < 5; ++i)
    {
        dimOut[i] = m_dimensions[i];
    }
}

void VolumeBase::VolumeStorage::getDimensions(int64_t& dimOut1, int64_t& dimOut2, int64_t& dimOut3, int64_t& dimTimeOut, int64_t& numComponents) const
{
    dimOut1 = m_dimensions[0];
    dimOut2 = m_dimensions[1];
    dimOut3 = m_dimensions[2];
    dimTimeOut = m_dimensions[3];
    numComponents = m_dimensions[4];
}

vector<int64_t> VolumeBase::VolumeStorage::getDimensions() const
{
    vector<int64_t> ret;
    getDimensions(ret);
    return ret;
}

void VolumeBase::VolumeStorage::clear()
{
    m_data.clear();
    for (int i = 0; i < 5; ++i)
    {
        m_dimensions[i] = 0;
        m_mult[i] = 0;
    }
}

/**
 * @return Is this instance modified?
 */
bool 
VolumeBase::isModifiedVolumeBase() const 
{ 
    if (m_ModifiedFlag) {
        return true;
    }
    return false;
}

/**
 * Clear this instance's modified status
 */
void 
VolumeBase::clearModifiedVolumeBase() 
{ 
    m_ModifiedFlag = false;
}

/**
 * Set this instance's status to modified.
 */
void 
VolumeBase::setModified()
{ 
    m_ModifiedFlag = true;
}
