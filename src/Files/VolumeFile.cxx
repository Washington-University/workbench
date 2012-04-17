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
#include <cmath>
#include "NiftiFile.h"
#include <iostream>
#include <sstream>
#include <string>
#include "Palette.h"
#include "FastStatistics.h"
#include "Histogram.h"

using namespace caret;
using namespace std;

const float VolumeFile::INVALID_INTERP_VALUE = 0.0f;//we may want NaN or something more obvious

VolumeFile::VolumeFile()
: VolumeBase(), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    validateMembers();
}

VolumeFile::VolumeFile(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents, SubvolumeAttributes::VolumeType whatType)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    validateMembers();
    setType(whatType);
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
{
    VolumeBase::reinitialize(dimensionsIn, indexToSpace, numComponents);
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents, SubvolumeAttributes::VolumeType whatType)
{
    VolumeBase::reinitialize(dimensionsIn, indexToSpace, numComponents);
    validateMembers();
    setType(whatType);
}

SubvolumeAttributes::VolumeType VolumeFile::getType() const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, 0);
    CaretAssert(m_caretVolExt.m_attributes[0] != NULL);
    return m_caretVolExt.m_attributes[0]->m_type;
}

void VolumeFile::setType(SubvolumeAttributes::VolumeType whatType)
{
    int numAttrs = (int)m_caretVolExt.m_attributes.size();
    for (int i = 0; i < numAttrs; ++i)
    {
        CaretAssert(m_caretVolExt.m_attributes[i] != NULL);
        if (m_caretVolExt.m_attributes[i]->m_type != whatType)
        {
            m_caretVolExt.m_attributes[i]->m_type = whatType;
            if (whatType == SubvolumeAttributes::LABEL)
            {
                m_caretVolExt.m_attributes[i]->m_palette.grabNew(NULL);
                m_caretVolExt.m_attributes[i]->m_labelTable.grabNew(new GiftiLabelTable());
            } else {
                m_caretVolExt.m_attributes[i]->m_palette.grabNew(new PaletteColorMapping());
                m_caretVolExt.m_attributes[i]->m_labelTable.grabNew(NULL);
                if (whatType == SubvolumeAttributes::ANATOMY)
                {
                    m_caretVolExt.m_attributes[i]->m_palette->setSelectedPaletteName(Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME);
                    m_caretVolExt.m_attributes[i]->m_palette->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
                }
            }
        }
    }
}

VolumeFile::~VolumeFile()
{
}

void VolumeFile::readFile(const AString& filename) throw (DataFileException)
{
    try {
        NiftiFile myNifti(true);
        this->setFileName(filename);
        myNifti.readVolumeFile(*this, filename);
        parseExtensions();
        clearModified();
    }
    catch (const CaretException& e) {
        throw DataFileException(e);
    }
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
    try {
        updateCaretExtension();
        NiftiFile myNifti(true);
        myNifti.writeVolumeFile(*this, filename);
    }
    catch (const CaretException& e) {
        throw DataFileException(e);
    }
}

float VolumeFile::interpolateValue(const float* coordIn, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component)
{
    return interpolateValue(coordIn[0], coordIn[1], coordIn[2], interp, validOut, brickIndex, component);
}

float VolumeFile::interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component)
{
    switch (interp)
    {
        case TRILINEAR:
        {
            float index1, index2, index3;
            spaceToIndex(coordIn1, coordIn2, coordIn3, index1, index2, index3);
            int ind1low = floor(index1);
            int ind2low = floor(index2);
            int ind3low = floor(index3);
            int ind1high = ind1low + 1;
            int ind2high = ind2low + 1;
            int ind3high = ind3low + 1;
            if (!indexValid(ind1low, ind2low, ind3low, brickIndex, component) || !indexValid(ind1high, ind2high, ind3high, brickIndex, component))
            {
                if (validOut != NULL) *validOut = false;
                return INVALID_INTERP_VALUE;
            }
            float xhighWeight = index1 - ind1low;
            float xlowWeight = 1.0f - xhighWeight;
            float xinterp[2][2];//manually unrolled, because loops of 2 seem silly
            xinterp[0][0] = xlowWeight * getValue(ind1low, ind2low, ind3low, brickIndex, component) + xhighWeight * getValue(ind1high, ind2low, ind3low, brickIndex, component);
            xinterp[1][0] = xlowWeight * getValue(ind1low, ind2high, ind3low, brickIndex, component) + xhighWeight * getValue(ind1high, ind2high, ind3low, brickIndex, component);
            xinterp[0][1] = xlowWeight * getValue(ind1low, ind2low, ind3high, brickIndex, component) + xhighWeight * getValue(ind1high, ind2low, ind3high, brickIndex, component);
            xinterp[1][1] = xlowWeight * getValue(ind1low, ind2high, ind3high, brickIndex, component) + xhighWeight * getValue(ind1high, ind2high, ind3high, brickIndex, component);
            float yhighWeight = index2 - ind2low;
            float ylowWeight = 1.0f - yhighWeight;
            float yinterp[2];
            yinterp[0] = ylowWeight * xinterp[0][0] + yhighWeight * xinterp[1][0];
            yinterp[1] = ylowWeight * xinterp[0][1] + yhighWeight * xinterp[1][1];
            float zlowWeight = index3 - ind3low;
            float zhighWeight = 1.0f - zlowWeight;
            float ret = zlowWeight * yinterp[0] + zhighWeight * yinterp[1];
            if (validOut != NULL) *validOut = true;
            return ret;
        }
        break;
        case ENCLOSING_VOXEL:
        {
            int64_t index1, index2, index3;
            enclosingVoxel(coordIn1, coordIn2, coordIn3, index1, index2, index3);
            if (indexValid(index1, index2, index3, brickIndex, component))
            {
                if (validOut != NULL) *validOut = true;
                return getValue(index1, index2, index3, brickIndex, component);
            } else {
                if (validOut != NULL) *validOut = false;
                return INVALID_INTERP_VALUE;
            }
        }
        break;
    }
    if (validOut != NULL) *validOut = false;
    return INVALID_INTERP_VALUE;
}

bool VolumeFile::matchesVolumeSpace(const VolumeFile* right) const
{
    for (int i = 0; i < 3; ++i)//only check the spatial dimensions
    {
        if (m_dimensions[i] != right->m_dimensions[i])
        {
            return false;
        }
    }
    const float TOLER_RATIO = 1.0002f;//ratio a spacing element can mismatch by
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float leftelem = m_indexToSpace[i][j];
            float rightelem = right->m_indexToSpace[i][j];
            if ((leftelem != rightelem) && (leftelem == 0.0f || rightelem == 0.0f || (abs(leftelem / rightelem) > TOLER_RATIO || abs(rightelem / leftelem) > TOLER_RATIO)))
            {
                return false;
            }
        }
    }
    return true;
}

bool VolumeFile::matchesVolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform) const
{
    for (int i = 0; i < 3; ++i)//only check the spatial dimensions
    {
        if (m_dimensions[i] != dims[i])
        {
            return false;
        }
    }
    const float TOLER_RATIO = 1.0002f;//ratio a spacing element can mismatch by
    CaretAssert(sform.size() >= 3);
    for (int i = 0; i < 3; ++i)
    {
        CaretAssert(sform[i].size() >= 4);
        for (int j = 0; j < 4; ++j)
        {
            float leftelem = m_indexToSpace[i][j];
            float rightelem = sform[i][j];
            if ((leftelem != rightelem) && (leftelem == 0.0f || rightelem == 0.0f || (abs(leftelem / rightelem) > TOLER_RATIO || abs(rightelem / leftelem) > TOLER_RATIO)))
            {
                return false;
            }
        }
    }
    return true;
}

void VolumeFile::parseExtensions()
{
    const int NIFTI_ECODE_CARET = 30;//this should probably go in nifti1.h
    int numExtensions = (int)m_extensions.size();
    int whichExt = -1, whichType = -1;//type will track caret's preference in which extension to read, the greater the type, the more it prefers it
    for (int i = 0; i < numExtensions; ++i)
    {
        switch (m_extensions[i]->getType())
        {
            case AbstractVolumeExtension::NIFTI1:
            case AbstractVolumeExtension::NIFTI2:
                {
                    NiftiAbstractVolumeExtension* myNiftiExtension = (NiftiAbstractVolumeExtension*)m_extensions[i].getPointer();
                    switch (myNiftiExtension->m_ecode)
                    {
                        case NIFTI_ECODE_CARET:
                            if (100 > whichType)//mostly to make it use the first caret extension it finds in the list of extensions
                            {
                                whichExt = i;
                                whichType = 100;//caret extension gets maximum priority
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }
            default:
                break;
        }
    }
    if (whichExt != -1)
    {
        switch (whichType)
        {
            case 100://caret extension
                {
                    QByteArray myByteArray(m_extensions[whichExt]->m_bytes, m_extensions[whichExt]->m_bytes.size());
                    myByteArray.append('\0');//give it a null byte to ensure it stops
                    AString myString(myByteArray);
                    m_caretVolExt.readFromXmlString(myString);
                }
                break;
            default:
                break;
        }
    }
    validateMembers();
}

void VolumeFile::updateCaretExtension()
{
    const int NIFTI_ECODE_CARET = 30;//this should probably go in nifti1.h
    int numExtensions = (int)m_extensions.size();
    for (int i = 0; i < numExtensions; ++i)
    {
        switch (m_extensions[i]->getType())
        {
            case AbstractVolumeExtension::NIFTI1:
            case AbstractVolumeExtension::NIFTI2:
                {
                    NiftiAbstractVolumeExtension* myNiftiExtension = (NiftiAbstractVolumeExtension*)m_extensions[i].getPointer();
                    if (myNiftiExtension->m_ecode == NIFTI_ECODE_CARET)
                    {
                        m_extensions.erase(m_extensions.begin() + i);
                        --i;
                        --numExtensions;
                    }
                }
                break;
            default:
                break;
        }
    }
    stringstream mystream;
    XmlWriter myWriter(mystream);
    m_caretVolExt.writeAsXML(myWriter);
    NiftiAbstractVolumeExtension* newExt = new NiftiAbstractVolumeExtension();//use default nifti version from this constructor
    newExt->m_ecode = NIFTI_ECODE_CARET;
    string myStr = mystream.str();
    int length = myStr.length();
    newExt->m_bytes = CaretArray<char>(length + 1);//add a null byte for safety
    for (int i = 0; i < length; ++i)
    {
        newExt->m_bytes[i] = myStr[i];
    }
    newExt->m_bytes[length] = 0;
    m_extensions.push_back(CaretPointer<AbstractVolumeExtension>(newExt));//doesn't matter whether this CaretPointer is the base type or inherited type, operator= will have it stored as base type inside the vector
}

void VolumeFile::validateMembers()
{
    int numMaps = getNumberOfMaps();
    m_brickAttributes.clear();//invalidate brick attributes first
    m_brickAttributes.resize(numMaps);//before resizing
    m_brickStatisticsValid = true;
    int curAttribNum = (int)m_caretVolExt.m_attributes.size();
    if (curAttribNum != numMaps)
    {
        m_caretVolExt.m_attributes.resize(numMaps);
    }
    bool isLabel = false;
    SubvolumeAttributes::VolumeType theType = SubvolumeAttributes::ANATOMY;
    if (numMaps > 0 && curAttribNum > 0 && m_caretVolExt.m_attributes[0] != NULL)
    {
        theType = m_caretVolExt.m_attributes[0]->m_type;
        if (theType == SubvolumeAttributes::UNKNOWN)
        {
            theType = SubvolumeAttributes::ANATOMY;
        }
        if (theType == SubvolumeAttributes::LABEL)
        {
            isLabel = true;
        }
    }
    for (int i = 0; i < numMaps; ++i)
    {
        if (m_caretVolExt.m_attributes[i] == NULL)
        {
            m_caretVolExt.m_attributes[i].grabNew(new SubvolumeAttributes());
        }
        m_caretVolExt.m_attributes[i]->m_type = theType;
        if (isLabel)
        {
            m_caretVolExt.m_attributes[i]->m_palette.grabNew(NULL);
            if (m_caretVolExt.m_attributes[i]->m_labelTable == NULL)
            {
                m_caretVolExt.m_attributes[i]->m_labelTable.grabNew(new GiftiLabelTable());//TODO: populate the label table by means of the frame values?
            }
        } else {
            m_caretVolExt.m_attributes[i]->m_labelTable.grabNew(NULL);
            if (m_caretVolExt.m_attributes[i]->m_palette == NULL)
            {
                m_caretVolExt.m_attributes[i]->m_palette.grabNew(new PaletteColorMapping());
                if (theType == SubvolumeAttributes::ANATOMY)
                {
                    m_caretVolExt.m_attributes[i]->m_palette->setSelectedPaletteName(Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME);
                    m_caretVolExt.m_attributes[i]->m_palette->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE);
                }
            }
        }
    }
}

void VolumeFile::setModified()
{
    DataFile::setModified();//do we need to do both of these?
    VolumeBase::setModified();
    m_brickStatisticsValid = false;
}


/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
VolumeFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
VolumeFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* no structure in volulme file */
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
VolumeFile::getMapName(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    AString name = m_caretVolExt.m_attributes[mapIndex]->m_guiLabel;
    if (name.isEmpty()) {
        name = "#" + AString::number(mapIndex + 1);
    }
    return name;
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
VolumeFile::setMapName(const int32_t mapIndex,
                          const AString& mapName)
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    m_caretVolExt.m_attributes[mapIndex]->m_guiLabel = mapName;
    setModified();
}

const GiftiMetaData* VolumeFile::getMapMetaData(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    if (m_brickAttributes[mapIndex].m_metadata == NULL)
    {
        m_brickAttributes[mapIndex].m_metadata.grabNew(new GiftiMetaData());
    }
    return m_brickAttributes[mapIndex].m_metadata;
}

GiftiMetaData* VolumeFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    if (m_brickAttributes[mapIndex].m_metadata == NULL)
    {
        m_brickAttributes[mapIndex].m_metadata.grabNew(new GiftiMetaData());
    }
    return m_brickAttributes[mapIndex].m_metadata;
}

void VolumeFile::checkStatisticsValid()
{
    if (m_brickStatisticsValid == false)
    {
        int32_t numMaps = getNumberOfMaps();
        for (int i = 0; i < numMaps; ++i)
        {
            m_brickAttributes[i].m_statistics.grabNew(NULL);
            m_brickAttributes[i].m_statisticsLimitedValues.grabNew(NULL);
            m_brickAttributes[i].m_fastStatistics.grabNew(NULL);
            m_brickAttributes[i].m_histogram.grabNew(NULL);
            m_brickAttributes[i].m_histogramLimitedValues.grabNew(NULL);
        }
        m_brickStatisticsValid = true;
    }
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
VolumeFile::getMapStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    
    if (m_brickAttributes[mapIndex].m_statistics == NULL) {
        DescriptiveStatistics* ds = new DescriptiveStatistics();
        ds->update(getFrame(mapIndex), m_dimensions[0] * m_dimensions[1] * m_dimensions[2]);
        m_brickAttributes[mapIndex].m_statistics.grabNew(ds);
    }
    
    return m_brickAttributes[mapIndex].m_statistics;
}

const FastStatistics* VolumeFile::getMapFastStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    if (m_brickAttributes[mapIndex].m_fastStatistics == NULL)
    {
        m_brickAttributes[mapIndex].m_fastStatistics.grabNew(new FastStatistics(getFrame(mapIndex), m_dimensions[0] * m_dimensions[1] * m_dimensions[2]));
    }
    return m_brickAttributes[mapIndex].m_fastStatistics;
}

const Histogram* VolumeFile::getMapHistogram(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    if (m_brickAttributes[mapIndex].m_histogram == NULL)
    {
        m_brickAttributes[mapIndex].m_histogram.grabNew(new Histogram(100, getFrame(mapIndex), m_dimensions[0] * m_dimensions[1] * m_dimensions[2]));
    }
    return m_brickAttributes[mapIndex].m_histogram;
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
VolumeFile::getMapStatistics(const int32_t mapIndex,
                             const float mostPositiveValueInclusive,
                             const float leastPositiveValueInclusive,
                             const float leastNegativeValueInclusive,
                             const float mostNegativeValueInclusive,
                             const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    
    if (m_brickAttributes[mapIndex].m_statisticsLimitedValues == NULL) {
        m_brickAttributes[mapIndex].m_statisticsLimitedValues.grabNew(new DescriptiveStatistics());
    }
    m_brickAttributes[mapIndex].m_statisticsLimitedValues->update(getFrame(mapIndex), 
                                                                m_dimensions[0] * m_dimensions[1] * m_dimensions[2],
                                                                mostPositiveValueInclusive,
                                                                leastPositiveValueInclusive,
                                                                leastNegativeValueInclusive,
                                                                mostNegativeValueInclusive,
                                                                includeZeroValues);
    
    return m_brickAttributes[mapIndex].m_statisticsLimitedValues;
}

const Histogram* VolumeFile::getMapHistogram(const int32_t mapIndex,
                                             const float mostPositiveValueInclusive,
                                             const float leastPositiveValueInclusive,
                                             const float leastNegativeValueInclusive,
                                             const float mostNegativeValueInclusive,
                                             const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    if (m_brickAttributes[mapIndex].m_histogramLimitedValues == NULL)
    {
        m_brickAttributes[mapIndex].m_histogramLimitedValues.grabNew(new Histogram(100));
    }
    m_brickAttributes[mapIndex].m_histogramLimitedValues->update(getFrame(mapIndex), 
                                                    m_dimensions[0] * m_dimensions[1] * m_dimensions[2],
                                                    mostPositiveValueInclusive,
                                                    leastPositiveValueInclusive,
                                                    leastNegativeValueInclusive,
                                                    mostNegativeValueInclusive,
                                                    includeZeroValues);
    return m_brickAttributes[mapIndex].m_histogramLimitedValues;
}

/**
 * @return Is the data in the file mapped to colors using
 * a palette.
 */
bool 
VolumeFile::isMappedWithPalette() const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, 0);
    CaretAssert(m_caretVolExt.m_attributes[0] != NULL);
    return (m_caretVolExt.m_attributes[0]->m_type != SubvolumeAttributes::LABEL);
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
VolumeFile::getMapPaletteColorMapping(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex]->m_palette != NULL);
    return m_caretVolExt.m_attributes[mapIndex]->m_palette;
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
VolumeFile::getMapPaletteColorMapping(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex]->m_palette != NULL);
    return m_caretVolExt.m_attributes[mapIndex]->m_palette;
}

/**
 * @return Is the data in the file mapped to colors using
 * a label table.
 */
bool 
VolumeFile::isMappedWithLabelTable() const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, 0);
    CaretAssert(m_caretVolExt.m_attributes[0] != NULL);
    return (m_caretVolExt.m_attributes[0]->m_type == SubvolumeAttributes::LABEL);
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
VolumeFile::getMapLabelTable(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex]->m_labelTable != NULL);
    return m_caretVolExt.m_attributes[mapIndex]->m_labelTable;
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
VolumeFile::getMapLabelTable(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex] != NULL);
    CaretAssert(m_caretVolExt.m_attributes[mapIndex]->m_labelTable != NULL);
    return m_caretVolExt.m_attributes[mapIndex]->m_labelTable;
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
VolumeFile::getMapUniqueID(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    if (m_brickAttributes[mapIndex].m_metadata == NULL)
    {
        m_brickAttributes[mapIndex].m_metadata.grabNew(new GiftiMetaData());
    }
    return m_brickAttributes[mapIndex].m_metadata->getUniqueID();
}

/**
 * @return Bounding box of the volumes spatial coordinates.
 */
BoundingBox 
VolumeFile::getSpaceBoundingBox() const
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
