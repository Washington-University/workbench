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
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include <QTemporaryFile>

#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CaretTemporaryFile.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "DataFileContentInformation.h"
#include "GroupAndNameHierarchyModel.h"
#include "FastStatistics.h"
#include "Histogram.h"
#include "MultiDimIterator.h"
#include "NiftiIO.h"
#include "Palette.h"
#include "SceneClass.h"
#include "VolumeFile.h"
#include "VolumeFileVoxelColorizer.h"
#include "VolumeSpline.h"

#include <limits>

using namespace caret;
using namespace std;

const float VolumeFile::INVALID_INTERP_VALUE = 0.0f;//we may want NaN or something more obvious
bool VolumeFile::s_voxelColoringEnabled = true;

/**
 * Static method that sets the status of voxel coloring.  Coloring may take
 * time and is almost never needed during command line operations (wb_command).
 * 
 * DO NOT CHANGE THIS VALUE if there are any instance of VolumeFile since
 * the coloring object is created when a VolumeFile instance is created.
 *
 * @param enabled
 *    New status for coloring.
 */
void
VolumeFile::setVoxelColoringEnabled(const bool enabled)
{
    s_voxelColoringEnabled = enabled;
    
    CaretLogConfig(AString(s_voxelColoringEnabled
                              ? "Volume coloring is enabled."
                           : "Volume coloring is disabled."));
}


VolumeFile::VolumeFile()
: VolumeBase(), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    m_classNameHierarchy = NULL;
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_voxelColorizer = NULL;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    validateMembers();
}

VolumeFile::VolumeFile(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents, SubvolumeAttributes::VolumeType whatType)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    m_classNameHierarchy = NULL;
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_voxelColorizer = NULL;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    validateMembers();
    setType(whatType);
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    m_classNameHierarchy = NULL;
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_voxelColorizer = NULL;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
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
    clear();

}

/**
 * Clear the file.
 */
void
VolumeFile::clear()
{
    CaretMappableDataFile::clear();
    
    if (m_voxelColorizer != NULL) {
        delete m_voxelColorizer;
        m_voxelColorizer = NULL;
    }
    
    if (m_classNameHierarchy != NULL) {
        delete m_classNameHierarchy;
        m_classNameHierarchy = NULL;
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    m_caretVolExt.clear();
    m_brickAttributes.clear();
    m_brickStatisticsValid = false;
    m_splinesValid = false;
    m_frameSplineValid.clear();
    m_frameSplines.clear();
    
    m_dataRangeValid = false;
}

void VolumeFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    AString fileToRead;
    try {
        if (DataFile::isFileOnNetwork(filename)) {
            CaretTemporaryFile tempFile;
            tempFile.readFile(filename);
            fileToRead = tempFile.getFileName();
            setFileName(filename);
        } else {
            setFileName(filename);
            fileToRead = filename;
        }
        checkFileReadability(fileToRead);
        NiftiIO myIO;//begin nifti specific code - should this go somewhere else?
        myIO.openRead(fileToRead);
        const NiftiHeader& inHeader = myIO.getHeader();
        int numComponents = myIO.getNumComponents();
        reinitialize(myIO.getDimensions(), inHeader.getSForm(), numComponents);
        vector<int64_t> myDims = myIO.getDimensions();
        vector<int64_t> extraDims;//non-spatial dims
        if (myDims.size() > 3)
        {
            extraDims = vector<int64_t>(myDims.begin() + 3, myDims.end());
        }
        int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
        if (numComponents != 1)
        {
            vector<float> tempFrame(frameSize), readBuffer(frameSize * numComponents);
            for (MultiDimIterator<int64_t> myiter(extraDims); !myiter.atEnd(); ++myiter)
            {
                myIO.readData(readBuffer.data(), 3, *myiter);
                for (int c = 0; c < numComponents; ++c)
                {
                    for (int64_t i = 0; i < frameSize; ++i)
                    {
                        tempFrame[i] = readBuffer[i * numComponents + c];
                    }
                    setFrame(tempFrame.data(), getBrickIndexFromNonSpatialIndexes(*myiter), c);
                }
            }
        } else {//avoid the added allocation for separating components
            vector<float> tempFrame(frameSize);
            for (MultiDimIterator<int64_t> myiter(extraDims); !myiter.atEnd(); ++myiter)
            {
                myIO.readData(tempFrame.data(), 3, *myiter);
                setFrame(tempFrame.data(), getBrickIndexFromNonSpatialIndexes(*myiter));
            }
        }
        m_header.grabNew(new NiftiHeader(inHeader));
        for (int64_t i = 0; i < (int64_t)inHeader.m_extensions.size(); ++i)
        {
            m_extensions.push_back(inHeader.m_extensions[i]);
        }//end nifti-specific code
        parseExtensions();
        clearModified();
    } catch (const CaretException& e) {
        clear();
        throw DataFileException(e);
    } catch (...) {
        clear();
        throw DataFileException("unknown error while trying to open volume file " + filename);
    }
    
    /*
     * This will update the map name/label hierarchy
     */
    if (isMappedWithLabelTable()) {
        m_forceUpdateOfGroupAndNameHierarchy = true;
        getGroupAndNameHierarchyModel();
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
    checkFileWritability(filename);
    
    try {
        if (getNumberOfComponents() != 1)
        {
            throw DataFileException("writing multi-component volumes is not currently supported");//its a hassle, and uncommon, and there is only one 3-component type, restricted to 0-255
        }
        updateCaretExtension();
        NiftiHeader outHeader;//begin nifti-specific code
        if (m_header != NULL && (m_header->getType() == AbstractHeader::NIFTI))
        {
            outHeader = *((NiftiHeader*)m_header.getPointer());
        }
        outHeader.clearDataScaling();
        outHeader.setSForm(m_volSpace.getSform());
        outHeader.setDimensions(m_origDims);
        outHeader.setDataType(NIFTI_TYPE_FLOAT32);
        outHeader.m_extensions.clear();
        for (int64_t i = 0; i < (int64_t)m_extensions.size(); ++i)
        {
            if (m_extensions[i]->getType() == AbstractVolumeExtension::NIFTI)
            {//ugliness due to smart pointer operator= not allowing you to go from base to derived - could use dynamic_cast internally, but might allow more stupid mistakes past the compiler
                outHeader.m_extensions.push_back(CaretPointer<NiftiExtension>(new NiftiExtension(*((NiftiExtension*)m_extensions[i].getPointer()))));
            }
        }
        NiftiIO myIO;
        int outVersion = 1;
        if (!outHeader.canWriteVersion(1)) outVersion = 2;
        myIO.writeNew(filename, outHeader, outVersion);
        const vector<int64_t>& origDims = getOriginalDimensions();
        vector<int64_t> extraDims;//non-spatial dims
        if (origDims.size() > 3)
        {
            extraDims = vector<int64_t>(origDims.begin() + 3, origDims.end());
        }
        for (MultiDimIterator<int64_t> myiter(extraDims); !myiter.atEnd(); ++myiter)
        {
            myIO.writeData(getFrame(getBrickIndexFromNonSpatialIndexes(*myiter)), 3, *myiter);//NOTE: does not deal with multi-component volumes
        }
        m_header.grabNew(new NiftiHeader(outHeader));//update header to last written version, end nifti-specific code
    }
    catch (const CaretException& e) {
        throw DataFileException(e);
    }
}

float VolumeFile::interpolateValue(const float* coordIn, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component) const
{
    return interpolateValue(coordIn[0], coordIn[1], coordIn[2], interp, validOut, brickIndex, component);
}

float VolumeFile::interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component) const
{
    switch (interp)
    {
        case CUBIC:
        {
            float indexSpace[3];
            spaceToIndex(coordIn1, coordIn2, coordIn3, indexSpace);
            int64_t ind1low = floor(indexSpace[0]);
            int64_t ind2low = floor(indexSpace[1]);
            int64_t ind3low = floor(indexSpace[2]);
            int64_t ind1high = ind1low + 1;
            int64_t ind2high = ind2low + 1;
            int64_t ind3high = ind3low + 1;
            if (!indexValid(ind1low, ind2low, ind3low, brickIndex, component) || !indexValid(ind1high, ind2high, ind3high, brickIndex, component))
            {
                if (validOut != NULL) *validOut = false;
                return INVALID_INTERP_VALUE;//check for valid coord before deconvolving the frame
            }
            int64_t whichFrame = component * m_dimensions[3] + brickIndex;
            validateSplines(brickIndex, component);
            if (validOut != NULL) *validOut = true;
            return m_frameSplines[whichFrame].sample(indexSpace);
        }
        case TRILINEAR:
        {
            float index1, index2, index3;
            spaceToIndex(coordIn1, coordIn2, coordIn3, index1, index2, index3);
            int64_t ind1low = floor(index1);
            int64_t ind2low = floor(index2);
            int64_t ind3low = floor(index3);
            int64_t ind1high = ind1low + 1;
            int64_t ind2high = ind2low + 1;
            int64_t ind3high = ind3low + 1;
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
            float zhighWeight = index3 - ind3low;
            float zlowWeight = 1.0f - zhighWeight;
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

void VolumeFile::validateSplines(const int64_t brickIndex, const int64_t component) const
{
    CaretAssert(brickIndex < m_dimensions[3]);//function is public, so check inputs
    CaretAssert(component < m_dimensions[4]);
    int64_t numFrames = m_dimensions[3] * m_dimensions[4], whichFrame = component * m_dimensions[3] + brickIndex;
    CaretMutexLocker locked(&m_splineMutex);//prevent concurrent access to spline state
    if (!m_splinesValid)
    {
        m_frameSplineValid.clear();
        m_frameSplines.clear();
        m_splinesValid = true;//the only purpose of this flag is for setModified to be fast
    }
    if ((int64_t)m_frameSplineValid.size() != numFrames)
    {
        m_frameSplineValid.resize(numFrames, false);
        m_frameSplines.resize(numFrames);
    }
    if (!m_frameSplineValid[whichFrame])
    {
        m_frameSplines[whichFrame] = VolumeSpline(getFrame(brickIndex, component), m_dimensions);
        if (m_frameSplines[whichFrame].ignoredNonNumeric())
        {
            CaretLogWarning("ignored non-numeric input value when calculating cubic splines in volume '" + getFileName() + "', frame #" + AString::number(brickIndex + 1));
        }
        m_frameSplineValid[whichFrame] = true;
    }
}

bool VolumeFile::matchesVolumeSpace(const VolumeFile* right) const
{
    return m_volSpace.matches(right->m_volSpace);
}

bool VolumeFile::matchesVolumeSpace(const VolumeSpace& otherSpace) const
{
    return m_volSpace.matches(otherSpace);
}

bool VolumeFile::matchesVolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform) const
{
    return m_volSpace.matches(VolumeSpace(dims, sform));
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
            case AbstractVolumeExtension::NIFTI:
            {
                NiftiExtension* myNiftiExtension = (NiftiExtension*)m_extensions[i].getPointer();
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
                    QByteArray myByteArray(m_extensions[whichExt]->m_bytes.data(), m_extensions[whichExt]->m_bytes.size());
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
            case AbstractVolumeExtension::NIFTI:
                {
                    NiftiExtension* myNiftiExtension = (NiftiExtension*)m_extensions[i].getPointer();
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
    NiftiExtension* newExt = new NiftiExtension();//use default nifti version from this constructor
    newExt->m_ecode = NIFTI_ECODE_CARET;
    string myStr = mystream.str();
    int length = myStr.length();
    newExt->m_bytes.resize(length + 1);//add a null byte for safety
    for (int i = 0; i < length; ++i)
    {
        newExt->m_bytes[i] = myStr[i];
    }
    newExt->m_bytes[length] = 0;
    m_extensions.push_back(CaretPointer<AbstractVolumeExtension>(newExt));//doesn't matter whether this CaretPointer is the base type or inherited type, operator= will have it stored as base type inside the vector
}

void VolumeFile::validateMembers()
{
    m_dataRangeValid = false;
    m_splinesValid = false;
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
    
    /*
     * Will handle colorization of voxel data.
     */
    if (m_voxelColorizer != NULL) {
        delete m_voxelColorizer;
    }
    if (s_voxelColoringEnabled) {
        m_voxelColorizer = new VolumeFileVoxelColorizer(this);
    }
    if (m_classNameHierarchy == NULL) {
        m_classNameHierarchy = new GroupAndNameHierarchyModel();
    }
    m_classNameHierarchy->clear();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * Set this file as modified.
 */
void
VolumeFile::setModified()
{
    DataFile::setModified();//do we need to do both of these?
    VolumeBase::setModified();
    m_brickStatisticsValid = false;
    m_splinesValid = false;
}

/**
 * Clear the modified status of this file.
 */
void
VolumeFile::clearModified()
{
    CaretMappableDataFile::clearModified();
    VolumeBase::clearModified();

    const int32_t numMaps = getNumberOfMaps();
    if (isMappedWithPalette())
    {
        for (int32_t i = 0; i < numMaps; i++) {
            PaletteColorMapping* pcm = getMapPaletteColorMapping(i);
            pcm->clearModified();
        }
    }
    else if (isMappedWithLabelTable()) {
        for (int32_t i = 0; i < numMaps; i++) {
            getMapLabelTable(i)->clearModified();
        }
    }
}

/**
 * @eturn The modified status of this file.
 *
 * NOTE: DOES NOT include palette color mapping modified status.
 */
bool
VolumeFile::isModifiedExcludingPaletteColorMapping() const
{
    if (CaretMappableDataFile::isModifiedExcludingPaletteColorMapping()) {
        return true;
    }
    if (VolumeBase::isModified()) {
        return true;
    }
    
    const int32_t numMaps = getNumberOfMaps();
    if (isMappedWithLabelTable()) {
        for (int32_t i = 0; i < numMaps; i++) {
            if (getMapLabelTable(i)->isModified()) {
                return true;
            }
        }
    }
    
    return false;
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
void
VolumeFile::getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.resetForUpdate();
    
    float coordinates[3];
    for (int i = 0; i < 2; ++i)//if the volume isn't plumb, we need to test all corners, so just always test all corners
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                this->indexToSpace(i * m_dimensions[0] - 0.5f, j * m_dimensions[1] - 0.5f, k * m_dimensions[2] - 0.5f, coordinates);//accounts for extra half voxel on each side of each center
                boundingBoxOut.update(coordinates);
            }
        }
    }
}

///**
// * Assign colors for all maps in this volume file.
// * Does nothing if coloring is not enabled.
// *
// * @param paletteFile
// *     File containing the palettes.
// */
//void
//VolumeFile::assignVoxelColorsForAllMaps(const PaletteFile* paletteFile)
//{
//    if (s_voxelColoringEnabled == false) {
//        return;
//    }
//    
//    CaretAssert(m_voxelColorizer);
//    
//    const int32_t numberOfMaps = getNumberOfMaps();
//    for (int32_t iMap = 0; iMap < numberOfMaps; iMap++) {
//        assignVoxelColorsForMap(iMap,
//                                paletteFile);
////        const bool usesPalette = isMappedWithPalette();
////        const PaletteColorMapping* pcm = (usesPalette
////                                          ? getMapPaletteColorMapping(iMap)
////                                          : NULL);
////        const AString paletteName = (usesPalette
////                                     ? pcm->getSelectedPaletteName()
////                                     : "");
////        const Palette* palette = (usesPalette
////                                  ? paletteFile->getPaletteByName(paletteName)
////                                  : NULL);
////        if (usesPalette
////            && (palette == NULL)) {
////            CaretLogSevere("No palette named \""
////                           + paletteName
////                           + "\" found for coloring map index="
////                           + AString::number(iMap)
////                           + " in "
////                           + getFileNameNoPath());
////        }
////        
////        m_voxelColorizer->assignVoxelColorsForMapInBackground(iMap,
////                                                              palette,
////                                                              NULL,
////                                                              0);
//    }
//}

/**
 * Update coloring for a map.
 * Does nothing if coloring is not enabled.
 *
 * @param mapIndex
 *     Index of map.
 * @param paletteFile
 *     File containing the palettes.
 */
void
VolumeFile::updateScalarColoringForMap(const int32_t mapIndex,
                                    const PaletteFile* paletteFile)
{
    if (s_voxelColoringEnabled == false) {
        return;
    }
    
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_voxelColorizer);
    
    const bool usesPalette = isMappedWithPalette();
    const PaletteColorMapping* pcm = (usesPalette
                                      ? getMapPaletteColorMapping(mapIndex)
                                      : NULL);
    const AString paletteName = (usesPalette
                                 ? pcm->getSelectedPaletteName()
                                 : "");
    const Palette* palette = (usesPalette
                              ? paletteFile->getPaletteByName(paletteName)
                              : NULL);
    if (usesPalette
        && (palette == NULL)) {
        CaretLogSevere("No palette named \""
                       + paletteName
                       + "\" found for coloring map index="
                       + AString::number(mapIndex)
                       + " in "
                       + getFileNameNoPath());
    }
    
    m_voxelColorizer->assignVoxelColorsForMapInBackground(mapIndex,
                                                          palette,
                                                          this,
                                                          mapIndex);
}

/**
 * Get the voxel RGBA coloring for a map.
 * Does nothing if coloring is not enabled and output colors are undefined
 * in this case.
 *
 * @param paletteFile
 *    The palette file.
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    Plane for which colors are requested.
 * @param sliceIndex
 *    Index of the slice.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    Contains colors upon exit.
 */
void
VolumeFile::getVoxelColorsForSliceInMap(const PaletteFile* /*paletteFile*/,
                                        const int32_t mapIndex,
                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                 const int64_t sliceIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                 uint8_t* rgbaOut) const
{
    if (s_voxelColoringEnabled == false) {
        return;
    }
    
    CaretAssert(m_voxelColorizer);
    
    m_voxelColorizer->getVoxelColorsForSliceInMap(mapIndex,
                                                  slicePlane,
                                                  sliceIndex,
                                                  displayGroup,
                                                  tabIndex,
                                                  rgbaOut);
}

/**
 * Get the voxel values for a slice in a map.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    Plane for which colors are requested.
 * @param sliceIndex
 *    Index of the slice.
 * @param sliceValuesOut
 *    Slice values output must be correct number of elements.
 */
void
VolumeFile::getVoxelValuesForSliceInMap(const int32_t mapIndex,
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                        const int64_t sliceIndex,
                                        float* sliceValuesOut) const
{
    CaretAssert(sliceValuesOut);
    
    if (s_voxelColoringEnabled == false) {
        return;
    }
    
    int64_t dimI, dimJ, dimK, dimTime, dimMaps;
    getDimensions(dimI, dimJ, dimK, dimTime, dimMaps);
    
    switch (slicePlane) {
        case VolumeSliceViewPlaneEnum::ALL:
            return;
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
        {
            int64_t counter = 0;
            for (int64_t j = 0; j < dimJ; j++) {
                for (int64_t i = 0; i < dimI; i++) {
                    sliceValuesOut[counter] = getValue(i, j, sliceIndex, 0, mapIndex);
                    counter++;
                }
            }

        }
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
        {
            int64_t counter = 0;
            for (int64_t k = 0; k < dimK; k++) {
                for (int64_t i = 0; i < dimI; i++) {
                    sliceValuesOut[counter] = getValue(i, sliceIndex, k, 0, mapIndex);
                    counter++;
                }
            }
            
        }
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
        {
            int64_t counter = 0;
            for (int64_t k = 0; k < dimK; k++) {
                for (int64_t j = 0; j < dimJ; j++) {
                    sliceValuesOut[counter] = getValue(sliceIndex, j, k, 0, mapIndex);
                    counter++;
                }
            }
            
        }
            break;
    }
}


/**
 * Get the RGBA color components for voxel.
 * Does nothing if coloring is not enabled and output colors are undefined
 * in this case.
 *
 * @param paletteFile
 *    The palette file.
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    Contains voxel coloring on exit.
 */
void
VolumeFile::getVoxelColorInMap(const PaletteFile* /*paletteFile*/,
                               const int64_t i,
                        const int64_t j,
                        const int64_t k,
                        const int64_t mapIndex,
                               const DisplayGroupEnum::Enum displayGroup,
                               const int32_t tabIndex,
                        uint8_t rgbaOut[4]) const
{
    if (s_voxelColoringEnabled == false) {
        return;
    }
    
    CaretAssert(m_voxelColorizer);

    m_voxelColorizer->getVoxelColorInMap(i,
                                         j,
                                         k,
                                         mapIndex,
                                         displayGroup,
                                         tabIndex,
                                         rgbaOut);
}

/**
 * Clear the voxel coloring for the given map.
 * Does nothing if coloring is not enabled.
 *
 * @param mapIndex
 *    Index of map.
 */
void
VolumeFile::clearVoxelColoringForMap(const int64_t mapIndex)
{
    if (s_voxelColoringEnabled == false) {
        return;
    }
    CaretAssert(m_voxelColorizer);
    
    m_voxelColorizer->clearVoxelColoringForMap(mapIndex);
}

/**
 * Set the RGBA coloring for a voxel in a map.
 * Does nothing if coloring is not enabled.
 *
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param rgba
 *    RGBA color components for voxel.
 */
//void
//VolumeFile::setVoxelColorInMap(const int64_t i,
//                         const int64_t j,
//                         const int64_t k,
//                         const int64_t mapIndex,
//                         const float rgba[4])
//
//{
//    if (s_voxelColoringEnabled == false) {
//        return;
//    }
//    CaretAssert(m_voxelColorizer);
//    
//    m_voxelColorizer->setVoxelColorInMap(i, j, k, mapIndex, rgba);
//}

/**
 * Get the minimum and maximum values from ALL maps in this file.
 * Note that not all files (due to size of file) are able to provide
 * the minimum and maximum values from the file.  The return value
 * indicates success/failure.  If the failure (false) is returned
 * the returned values are likely +/- the maximum float values.
 *
 * @param dataRangeMinimumOut
 *    Minimum data value found.
 * @param dataRangeMaximumOut
 *    Maximum data value found.
 * @return
 *    True if the values are valid, else false.
 */
bool
VolumeFile::getDataRangeFromAllMaps(float& dataRangeMinimumOut,
                                               float& dataRangeMaximumOut) const
{
    /*
     * No data?
     */
    if (m_dataSize <= 0) {
        dataRangeMaximumOut = std::numeric_limits<float>::max();
        dataRangeMinimumOut = -dataRangeMaximumOut;
        return false;
    }
    
    /*
     * If valid, no need to update
     */
    if (m_dataRangeValid) {
        dataRangeMinimumOut = m_dataRangeMinimum;
        dataRangeMaximumOut = m_dataRangeMaximum;
        return true;
    }
    
    /*
     * Update range.
     */
    m_dataRangeMaximum = -std::numeric_limits<float>::max();
    m_dataRangeMinimum = std::numeric_limits<float>::max();
    
    for (int64_t i = 0; i < m_dataSize; i++) {
        if (m_data[i] > m_dataRangeMaximum) {
            m_dataRangeMaximum = m_data[i];
        }
        if (m_data[i] < m_dataRangeMinimum) {
            m_dataRangeMinimum = m_data[i];
        }
    }
    
    dataRangeMinimumOut = m_dataRangeMinimum;
    dataRangeMaximumOut = m_dataRangeMaximum;
    
    m_dataRangeValid = true;
    
    return true;
}

/**
 * Get the voxel indices of all voxels in the given map with the given label key.
 *
 * @param mapIndex
 *    Index of map.
 * @param labelKey
 *    Key of the label.
 * @param voxelIndicesOut
 *    Output containing indices of voxels with the given label key.
 */
void
VolumeFile::getVoxelIndicesWithLabelKey(const int32_t mapIndex,
                                        const int32_t labelKey,
                                        std::vector<VoxelIJK>& voxelIndicesOut) const
{
    voxelIndicesOut.clear();
    
    std::vector<int64_t> dims;
    getDimensions(dims);
    
    const int64_t dimI = dims[0];
    const int64_t dimJ = dims[1];
    const int64_t dimK = dims[2];
    
    for (int64_t i = 0; i < dimI; i++) {
        for (int64_t j = 0; j < dimJ; j++) {
            for (int64_t k = 0; k < dimK; k++) {
                const float keyValue = static_cast<int32_t>(getValue(i, j, k, mapIndex));
                if (keyValue == labelKey) {
                    voxelIndicesOut.push_back(VoxelIJK(i, j, k));
                }
            }
        }
    }
}

/**
 * Get the unique label keys in the given map.
 * @param mapIndex
 *    Index of the map.
 * @return
 *    Keys used by the map.
 */
std::vector<int32_t>
VolumeFile::getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const
{
    std::vector<int64_t> dims;
    getDimensions(dims);
    
    const int64_t dimI = dims[0];
    const int64_t dimJ = dims[1];
    const int64_t dimK = dims[2];
    
    std::set<int32_t> uniqueKeys;
    for (int64_t i = 0; i < dimI; i++) {
        for (int64_t j = 0; j < dimJ; j++) {
            for (int64_t k = 0; k < dimK; k++) {
                const float keyValue = static_cast<int32_t>(getValue(i, j, k, mapIndex));
                uniqueKeys.insert(keyValue);
            }
        }
    }

    std::vector<int32_t> keyVector;
    keyVector.insert(keyVector.end(),
                     uniqueKeys.begin(),
                     uniqueKeys.end());
    return keyVector;
}

/**
 * @return The class and name hierarchy.
 */
GroupAndNameHierarchyModel*
VolumeFile::getGroupAndNameHierarchyModel()
{
    m_classNameHierarchy->update(this,
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * @return The class and name hierarchy.
 */
const GroupAndNameHierarchyModel*
VolumeFile::getGroupAndNameHierarchyModel() const
{
    m_classNameHierarchy->update(const_cast<VolumeFile*>(this),
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
VolumeFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                           SceneClass* sceneClass)
{
    CaretMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    sceneClass->addBooleanArray("m_chartingEnabledForTab",
                                m_chartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    if (isMappedWithLabelTable()) {
        sceneClass->addClass(m_classNameHierarchy->saveToScene(sceneAttributes,
                                                               "m_classNameHierarchy"));
    }
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
VolumeFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    CaretMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (tabArray != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_chartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_chartingEnabledForTab[i] = false;
        }
    }
    
    if (isMappedWithLabelTable()) {
        const SceneClass* sc = sceneClass->getClass("m_classNameHierarchy");
        m_classNameHierarchy->restoreFromScene(sceneAttributes,
                                               sc);
        m_forceUpdateOfGroupAndNameHierarchy = false;
    }
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
VolumeFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretMappableDataFile::addToDataFileContentInformation(dataFileInformation);

    dataFileInformation.addNameAndValue("Orthogonal", isPlumb());
    
    if (m_header != NULL && m_header->getType() == AbstractHeader::NIFTI) {
        const NiftiHeader& myHeader = *((NiftiHeader*)m_header.getPointer());
        dataFileInformation.addNameAndValue("NIFTI Version",
                                            myHeader.versionRead());
        bool ok = false;
        dataFileInformation.addNameAndValue("NIFTI Data Type",
                                            NiftiDataTypeEnum::toName(NiftiDataTypeEnum::fromIntegerCode(myHeader.getDataType(), &ok)));//fromIntegerCode basically just ignores invalid values
        if (!ok)
        {
            CaretLogWarning("found invalid NIFTI datatype code while adding file information");
        }
        vector<int64_t> dims = myHeader.getDimensions();
        const int32_t numDims = static_cast<int32_t>(dims.size());
        dataFileInformation.addNameAndValue(("Dim[0]"),
                                            AString::number(numDims));
        for (int32_t i = 0; i < numDims; i++) {
            dataFileInformation.addNameAndValue(("Dim["
                                                 + AString::number(i + 1)
                                                 + "]"),
                                                AString::number(dims[i]));
        }//*/
    }
    else {
        int64_t dimI, dimJ, dimK, dimMaps, dimComponents;
        getDimensions(dimI, dimJ, dimK, dimMaps, dimComponents);
        
        dataFileInformation.addNameAndValue("Dim I",
                                            AString::number(dimI));
        dataFileInformation.addNameAndValue("Dim J",
                                            AString::number(dimJ));
        dataFileInformation.addNameAndValue("Dim K",
                                            AString::number(dimK));
        dataFileInformation.addNameAndValue("Dim Maps",
                                            AString::number(dimMaps));
        dataFileInformation.addNameAndValue("Dim Components",
                                            AString::number(dimComponents));
    }

    const int64_t zero64 = 0;
    if (indexValid(zero64, zero64, zero64)) {
        float x, y, z;
        indexToSpace(zero64, zero64, zero64, x, y, z);
        dataFileInformation.addNameAndValue("IJK = (0,0,0)",
                                            ("XYZ = ("
                                             + AString::number(x)
                                             + ", "
                                             + AString::number(y)
                                             + ", "
                                             + AString::number(z)
                                             + ")"));
    }
    
    BoundingBox boundingBox;
    getVoxelSpaceBoundingBox(boundingBox);
    dataFileInformation.addNameAndValue("X-minimum", boundingBox.getMinX());
    dataFileInformation.addNameAndValue("X-maximum", boundingBox.getMaxX());
    dataFileInformation.addNameAndValue("Y-minimum", boundingBox.getMinY());
    dataFileInformation.addNameAndValue("Y-maximum", boundingBox.getMaxY());
    dataFileInformation.addNameAndValue("Z-minimum", boundingBox.getMinZ());
    dataFileInformation.addNameAndValue("Z-maximum", boundingBox.getMaxZ());
    
    VolumeSpace::OrientTypes orientation[3];
    
    getOrientation(orientation);
    for (int32_t i = 0; i < 3; i++) {
        AString orientName;
        
        switch (orientation[i]) {
            case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                orientName = "Anterior to Posterior";
                break;
            case VolumeSpace::INFERIOR_TO_SUPERIOR:
                orientName = "Inferior to Superior";
                break;
            case VolumeSpace::LEFT_TO_RIGHT:
                orientName = "Left to Right";
                break;
            case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                orientName = "Posterior to Anterior";
                break;
            case VolumeSpace::RIGHT_TO_LEFT:
                orientName = "Right to Left";
                break;
            case VolumeSpace::SUPERIOR_TO_INFERIOR:
                orientName = "Superior to Inferior";
                break;
        }
        dataFileInformation.addNameAndValue(("Orientation[" + AString::number(i) + "]"),
                                            orientName);
    }
    
    float spacing[3];
    getVoxelSpacing(spacing[0], spacing[1], spacing[2]);
    spacing[0] = std::fabs(spacing[0]);
    spacing[1] = std::fabs(spacing[1]);
    spacing[2] = std::fabs(spacing[2]);
    dataFileInformation.addNameAndValue("Spacing",
                                        AString::fromNumbers(spacing, 3, ", "));
    
}

/**
 * @return Is charting enabled for this file?
 */
bool
VolumeFile::isChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
VolumeFile::isChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
        return true;
    }
    
    return false;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
VolumeFile::setChartingEnabled(const int32_t tabIndex,
                               const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
VolumeFile::getSupportedChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedBrainordinateChartDataTypes(chartDataTypesOut);
}

/**
 * Load charting data for the surface with the given structure and node index.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
VolumeFile::loadChartDataForSurfaceNode(const StructureEnum::Enum /*structure*/,
                                        const int32_t /*nodeIndex*/) throw (DataFileException)
{
    ChartDataCartesian* chartData = NULL;
    return chartData;
}

/**
 * Load average charting data for the surface with the given structure and node indices.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndices
 *     Indices of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
VolumeFile::loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum /*structure*/,
                                                const std::vector<int32_t>& /*nodeIndices*/) throw (DataFileException)
{
    ChartDataCartesian* chartData = NULL;
    return chartData;
}

/**
 * Load charting data for the voxel enclosing the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
VolumeFile::loadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    ChartDataCartesian* chartData = NULL;

    if (isMappedWithPalette()) {
        int64_t ijk[3];
        enclosingVoxel(xyz,
                       ijk);
        
        if (indexValid(ijk)) {
            std::vector<float> data;
            
            const int32_t numMaps = getNumberOfMaps();
            for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                data.push_back(getValue(ijk, iMap));
            }
            
            try {
                chartData = helpCreateCartesianChartData(data);
                ChartDataSource* dataSource = chartData->getChartDataSource();
                dataSource->setVolumeVoxel(getFileName(), xyz);
            }
            catch (const DataFileException& dfe) {
                if (chartData != NULL) {
                    delete chartData;
                    chartData = NULL;
                }
                
                throw dfe;
            }
        }
    }
    
    return chartData;
}


