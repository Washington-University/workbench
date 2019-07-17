
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __VOLUME_DYNN_CONN_FILE_DECLARE__
#include "VolumeDynamicConnectivityFile.h"
#undef __VOLUME_DYNN_CONN_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ConnectivityCorrelation.h"
#include "ConnectivityDataLoaded.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::VolumeDynamicConnectivityFile 
 * \brief Dynamic connectivity volume file
 * \ingroup Files
 */

/**
 * Constructor.
 */
VolumeDynamicConnectivityFile::VolumeDynamicConnectivityFile(const VolumeFile* parentVolumeFile)
: VolumeFile(DataFileTypeEnum::VOLUME_DYNAMIC),
m_parentVolumeFile(parentVolumeFile)
{
    CaretAssert(m_parentVolumeFile);
    
    m_connectivityDataLoaded.reset(new ConnectivityDataLoaded());

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_dataLoadingEnabledFlag",
                          &m_dataLoadingEnabledFlag);
    m_sceneAssistant->add("m_enabledAsLayer",
                          &m_enabledAsLayer);
    m_sceneAssistant->add("m_connectivityDataLoaded",
                          "ConnectivityDataLoaded",
                          m_connectivityDataLoaded.get());
}

/**
 * Destructor.
 */
VolumeDynamicConnectivityFile::~VolumeDynamicConnectivityFile()
{
}

/**
 * Clear the file.
 */
void
VolumeDynamicConnectivityFile::clear()
{
    VolumeFile::clear();
    clearPrivateData();
}

/**
 * Clear the file.
 */
void
VolumeDynamicConnectivityFile::clearPrivateData()
{
    m_voxelData = NULL;
    m_numberOfVoxels = 0;
    m_validDataFlag = false;
    m_enabledAsLayer = false;
    m_connectivityCorrelation.reset();
    m_connectivityDataLoaded->reset();
}

/**
 * @return Pointer to the information about last loaded connectivity data.
 */
const ConnectivityDataLoaded*
VolumeDynamicConnectivityFile::getConnectivityDataLoaded() const
{
    return m_connectivityDataLoaded.get();
}

/**
 * @return True if enabled as a layer.
 */
bool
VolumeDynamicConnectivityFile::isEnabledAsLayer() const
{
    return m_enabledAsLayer;
}

/**
 * Set enabled as a layer.
 *
 * @param True if enabled as a layer.
 */
void
VolumeDynamicConnectivityFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayer = enabled;
}

/**
 * @return True if data loading enabled.
 */
bool
VolumeDynamicConnectivityFile::isDataLoadingEnabled() const
{
    return m_dataLoadingEnabledFlag;
}

/**
 * Set data loading enabled.
 *
 * @param True if data loading enabled.
 */
void
VolumeDynamicConnectivityFile::setDataLoadingEnabled(const bool enabled)
{
    m_dataLoadingEnabledFlag = enabled;
}

/**
 * Initialize the file using information from parent volume file
 */
void
VolumeDynamicConnectivityFile::initializeFile()
{
    clearPrivateData();
    
    CaretAssert(m_parentVolumeFile);
    const int64_t numberOfFrames(1);
    const int32_t numberOfComponents(1);
    
    reinitialize(m_parentVolumeFile->getVolumeSpace(),
                 numberOfFrames,
                 numberOfComponents,
                 SubvolumeAttributes::FUNCTIONAL,
                 m_parentVolumeFile->m_header);
    
    AString path, nameNoExt, ext;
    FileInformation fileInfo(m_parentVolumeFile->getFileName());
    fileInfo.getFileComponents(path, nameNoExt, ext);
    setFileName(FileInformation::assembleFileComponents(path,
                                                        nameNoExt,
                                                        DataFileTypeEnum::toFileExtension(DataFileTypeEnum::VOLUME_DYNAMIC)));
    
    std::vector<int64_t> dims;
    getDimensions(dims);
    m_numberOfVoxels = (dims[0] * dims[1] * dims[2]);
    m_dimI    = dims[0];
    m_dimJ    = dims[1];
    m_dimK    = dims[2];
    m_dimTime = dims[3];
    CaretAssert(m_dimTime == 1);
    if (m_numberOfVoxels > 0) {
        m_voxelData = const_cast<float*>(getFrame(0));
        m_sliceStride          = m_dimI * m_dimJ;
        m_timePointIndexStride = m_numberOfVoxels;
    }
    
    clearVoxels();
    
    m_validDataFlag = true;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Dense files do NOT support writing.
 */
bool
VolumeDynamicConnectivityFile::supportsWriting() const
{
    return false;
}

/**
 * @return The parent volume file
 */
VolumeFile*
VolumeDynamicConnectivityFile::getParentVolumeFile()
{
    return const_cast<VolumeFile*>(m_parentVolumeFile);
}

/**
 * @return The parent volume file (const method)
 */
const VolumeFile*
VolumeDynamicConnectivityFile::getParentVolumeFile() const
{
    return m_parentVolumeFile;
}

/**
 * @return True if the data is valid
 */
bool
VolumeDynamicConnectivityFile::isDataValid() const
{
    return m_validDataFlag;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
VolumeDynamicConnectivityFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    VolumeFile::addToDataFileContentInformation(dataFileInformation);
}

/**
 * Read the file with the given name.
 *
 * @param filename
 *     Name of file
 * @throws DataFileException
 *     If error occurs
 */
void
VolumeDynamicConnectivityFile::readFile(const AString& /*filename*/)
{
    throw DataFileException("Read of Volume Dynamic Connectivity File is not allowed");
}

/**
 * Read the file with the given name.
 *
 * @param filename
 *     Name of file
 * @throws DataFileException
 *     If error occurs
 */
void
VolumeDynamicConnectivityFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Volume Dynamic Connectivity File is not allowed");
}

/**
 * Clear voxels in this volume
 */
void
VolumeDynamicConnectivityFile::clearVoxels()
{
    if (m_voxelData != NULL) {
        std::fill(m_voxelData, m_voxelData + m_numberOfVoxels, 0.0f);
        updateScalarColoringForMap(0);
        m_connectivityDataLoaded->reset();
    }
    m_dataLoadedName = "";
}

/**
 * Get the timepoints for a given voxel
 *
 * @param i
 *    index "I"
 * @param j
 *    index "J"
 * @param k
 *    index "K"
 * @param dataOut
 *     Output with time points
 */
void
VolumeDynamicConnectivityFile::getTimePointsForVoxel(const int64_t i,
                                                     const int64_t j,
                                                     const int64_t k,
                                                     std::vector<float>& dataOut) const
{
    CaretAssert(indexValid(i, j, k));
    
    const int64_t ijk[3] { i, j, k };
    const int64_t componentIndex(0);
    
    dataOut.resize(m_dimTime);
    for (int64_t iTime = 0; iTime < m_dimTime; iTime++) {
        dataOut[iTime] = m_parentVolumeFile->getValue(ijk,
                                                      iTime,
                                                      componentIndex);
    }
}

/**
 * Load connectivity data for the voxel indices and then average the data.
 *
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 * @return
 *    True if data was loaded, else false
 */
bool
VolumeDynamicConnectivityFile::loadMapAverageDataForVoxelIndices(const int64_t volumeDimensionIJK[3],
                                                                 const std::vector<VoxelIJK>& voxelIndices)
{
    /*
     * Loading of data disabled?
     */
    if ( ! isDataValid()) {
        return false;
    }
    if ( ! isDataLoadingEnabled()) {
        return false;
    }

    if ( ! matchesDimensions(volumeDimensionIJK[0],
                             volumeDimensionIJK[1],
                             volumeDimensionIJK[2])) {
        return false;
    }
    
    ConnectivityCorrelation* connCorrelation = getConnectivityCorrelation();
    if (connCorrelation == NULL) {
        return false;
    }
    
    /*
     * Zero out here so that data only gets cleared when data
     * is to be loaded.
     */
    clearVoxels();
    
    std::vector<int64_t> brainordinateIndices;
    for (auto voxel : voxelIndices) {
        const int64_t offset = getVoxelOffset(voxel.m_ijk[0], voxel.m_ijk[1], voxel.m_ijk[2], 0);
        brainordinateIndices.push_back(offset);
    }
    std::vector<float> data(m_numberOfVoxels);
    connCorrelation->getCorrelationForBrainordinateROI(brainordinateIndices,
                                                       data);
    if (m_numberOfVoxels == static_cast<int64_t>(data.size())) {
        for (int64_t i = 0; i < m_numberOfVoxels; i++) {
            m_voxelData[i] = data[i];
        }
        
        const int32_t mapIndex(0);
        const int64_t validDataCount(static_cast<int64_t>(brainordinateIndices.size()));
        setMapName(mapIndex,
                   ("Average Voxel Count: "
                    + AString::number(validDataCount, 'f', 0)));
        m_dataLoadedName = ("Average_Voxel_Count_"
                            + AString::number(validDataCount, 'f', 0));
        
        updateScalarColoringForMap(mapIndex);
        
        return true;
    }
    
    return false;
}

/**
 * Load the connectivity for the voxel at the given coordinate.
 * The loaded data will be in the voxels inside this volume.
 * If the voxel index at the coordinate is invalid, zeros are loaded into all voxels.
 *
 * @param xyz
 *     The voxel XYZ.
 * @return
 *     True if data was loaded, else false.
 */
bool
VolumeDynamicConnectivityFile::loadConnectivityForVoxelXYZ(const float xyz[3])
{
    float indicesFloat[3];
    spaceToIndex(xyz,
                 indicesFloat);
    const int64_t ijk[3] {
        static_cast<int64_t>(indicesFloat[0]),
        static_cast<int64_t>(indicesFloat[1]),
        static_cast<int64_t>(indicesFloat[2])
    };
    
    if (loadConnectivityForVoxelIndex(ijk)) {
        const int32_t invalidRowColumnIndex(-1);
        m_connectivityDataLoaded->setVolumeXYZLoading(xyz,
                                                      invalidRowColumnIndex,
                                                      invalidRowColumnIndex);
        setMapName(0,
                   ("Voxel XYZ: ("
                    + AString::fromNumbers(xyz, 3, ",")
                    + ")"));
        m_dataLoadedName = ("Voxel_x"
                            + AString::number(static_cast<int32_t>(xyz[0]))
                            + "_y"
                            + AString::number(static_cast<int32_t>(xyz[1]))
                            + "_z"
                            + AString::number(static_cast<int32_t>(xyz[2])));
        m_dataLoadedName = m_dataLoadedName.replace('-', 'n');
        
        return true;
    }
    
    return false;
}


/**
 * Load the connectivity for the given voxel.
 * The loaded data will be in the voxels inside this volume.
 * If the voxel index is invalid, zeros are loaded into all voxels.
 *
 * @param ijk
 *     The voxel index.
 */
bool
VolumeDynamicConnectivityFile::loadConnectivityForVoxelIndex(const int64_t ijk[3]) 
{
    bool validFlag(false);
    
    if ( ! isDataValid()) {
        return validFlag;
    }
    if ( ! m_dataLoadingEnabledFlag) {
        return validFlag;
    }
    
    clearVoxels();
    
    std::vector<float> data;
    if (getConnectivityForVoxelIndex(ijk, data)) {
        CaretAssert(m_numberOfVoxels == static_cast<int64_t>(data.size()));
        std::copy(data.begin(),
                  data.end(),
                  m_voxelData);
        
        validFlag = true;
    }
    
    const int32_t mapIndex(0);
    updateScalarColoringForMap(mapIndex);
    
    return validFlag;
}

/**
 * Get the connectivity for the given voxel.
 * If the voxel index is invalid, zeros are loaded into all voxels.
 *
 * @param ijk
 *     The voxel index.
 * @param voxelsOut
 *     Output containing voxel data
 * @return
 *     True if data was loaded.
 */
bool
VolumeDynamicConnectivityFile::getConnectivityForVoxelIndex(const int64_t ijk[3],
                                                            std::vector<float>& voxelsOut)
{
    bool validFlag(false);
    
    ConnectivityCorrelation* connCorrelation = getConnectivityCorrelation();
    if (connCorrelation != NULL) {
        if (indexValid(ijk)) {
            const int64_t voxelCount(m_dimI * m_dimJ * m_dimK);
            
            if (connCorrelation) {
                const int64_t myTimePointOffset = getVoxelOffset(ijk[0], ijk[1], ijk[2], 0);
                connCorrelation->getCorrelationForBrainordinate(myTimePointOffset,
                                                                voxelsOut);
                CaretAssert(voxelCount == static_cast<int64_t>(voxelsOut.size()));
                validFlag = true;
            }
        }
    }

    if ( ! validFlag) {
        voxelsOut.resize(m_numberOfVoxels);
        std::fill(voxelsOut.begin(), voxelsOut.end(),
                  0.0f);
    }
    
    return validFlag;
}

/**
 * @return Pointer to connectivity correlation or NULL if not valid
 */
ConnectivityCorrelation*
VolumeDynamicConnectivityFile::getConnectivityCorrelation()
{
    if ( ! m_connectivityCorrelationFailedFlag) {
        if (m_voxelData != NULL) {
            if (m_connectivityCorrelation == NULL) {
                /*
                 * Need data and timepoint count from parent volume
                 * IJK dimensions are same in this and parent volume
                 */
                CaretAssert(m_parentVolumeFile);
                const float* parentVoxels = m_parentVolumeFile->getFrame(0);
                CaretAssert(parentVoxels);
                std::vector<int64_t> parentVolumeDimensions;
                m_parentVolumeFile->getDimensions(parentVolumeDimensions);
                CaretAssert(parentVolumeDimensions.size() >= 4);
                const int64_t timePointCount = parentVolumeDimensions[3];
                
                const int64_t voxelCount(m_dimI * m_dimJ * m_dimK);
                const int64_t numberOfBrainordinates(voxelCount);   // Each IJK voxel is a group
                const int64_t nextBrainordinateStride(1);           // All groups have one element in each 'brick'
                const int64_t numberOfTimePoints(timePointCount);   // Each group contains timepoints
                const int64_t nextTimePointStride(voxelCount);      // Each group element is in separate IJK 'brick'
                
                AString errorMessage;
                ConnectivityCorrelation* cc = ConnectivityCorrelation::newInstance(parentVoxels,
                                                                                   numberOfBrainordinates,
                                                                                   nextBrainordinateStride,
                                                                                   numberOfTimePoints,
                                                                                   nextTimePointStride,
                                                                                   errorMessage);
                if (cc != NULL) {
                    m_connectivityCorrelation.reset(cc);
                }
                else {
                    m_connectivityCorrelationFailedFlag = true;
                    CaretLogSevere("Failed to create connectvity correlation for "
                                   + m_parentVolumeFile->getFileNameNoPath());
                }
            }
        }
    }
    
    return m_connectivityCorrelation.get();
}

/**
 * @return True if this matches the given dimensions
 *
 * @param dimI
 *     I dimension
 * @param dimJ
 *     J dimension
 * @param dimK
 *     K dimension
 */
bool
VolumeDynamicConnectivityFile::matchesDimensions(const int64_t dimI,
                                                 const int64_t dimJ,
                                                 const int64_t dimK) const
{
    if ((dimI == m_dimI)
        && (dimJ == m_dimJ)
        && (dimK == m_dimK)) {
        return true;
    }
    
    return false;
}

/**
 * @return A volume file using the loaded data (will return NULL if there is an error).
 *
 * @param directoryName
 *     Directory for file
 * @param errorMessageOut
 *     Contains error information
 */
VolumeFile*
VolumeDynamicConnectivityFile::newVolumeFileFromLoadedData(const AString& directoryName,
                                                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    bool validDataFlag(false);
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_COLUMN:
            break;
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_ROW:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
            validDataFlag = true;
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
            validDataFlag = true;
            break;
    }
    
    if ( ! validDataFlag) {
        errorMessageOut = "No voxel connectivity data is loaded";
        return NULL;
    }
    
    VolumeFile* vf(NULL);
    
    try {
        std::vector<int64_t> dimensions;
        dimensions.push_back(m_dimI);
        dimensions.push_back(m_dimJ);
        dimensions.push_back(m_dimK);
        dimensions.push_back(1);
        dimensions.push_back(1);
        
        const VolumeSpace vs = getVolumeSpace();
        vf = new VolumeFile(dimensions,
                            vs.getSform(),
                            1,
                            SubvolumeAttributes::FUNCTIONAL,
                            m_header);
        
        float* voxelData = const_cast<float*>(vf->getFrame(0));
        std::copy(m_voxelData, m_voxelData + m_numberOfVoxels,
                  voxelData);
        
        /*
         * May need to convert a remote path to a local path
         */
        FileInformation fileNameInfo(getFileName());
        const AString volumeFileName = fileNameInfo.getAsLocalAbsoluteFilePath(directoryName,
                                                                               vf->getDataFileType());
        
        /*
         * Create name of volume file data loaded  information
         */
        FileInformation volumeFileInfo(volumeFileName);
        AString thePath, theName, theExtension;
        volumeFileInfo.getFileComponents(thePath,
                                         theName,
                                         theExtension);
        theName.append("_" + m_dataLoadedName);
        AString newFileName = FileInformation::assembleFileComponents(thePath,
                                                                      theName,
                                                                      theExtension);
        if (newFileName.endsWith(".nii")) {
            newFileName.append(".gz");
        }
        vf->setFileName(newFileName);
        
        
        /*
         * Need to copy color palette since it may be the default
         */
        PaletteColorMapping* volumePalette = vf->getMapPaletteColorMapping(0);
        CaretAssert(volumePalette);
        const PaletteColorMapping* myPalette = getMapPaletteColorMapping(0);
        CaretAssert(myPalette);
        volumePalette->copy(*myPalette,
                            true);
        
        vf->updateAfterFileDataChanges();
        vf->updateScalarColoringForMap(0);
        vf->setModified();
    }
    catch (const DataFileException& dfe) {
        errorMessageOut = dfe.whatString();
        if (vf != NULL) {
            delete vf;
            vf = NULL;
        }
    }
    
    return vf;
}


/**
 * Save data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
VolumeDynamicConnectivityFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                   SceneClass* sceneClass)
{
    VolumeFile::saveFileDataToScene(sceneAttributes,
                                    sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
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
VolumeDynamicConnectivityFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                        const SceneClass* sceneClass)
{
    m_connectivityDataLoaded->reset();
    
    VolumeFile::restoreFileDataFromScene(sceneAttributes,
                                         sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    
    switch (m_connectivityDataLoaded->getMode()) {
        case ConnectivityDataLoaded::MODE_COLUMN:
            break;
        case ConnectivityDataLoaded::MODE_NONE:
            break;
        case ConnectivityDataLoaded::MODE_ROW:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE:
            break;
        case ConnectivityDataLoaded::MODE_SURFACE_NODE_AVERAGE:
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_IJK_AVERAGE:
        {
            int64_t dimIJK[3];
            std::vector<VoxelIJK> voxelIJKs;
            m_connectivityDataLoaded->getVolumeAverageVoxelLoading(dimIJK,
                                                                   voxelIJKs);
            loadMapAverageDataForVoxelIndices(dimIJK,
                                              voxelIJKs);
        }
            break;
        case ConnectivityDataLoaded::MODE_VOXEL_XYZ:
        {
            float xyz[3];
            int64_t rowIndex(-1);
            int64_t columnIndex(-1);

            m_connectivityDataLoaded->getVolumeXYZLoading(xyz,
                                                          rowIndex,
                                                          columnIndex);
            loadConnectivityForVoxelXYZ(xyz);
        }
            break;
    }
}

