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
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "GroupAndNameHierarchyModel.h"
#include "FastStatistics.h"
#include "Histogram.h"
#include "MultiDimIterator.h"
#include "NiftiIO.h"
#include "Palette.h"
#include "SceneClass.h"
#include "VolumeFile.h"
#include "VolumeFileEditorDelegate.h"
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
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
    m_forceUpdateOfGroupAndNameHierarchy = true;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    m_volumeFileEditorDelegate.grabNew(NULL);
    validateMembers();
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
    m_forceUpdateOfGroupAndNameHierarchy = true;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    m_volumeFileEditorDelegate.grabNew(NULL);
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
{
    clear();
    VolumeBase::reinitialize(dimensionsIn, indexToSpace, numComponents);
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const VolumeSpace& volSpaceIn, const int64_t numFrames, const int64_t numComponents, SubvolumeAttributes::VolumeType whatType)
{
    CaretAssert(numFrames > 0);
    const int64_t* dimsPtr = volSpaceIn.getDims();
    vector<int64_t> dims(dimsPtr, dimsPtr + 3);
    if (numFrames > 1)
    {
        dims.push_back(numFrames);
    }
    reinitialize(dims, volSpaceIn.getSform(), numComponents, whatType);
}

void VolumeFile::addSubvolumes(const int64_t& numToAdd)
{
    VolumeBase::addSubvolumes(numToAdd);
    validateMembers();
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
    m_voxelColorizer.grabNew(NULL);
    m_classNameHierarchy.grabNew(NULL);
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
    
    m_caretVolExt.clear();
    m_brickAttributes.clear();
    m_brickStatisticsValid = false;
    m_splinesValid = false;
    m_frameSplineValid.clear();
    m_frameSplines.clear();
    
    m_dataRangeValid = false;
    VolumeBase::clear();
    
    m_volumeFileEditorDelegate->clear();
}

void VolumeFile::readFile(const AString& filename)
{
    ElapsedTimer timer;
    timer.start();
    
    clear();
    
    {
        /*
         * CaretTemporaryFile must be outside of the "if" statment block of code.
         * Otherwise, at the end of the "if" statement block, the 
         * CaretTemporaryFile object will go out of scope and the temporary
         * file will be deleted so there will be no file to read.
         */
        AString fileToRead;
        CaretTemporaryFile tempFile;
        if (DataFile::isFileOnNetwork(filename)) {
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
        for (int i = 0; i < (int)inHeader.m_extensions.size(); ++i)
        {//check for actually being cifti
            if (inHeader.m_extensions[i]->m_ecode == NIFTI_ECODE_CIFTI)
            {
                throw DataFileException(filename, "Cifti files cannot be used as volume files");
            }
        }
        int numComponents = myIO.getNumComponents();
        vector<int64_t> myDims = myIO.getDimensions();
        int fullDims = 3;//deal with nifti with less than 3 dimensions
        if (myDims.size() < 3) fullDims = (int)myDims.size();
        vector<int64_t> extraDims;//non-spatial dims
        if (myDims.size() > 3)
        {
            extraDims = vector<int64_t>(myDims.begin() + 3, myDims.end());
        }
        while (myDims.size() < 3) myDims.push_back(1);//pretend we have 3 dimensions in header, always, things that use getOriginalDimensions assume this (because "VolumeFile")
        if (myDims.size() > 3)
        {//check for cifti-like dimensions here, so that we have the filename to report in the error
            int64_t numFrames = myDims[3];
            for (int i = 4; i < (int)myDims.size(); ++i)
            {
                numFrames *= myDims[i];
            }
            if (myDims[0] == 1 && myDims[1] == 1 && myDims[2] == 1 && numFrames > 10000)
            {
                throw DataFileException(filename, "volume FOV is 1x1x1 voxel, with over 10,000 frames, which suggests a broken cifti file (no header extension)");
            }
        }//this check is also done in reinitialize(), but we don't want to call getSForm before this check when reading a file
        reinitialize(myDims, inHeader.getSForm(), numComponents);
        setFileName(filename);  // must be done after reinitialize() since it calls clear() which clears the name of the file
        int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
        if (numComponents != 1)
        {
            vector<float> tempFrame(frameSize), readBuffer(frameSize * numComponents);
            for (MultiDimIterator<int64_t> myiter(extraDims); !myiter.atEnd(); ++myiter)
            {
                myIO.readData(readBuffer.data(), fullDims, *myiter);
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
                myIO.readData(tempFrame.data(), fullDims, *myiter);
                setFrame(tempFrame.data(), getBrickIndexFromNonSpatialIndexes(*myiter));
            }
        }
        
        CaretLogFine("Time to read volume data is "
                     + AString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                     + " seconds.");
        m_header.grabNew(new NiftiHeader(inHeader));//end nifti-specific code
        parseExtensions();
        clearModified();
    }
    
    m_volumeFileEditorDelegate->updateIfVolumeFileChangedNumberOfMaps();
    
    /*
     * This will update the map name/label hierarchy
     */
    if (isMappedWithLabelTable()) {
        m_forceUpdateOfGroupAndNameHierarchy = true;
        getGroupAndNameHierarchyModel();
    }
    
    CaretLogFine("Total Time to read and process volume is "
                 + AString::number(timer.getElapsedTimeSeconds(), 'f', 3)
                 + " seconds.");
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
VolumeFile::writeFile(const AString& filename)
{
    if (!(filename.endsWith(".nii.gz") || filename.endsWith(".nii")))
    {
        CaretLogWarning("volume file '" + filename + "' should be saved ending in .nii.gz or .nii, other formats are not supported");
    }
    checkFileWritability(filename);
    
    if (getNumberOfComponents() != 1)
    {
        throw DataFileException(filename,
                                "writing multi-component volumes is not currently supported");//its a hassle, and uncommon, and there is only one 3-component type, restricted to 0-255
    }
    updateCaretExtension();
    
    NiftiHeader outHeader;//begin nifti-specific code
    if (m_header != NULL && (m_header->getType() == AbstractHeader::NIFTI))
    {
        outHeader = *((NiftiHeader*)m_header.getPointer());//also shallow copies extensions
    }
    outHeader.clearDataScaling();
    outHeader.setSForm(getVolumeSpace().getSform());
    outHeader.setDimensions(getOriginalDimensions());
    outHeader.setDataType(NIFTI_TYPE_FLOAT32);
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
    myIO.close();//call close explicitly to get a throw rather than a severe log when there is a problem
    m_header.grabNew(new NiftiHeader(outHeader));//update header to last written version, end nifti-specific code
    
    m_volumeFileEditorDelegate->clear();
    m_volumeFileEditorDelegate->updateIfVolumeFileChangedNumberOfMaps();
}

bool VolumeFile::hasGoodSpatialInformation() const
{
    if (m_header != NULL)
    {
        return m_header->hasGoodSpatialInformation();
    }
    return true;
}

float VolumeFile::interpolateValue(const float* coordIn, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component) const
{
    return interpolateValue(coordIn[0], coordIn[1], coordIn[2], interp, validOut, brickIndex, component);
}

float VolumeFile::interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component) const
{
    /*
     * If the volume is a single slice, CUBIC and TRILINEAR will fail they
     * require and interpolate between adjacent slices.
     */
    if (m_singleSliceFlag) {
        interp = ENCLOSING_VOXEL;
    }
    
    const int64_t* dimensions = getDimensionsPtr();
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
            int64_t whichFrame = component * dimensions[3] + brickIndex;
            validateSpline(brickIndex, component);
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

void VolumeFile::validateSpline(const int64_t brickIndex, const int64_t component) const
{
    const int64_t* dimensions = getDimensionsPtr();
    CaretAssert(brickIndex >= 0 && brickIndex < dimensions[3]);//function is public, so check inputs
    CaretAssert(component >= 0 && component < dimensions[4]);
    int64_t numFrames = dimensions[3] * dimensions[4], whichFrame = component * dimensions[3] + brickIndex;
    if (!m_splinesValid)
    {
        CaretMutexLocker locked(&m_splineMutex);//prevent concurrent modify access to spline state
        if (!m_splinesValid)//double check
        {
            m_frameSplineValid = vector<bool>(numFrames, false);
            m_frameSplines = vector<VolumeSpline>(numFrames);//release the old spline memory
            m_splinesValid = true;//the only purpose of this flag is for setModified to be fast, don't worry about it becoming false again before the below happens
        }
    }
    CaretAssert((int64_t)m_frameSplineValid.size() == numFrames);
    CaretAssert((int64_t)m_frameSplines.size() == numFrames);
    if (!m_frameSplineValid[whichFrame])
    {
        CaretMutexLocker locked(&m_splineMutex);//prevent concurrent modify access to spline state
        if (!m_frameSplineValid[whichFrame])//double check
        {
            m_frameSplines[whichFrame] = VolumeSpline(getFrame(brickIndex, component), dimensions);
            if (m_frameSplines[whichFrame].ignoredNonNumeric())
            {
                CaretLogWarning("ignored non-numeric input value when calculating cubic splines in volume '" + getFileName() + "', frame #" + AString::number(brickIndex + 1));
            }
            m_frameSplineValid[whichFrame] = true;
        }
    }
}

void VolumeFile::freeSpline(const int64_t brickIndex, const int64_t component) const
{
    const int64_t* dimensions = getDimensionsPtr();
    CaretAssert(brickIndex >= 0 && brickIndex < dimensions[3]);//function is public, so check inputs
    CaretAssert(component >= 0 && component < dimensions[4]);
    int64_t numFrames = dimensions[3] * dimensions[4], whichFrame = component * dimensions[3] + brickIndex;
    if (!m_splinesValid)
    {
        CaretMutexLocker locked(&m_splineMutex);//prevent concurrent modify access to spline state
        if (!m_splinesValid)//double check
        {
            m_frameSplineValid = vector<bool>(numFrames, false);
            m_frameSplines = vector<VolumeSpline>(numFrames);//release the old spline memory
            m_splinesValid = true;//the only purpose of this flag is for setModified to be fast
        }
        return;//already freed, we are done
    }
    CaretAssert((int64_t)m_frameSplineValid.size() == numFrames);
    CaretAssert((int64_t)m_frameSplines.size() == numFrames);
    if (m_frameSplineValid[whichFrame])
    {
        CaretMutexLocker locked(&m_splineMutex);//prevent concurrent modify access to spline state
        if (m_frameSplineValid[whichFrame])//double check
        {
            m_frameSplines[whichFrame] = VolumeSpline();
            m_frameSplineValid[whichFrame] = false;
        }
    }
}

bool VolumeFile::matchesVolumeSpace(const VolumeFile* right) const
{
    return getVolumeSpace().matches(right->getVolumeSpace());
}

bool VolumeFile::matchesVolumeSpace(const VolumeSpace& otherSpace) const
{
    return getVolumeSpace().matches(otherSpace);
}

bool VolumeFile::matchesVolumeSpace(const int64_t dims[3], const vector<vector<float> >& sform) const
{
    return getVolumeSpace().matches(VolumeSpace(dims, sform));
}

void VolumeFile::parseExtensions()
{
    const int NIFTI_ECODE_CARET = 30;//this should probably go in nifti1.h
    if (m_header != NULL && m_header->getType() == AbstractHeader::NIFTI)
    {
        const NiftiHeader& myHeader = *((NiftiHeader*)m_header.getPointer());
        int numExtensions = (int)myHeader.m_extensions.size();
        int whichExt = -1, whichType = -1;//type will track caret's preference in which extension to read, the greater the type, the more it prefers it
        for (int i = 0; i < numExtensions; ++i)
        {
            const NiftiExtension& myNiftiExtension = *(myHeader.m_extensions[i]);
            switch (myNiftiExtension.m_ecode)
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
        if (whichExt != -1)
        {
            switch (whichType)
            {
                case 100://caret extension
                {
                    QByteArray myByteArray(myHeader.m_extensions[whichExt]->m_bytes.data(), myHeader.m_extensions[whichExt]->m_bytes.size());
                    AString myString(myByteArray);
                    m_caretVolExt.readFromXmlString(myString);
                    break;
                }
                default:
                    break;
            }
        }
    }
    validateMembers();
}

void VolumeFile::updateCaretExtension()
{
    const int NIFTI_ECODE_CARET = 30;//this should probably go in nifti1.h
    stringstream mystream;
    XmlWriter myWriter(mystream);
    m_caretVolExt.writeAsXML(myWriter);
    string myStr = mystream.str();
    if (m_header == NULL) m_header.grabNew(new NiftiHeader());
    switch (m_header->getType())
    {
        case AbstractHeader::NIFTI:
        {
            NiftiHeader& myHeader = *((NiftiHeader*)m_header.getPointer());
            int numExtensions = (int)myHeader.m_extensions.size();
            for (int i = 0; i < numExtensions; ++i)//erase all existing caret extensions
            {
                NiftiExtension* myNiftiExtension = myHeader.m_extensions[i];
                if (myNiftiExtension->m_ecode == NIFTI_ECODE_CARET)
                {
                    myHeader.m_extensions.erase(myHeader.m_extensions.begin() + i);
                    --i;
                    --numExtensions;
                }
            }
            CaretPointer<NiftiExtension> newExt(new NiftiExtension());
            newExt->m_ecode = NIFTI_ECODE_CARET;
            int length = myStr.length();
            newExt->m_bytes.resize(length + 1);//allocate a null byte for safety
            for (int i = 0; i < length; ++i)
            {
                newExt->m_bytes[i] = myStr[i];
            }
            newExt->m_bytes[length] = '\0';
            myHeader.m_extensions.push_back(newExt);
            break;
        }
    }
}

void VolumeFile::validateMembers()
{
    m_dataRangeValid = false;
    const int64_t* dimensions = getDimensionsPtr();
    m_frameSplineValid = vector<bool>(dimensions[3] * dimensions[4], false);
    m_frameSplines = vector<VolumeSpline>(dimensions[3] * dimensions[4]);//release any previous spline memory
    m_splinesValid = true;//this now indicates only if they need to all be recalculated - the frame vectors will always have the correct length
    int numMaps = getNumberOfMaps();
    m_brickAttributes.resize(numMaps);//only resize, if this was called from reinitialize, it has called clear() beforehand
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
    
    //setPaletteNormalizationMode(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
    
    m_singleSliceFlag = false;
    if ((dimensions[0] == 1)
        || (dimensions[1] == 1)
        || (dimensions[2] == 1)) {
        m_singleSliceFlag = true;
    }
    
    /*
     * Will handle colorization of voxel data.
     */
    if (m_voxelColorizer != NULL) {
        m_voxelColorizer.grabNew(NULL);
    }
    if (s_voxelColoringEnabled) {
        m_voxelColorizer.grabNew(new VolumeFileVoxelColorizer(this));
    }
    if (m_classNameHierarchy == NULL) {
        m_classNameHierarchy.grabNew(new GroupAndNameHierarchyModel());
    }
    m_classNameHierarchy->clear();
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    m_volumeFileEditorDelegate.grabNew(new VolumeFileEditorDelegate(this));
    m_volumeFileEditorDelegate->updateIfVolumeFileChangedNumberOfMaps();
}

/**
 * Set this file as modified.
 */
void
VolumeFile::setModified()
{
    DataFile::setModified();
    VolumeBase::setModified();
    m_brickStatisticsValid = false;
    m_splinesValid = false;
    m_fileFastStatistics.grabNew(NULL);
    m_fileHistogram.grabNew(NULL);
    m_fileHistorgramLimitedValues.grabNew(NULL);
}

/**
 * Clear the modified status of this file.
 */
void
VolumeFile::clearModified()
{
    CaretMappableDataFile::clearModified();
    clearModifiedVolumeBase();

    getFileMetaData()->clearModified();
    
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
    
    for (int32_t i = 0; i < numMaps; i++) {
        getMapMetaData(i)->clearModified();
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
    if (isModifiedVolumeBase()) {
        return true;
    }
    
    if (getFileMetaData()->isModified()) {
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
    for (int32_t i = 0; i < numMaps; i++) {
        if (getMapMetaData(i)->isModified()) {
            return true;
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
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    return &m_caretVolExt.m_attributes[mapIndex]->m_metadata;
}

GiftiMetaData* VolumeFile::getMapMetaData(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    return &m_caretVolExt.m_attributes[mapIndex]->m_metadata;
}

void VolumeFile::checkStatisticsValid()
{
    if (m_brickStatisticsValid == false)
    {
        int32_t numMaps = getNumberOfMaps();
        for (int i = 0; i < numMaps; ++i)
        {
            m_brickAttributes[i].m_fastStatistics.grabNew(NULL);
            m_brickAttributes[i].m_histogram.grabNew(NULL);
            m_brickAttributes[i].m_histogramLimitedValues.grabNew(NULL);
        }
        m_brickStatisticsValid = true;
    }
}

const FastStatistics* VolumeFile::getMapFastStatistics(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    const int64_t* dimensions = getDimensionsPtr();
    if (m_brickAttributes[mapIndex].m_fastStatistics == NULL)
    {
        m_brickAttributes[mapIndex].m_fastStatistics.grabNew(new FastStatistics(getFrame(mapIndex), dimensions[0] * dimensions[1] * dimensions[2]));
    }
    return m_brickAttributes[mapIndex].m_fastStatistics;
}

const Histogram* VolumeFile::getMapHistogram(const int32_t mapIndex)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    const int64_t* dimensions = getDimensionsPtr();
    if (m_brickAttributes[mapIndex].m_histogram == NULL)
    {
        m_brickAttributes[mapIndex].m_histogram.grabNew(new Histogram(100, getFrame(mapIndex), dimensions[0] * dimensions[1] * dimensions[2]));
    }
    return m_brickAttributes[mapIndex].m_histogram;
}

/**
 * Update the Histogram for limited values.
 *
 * @param data
 *     Data for histogram.
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
 */
const Histogram*
VolumeFile::getMapHistogram(const int32_t mapIndex,
                                             const float mostPositiveValueInclusive,
                                             const float leastPositiveValueInclusive,
                                             const float leastNegativeValueInclusive,
                                             const float mostNegativeValueInclusive,
                                             const bool includeZeroValues)
{
    CaretAssertVectorIndex(m_brickAttributes, mapIndex);
    checkStatisticsValid();
    const int64_t* dimensions = getDimensionsPtr();
    
    bool updateHistogramFlag = false;
    
    if (m_brickAttributes[mapIndex].m_histogramLimitedValues == NULL)
    {
        m_brickAttributes[mapIndex].m_histogramLimitedValues.grabNew(new Histogram(100));
        updateHistogramFlag = true;
    }
    else if ((mostPositiveValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesMostPositiveValueInclusive)
             || (leastPositiveValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastPositiveValueInclusive)
             || (leastNegativeValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastNegativeValueInclusive)
             || (mostNegativeValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesMostNegativeValueInclusive)
             || (includeZeroValues != m_brickAttributes[mapIndex].m_histogramLimitedValuesIncludeZeroValues)) {
        updateHistogramFlag = true;
    }
    
    if (updateHistogramFlag) {
        m_brickAttributes[mapIndex].m_histogramLimitedValues->update(getFrame(mapIndex),
                                                                     dimensions[0] * dimensions[1] * dimensions[2],
                                                                     mostPositiveValueInclusive,
                                                                     leastPositiveValueInclusive,
                                                                     leastNegativeValueInclusive,
                                                                     mostNegativeValueInclusive,
                                                                     includeZeroValues);
        m_brickAttributes[mapIndex].m_histogramLimitedValuesMostPositiveValueInclusive = mostPositiveValueInclusive;
        m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
        m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
        m_brickAttributes[mapIndex].m_histogramLimitedValuesMostNegativeValueInclusive = mostNegativeValueInclusive;
        m_brickAttributes[mapIndex].m_histogramLimitedValuesIncludeZeroValues = includeZeroValues;
    }
    
    return m_brickAttributes[mapIndex].m_histogramLimitedValues;
}

/**
 * @return The estimated size of data after it is uncompressed
 * and loaded into RAM.  A negative value indicates that the
 * file size cannot be computed.
 */
int64_t
VolumeFile::getDataSizeUncompressedInBytes() const
{
    int64_t dimI, dimJ, dimK, dimTime, dimComp;
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    const int64_t numBytes = (dimI
                              * dimJ
                              * dimK
                              * dimTime
                              * dimComp
                              * sizeof(float));

    return numBytes;
}

/**
 * Get statistics describing the distribution of data
 * mapped with a color palette for all data within the file.
 *
 * @return
 *    Fast statistics for data (will be NULL for data
 *    not mapped using a palette).
 */
const FastStatistics*
VolumeFile::getFileFastStatistics()
{
    if (m_fileFastStatistics == NULL) {
        std::vector<float> fileData;
        getFileData(fileData);
        if ( ! fileData.empty()) {
            m_fileFastStatistics.grabNew(new FastStatistics());
            m_fileFastStatistics->update(&fileData[0],
                                         fileData.size());
        }
    }
    
    return m_fileFastStatistics;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data within
 * the file.
 *
 * @return
 *    Histogram for data (will be NULL for data
 *    not mapped using a palette).
 */
const Histogram*
VolumeFile::getFileHistogram()
{
    if (m_fileHistogram == NULL) {
        std::vector<float> fileData;
        getFileData(fileData);
        if ( ! fileData.empty()) {
            m_fileHistogram.grabNew(new Histogram());
            m_fileHistogram->update(&fileData[0],
                                    fileData.size());
        }
    }
    return m_fileHistogram;
}

/**
 * Get histogram describing the distribution of data
 * mapped with a color palette for all data in the file
 * within the given range of values.
 *
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
const Histogram*
VolumeFile::getFileHistogram(const float mostPositiveValueInclusive,
                                           const float leastPositiveValueInclusive,
                                           const float leastNegativeValueInclusive,
                                           const float mostNegativeValueInclusive,
                                           const bool includeZeroValues)
{
    bool updateHistogramFlag = false;
    if (m_fileHistorgramLimitedValues != NULL) {
        if ((mostPositiveValueInclusive != m_fileHistogramLimitedValuesMostPositiveValueInclusive)
            || (leastPositiveValueInclusive != m_fileHistogramLimitedValuesLeastPositiveValueInclusive)
            || (leastNegativeValueInclusive != m_fileHistogramLimitedValuesLeastNegativeValueInclusive)
            || (mostNegativeValueInclusive != m_fileHistogramLimitedValuesMostNegativeValueInclusive)
            || (includeZeroValues != m_fileHistogramLimitedValuesIncludeZeroValues)) {
            updateHistogramFlag = true;
        }
    }
    else {
        updateHistogramFlag = true;
    }
    
    if (updateHistogramFlag) {
        std::vector<float> fileData;
        getFileData(fileData);
        if ( ! fileData.empty()) {
            if (m_fileHistorgramLimitedValues == NULL) {
                m_fileHistorgramLimitedValues.grabNew(new Histogram());
            }
            m_fileHistorgramLimitedValues->update(&fileData[0],
                                                  fileData.size(),
                                                  mostPositiveValueInclusive,
                                                  leastPositiveValueInclusive,
                                                  leastNegativeValueInclusive,
                                                  mostNegativeValueInclusive,
                                                  includeZeroValues);
            
            m_fileHistogramLimitedValuesMostPositiveValueInclusive  = mostPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastPositiveValueInclusive = leastPositiveValueInclusive;
            m_fileHistogramLimitedValuesLeastNegativeValueInclusive = leastNegativeValueInclusive;
            m_fileHistogramLimitedValuesMostNegativeValueInclusive  = mostNegativeValueInclusive;
            m_fileHistogramLimitedValuesIncludeZeroValues           = includeZeroValues;
        }
    }
    
    return m_fileHistorgramLimitedValues;
}

/**
 * @return Pointer to file's metadata.
 */
GiftiMetaData*
VolumeFile::getFileMetaData()
{
    return &m_caretVolExt.m_metadata;
}

/**
 * @return Pointer to file's metadata.
 */
const GiftiMetaData*
VolumeFile::getFileMetaData() const
{
    return &m_caretVolExt.m_metadata;
}


/**
 * Get all data for a volume file.  If the file is very
 * large this method may take a large amount of time!
 *
 * @param dataOut
 *    Output with all data for a file.  Empty if no data in file
 *    or data is not float.
 */
void
VolumeFile::getFileData(std::vector<float>& dataOut) const
{
    int64_t dimI, dimJ, dimK, dimTime, dimComp;
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    const int64_t mapSize     = dimI * dimJ * dimK * dimComp;
    const int64_t numMaps     = dimTime;
    const int64_t dataSize    = mapSize * numMaps;
    
    if (dataSize <= 0) {
        dataOut.clear();
        return;
    }
    
    dataOut.resize(dataSize);
    int64_t dataOffset = 0;
    
    for (int iMap = 0; iMap < numMaps; iMap++) {
        const float* mapData = getFrame(iMap);
        
        for (int64_t i = 0; i < mapSize; i++) {
            CaretAssertVectorIndex(dataOut, dataOffset);
            dataOut[dataOffset] = mapData[i];
            ++dataOffset;
        }
    }
    
    CaretAssert(dataOffset == static_cast<int64_t>(dataOut.size()));
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
    
    bool mapsWithPaletteFlag = true;
    switch (m_caretVolExt.m_attributes[0]->m_type) {
        case SubvolumeAttributes::ANATOMY:
            break;
        case SubvolumeAttributes::FUNCTIONAL:
            break;
        case SubvolumeAttributes::LABEL:
            mapsWithPaletteFlag = false;
            break;
        case SubvolumeAttributes::RGB:
            mapsWithPaletteFlag = false;
            break;
        case SubvolumeAttributes::SEGMENTATION:
            break;
        case SubvolumeAttributes::UNKNOWN:
            break;
        case SubvolumeAttributes::VECTOR:
            break;
    }
    return mapsWithPaletteFlag;
//    return (m_caretVolExt.m_attributes[0]->m_type != SubvolumeAttributes::LABEL);
}

/**
 * Get the palette normalization modes that are supported by the file.
 *
 * @param modesSupportedOut
 *     Palette normalization modes supported by a file.  Will be
 *     empty for files that are not mapped with a palette.  If there
 *     is more than one suppported mode, the first mode in the
 *     vector is assumed to be the default mode.
 */
void
VolumeFile::getPaletteNormalizationModesSupported(std::vector<PaletteNormalizationModeEnum::Enum>& modesSupportedOut) const
{
    modesSupportedOut.clear();
    
    modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
    modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
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
 * @return Is the data in the file mapped to colors using
 * Red, Green, Blue, Alpha values.
 */
bool
VolumeFile::isMappedWithRGBA() const
{
    bool mapsWithRgbaFlag = false;
    switch (m_caretVolExt.m_attributes[0]->m_type) {
        case SubvolumeAttributes::ANATOMY:
            break;
        case SubvolumeAttributes::FUNCTIONAL:
            break;
        case SubvolumeAttributes::LABEL:
            break;
        case SubvolumeAttributes::RGB:
            mapsWithRgbaFlag = true;
            break;
        case SubvolumeAttributes::SEGMENTATION:
            break;
        case SubvolumeAttributes::UNKNOWN:
            break;
        case SubvolumeAttributes::VECTOR:
            break;
    }
    return mapsWithRgbaFlag;
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
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    return m_caretVolExt.m_attributes[mapIndex]->m_metadata.getUniqueID();
}

/**
 * @return Bounding box of the volumes spatial coordinates.
 */
void
VolumeFile::getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.resetForUpdate();
    
    float coordinates[3];
    const int64_t* dimensions = getDimensionsPtr();
    for (int i = 0; i < 2; ++i)//if the volume isn't plumb, we need to test all corners, so just always test all corners
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                this->indexToSpace(i * dimensions[0] - 0.5f, j * dimensions[1] - 0.5f, k * dimensions[2] - 0.5f, coordinates);//accounts for extra half voxel on each side of each center
                boundingBoxOut.update(coordinates);
            }
        }
    }
}

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
    Palette* palette = NULL;
    
    if (usesPalette) {
        if (paletteFile != NULL) {
            palette = paletteFile->getPaletteByName(paletteName);
        }
        
        if (palette == NULL) {
            EventPaletteGetByName getPaletteEvent(paletteName);
            EventManager::get()->sendEvent(getPaletteEvent.getPointer());
            palette = getPaletteEvent.getPalette();
        }
    }
    
    if (usesPalette
        && (palette == NULL)) {
        CaretLogSevere("No palette named \""
                       + paletteName
                       + "\" found for coloring map index="
                       + AString::number(mapIndex)
                       + " in "
                       + getFileNameNoPath());
    }
    
    m_voxelColorizer->assignVoxelColorsForMap(mapIndex,
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
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
VolumeFile::getVoxelColorsForSliceInMap(const PaletteFile* /*paletteFile*/,
                                        const int32_t mapIndex,
                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                 const int64_t sliceIndex,
                                        const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                 uint8_t* rgbaOut) const
{
    if (s_voxelColoringEnabled == false) {
        return 0;
    }
    
    CaretAssert(m_voxelColorizer);
    
    return m_voxelColorizer->getVoxelColorsForSliceInMap(mapIndex,
                                                  slicePlane,
                                                  sliceIndex,
                                                  displayGroup,
                                                  tabIndex,
                                                  rgbaOut);
}

/**
 * Get voxel coloring for a set of voxels.
 *
 * @param mapIndex
 *     Index of map.
 * @param firstVoxelIJK
 *    IJK Indices of first voxel
 * @param rowStepIJK
 *    IJK Step for moving to next row.
 * @param columnStepIJK
 *    IJK Step for moving to next column.
 * @param numberOfRows
 *    Number of rows.
 * @param numberOfColumns
 *    Number of columns.
 * @param displayGroup
 *    The selected display group.
 * @param tabIndex
 *    Index of selected tab.
 * @param rgbaOut
 *    RGBA color components out.
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
VolumeFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                const int64_t firstVoxelIJK[3],
                                const int64_t rowStepIJK[3],
                                const int64_t columnStepIJK[3],
                                const int64_t numberOfRows,
                                const int64_t numberOfColumns,
                                const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                uint8_t* rgbaOut) const
{
    if (s_voxelColoringEnabled == false) {
        return 0;
    }
    
    CaretAssert(m_voxelColorizer);
    
    return m_voxelColorizer->getVoxelColorsForSliceInMap(mapIndex,
                                                 firstVoxelIJK,
                                                 rowStepIJK,
                                                 columnStepIJK,
                                                 numberOfRows,
                                                 numberOfColumns,
                                                 displayGroup,
                                                 tabIndex,
                                                 rgbaOut);
}

/**
  * Get the voxel colors for a sub slice in the map.
  *
  * @param paletteFile
  *    The palette file.
  * @param mapIndex
  *    Index of the map.
  * @param slicePlane
  *    The slice plane.
  * @param sliceIndex
  *    Index of the slice.
  * @param firstCornerVoxelIndex
  *    Indices of voxel for first corner of sub-slice (inclusive).
  * @param lastCornerVoxelIndex
  *    Indices of voxel for last corner of sub-slice (inclusive).
  * @param voxelCountIJK
  *    Voxel counts for each axis.
  * @param displayGroup
  *    The selected display group.
  * @param tabIndex
  *    Index of selected tab.
  * @param rgbaOut
  *    Output containing the rgba values (must have been allocated
  *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
  */
int64_t
VolumeFile::getVoxelColorsForSubSliceInMap(const PaletteFile* /*paletteFile*/,
                                           const int32_t mapIndex,
                                           const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                           const int64_t sliceIndex,
                                           const int64_t firstCornerVoxelIndex[3],
                                           const int64_t lastCornerVoxelIndex[3],
                                           const int64_t voxelCountIJK[3],
                                           const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           uint8_t* rgbaOut) const
{
    if (s_voxelColoringEnabled == false) {
        return 0;
    }
    
    CaretAssert(m_voxelColorizer);
    
    return m_voxelColorizer->getVoxelColorsForSubSliceInMap(mapIndex,
                                                     slicePlane,
                                                     sliceIndex,
                                                     firstCornerVoxelIndex,
                                                     lastCornerVoxelIndex,
                                                     voxelCountIJK,
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
    
    if (isMappedWithLabelTable()) {
        m_forceUpdateOfGroupAndNameHierarchy = true;
    }
}

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
    if (isEmpty()) {
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
    
    const int64_t* dimensions = getDimensionsPtr();
    int64_t m_dataSize = dimensions[0] * dimensions[1] * dimensions[2] * dimensions[3] * dimensions[4];
    const float* data = getFrame();//HACK: use first frame knowing all data is contiguous after it
    for (int64_t i = 0; i < m_dataSize; i++) {
        if (data[i] > m_dataRangeMaximum) {
            m_dataRangeMaximum = data[i];
        }
        if (data[i] < m_dataRangeMinimum) {
            m_dataRangeMinimum = data[i];
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
 * @return The volume file editor delegate used for interactive
 * editing of a volume's voxels.
 */
VolumeFileEditorDelegate*
VolumeFile::getVolumeFileEditorDelegate()
{
    CaretAssert(m_volumeFileEditorDelegate);
    return m_volumeFileEditorDelegate;
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
                                            myHeader.version());
        bool ok = false;
        dataFileInformation.addNameAndValue("NIFTI Data Type",
                                            NiftiDataTypeEnum::toName(NiftiDataTypeEnum::fromIntegerCode(myHeader.getDataType(), &ok)));//fromIntegerCode basically just ignores invalid values
        if (!ok)
        {
            CaretLogWarning("found invalid NIFTI datatype code while adding file information");
        }
    }
    AString dimString;
    vector<int64_t> dims = getOriginalDimensions();
    for (int i = 0; i < (int)dims.size(); ++i)
    {
        if (i != 0) dimString += ", ";
        dimString += AString::number(dims[i]);
    }
    dataFileInformation.addNameAndValue("Dimensions", dimString);
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
VolumeFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
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
VolumeFile::isLineSeriesChartingSupported() const
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
VolumeFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
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
VolumeFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
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
VolumeFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum /*structure*/,
                                        const int32_t /*nodeIndex*/)
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
VolumeFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum /*structure*/,
                                                const std::vector<int32_t>& /*nodeIndices*/)
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
VolumeFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
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


