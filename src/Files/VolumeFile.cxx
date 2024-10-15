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

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <QTemporaryFile>

#include "ApplicationInformation.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileClusterFinder.h"
#include "CaretResult.h"
#include "CaretTemporaryFile.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ClusterContainer.h"
#include "DataFileContentInformation.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "GiftiLabel.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GroupAndNameHierarchyModel.h"
#include "FastStatistics.h"
#include "Histogram.h"
#include "ImageFile.h"
#include "MapFileDataSelector.h"
#include "MultiDimIterator.h"
#include "NiftiIO.h"
#include "Palette.h"
#include "SceneClass.h"
#include "VolumeDynamicConnectivityFile.h"
#include "VolumeFile.h"
#include "VolumeFileEditorDelegate.h"
#include "VolumeFileVoxelColorizer.h"
#include "VolumeGraphicsPrimitiveManager.h"
#include "VolumeSpline.h"
#include "VoxelColorUpdate.h"

#include <limits>

using namespace caret;
using namespace std;

const float VolumeFile::INVALID_INTERP_VALUE = 0.0f;//we may want NaN or something more obvious
bool VolumeFile::s_voxelColoringEnabled = true;
const AString VolumeFile::s_paletteColorMappingNameInMetaData = "__DYNAMIC_FILE_PALETTE_COLOR_MAPPING__";

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

/** protected, used by dynamic volume file */
VolumeFile::VolumeFile(const DataFileTypeEnum::Enum dataFileType)
: VolumeBase(),
CaretMappableDataFile(dataFileType),
ChartableLineSeriesBrainordinateInterface(),
GroupAndNameHierarchyUserInterface()
{//CaretPointers initialize to NULL, and this isn't an operator=
    CaretAssert((dataFileType == DataFileTypeEnum::VOLUME)
                || (dataFileType == DataFileTypeEnum::VOLUME_DYNAMIC));
    m_forceUpdateOfGroupAndNameHierarchy = true;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    m_writingDoScale = false;
    m_writingDType = NIFTI_TYPE_FLOAT32;
    m_minScalingVal = -1.0;//unused, but make them consistent
    m_maxScalingVal = 1.0;
    m_graphicsPrimitiveManager.reset(new VolumeGraphicsPrimitiveManager(this, this));
    validateMembers();
}


VolumeFile::VolumeFile()
: VolumeBase(),
CaretMappableDataFile(DataFileTypeEnum::VOLUME),
ChartableLineSeriesBrainordinateInterface(),
GroupAndNameHierarchyUserInterface()
{//CaretPointers initialize to NULL, and this isn't an operator=
    m_forceUpdateOfGroupAndNameHierarchy = true;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    m_writingDoScale = false;
    m_writingDType = NIFTI_TYPE_FLOAT32;
    m_minScalingVal = -1.0;//unused, but make them consistent
    m_maxScalingVal = 1.0;
    m_graphicsPrimitiveManager.reset(new VolumeGraphicsPrimitiveManager(this, this));
    validateMembers();
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents,
                       SubvolumeAttributes::VolumeType whatType, const AbstractHeader* templateHeader)
: VolumeBase(dimensionsIn, indexToSpace, numComponents),
CaretMappableDataFile(DataFileTypeEnum::VOLUME),
ChartableLineSeriesBrainordinateInterface(),
GroupAndNameHierarchyUserInterface()
{//CaretPointers initialize to NULL, and this isn't an operator=
    m_forceUpdateOfGroupAndNameHierarchy = true;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    if (templateHeader != NULL) m_header.grabNew(templateHeader->clone());
    m_writingDoScale = false;
    m_writingDType = NIFTI_TYPE_FLOAT32;
    m_minScalingVal = -1.0;//unused, but make them consistent
    m_maxScalingVal = 1.0;
    m_graphicsPrimitiveManager.reset(new VolumeGraphicsPrimitiveManager(this, this));
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents,
                              SubvolumeAttributes::VolumeType whatType, const AbstractHeader* templateHeader)
{
    clear();
    VolumeBase::reinitialize(dimensionsIn, indexToSpace, numComponents);
    if (templateHeader != NULL) m_header.grabNew(templateHeader->clone());
    m_graphicsPrimitiveManager->clear();
    validateMembers();
    setType(whatType);
}

void VolumeFile::reinitialize(const VolumeSpace& volSpaceIn, const int64_t numFrames, const int64_t numComponents,
                              SubvolumeAttributes::VolumeType whatType, const AbstractHeader* templateHeader)
{
    CaretAssert(numFrames > 0);
    const int64_t* dimsPtr = volSpaceIn.getDims();
    vector<int64_t> dims(dimsPtr, dimsPtr + 3);
    if (numFrames > 1)
    {
        dims.push_back(numFrames);
    }
    reinitialize(dims, volSpaceIn.getSform(), numComponents, whatType, templateHeader);
    m_graphicsPrimitiveManager->clear();

}

void VolumeFile::reinitialize(const VolumeFile* headerTemplate, const int64_t numFrames, const int64_t numComponents)
{//really just a convenience wrapper
    reinitialize(headerTemplate->getVolumeSpace(), numFrames, numComponents, headerTemplate->getType(), headerTemplate->m_header);
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
    //it is an error to get the palette for a label-type volume
    if (!isMappedWithLabelTable() && getNumberOfMaps() >= 3) {
        if (getMapPaletteColorMapping(0)->getSelectedPaletteName() == Palette::SPECIAL_RGB_VOLUME_PALETTE_NAME) {
            return SubvolumeAttributes::RGB_WORKBENCH;
        }
    }
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
    m_nonZeroVoxelCoordinateBoundingBoxes.clear();
    VolumeBase::clear();
    
    m_volumeFileEditorDelegate->clear();
    m_lazyInitializedDynamicConnectivityFile.reset();

    m_writingDoScale = false;
    m_writingDType = NIFTI_TYPE_FLOAT32;
    m_minScalingVal = -1.0;//unused, but make them consistent
    m_maxScalingVal = 1.0;
    
    m_graphicsPrimitiveManager->clear();
    m_mapLabelClusterContainers.clear();
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
        updateAfterFileDataChanges();
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
    
    if ( ! hasGoodSpatialInformation()) {
        addFileReadWarning("No spatial information in file (in NIFTI header, both qform and sform are invalid).");
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
    
    /*
     * Put the child dynamic data-series file's palette in the file's metadata.
     */
    if (m_lazyInitializedDynamicConnectivityFile != NULL) {
        GiftiMetaData* fileMetaData = m_lazyInitializedDynamicConnectivityFile->getCiftiXML().getFileMetaData();
        CaretAssert(fileMetaData);
        if (m_lazyInitializedDynamicConnectivityFile->getNumberOfMaps() > 0) {
            fileMetaData->set(s_paletteColorMappingNameInMetaData,
                              m_lazyInitializedDynamicConnectivityFile->getMapPaletteColorMapping(0)->encodeInXML());
        }
        else {
            fileMetaData->remove(s_paletteColorMappingNameInMetaData);
        }
    }

    updateCaretExtension();
    
    NiftiHeader outHeader;//begin nifti-specific code
    if (m_header != NULL && (m_header->getType() == AbstractHeader::NIFTI))
    {
        outHeader = *((NiftiHeader*)m_header.getPointer());//also shallow copies extensions
    }
    outHeader.clearDataScaling();
    outHeader.setDescription(("Connectome Workbench, version " + ApplicationInformation().getVersion()).toLatin1().constData());//30 chars, plus however many chars the version is (generally 5)
    outHeader.setSForm(getVolumeSpace().getSform());
    outHeader.setDimensions(getOriginalDimensions());
    if (getType() == SubvolumeAttributes::LABEL)
    {
        switch (m_writingDType)
        {
            case NIFTI_TYPE_INT16://ensure integer when type is label
            case NIFTI_TYPE_INT32://int8 could be problematic for users, so disallow it?
            case NIFTI_TYPE_INT64:
            case NIFTI_TYPE_UINT16:
            case NIFTI_TYPE_UINT32:
            case NIFTI_TYPE_UINT64:
                outHeader.setDataType(m_writingDType);
                break;
            default:
                outHeader.setDataType(NIFTI_TYPE_INT32);
                break;
        }
    } else {
        if (m_writingDoScale)
        {
            outHeader.setDataTypeAndScaleRange(m_writingDType, m_minScalingVal, m_maxScalingVal);
        } else {
            outHeader.setDataType(m_writingDType);
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
    myIO.close();//call close explicitly to get a throw rather than a severe log when there is a problem
    m_header.grabNew(new NiftiHeader(outHeader));//update header to last written version, end nifti-specific code
    
    m_volumeFileEditorDelegate->clear();
    m_volumeFileEditorDelegate->updateIfVolumeFileChangedNumberOfMaps();
    clearModified();
}

void VolumeFile::setWritingDataTypeNoScaling(const int16_t& type)
{
    m_writingDType = type;//could do some validation here
    m_writingDoScale = false;
    m_minScalingVal = -1.0;
    m_maxScalingVal = 1.0;
}

void VolumeFile::setWritingDataTypeAndScaling(const int16_t& type, const double& minval, const double& maxval)
{
    m_writingDType = type;//could do some validation here
    m_writingDoScale = true;
    m_minScalingVal = minval;
    m_maxScalingVal = maxval;
}

bool VolumeFile::hasGoodSpatialInformation() const
{
    if (m_header != NULL)
    {
        return m_header->hasGoodSpatialInformation();
    }
    return true;
}

float
VolumeFile::interpolateValue(const float* coordIn,
                             const VoxelInterpolationTypeEnum::Enum interpType,
                             bool* validOut,
                             const int64_t brickIndex,
                             const int64_t component,
                             const float backgroundVal) const
{
    InterpType interp = CUBIC;
    switch (interpType) {
        case VoxelInterpolationTypeEnum::CUBIC:
            interp = CUBIC;
            break;
        case VoxelInterpolationTypeEnum::TRILINEAR:
            interp = TRILINEAR;
            break;
        case VoxelInterpolationTypeEnum::ENCLOSING_VOXEL:
            interp = ENCLOSING_VOXEL;
            break;
    }
    
    return interpolateValue(coordIn,
                            interp,
                            validOut,
                            brickIndex,
                            component,
                            backgroundVal);
}

float VolumeFile::interpolateValue(const float* coordIn, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component, const float backgroundVal) const
{
    return interpolateValue(coordIn[0], coordIn[1], coordIn[2], interp, validOut, brickIndex, component, backgroundVal);
}

float VolumeFile::interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component, const float backgroundVal) const
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
            int64_t ind1low = floor(indexSpace[0] + 0.01f);//allow some rounding error
            int64_t ind2low = floor(indexSpace[1] + 0.01f);//here these are ONLY used for checking validity
            int64_t ind3low = floor(indexSpace[2] + 0.01f);//need to do something different in trilinear
            int64_t ind1high = ceil(indexSpace[0] - 0.01f);
            int64_t ind2high = ceil(indexSpace[1] - 0.01f);
            int64_t ind3high = ceil(indexSpace[2] - 0.01f);
            if (!indexValid(ind1low, ind2low, ind3low, brickIndex, component) || !indexValid(ind1high, ind2high, ind3high, brickIndex, component))
            {
                if (validOut != NULL) *validOut = false;//check for valid coord before deconvolving the frame
                if (getType() == SubvolumeAttributes::LABEL)
                {
                    return getMapLabelTable(brickIndex)->getUnassignedLabelKey();
                } else {
                    return backgroundVal;
                }
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
            {
                int64_t ind1low = floor(index1 + 0.01f);//allow some rounding error for ONLY sanity checking
                int64_t ind2low = floor(index2 + 0.01f);//need to do something different in the math
                int64_t ind3low = floor(index3 + 0.01f);
                int64_t ind1high = ceil(index1 - 0.01f);
                int64_t ind2high = ceil(index2 - 0.01f);
                int64_t ind3high = ceil(index3 - 0.01f);
                if (!indexValid(ind1low, ind2low, ind3low, brickIndex, component) || !indexValid(ind1high, ind2high, ind3high, brickIndex, component))
                {
                    if (validOut != NULL) *validOut = false;
                    if (getType() == SubvolumeAttributes::LABEL)
                    {
                        return getMapLabelTable(brickIndex)->getUnassignedLabelKey();
                    } else {
                        return backgroundVal;
                    }
                }
            }
            const int64_t* dims = getDimensionsPtr();
            int64_t ind1low = min(max(int64_t(floor(index1)), int64_t(0)), dims[0] - 2);
            int64_t ind2low = min(max(int64_t(floor(index2)), int64_t(0)), dims[1] - 2);
            int64_t ind3low = min(max(int64_t(floor(index3)), int64_t(0)), dims[2] - 2);
            int64_t ind1high = ind1low + 1;
            int64_t ind2high = ind2low + 1;
            int64_t ind3high = ind3low + 1;
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
                if (getType() == SubvolumeAttributes::LABEL)
                {
                    return getMapLabelTable(brickIndex)->getUnassignedLabelKey();
                } else {
                    return backgroundVal;
                }
            }
        }
        break;
    }
    if (validOut != NULL) *validOut = false;//this shouldn't be reached unless the enum value is invalid
    if (getType() == SubvolumeAttributes::LABEL)
    {
        return getMapLabelTable(brickIndex)->getUnassignedLabelKey();
    } else {
        return backgroundVal;
    }
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
    m_nonZeroVoxelCoordinateBoundingBoxes.clear();
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
                m_caretVolExt.m_attributes[i]->m_palette->setScaleMode(PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE);
                if ((theType == SubvolumeAttributes::ANATOMY) && (numMaps == 1))
                {
                    m_caretVolExt.m_attributes[i]->m_palette->setSelectedPaletteName(Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME);
                } else {
                    m_caretVolExt.m_attributes[i]->m_palette->setSelectedPaletteName(Palette::ROY_BIG_BL_PALETTE_NAME);
                }
            }
        }
    }
    
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
        m_classNameHierarchy.grabNew(new GroupAndNameHierarchyModel(this));
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
    
    if (m_lazyInitializedDynamicConnectivityFile != NULL) {
        m_lazyInitializedDynamicConnectivityFile->clearModified();
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
    
    bool updateHistogramFlag = false;
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }
    if (m_brickAttributes[mapIndex].m_histogram == NULL)
    {
        m_brickAttributes[mapIndex].m_histogram.grabNew(new Histogram(numberOfBuckets));
        updateHistogramFlag = true;
    }
    else if (numberOfBuckets != m_brickAttributes[mapIndex].m_histogramNumberOfBuckets)
    {
        updateHistogramFlag = true;
    }
    
    if (updateHistogramFlag)
    {
        m_brickAttributes[mapIndex].m_histogram->update(numberOfBuckets, getFrame(mapIndex), dimensions[0] * dimensions[1] * dimensions[2]);
        m_brickAttributes[mapIndex].m_histogramNumberOfBuckets = numberOfBuckets;
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
    
    int32_t numberOfBuckets = 0;
    switch (getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            numberOfBuckets = getFileHistogramNumberOfBuckets();
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            numberOfBuckets = getMapPaletteColorMapping(mapIndex)->getHistogramNumberOfBuckets();
            break;
    }
    if (m_brickAttributes[mapIndex].m_histogramLimitedValues == NULL)
    {
        m_brickAttributes[mapIndex].m_histogramLimitedValues.grabNew(new Histogram(100));
        updateHistogramFlag = true;
    }
    else if ((numberOfBuckets != m_brickAttributes[mapIndex].m_histogramLimitedValuesNumberOfBuckets)
             || (mostPositiveValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesMostPositiveValueInclusive)
             || (leastPositiveValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastPositiveValueInclusive)
             || (leastNegativeValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesLeastNegativeValueInclusive)
             || (mostNegativeValueInclusive != m_brickAttributes[mapIndex].m_histogramLimitedValuesMostNegativeValueInclusive)
             || (includeZeroValues != m_brickAttributes[mapIndex].m_histogramLimitedValuesIncludeZeroValues)) {
        updateHistogramFlag = true;
    }
    
    if (updateHistogramFlag) {
        m_brickAttributes[mapIndex].m_histogramLimitedValues->update(numberOfBuckets,
                                                                     getFrame(mapIndex),
                                                                     dimensions[0] * dimensions[1] * dimensions[2],
                                                                     mostPositiveValueInclusive,
                                                                     leastPositiveValueInclusive,
                                                                     leastNegativeValueInclusive,
                                                                     mostNegativeValueInclusive,
                                                                     includeZeroValues);
        m_brickAttributes[mapIndex].m_histogramLimitedValuesNumberOfBuckets = numberOfBuckets;
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
    const int32_t numBuckets = getFileHistogramNumberOfBuckets();
    bool updateHistogramFlag = false;
    if (m_fileHistogram != NULL) {
        if (numBuckets != m_fileHistogramNumberOfBuckets) {
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
            if (m_fileHistogram == NULL) {
                m_fileHistogram.grabNew(new Histogram(numBuckets));
            }
            m_fileHistogram->update(numBuckets,
                                    &fileData[0],
                                    fileData.size());
            m_fileHistogramNumberOfBuckets = numBuckets;
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
    const int32_t numberOfBuckets = getFileHistogramNumberOfBuckets();
    bool updateHistogramFlag = false;
    if (m_fileHistorgramLimitedValues != NULL) {
        if ((numberOfBuckets != m_fileHistogramLimitedValuesNumberOfBuckets)
            || (mostPositiveValueInclusive != m_fileHistogramLimitedValuesMostPositiveValueInclusive)
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
            m_fileHistorgramLimitedValues->update(numberOfBuckets,
                                                  &fileData[0],
                                                  fileData.size(),
                                                  mostPositiveValueInclusive,
                                                  leastPositiveValueInclusive,
                                                  leastNegativeValueInclusive,
                                                  mostNegativeValueInclusive,
                                                  includeZeroValues);
            
            m_fileHistogramLimitedValuesNumberOfBuckets = numberOfBuckets;
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
        case SubvolumeAttributes::RGB_WORKBENCH:
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
    
    if (getDataFileType() == DataFileTypeEnum::VOLUME) {
        modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
        modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA);
    }
    else if (getDataFileType() == DataFileTypeEnum::VOLUME_DYNAMIC) {
        modesSupportedOut.push_back(PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA);
    }
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
 * @return The clusters for the given map's label table (may be NULL)
 * @param mapIndex
 *    Index of the map
 */
const ClusterContainer*
VolumeFile::getMapLabelTableClusters(const int32_t mapIndex) const
{
    if (isMappedWithLabelTable()) {
        /*
         * If it does not exist, no attempt has been made to create it
         */
        if (m_mapLabelClusterContainers.find(mapIndex) == m_mapLabelClusterContainers.end()) {
            CaretMappableDataFileClusterFinder finder(CaretMappableDataFileClusterFinder::FindMode::VOLUME_LABEL,
                                                      this,
                                                      mapIndex);
            const auto result(finder.findClusters());
            if (result->isSuccess()) {
                m_mapLabelClusterContainers[mapIndex] = std::unique_ptr<ClusterContainer>(finder.takeClusterContainer());
            }
            else {
                CaretLogWarning(result->getErrorDescription());
                ClusterContainer* nullPointer(NULL);
                /*
                 * Putting a NULL in here, prevents running find clusters again
                 */
                m_mapLabelClusterContainers[mapIndex] = std::unique_ptr<ClusterContainer>(nullPointer);
                CaretAssertToDoFatal();
            }
        }

        return m_mapLabelClusterContainers[mapIndex].get();
    }
    
    return NULL;
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
        case SubvolumeAttributes::RGB_WORKBENCH:
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
 * (static method) Given dimensions, origin, and spacing, find the edges of the voxels
 * @param dimensions
 *    Dimensions of volume
 * @param origin
 *    Origin of volume
 * @param spacing
 *    Spacing of volume
 * @param firstVoxelEdgeOut
 *    Output with edge of first voxel in volume
 * @param lastVoxelEdgeOut
 *    Output with edge of last voxel in volume
 */
void
VolumeFile::dimensionOriginSpacingXyzToVoxelEdges(const Vector3D& dimensions,
                                                  const Vector3D& origin,
                                                  const Vector3D& spacing,
                                                  Vector3D& firstVoxelEdgeOut,
                                                  Vector3D& lastVoxelEdgeOut)
{
    const Vector3D halfSpacing(spacing / 2.0);
    firstVoxelEdgeOut = (origin
                         - halfSpacing);
    lastVoxelEdgeOut = (firstVoxelEdgeOut
                        + Vector3D(spacing[0] * dimensions[0],
                                   spacing[1] * dimensions[1],
                                   spacing[2] * dimensions[2]));
}

/**
 * Get a bounding box containing the non-zero voxel coordinate ranges
 * @param mapIndex
 *    Index of map
 * @param boundingBoxOut
 *    Output containing coordinate range of non-zero voxels
 */
void
VolumeFile::getNonZeroVoxelCoordinateBoundingBox(const int32_t mapIndex,
                                           BoundingBox& boundingBoxOut) const
{
    if (static_cast<int32_t>(m_nonZeroVoxelCoordinateBoundingBoxes.size()) <= mapIndex) {
        m_nonZeroVoxelCoordinateBoundingBoxes.resize(mapIndex + 1);
    }
    CaretAssertVectorIndex(m_nonZeroVoxelCoordinateBoundingBoxes, mapIndex);
    
    /*
     * If pointer is valid, then the bounding box is valid
     * and does not need to be updated.
     */
    if (m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex]) {
        boundingBoxOut = *m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex];
        return;
    }
    
    m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex].reset(new BoundingBox());
    m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex]->resetForUpdate();
    
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    const int64_t BIG_DIMS(400 * 400 * 400);
    const int64_t volDim3(dimI * dimJ * dimK);
    if (volDim3 > BIG_DIMS) {
        /*
         * Computation below can be very slow for LARGE
         * VOLUMES.  Until that code can be improved
         * just use coordinate bounds for entire volume.
         * In almost every instance the non-zero volume
         * data fills the volume.
         */
        getVoxelSpaceBoundingBox(boundingBoxOut);
        return;
    }
    
    CaretAssert((mapIndex >= 0) && (mapIndex < getNumberOfMaps()));
    
    /*
     * Note: Adding "collapse(4)" was faster but results were
     * incorrect.  Adding "collapse(3) was not faster than
     * without it but results were correct.
     *
     * For a 1000x1000x1000 volume using OpenMP reduces
     * computation time from 915 second to 247 seconds
     * on a MacBook Air with M1 processor.
     *
     * Optimization suggestion for a 'plumb' volume.
     * Start at each face of volume
     * and move one slice at a time until a non-zero voxel is
     * encountered.  In other words, start with axial slice 0,
     * then slice 1, etc.  Then start with last axial slice,
     * next to last axial slice, etc. and same for other axis.
     */
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t i = 0; i < dimI; i++) {
        for (int32_t j = 0; j < dimJ; j++) {
            for (int32_t k = 0; k < dimK; k++) {
                for (int32_t m = 0; m < dimComp; m++) {
                    if (getValue(i, j, k, mapIndex, m) != 0.0) {
                        float xyz[3];
                        indexToSpace(i, j, k, xyz);
#pragma omp critical
                        {
                            m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex]->update(xyz);
                        }
                        break;
                    }
                }
            }
        }
    }
    boundingBoxOut = *m_nonZeroVoxelCoordinateBoundingBoxes[mapIndex];
}

/**
 * @return Instance cast to a Volume Mappable CaretMappableDataFile
 */
CaretMappableDataFile*
VolumeFile::castToVolumeMappableDataFile()
{
    return this;
}

/**
 * @return Instance cast to a Volume Mappable CaretMappableDataFile (const method)
 */
const CaretMappableDataFile*
VolumeFile::castToVolumeMappableDataFile() const
{
    return this;
}

/**
 * Update coloring for a map.
 * Does nothing if coloring is not enabled.
 *
 * @param mapIndex
 *     Index of map.
 */
void
VolumeFile::updateScalarColoringForMap(const int32_t mapIndex)
{
    if (s_voxelColoringEnabled == false) {
        return;
    }
    
    CaretAssertVectorIndex(m_caretVolExt.m_attributes, mapIndex);
    CaretAssert(m_voxelColorizer);

    m_voxelColorizer->assignVoxelColorsForMap(mapIndex);

    m_graphicsPrimitiveManager->invalidateColoringForMap(mapIndex);
    
    invalidateHistogramChartColoring();
}

/**
 * Get the voxel RGBA coloring for a map.
 * Does nothing if coloring is not enabled and output colors are undefined
 * in this case.
 *
 * @param mapIndex
 *    Index of the map.
 * @param slicePlane
 *    Plane for which colors are requested.
 * @param sliceIndex
 *    Index of the slice.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *    Contains colors upon exit.
 * @return
 *    Number of voxels with alpha greater than zero
 */
int64_t
VolumeFile::getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                        const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                        const int64_t sliceIndex,
                                        const TabDrawingInfo& tabDrawingInfo,
                                        uint8_t* rgbaOut) const
{
    if (s_voxelColoringEnabled == false) {
        return 0;
    }
    
    CaretAssert(m_voxelColorizer);
    
    return m_voxelColorizer->getVoxelColorsForSliceInMap(mapIndex,
                                                  slicePlane,
                                                  sliceIndex,
                                                  tabDrawingInfo,
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
 * @param tabDrawingInfo
 *    Info for drawing the tab
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
                                        const TabDrawingInfo& tabDrawingInfo,
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
                                                 tabDrawingInfo,
                                                 rgbaOut);
}

/**
  * Get the voxel colors for a sub slice in the map.
  *
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
 * @param tabDrawingInfo
 *    Info for drawing the tab
  * @param rgbaOut
  *    Output containing the rgba values (must have been allocated
  *    by caller to sufficient count of elements in the slice).
 * @return
 *    Number of voxels with alpha greater than zero
  */
int64_t
VolumeFile::getVoxelColorsForSubSliceInMap(const int32_t mapIndex,
                                           const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                           const int64_t sliceIndex,
                                           const int64_t firstCornerVoxelIndex[3],
                                           const int64_t lastCornerVoxelIndex[3],
                                           const int64_t voxelCountIJK[3],
                                           const TabDrawingInfo& tabDrawingInfo,
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
                                                     tabDrawingInfo,
                                                     rgbaOut);
}

/**
 * Get the graphics primitive for drawing this volume using a graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
VolumeFile::getVolumeDrawingTriangleStripPrimitive(const int32_t mapIndex,
                                                   const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLE_STRIP,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Get the graphics primitive for drawing this volume using a FAN graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
VolumeFile::getVolumeDrawingTriangleFanPrimitive(const int32_t mapIndex,
                                                 const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLE_FAN,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Get the graphics primitive for drawing this volume using a TRIANGLES graphics primitive
 *
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @return
 *    Graphics primitive or NULL if unable to draw
 */
GraphicsPrimitiveV3fT3f*
VolumeFile::getVolumeDrawingTrianglesPrimitive(const int32_t mapIndex,
                                               const TabDrawingInfo& tabDrawingInfo) const
{
    return m_graphicsPrimitiveManager->getVolumeDrawingPrimitiveForMap(VolumeGraphicsPrimitiveManager::PrimitiveShape::TRIANGLES,
                                                                       mapIndex,
                                                                       tabDrawingInfo);
}

/**
 * Create a graphics primitive for showing part of volume that intersects with an image from histology
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param mediaFile
 *    The medial file for drawing histology
 * @param volumeMappingMode
 *    The volume to image mapping mode
 * @param volumeSliceThickness
 *    The volume slice thickness for mapping volume to image
 * @param errorMessageOut
 *    Ouput with error message
 * @return
 *    Primitive for drawing intersection or NULL if failure
 */
GraphicsPrimitive*
VolumeFile::getHistologyImageIntersectionPrimitive(const int32_t mapIndex,
                                                   const TabDrawingInfo& tabDrawingInfo,
                                                   const MediaFile* mediaFile,
                                                   const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                   const float volumeSliceThickness,
                                                   AString& errorMessageOut) const
{
    return m_graphicsPrimitiveManager->getImageIntersectionDrawingPrimitiveForMap(mediaFile,
                                                                                  mapIndex,
                                                                                  tabDrawingInfo,
                                                                                  volumeMappingMode,
                                                                                  volumeSliceThickness,
                                                                                  errorMessageOut);
}

/**
 * Create a graphics primitive for showing part of volume that intersects with an image from histology
 * @param mapIndex
 *    Index of the map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param histologySlice
 *    The histology slice being drawn
 * @param errorMessageOut
 *    Ouput with error message
 * @return
 *    Primitive for drawing intersection or NULL if failure
 */
std::vector<GraphicsPrimitive*>
VolumeFile::getHistologySliceIntersectionPrimitive(const int32_t mapIndex,
                                                   const TabDrawingInfo& tabDrawingInfo,
                                                   const HistologySlice* histologySlice,
                                                   const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                   const float volumeSliceThickness,
                                                   AString& errorMessageOut) const
{
    return m_graphicsPrimitiveManager->getImageIntersectionDrawingPrimitiveForMap(histologySlice,
                                                                                  mapIndex,
                                                                                  tabDrawingInfo,
                                                                                  volumeMappingMode,
                                                                                  volumeSliceThickness,
                                                                                  errorMessageOut);
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
 * Get the RGBA color components for voxel in a map.
 * Does nothing if coloring is not enabled and output colors are undefined
 * in this case.
 *
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param rgbaOut
 *    Contains voxel coloring on exit.
 */
void
VolumeFile::getVoxelColorInMap(const int64_t i,
                               const int64_t j,
                               const int64_t k,
                               const int64_t mapIndex,
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
                                         rgbaOut);
}

/**
 * Get the RGBA color components for voxel in map with display group and tab.
 * Does nothing if coloring is not enabled and output colors are undefined
 * in this case.
 *
 * @param i
 *    Parasaggital index
 * @param j
 *    Coronal index
 * @param k
 *    Axial index
 * @param mapIndex
 *    Index of map.
 * @param tabDrawingInfo
 *    Info for drawing the tab
 * @param rgbaOut
 *    Contains voxel coloring on exit.
 */
void
VolumeFile::getVoxelColorInMap(const int64_t i,
                               const int64_t j,
                               const int64_t k,
                               const int64_t mapIndex,
                               const TabDrawingInfo& tabDrawingInfo,
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
                                         tabDrawingInfo,
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
    
    m_graphicsPrimitiveManager->invalidateColoringForMap(mapIndex);
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
    if (m_lazyInitializedDynamicConnectivityFile != NULL) {
        sceneClass->addClass(m_lazyInitializedDynamicConnectivityFile->saveToScene(sceneAttributes,
                                                                                   "m_lazyInitializedDynamicConnectivityFile"));
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

    const SceneClass* dynamicFileSceneClass = sceneClass->getClass("m_lazyInitializedDynamicConnectivityFile");
    if (dynamicFileSceneClass != NULL) {
        VolumeDynamicConnectivityFile* denseDynamicFile = getVolumeDynamicConnectivityFile();
        denseDynamicFile->restoreFromScene(sceneAttributes,
                                           dynamicFileSceneClass);
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
    
    if (dims.size() >= 3) {
        const int64_t maxI((dims[0] > 1) ? dims[0] - 1 : 0);
        const int64_t maxJ((dims[1] > 1) ? dims[1] - 1 : 0);
        const int64_t maxK((dims[2] > 1) ? dims[2] - 1 : 0);
        int64_t corners[8][3] = {
            {    0,    0,    0 },
            { maxI,    0,    0 },
            { maxI, maxJ,    0 },
            {    0, maxJ,    0},
            {    0,    0, maxK },
            { maxI,    0, maxK },
            { maxI, maxJ, maxK },
            {    0, maxJ, maxK}
        };
        for (int32_t m = 0; m < 8; m++) {
            const int64_t i(corners[m][0]);
            const int64_t j(corners[m][1]);
            const int64_t k(corners[m][2]);
            if (indexValid(i, j, k)) {
                float x, y, z;
                indexToSpace(i, j, k, x, y, z);
                dataFileInformation.addNameAndValue("IJK = ("
                                                    + AString::number(i)
                                                    + ","
                                                    + AString::number(j)
                                                    + ","
                                                    + AString::number(k)
                                                    + ")",
                                                    ("XYZ = ("
                                                     + AString::number(x)
                                                     + ", "
                                                     + AString::number(y)
                                                     + ", "
                                                     + AString::number(z)
                                                     + ")"));
            }
        }
    }
    
    const std::vector<std::vector<float>>& sform = getVolumeSpace().getSform();
    QString sformName("sform");
    for (const auto& row : sform) {
        AString s;
        for (const auto element : row) {
            s.append(AString::number(element, 'f', 6) + " ");
        }
        dataFileInformation.addNameAndValue(sformName, s);
        sformName.clear();
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
    
    if (isMappedWithLabelTable()) {
        for (int32_t i = 0; i < getNumberOfMaps(); i++) {
            CaretMappableDataFileClusterFinder finder(CaretMappableDataFileClusterFinder::FindMode::VOLUME_LABEL,
                                                      this,
                                                      i);
            const auto result(finder.findClusters());
            if (result->isSuccess()) {
                const AString mapName(getMapName(i).isEmpty()
                                      ? AString::number(i + 1)
                                      : getMapName(i));
                dataFileInformation.addText("Clusters for map: " + mapName + "\n");
                dataFileInformation.addText(finder.getClustersInFormattedString());                
            }
            else {
                CaretLogWarning("Finding clusters error: "
                                + result->getErrorDescription());
            }
        }
    }
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
VolumeFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
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

/**
 * Get data from the file as requested in the given map file data selector.
 *
 * @param mapFileDataSelector
 *     Specifies selection of data.
 * @param dataOut
 *     Output with data.  Will be empty if data does not support the map file data selector.
 */
void
VolumeFile::getDataForSelector(const MapFileDataSelector& mapFileDataSelector,
                               std::vector<float>& dataOut) const
{
    dataOut.clear();
    
    switch (mapFileDataSelector.getDataSelectionType()) {
        case MapFileDataSelector::DataSelectionType::INVALID:
            break;
        case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
            break;
        case MapFileDataSelector::DataSelectionType::ROW_DATA:
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
            break;
        case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
        {
             if (isMappedWithPalette()) {
                 float xyz[3];
                 mapFileDataSelector.getVolumeVoxelXYZ(xyz);
                 
                int64_t ijk[3];
                enclosingVoxel(xyz,
                               ijk);
                
                if (indexValid(ijk)) {
                    const int32_t numMaps = getNumberOfMaps();
                    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                        dataOut.push_back(getValue(ijk, iMap));
                    }
                }
            }
        }
            break;
    }
}

/**
 * Are all brainordinates in this file also in the given file?
 * That is, the brainordinates are equal to or a subset of the brainordinates
 * in the given file.
 *
 * @param mapFile
 *     The given map file.
 * @return
 *     Match status.
 */
CaretMappableDataFile::BrainordinateMappingMatch
VolumeFile::getBrainordinateMappingMatch(const CaretMappableDataFile* mapFile) const
{
    CaretAssert(mapFile);
    if (mapFile->getDataFileType() == DataFileTypeEnum::VOLUME) {
        const VolumeFile* otherVolumeFile = dynamic_cast<const VolumeFile*>(mapFile);
        CaretAssert(otherVolumeFile);
        
        std::vector<int64_t> myDims, otherDims;
        getDimensions(myDims);
        otherVolumeFile->getDimensions(otherDims);
        
        for (int32_t i = 0; i < 3; i++) {
            CaretAssertVectorIndex(myDims, i);
            CaretAssertVectorIndex(otherDims, i);
            if (myDims[i] != otherDims[i]) {
                return BrainordinateMappingMatch::NO;
            }
        }
        
        return BrainordinateMappingMatch::EQUAL;
    }
    
    return BrainordinateMappingMatch::NO;
}

/**
 * Get the identification information for a surface node in the given maps.
 *
 * @param mapIndices
 *    Indices of maps for which identification information is requested.
 * @param xyz
 *     Coordinate of voxel.
 * @param dataValueSeparator
 *    Separator between multiple data values
 * @param ijkOut
 *     Voxel indices of value.
 * @param textOut
 *    Output containing identification information.
 */
bool
VolumeFile::getVolumeVoxelIdentificationForMaps(const std::vector<int32_t>& mapIndices,
                                                const float xyz[3],
                                                const AString& dataValueSeparator,
                                                const int32_t digitsRightOfDecimal,
                                                int64_t ijkOut[3],
                                                AString& textOut) const
{
    float floatIJK[3];
    spaceToIndex(xyz, floatIJK);
    
    ijkOut[0] = floatIJK[0];
    ijkOut[1] = floatIJK[1];
    ijkOut[2] = floatIJK[2];
    
    bool anyValidFlag = false;
    AString valuesText;
    for (const auto mapIndex : mapIndices) {
        if ( ! valuesText.isEmpty()) {
            valuesText.append(dataValueSeparator);
        }
        bool validFlag(false);
        const float value = getVoxelValue(xyz,
                                          &validFlag,
                                          mapIndex);
        if (validFlag) {
            anyValidFlag = true;
            if (isMappedWithLabelTable()) {
                const GiftiLabelTable* labelTable = getMapLabelTable(mapIndex);
                CaretAssert(labelTable);
                const int32_t key = static_cast<int32_t>(value);
                const GiftiLabel* label = labelTable->getLabel(key);
                if (label != NULL) {
                    valuesText.append(label->getName());
                }
                else {
                    valuesText.append("?");
                }
                valuesText.append(" ("
                                  + getMapName(mapIndex)
                                  + ")");
            }
            else {
                valuesText.append(AString::number(value, 'f', digitsRightOfDecimal));
            }
        }
        else {
            valuesText.append("invalid");
        }
    }
    
    if (anyValidFlag) {
        textOut = valuesText;
        return true;
    }
    
    return false;
}


/**
 * @return The units for the 'interval' between two consecutive maps.
 */
NiftiTimeUnitsEnum::Enum
VolumeFile::getMapIntervalUnits() const
{
    NiftiTimeUnitsEnum::Enum units = NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
    
    if (m_header != NULL && m_header->getType() == AbstractHeader::NIFTI) {
        const NiftiHeader& myHeader = *((NiftiHeader*)m_header.getPointer());
        
        std::vector<int64_t> dims;
        getDimensions(dims);
        if (dims.size() >= 4) {
            if (dims[3] > 1) {
                /*
                 * Timestep from NiftiHeader is always seconds
                 */
                const float timeStep = myHeader.getTimeStep();
                if (timeStep > 0.0) {
                    units = NiftiTimeUnitsEnum::NIFTI_UNITS_SEC;
                }
            }
        }
    }

    return units;
}

/**
 * Get the units value for the first map and the
 * quantity of units between consecutive maps.  If the
 * units for the maps is unknown, value of one (1) are
 * returned for both output values.
 *
 * @param firstMapUnitsValueOut
 *     Output containing units value for first map.
 * @param mapIntervalStepValueOut
 *     Output containing number of units between consecutive maps.
 */
void
VolumeFile::getMapIntervalStartAndStep(float& firstMapUnitsValueOut,
                                       float& mapIntervalStepValueOut) const
{
    firstMapUnitsValueOut   = 0.0;
    mapIntervalStepValueOut = 1.0;
    
    if (m_header != NULL && m_header->getType() == AbstractHeader::NIFTI) {
        const NiftiHeader& myHeader = *((NiftiHeader*)m_header.getPointer());
        
        std::vector<int64_t> dims;
        getDimensions(dims);
        if (dims.size() >= 4) {
            if (dims[3] > 1) {
                /*
                 * Timestep from NiftiHeader is always seconds
                 */
                const float timeStep = myHeader.getTimeStep();
                if (timeStep > 0.0) {
                    mapIntervalStepValueOut = timeStep;
                }
            }
        }
    }
}

/**
 * @return The volume dynamic connectivity file for a data-series (functional) file
 *         that contains at least two time points.  Note that some files may
 *         have type anatomy but still contain functional data.
 *         Will return NULL for other types.
 */
const VolumeDynamicConnectivityFile*
VolumeFile::getVolumeDynamicConnectivityFile() const
{
    VolumeFile* nonConstThis = const_cast<VolumeFile*>(this);
    return nonConstThis->getVolumeDynamicConnectivityFile();
}

/**
 * @return The volume dynamic connectivity file for a data-series (functional) file
 *         that contains at least two time points.  Note that some files may
 *         have type anatomy but still contain functional data.
 *         Will return NULL for other types.
 */
VolumeDynamicConnectivityFile*
VolumeFile::getVolumeDynamicConnectivityFile()
{
    if (m_lazyInitializedDynamicConnectivityFile == NULL) {
        if ((getType() == SubvolumeAttributes::ANATOMY)
            || (getType() == SubvolumeAttributes::FUNCTIONAL)) {
            std::vector<int64_t> dims;
            getDimensions(dims);
            if (dims.size() >= 4) {
                const int64_t minimumNumberOfTimePoints(8);
                if (dims[3] > minimumNumberOfTimePoints) {
                    m_lazyInitializedDynamicConnectivityFile.reset(new VolumeDynamicConnectivityFile(this));
                    
                    m_lazyInitializedDynamicConnectivityFile->initializeFile();
                    
                    /*
                     * Palette for dynamic file is in file metadata
                     */
                    GiftiMetaData* fileMetaData = getFileMetaData();
                    const AString encodedPaletteColorMappingString = fileMetaData->get(s_paletteColorMappingNameInMetaData);
                    if ( ! encodedPaletteColorMappingString.isEmpty()) {
                        if (m_lazyInitializedDynamicConnectivityFile->getNumberOfMaps() > 0) {
                            PaletteColorMapping* pcm = m_lazyInitializedDynamicConnectivityFile->getMapPaletteColorMapping(0);
                            CaretAssert(pcm);
                            pcm->decodeFromStringXML(encodedPaletteColorMappingString);
                        }
                    }
                    
                    m_lazyInitializedDynamicConnectivityFile->clearModified();
                }
            }
        }
    }
    
    return m_lazyInitializedDynamicConnectivityFile.get();
}

/**
 * @return True if any of the maps in this file contain a
 * color mapping that possesses a modified status.
 */
bool
VolumeFile::isModifiedPaletteColorMapping() const
{
    /*
     * This method is override because we need to know if the
     * encapsulated dynamic dense file has a modified palette.
     * When restoring a scene, a file with any type of modification
     * must be reloaded to remove any modifications.  Note that
     * when a scene is restored, files that are not modified and
     * are in the new scene are NOT reloaded to save time.
     */
    if (CaretMappableDataFile::isModifiedPaletteColorMapping()) {
        return true;
    }
    
    if (m_lazyInitializedDynamicConnectivityFile != NULL) {
        if (m_lazyInitializedDynamicConnectivityFile->isModifiedPaletteColorMapping()) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return The modified status for aall palettes in this file.
 * Note that 'modified' overrides any 'modified by show scene'.
 */
PaletteModifiedStatusEnum::Enum
VolumeFile::getPaletteColorMappingModifiedStatus() const
{
    const std::array<PaletteModifiedStatusEnum::Enum, 2> fileModStatus = { {
        CaretMappableDataFile::getPaletteColorMappingModifiedStatus(),
        ((m_lazyInitializedDynamicConnectivityFile != NULL)
         ? m_lazyInitializedDynamicConnectivityFile->getPaletteColorMappingModifiedStatus()
         : PaletteModifiedStatusEnum::UNMODIFIED)
    } };
    
    PaletteModifiedStatusEnum::Enum modStatus = PaletteModifiedStatusEnum::UNMODIFIED;
    for (auto status : fileModStatus) {
        switch (status) {
            case PaletteModifiedStatusEnum::MODIFIED:
                modStatus = PaletteModifiedStatusEnum::MODIFIED;
                break;
            case PaletteModifiedStatusEnum::MODIFIED_BY_SHOW_SCENE:
                modStatus = PaletteModifiedStatusEnum::MODIFIED_BY_SHOW_SCENE;
                break;
            case PaletteModifiedStatusEnum::UNMODIFIED:
                break;
        }
        
        if (modStatus == PaletteModifiedStatusEnum::MODIFIED) {
            /*
             * 'MODIFIED' overrides 'MODIFIED_BY_SHOW_SCENE'
             * so no need to continue loop
             */
            break;
        }
    }
    
    return modStatus;
}

/**
 * Called when a group and name hierarchy item has attribute/status changed
 */
void
VolumeFile::groupAndNameHierarchyItemStatusChanged()
{
    m_graphicsPrimitiveManager->invalidateAllColoring();
}

/**
 * Set the values for the given voxels in the given map to the given value.
 * This is used when the user is editing voxels and may  be more efficient
 * than calling setValue() for each voxel.
 * @param mapIndex
 *    Index of the map
 * @param voxelsIJK
 *    IJK indices of the voxels
 * @param value
 *    New data value for the voxels
 */
void
VolumeFile::setValuesForVoxelEditing(const int32_t mapIndex,
                                     const std::vector<VoxelIJK>& voxelsIJK,
                                     const float value)
{
    if ((mapIndex >= 0)
        && (mapIndex < getNumberOfMaps())) {
        /*
         * Set the voxels
         */
        for (const auto& ijk : voxelsIJK) {
            setValue(value,
                     ijk.m_ijk,
                     mapIndex);
        }
        
        bool updateAllColoringFlag(true);
        
        if (isMappedWithLabelTable()) {
            /*
             * Update coloring for voxels with color from label
             */
            const GiftiLabelTable* labelTable(getMapLabelTable(mapIndex));
            CaretAssert(labelTable);
            const int32_t labelIndex(static_cast<int32_t>(value));
            const GiftiLabel* label(labelTable->getLabel(labelIndex));
            if (label != NULL) {
                float rgbaFloat[4];
                label->getColor(rgbaFloat);
                
                std::array<uint8_t, 4> rgba {
                    static_cast<uint8_t>(rgbaFloat[0] * 255.0),
                    static_cast<uint8_t>(rgbaFloat[1] * 255.0),
                    static_cast<uint8_t>(rgbaFloat[2] * 255.0),
                    static_cast<uint8_t>(rgbaFloat[3] * 255.0)
                };
                
                VoxelColorUpdate voxelColorUpdate;
                voxelColorUpdate.setMapIndex(mapIndex);
                voxelColorUpdate.setRGBA(rgba);
                voxelColorUpdate.addVoxels(voxelsIJK);
                
                if (m_voxelColorizer) {
                    m_voxelColorizer->updateVoxelColorsInMap(voxelColorUpdate);
                    updateAllColoringFlag = false;
                }
                
                if (m_graphicsPrimitiveManager) {
                    m_graphicsPrimitiveManager->updateVoxelColorsInMapTexture(voxelColorUpdate);
                }
            }
        }
        
        if (updateAllColoringFlag) {
            /*
             * Still need to update all coloring since changing
             * values may affect palette parameters
             */
            if (m_voxelColorizer) {
                m_voxelColorizer->invalidateColoring();
            }
            
            if (m_graphicsPrimitiveManager) {
                /*
                 * Need to invalidate the GraphicsPrimitive so that the
                 * texture gets reloaded with the new RGBA coloring.
                 */
                m_graphicsPrimitiveManager->invalidateColoringForMap(mapIndex);
            }
        }
    }
}

/**
 * Get the 26 connected neighbors for a voxel.l
 * @param voxelIJK
 *    Indices of voxel
 *  @param voxelValues
 *    Pointer to first value in the slice (must contain I * J * K values) also known as 'frame'
 * @param minValue
 *    Minimum data value for neighbor (inclusive)
 * @param maxValue
 *    Maximum data value for neighbor (inclusive)
 * @param voxelHasBeenSearchedFlags
 *    A vector that indicates a voxel has been searched and is not added to the neighbors.
 *    These flags are updated for any voxel that is added to the neighbors.
 * @param neighborIJKs
 *    Voxels that are neighbors with a value in the range and have not been searched
 *    are APPENDED to this vector (any values in thie vector on entry remain in the
 *    vector on exit)..
 */
void
VolumeFile::getNeigbors26(const VoxelIJK& voxelIJK,
                          const float* voxelValues,
                          const float minimumValue,
                          const float maximumValue,
                          std::vector<char>& voxelHasBeenSearchedFlags,
                          std::vector<VoxelIJK>& neighborIJKs) const
{
    const VolumeSpace vs(getVolumeSpace());
    CaretUsedInDebugCompileOnly(const int64_t* dims(vs.getDims()));
    CaretAssert(static_cast<int64_t>(voxelHasBeenSearchedFlags.size())
                == (dims[0] * dims[1] * dims[2]));
    
    CaretAssert((voxelIJK.m_ijk[0] >= 0) && (voxelIJK.m_ijk[0] < dims[0]));
    CaretAssert((voxelIJK.m_ijk[1] >= 0) && (voxelIJK.m_ijk[1] < dims[1]));
    CaretAssert((voxelIJK.m_ijk[2] >= 0) && (voxelIJK.m_ijk[2] < dims[2]));

    for (int64_t i = -1; i <= 1; i++) {
        for (int64_t j = -1; j <= 1; j++) {
            for (int64_t k = -1; k <= 1; k++) {
                if ((i != 0) || (j != 0) || (k != 0)) { /* ignore self */
                    const int64_t vi(voxelIJK.m_ijk[0] + i);
                    const int64_t vj(voxelIJK.m_ijk[1] + j);
                    const int64_t vk(voxelIJK.m_ijk[2] + k);
                    if (vs.indexValid(vi, vj, vk)) {
                        const int64_t offset(vs.getIndex(vi, vj, vk));
                        CaretAssertVectorIndex(voxelHasBeenSearchedFlags, offset);
                        if ( ! voxelHasBeenSearchedFlags[offset]) {
                            const float v(voxelValues[offset]);
                            if ((v >= minimumValue)
                                && (v <= maximumValue)) {
                                CaretAssert((vi >= 0) && (vi < dims[0]));
                                CaretAssert((vj >= 0) && (vj < dims[1]));
                                CaretAssert((vk >= 0) && (vk < dims[2]));
                                neighborIJKs.emplace_back(vi, vj, vk);
                                voxelHasBeenSearchedFlags[offset] = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

