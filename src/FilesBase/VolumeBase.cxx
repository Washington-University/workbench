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

#include "VolumeBase.h"
#include "FloatMatrix.h"
#include <algorithm>
#include <cmath>
#include "DescriptiveStatistics.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "DataFileException.h"

using namespace caret;
using namespace std;

AbstractHeader::~AbstractHeader()
{
}

AbstractVolumeExtension::~AbstractVolumeExtension()
{
}

void VolumeBase::reinitialize(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents)
{
    vector<int64_t> dimensionCast;
    int32_t dimSize = (int32_t)dimensionsIn.size();
    dimensionCast.resize(dimSize);
    for (int32_t i = 0; i < dimSize; ++i)
    {
        dimensionCast[i] = (int64_t)dimensionsIn[i];
    }
    reinitialize(dimensionCast, indexToSpace, numComponents);
}

void VolumeBase::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    freeMemory();
    CaretAssert(dimensionsIn.size() >= 3);
    CaretAssert(indexToSpace.size() == 3 || indexToSpace.size() == 4);//support using 3x4 and 4x4 as input
    CaretAssert(indexToSpace[0].size() == 4);
    CaretAssert(indexToSpace[1].size() == 4);
    CaretAssert(indexToSpace[2].size() == 4);
    m_indexToSpace = indexToSpace;
    m_indexToSpace.resize(4);//ensure row 4 exists
    m_indexToSpace[3].resize(4);//give it the right length
    m_indexToSpace[3][0] = 0.0f;//and overwrite it
    m_indexToSpace[3][1] = 0.0f;
    m_indexToSpace[3][2] = 0.0f;
    m_indexToSpace[3][3] = 1.0f;//explicitly set last row to 0 0 0 1, never trust input's fourth row
    FloatMatrix temp(m_indexToSpace);
    FloatMatrix temp2 = temp.inverse();//invert the space to get the reverse space
    m_spaceToIndex = temp2.getMatrix();
    m_indexToSpace.resize(3);//reduce them both back to 3x4
    m_spaceToIndex.resize(3);
    m_origDims = dimensionsIn;//save the original dimensions
    m_dimensions[0] = dimensionsIn[0];
    m_dimensions[1] = dimensionsIn[1];
    m_dimensions[2] = dimensionsIn[2];
    m_dimensions[3] = 1;
    for (int i = 0; i < (int)dimensionsIn.size(); ++i)
    {
        if (m_dimensions[i] < 1)
        {
            throw DataFileException("invalid dimensions specified");
        }
        if (i > 2)
        {
            m_dimensions[3] *= dimensionsIn[i];
        }
    }
    if (m_dimensions[0] == 1 && m_dimensions[1] == 1 && m_dimensions[2] == 1 && m_dimensions[3] > 10000)
    {
        throw DataFileException("this file doesn't appear to be a volume file");
    }
    m_dimensions[4] = numComponents;
    m_dataSize = m_dimensions[0] * m_dimensions[1] * m_dimensions[2] * m_dimensions[3] * m_dimensions[4];
    m_data = new float[m_dataSize];
    CaretAssert(m_data != NULL);
    setupIndexing();
    m_niftiIntent = NiftiIntentEnum::NIFTI_INTENT_NONE;
    //TODO: adjust any existing nifti header to match, or remove nifti header?
    
    createAttributes();
}

VolumeBase::VolumeBase()
{
    m_data = NULL;
    m_dataSize = 0;
    m_indexRef = NULL;
    m_jMult = NULL;
    m_kMult = NULL;
    m_bMult = NULL;
    m_cMult = NULL;
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
    m_labelTable = NULL;
    m_metadata = NULL;
    m_paletteColorMapping = NULL;
    createAttributes();
    m_niftiIntent = NiftiIntentEnum::NIFTI_INTENT_NONE;
    m_ModifiedFlag = false;
}

VolumeBase::VolumeBase(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents)
{
    m_data = NULL;
    m_dataSize = 0;
    m_indexRef = NULL;
    m_jMult = NULL;
    m_kMult = NULL;
    m_bMult = NULL;
    m_cMult = NULL;
    m_labelTable = NULL;
    m_metadata = NULL;
    m_paletteColorMapping = NULL;
    m_niftiIntent = NiftiIntentEnum::NIFTI_INTENT_NONE;
    reinitialize(dimensionsIn, indexToSpace, numComponents);//use the overloaded version to convert
    m_ModifiedFlag = false;
}

VolumeBase::VolumeBase(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
{
    m_data = NULL;
    m_dataSize = 0;
    m_indexRef = NULL;
    m_jMult = NULL;
    m_kMult = NULL;
    m_bMult = NULL;
    m_cMult = NULL;
    m_labelTable = NULL;
    m_metadata = NULL;
    m_paletteColorMapping = NULL;
    m_niftiIntent = NiftiIntentEnum::NIFTI_INTENT_NONE;
    reinitialize(dimensionsIn, indexToSpace, numComponents);
    m_ModifiedFlag = false;
}

void VolumeBase::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* centerOut) const
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
    indexOut1 = (int32_t)floor(0.5f + tempInd1);
    indexOut2 = (int32_t)floor(0.5f + tempInd2);
    indexOut3 = (int32_t)floor(0.5f + tempInd3);
}

void VolumeBase::getDimensions(vector<int64_t>& dimOut) const
{
    dimOut.resize(5);
    getDimensions(dimOut[0], dimOut[1], dimOut[2], dimOut[3], dimOut[4]);
}

void VolumeBase::getDimensions(int64_t& dimOut1, int64_t& dimOut2, int64_t& dimOut3, int64_t& dimBricksOut, int64_t& numComponents) const
{
    dimOut1 = m_dimensions[0];
    dimOut2 = m_dimensions[1];
    dimOut3 = m_dimensions[2];
    dimBricksOut = m_dimensions[3];
    numComponents = m_dimensions[4];
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
    CaretAssert(ret < m_dimensions[3]);//otherwise, m_dimensions[3] and m_origDims don't match
    return ret;
}

vector<int64_t> VolumeBase::getNonSpatialIndexesFromBrickIndex(const int64_t& brickIndex) const
{
    CaretAssert(brickIndex >= 0 && brickIndex < m_dimensions[3]);
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
    coordOut1 = m_indexToSpace[0][0] * indexIn1 + m_indexToSpace[0][1] * indexIn2 + m_indexToSpace[0][2] * indexIn3 + m_indexToSpace[0][3];
    coordOut2 = m_indexToSpace[1][0] * indexIn1 + m_indexToSpace[1][1] * indexIn2 + m_indexToSpace[1][2] * indexIn3 + m_indexToSpace[1][3];
    coordOut3 = m_indexToSpace[2][0] * indexIn1 + m_indexToSpace[2][1] * indexIn2 + m_indexToSpace[2][2] * indexIn3 + m_indexToSpace[2][3];
}

bool VolumeBase::isPlumb() const
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
    indexOut1 = m_spaceToIndex[0][0] * coordIn1 + m_spaceToIndex[0][1] * coordIn2 + m_spaceToIndex[0][2] * coordIn3 + m_spaceToIndex[0][3];
    indexOut2 = m_spaceToIndex[1][0] * coordIn1 + m_spaceToIndex[1][1] * coordIn2 + m_spaceToIndex[1][2] * coordIn3 + m_spaceToIndex[1][3];
    indexOut3 = m_spaceToIndex[2][0] * coordIn1 + m_spaceToIndex[2][1] * coordIn2 + m_spaceToIndex[2][2] * coordIn3 + m_spaceToIndex[2][3];
}

void VolumeBase::freeMemory()
{
    if (m_data != NULL)
    {
        delete[] m_data;
        m_data = NULL;
    }
    m_dataSize = 0;
    if (m_indexRef != NULL)
    {//assume the entire thing exists
        delete[] m_indexRef[0];//they were actually allocated as only 2 flat arrays
        delete[] m_indexRef;
        m_indexRef = NULL;
    }
    if (m_jMult != NULL) delete[] m_jMult;
    if (m_kMult != NULL) delete[] m_kMult;
    if (m_bMult != NULL) delete[] m_bMult;
    if (m_cMult != NULL) delete[] m_cMult;
    m_jMult = NULL;
    m_kMult = NULL;
    m_bMult = NULL;
    m_cMult = NULL;
    
    m_extensions.clear();
    freeAttributes();
}

void VolumeBase::setupIndexing()
{//must have valid m_dimensions and m_data before calling this, and already have the previous indexing freed
    int64_t dim43 = m_dimensions[4] * m_dimensions[3];//sizes for the reverse indexing lookup arrays
    int64_t dim01 = m_dimensions[0] * m_dimensions[1];//size of an xy slice
    int64_t dim012 = dim01 * m_dimensions[2];//size of a frame
    int64_t dim0123 = dim012 * m_dimensions[3];//*/ //size of a timeseries (single component)
    m_cMult = new int64_t[m_dimensions[4]];//these aren't the size of the lookup arrays because we can do the math manually and take less memory (and cache space)
    m_bMult = new int64_t[m_dimensions[3]];//it is fastest (due to cache size) to do part lookups, then part math
    m_kMult = new int64_t[m_dimensions[2]];//m_iMult doesn't exist because the first index isn't multiplied by anthing, so can be added directly
    m_jMult = new int64_t[m_dimensions[1]];
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
    }
    for (int64_t i = 0; i < m_dimensions[4]; ++i)
    {
        m_cMult[i] = i * dim0123;
    }
    if ((dim012 < (int64_t)(8 * sizeof(float*) / sizeof(float))) && ((dim43 * sizeof(float*)) > (32<<20)))
    {//if the final dimensions are small enough that the added memory usage of the last level would be more than 12.5%, and the last level would take more than 32MB of memory
        m_indexRef = NULL;//don't use memory indexing, use the precalculated multiples
    } else {//among other things, this prevents VolumeBase from exploding if you accidentally load a cifti instead of an actual volume
        //
        //EXPLANATION TIME
        //
        //Apologies for the oddity below, it is highly obtuse due to the effort of avoiding a large number of multiplies
        //what it actually does is set up m_indexRef to be an array of references into m_indexRef[0], with a skip size equal to dim[3], and each m_indexRef[i] indexes into m_data with a skip of dim[2] * dim[1] * dim[0]
        //what this accomplishes is that the lookup m_indexRef[component][brick][i + j * dim[0] + k * dim[0] * dim[1]] will be the data value at the index (i, j, k, brick, component)
        //however, it uses the lookup tables generated above instead of multiplies, meaning that it does no multiplies at all
        //this allows getVoxel and setVoxel to be faster than a standard index calculating flat array scheme, and actually makes it faster to get a value from the array at an index
        //as long as the dimensions of a frame are large, it takes relatively little memory to accomplish, compared to the data of the entire volume
        //
        m_indexRef = new float**[m_dimensions[4]];//do dimensions in reverse order, since dim[0] moves by one float at a time
        m_indexRef[0] = new float*[dim43];//this way, you can use m_indexRef[c][t][z][y][x] to get the value with only lookups
        int64_t cbase = 0;
        int64_t bbase = 0;
        for (int64_t c = 0; c < m_dimensions[4]; ++c)
        {
            m_indexRef[c] = m_indexRef[0] + cbase;//pointer math, redundant for [0], but [0][1], etc needs to get set, so it is easier to loop including 0
            for (int64_t b = 0; b < m_dimensions[3]; ++b)
            {
                m_indexRef[c][b] = m_data + bbase;
                bbase += dim012;
            }
            cbase += m_dimensions[3];
        }
    }
}

VolumeBase::~VolumeBase()
{
    freeMemory();
}

void 
VolumeBase::createAttributes()
{
    m_labelTable = new GiftiLabelTable();
    m_metadata   = new GiftiMetaData();
    m_paletteColorMapping = new PaletteColorMapping();
    m_paletteColorMapping->setSelectedPaletteName(Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME);
    m_paletteColorMapping->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
    for (int64_t i = 0; i < m_dimensions[3]; i++) {
        m_brickAttributes.push_back(new BrickAttributes());
    }
}

void 
VolumeBase::freeAttributes()
{
    delete m_metadata;
    delete m_labelTable;
    delete m_paletteColorMapping;
    for (std::vector<BrickAttributes*>::iterator iter = m_brickAttributes.begin();
         iter != m_brickAttributes.end();
         iter++) {
        delete *iter;
    }
    m_brickAttributes.clear();
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
    return (m_dimensions[0] <= 0);
}

const float* VolumeBase::getFrame(const int64_t brickIndex, const int64_t component) const
{
    int64_t startIndex = getIndex(0, 0, 0, brickIndex, component);
    return m_data + startIndex;
}

void VolumeBase::setFrame(const float* frameIn, const int64_t brickIndex, const int64_t component)
{
    int64_t startIndex = getIndex(0, 0, 0, brickIndex, component);
    int64_t endIndex = startIndex + m_dimensions[0] * m_dimensions[1] * m_dimensions[2];
    int64_t inIndex = 0;//could use memcpy, but this is more obvious and should get optimized
    for (int64_t myIndex = startIndex; myIndex < endIndex; ++myIndex)
    {
        m_data[myIndex] = frameIn[inIndex];
        ++inIndex;
    }
    setModified();
}

void 
VolumeBase::setValueAllVoxels(const float value)
{
    for (int64_t i = 0; i < m_dataSize; i++) {
        m_data[i] = value;
    }
    setModified();
    //std::fill(m_data, (m_data + m_dataSize), value);
}


/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
VolumeBase::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
VolumeBase::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* no structure in volulme file */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
VolumeBase::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
VolumeBase::getFileMetaData() const
{
    return m_metadata;
}


/**
 * @return Is the data mappable to a surface?
 */
bool 
VolumeBase::isSurfaceMappable() const
{
    return false;
}

/**
 * @return Is the data mappable to a volume?
 */
bool 
VolumeBase::isVolumeMappable() const
{
    return true;
}

/**
 * @return The number of maps in the file.  
 * Note: Caret5 used the term 'columns'.
 */
int32_t 
VolumeBase::getNumberOfMaps() const
{
    return m_dimensions[3];
}

/**
 * Get the name of the map at the given index.
 * 
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Name of the map.
 */
AString 
VolumeBase::getMapName(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    AString name = m_brickAttributes[mapIndex]->m_metadata->get(GiftiXmlElements::TAG_METADATA_NAME);
    if (name.isEmpty()) {
        name = "#" + AString::number(mapIndex + 1);
    }
    return name;
}

/**
 * Find the index of the map that uses the given name.
 * 
 * @param mapName
 *    Name of the desired map.
 * @return
 *    Index of the map using the given name.  If there is more
 *    than one map with the given name, this method is likely
 *    to return the index of the first map with the name.
 */
int32_t 
VolumeBase::getMapIndexFromName(const AString& mapName)
{
    for (int64_t i = 0; i < m_dimensions[3]; i++) {
        if (this->getMapName(i) == mapName) {
            return i;
        }
    }
    return -1;
}

/**
 * Set the name of the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mapName
 *    New name for the map.
 */
void 
VolumeBase::setMapName(const int32_t mapIndex,
                          const AString& mapName)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    m_brickAttributes[mapIndex]->m_metadata->set(GiftiXmlElements::TAG_METADATA_NAME, mapName);
    setModified();
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map (const value).
 */         
const GiftiMetaData* 
VolumeBase::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    return m_brickAttributes[mapIndex]->m_metadata;
}

/**
 * Get the metadata for the map at the given index
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Metadata for the map.
 */         
GiftiMetaData* 
VolumeBase::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    return m_brickAttributes[mapIndex]->m_metadata;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */         
const DescriptiveStatistics* 
VolumeBase::getMapStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    
    if (m_brickAttributes[mapIndex]->m_statistics == NULL) {
        DescriptiveStatistics* ds = new DescriptiveStatistics();
        ds->update(m_data, m_dataSize);
        m_brickAttributes[mapIndex]->m_statistics = ds;
    }
    
    return m_brickAttributes[mapIndex]->m_statistics;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette at the given index for 
 * data within the given ranges.
 *
 * @param mapIndex
 *    Index of the map.
 * @param mostPositiveValueInclusive
 *    Values more positive than this value are excluded.
 * @param leastPositiveValueInclusive
 *    Values less positive than this value are excluded.
 * @param leastNegativeValueInclusive
 *    Values less negative than this value are excluded.
 * @param mostNegativeValueInclusive
 *    Values more negative than this value are excluded.
 * @param includeZeroValues
 *    If true zero values (very near zero) are included.
 * @return
 *    Descriptive statistics for data (will be NULL for data
 *    not mapped using a palette).
 */         
const DescriptiveStatistics* 
VolumeBase::getMapStatistics(const int32_t mapIndex,
                             const float mostPositiveValueInclusive,
                             const float leastPositiveValueInclusive,
                             const float leastNegativeValueInclusive,
                             const float mostNegativeValueInclusive,
                             const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    
    if (m_brickAttributes[mapIndex]->m_statisticsLimitedValues == NULL) {
        m_brickAttributes[mapIndex]->m_statisticsLimitedValues = new DescriptiveStatistics();
    }
    m_brickAttributes[mapIndex]->m_statisticsLimitedValues->update(m_data, 
                                                                   m_dataSize,
                                                                   mostPositiveValueInclusive,
                                                                   leastPositiveValueInclusive,
                                                                   leastNegativeValueInclusive,
                                                                   mostNegativeValueInclusive,
                                                                   includeZeroValues);
    
    return m_brickAttributes[mapIndex]->m_statisticsLimitedValues;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
VolumeBase::isMappedWithPalette() const
{
    return (m_niftiIntent != NiftiIntentEnum::NIFTI_INTENT_LABEL);
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (will be NULL for data
 *    not mapped using a palette).
 */         
PaletteColorMapping* 
VolumeBase::getMapPaletteColorMapping(const int32_t /*mapIndex*/)
{
    /*
     * Use one palette for all bricks
     */
    return m_paletteColorMapping;
}

/**
 * Get the palette color mapping for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Palette color mapping for the map (constant) (will be NULL for data
 *    not mapped using a palette).
 */         
const PaletteColorMapping* 
VolumeBase::getMapPaletteColorMapping(const int32_t /*mapIndex*/) const
{
    /*
     * Use one palette for all bricks
     */
    return m_paletteColorMapping;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
VolumeBase::isMappedWithLabelTable() const
{
    return (m_niftiIntent == NiftiIntentEnum::NIFTI_INTENT_LABEL);
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (will be NULL for data
 *    not mapped using a label table).
 */         
GiftiLabelTable* 
VolumeBase::getMapLabelTable(const int32_t /*mapIndex*/)
{
    /*
     * Use file's label table since volume uses one
     * label table for all data arrays.
     */
    return m_labelTable;
}

/**
 * Get the label table for the map at the given index.
 *
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Label table for the map (constant) (will be NULL for data
 *    not mapped using a label table).
 */         
const GiftiLabelTable* 
VolumeBase::getMapLabelTable(const int32_t /*mapIndex*/) const
{
    /*
     * Use file's label table since volume uses one
     * label table for all data arrays.
     */
    return m_labelTable;
}

/**
 * Get the unique ID (UUID) for the map at the given index.
 * 
 * @param mapIndex
 *    Index of the map.
 * @return
 *    String containing UUID for the map.
 */
AString 
VolumeBase::getMapUniqueID(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    return m_brickAttributes[mapIndex]->m_metadata->getUniqueID();
}

/**
 * Find the index of the map that uses the given unique ID (UUID).
 * 
 * @param uniqueID
 *    Unique ID (UUID) of the desired map.
 * @return
 *    Index of the map using the given UUID.
 */
int32_t 
VolumeBase::getMapIndexFromUniqueID(const AString& uniqueID) const
{
    for (int64_t i = 0; i < m_dimensions[3]; i++) {
        if (m_brickAttributes[i]->m_metadata->getUniqueID() == uniqueID) {
            return i;
        }
    }
    return -1;
}

/**
 * @return Bounding box of the volumes spatial coordinates.
 */
BoundingBox 
VolumeBase::getSpaceBoundingBox() const
{
    BoundingBox bb;
    float coordinates[3];
    for (int i = 0; i < 2; ++i)//if the volume isn't plumb, we need to test all corners, so just always test all corners
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                this->indexToSpace(i * m_dimensions[0] - 0.5f, j * m_dimensions[1] - 0.5f, k * m_dimensions[2] - 0.5f, coordinates);//accounts for extra half voxel on each side of each center
                bb.update(coordinates);
            }
        }
    }
    return bb;
}




//==================================================================================
VolumeBase::BrickAttributes::BrickAttributes()
{
    m_metadata = new GiftiMetaData();
    m_statistics = NULL;
    m_statisticsLimitedValues = NULL;
}

VolumeBase::BrickAttributes::~BrickAttributes()
{
    delete m_metadata;
    if (m_statistics != NULL) {
        delete m_statistics;
    }
    if (m_statisticsLimitedValues != NULL) {
        delete m_statisticsLimitedValues;
    }
}
