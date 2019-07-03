
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
#include "ConnectivityCorrelation.h"
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
: VolumeFile(),
m_parentVolumeFile(parentVolumeFile)
{
    CaretAssert(m_parentVolumeFile);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabledAsLayer",
                          &m_enabledAsLayer);
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
    m_voxelData = NULL;
    m_numberOfVoxels = 0;
    m_validDataFlag = false;
    m_enabledAsLayer = false;
    m_connectivityCorrelation.reset();
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
                                                        "dynconn.nii.gz")); //DataFileTypeEnum::toFileExtension(DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC)));
    
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
 * Save subclass data to the scene.
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
VolumeDynamicConnectivityFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
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
VolumeDynamicConnectivityFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
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
    }
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
 * Load the connectivity for the voxel at the given coordinate.
 * The loaded data will be in the voxels inside this volume.
 * If the voxel index at the coordinate is invalid, zeros are loaded into all voxels.
 *
 * @param xyz
 *     The voxel XYZ.
 */
void
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
    loadConnectivityForVoxelIndex(ijk);
}


/**
 * Load the connectivity for the given voxel.
 * The loaded data will be in the voxels inside this volume.
 * If the voxel index is invalid, zeros are loaded into all voxels.
 *
 * @param ijk
 *     The voxel index.
 */
void
VolumeDynamicConnectivityFile::loadConnectivityForVoxelIndex(const int64_t ijk[3]) 
{
    if ( ! isDataValid()) {
        return;
    }
    if ( ! m_dataLoadingEnabledFlag) {
        return;
    }
    
    clearVoxels();
    
    if (m_voxelData != NULL) {
        if (indexValid(ijk)) {
            const int64_t voxelCount(m_dimI * m_dimJ * m_dimK);
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
                
                const int64_t groupCount(voxelCount);           // Each IJK voxel is a group
                const int64_t groupStride(1);                   // All groups have one element in each 'brick'
                const int64_t groupDataCount(timePointCount);   // Each group contains timepoints
                const int64_t groupDataStride(voxelCount);      // Each group element is in separate IJK 'brick'
                m_connectivityCorrelation.reset(new ConnectivityCorrelation(parentVoxels,
                                                                            groupCount,
                                                                            groupStride,
                                                                            groupDataCount,
                                                                            groupDataStride));
            }
            
            const int64_t myTimePointOffset = getVoxelOffset(ijk[0], ijk[1], ijk[2], 0);
            std::vector<float> coefficients;
            m_connectivityCorrelation->getCorrelationForGroup(myTimePointOffset,
                                                              coefficients);
            CaretAssert(voxelCount == static_cast<int64_t>(coefficients.size()));
            std::copy(coefficients.begin(),
                      coefficients.end(),
                      m_voxelData);
            
            setMapName(0,
                       ("Voxel ("
                        + AString::fromNumbers(ijk, 3, ", ")
                        + ")"));
        }
    }
    
    const int32_t mapIndex(0);
    updateScalarColoringForMap(mapIndex);
}

