
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __BRAINORDINATE_REGION_OF_INTEREST_DECLARE__
#include "BrainordinateRegionOfInterest.h"
#undef __BRAINORDINATE_REGION_OF_INTEREST_DECLARE__

#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelsMap.h"
#include "EventSurfaceStructuresValidGet.h"
#include "EventManager.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;



/**
 * \class caret::BrainordinateRegionOfInterest
 * \brief Contains a region of interest
 * \ingroup Files
 *
 * Contains a region of interest that may include nodes from multiple
 * surfaces and voxels within a volume space.
 */

/**
 * Constructor.
 */
BrainordinateRegionOfInterest::BrainordinateRegionOfInterest()
: CaretObject()
{
    clear();
    
    m_sceneAssistant = new SceneClassAssistant();
    
}

/**
 * Destructor.
 */
BrainordinateRegionOfInterest::~BrainordinateRegionOfInterest()
{
    clear();
    
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BrainordinateRegionOfInterest::BrainordinateRegionOfInterest(const BrainordinateRegionOfInterest& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperBrainordinateRegionOfInterest(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
BrainordinateRegionOfInterest&
BrainordinateRegionOfInterest::operator=(const BrainordinateRegionOfInterest& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperBrainordinateRegionOfInterest(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
BrainordinateRegionOfInterest::copyHelperBrainordinateRegionOfInterest(const BrainordinateRegionOfInterest& obj)
{
    clear();
    
    m_highlighingEnabled = obj.m_highlighingEnabled;
    
    for (std::vector<SurfaceNodesInfo*>::const_iterator iter = obj.m_surfaceNodesInfo.begin();
         iter != obj.m_surfaceNodesInfo.end();
         iter++) {
        const SurfaceNodesInfo* sni = *iter;
        m_surfaceNodesInfo.push_back(new SurfaceNodesInfo(*sni));
    }
    
    m_voxelSize[0] = obj.m_voxelSize[0];
    m_voxelSize[1] = obj.m_voxelSize[1];
    m_voxelSize[2] = obj.m_voxelSize[2];
    
    /*
     * Voxel sizes must be non-negative
     */
    CaretAssert(m_voxelSize[0] >= 0.0);
    CaretAssert(m_voxelSize[1] >= 0.0);
    CaretAssert(m_voxelSize[2] >= 0.0);
    
    m_voxelXYZ = obj.m_voxelXYZ;
}

/**
 * Clear this region of interest.
 */
void
BrainordinateRegionOfInterest::clear()
{
    for (std::vector<SurfaceNodesInfo*>::iterator iter = m_surfaceNodesInfo.begin();
         iter != m_surfaceNodesInfo.end();
         iter++) {
        SurfaceNodesInfo* sni = *iter;
        CaretAssert(sni);
        delete sni;
    }
    
    m_surfaceNodesInfo.clear();
    
    m_voxelSize[0] = 0.0;
    m_voxelSize[1] = 0.0;
    m_voxelSize[2] = 0.0;
    
    m_voxelXYZ.clear();
    
    m_highlighingEnabled = false;
}

/**
 * @return True if this region of interest contains a surface with
 * valid nodes, else false.
 */
bool
BrainordinateRegionOfInterest::hasSurfaceNodes() const
{
    for (std::vector<SurfaceNodesInfo*>::const_iterator iter = m_surfaceNodesInfo.begin();
         iter != m_surfaceNodesInfo.end();
         iter++) {
        const SurfaceNodesInfo* sni = *iter;
        CaretAssert(sni);
        
        if ( ! sni->m_surfaceNodeIndices.empty()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Does this region of interest contains surface nodes for the surface
 * with the given structure and number of nodes?
 *
 * @param structure
 *     The surface's structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @return
 *     True if found, else false.
 */
bool
BrainordinateRegionOfInterest::hasNodesForSurfaceStructure(const StructureEnum::Enum structure,
                                                           const int64_t surfaceNumberOfNodes) const
{
    for (std::vector<SurfaceNodesInfo*>::const_iterator iter = m_surfaceNodesInfo.begin();
         iter != m_surfaceNodesInfo.end();
         iter++) {
        const SurfaceNodesInfo* sni = *iter;
        CaretAssert(sni);
        
        if ( ! sni->m_surfaceNodeIndices.empty()) {
            if ((sni->m_structure == structure)
                && (sni->m_surfaceNumberOfNodes == surfaceNumberOfNodes)) {
                return true;
            }
        }
    }
    
    return false;
}


/**
 * Get the nodes in the region of interest for the surface
 * with the given structure and number of nodes?
 *
 * @param structure
 *     The surface's structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @return
 *     Vector containing node indices.  Empty if surface node found.
 */
const std::vector<int64_t>&
BrainordinateRegionOfInterest::getNodesForSurfaceStructure(const StructureEnum::Enum structure,
                                                           const int64_t surfaceNumberOfNodes) const
{
    for (std::vector<SurfaceNodesInfo*>::const_iterator iter = m_surfaceNodesInfo.begin();
         iter != m_surfaceNodesInfo.end();
         iter++) {
        const SurfaceNodesInfo* sni = *iter;
        CaretAssert(sni);
        
        if ( ! sni->m_surfaceNodeIndices.empty()) {
            if ((sni->m_structure == structure)
                && (sni->m_surfaceNumberOfNodes == surfaceNumberOfNodes)) {
                return sni->m_surfaceNodeIndices;
            }
        }
    }
    
    return s_emptySurfaceNodes;
}

/**
 * @return True if the region of interest contains volume voxels.
 */
bool
BrainordinateRegionOfInterest::hasVolumeVoxels() const
{
    if ( ! m_voxelXYZ.empty()) {
        return true;
    }
    
    return false;
}

/**
 * Get the volume voxel size.
 *
 * @param voxelSizeOut
 *    Size of the volume voxels.
 */
void
BrainordinateRegionOfInterest::getVolumeVoxelSize(float voxelSizeOut[3]) const
{
    /*
     * Voxel sizes must be non-negative
     */
    CaretAssert(m_voxelSize[0] >= 0.0);
    CaretAssert(m_voxelSize[1] >= 0.0);
    CaretAssert(m_voxelSize[2] >= 0.0);
    
    voxelSizeOut[0] = m_voxelSize[0];
    voxelSizeOut[1] = m_voxelSize[1];
    voxelSizeOut[2] = m_voxelSize[2];
}


/**
 * Get the volume voxels in the region-of-interest.
 *
 * @return
 *    XYZ coordinates of the voxels.   (3 per voxel ==> XYZ)
 *    Empty if no voxels ni region of interest.
 */
const std::vector<float>&
BrainordinateRegionOfInterest::getVolumeVoxelsXYZ() const
{
    return m_voxelXYZ;
}


/**
 * Set the nodes in the region of interest for the surface
 * with the given structure and number of nodes.  If there is an
 * ROI for the struture and number of nodes, it is replaced.
 *
 * @param structure
 *     The surface's structure.
 * @param surfaceNumberOfNodes
 *     Number of nodes in surface.
 * @param surfaceNodeIndices
 *     The surface node indices.
 */
void
BrainordinateRegionOfInterest::setSurfaceNodes(const StructureEnum::Enum structure,
                                               const int64_t surfaceNumberOfNodes,
                                               const std::vector<int64_t>& surfaceNodeIndices)
{
    int64_t replaceAtIndex = -1;
    const int64_t numItems = static_cast<int64_t>(m_surfaceNodesInfo.size());
    for (int64_t i = 0; i < numItems; i++) {
        const SurfaceNodesInfo* sni = m_surfaceNodesInfo[i];
        CaretAssert(sni);
        
        if ((sni->m_structure == structure)
            && (sni->m_surfaceNumberOfNodes == surfaceNumberOfNodes)) {
            delete sni;
            m_surfaceNodesInfo[i] = NULL;
            replaceAtIndex = i;
            break;
        }
    }
    
    SurfaceNodesInfo* sni = new SurfaceNodesInfo(structure,
                                                 surfaceNumberOfNodes,
                                                 surfaceNodeIndices);
    
    if (replaceAtIndex >= 0) {
        CaretAssertVectorIndex(m_surfaceNodesInfo, replaceAtIndex);
        CaretAssert(m_surfaceNodesInfo[replaceAtIndex] = NULL);
        m_surfaceNodesInfo[replaceAtIndex] = sni;
    }
    else {
        m_surfaceNodesInfo.push_back(sni);
    }
}


/**
 * Set the volume voxels in the region of interest.  Replaces all
 * voxels in the region of interest.
 *
 * @param voxelSize
 *     Size of the voxels.
 * @param voxelsXYZ
 *     Coordinates of the voxels (3 per voxel ==> XYZ)
 */
void
BrainordinateRegionOfInterest::setVolumeVoxels(const float voxelSize[3],
                                               const std::vector<float>& voxelsXYZ)

{
    m_voxelSize[0] = std::fabs(voxelSize[0]);
    m_voxelSize[1] = std::fabs(voxelSize[1]);
    m_voxelSize[2] = std::fabs(voxelSize[2]);
    m_voxelXYZ     = voxelsXYZ;
    
    /*
     * Voxel sizes must be non-negative
     */
    CaretAssert(m_voxelSize[0] >= 0.0);
    CaretAssert(m_voxelSize[1] >= 0.0);
    CaretAssert(m_voxelSize[2] >= 0.0);
}

/**
 * @return Is brainordinate highlighting enabled
 *
 */
bool
BrainordinateRegionOfInterest::isBrainordinateHighlightingEnabled() const
{
    return m_highlighingEnabled;
}

/**
 * Set brainordinate highlighting.
 *
 * @param enabled
 *    New status for highlighting.
 */
void
BrainordinateRegionOfInterest::setBrainordinateHighlightingEnabled(const bool enabled)
{
    m_highlighingEnabled = enabled;
}

/**
 * Set the region of interest to the brainordinates in the given named label
 * from the labels used for loading data in the given CIFTI file.
 *
 * @param ciftiMappableDataFile
 *     The CIFTI file.
 * @param mapIndex
 *     Index of the map.
 * @param labelName
 *     Name of the parcel.
 * @param errorMessageOut
 *     Will contain error message upon exit.
 * @return
 *     True if successful. If failure, false is returned and errorMessageOut
 *     will contain cause of failure.
 */
bool
BrainordinateRegionOfInterest::setWithLabelFileLabel(const CaretMappableDataFile* caretMappableDataFile,
                           const int32_t mapIndex,
                           const AString& labelName,
                           AString& errorMessageOut)
{
    clear();
    
    errorMessageOut.clear();
    
    if (caretMappableDataFile == NULL) {
        errorMessageOut = "File is not valid (NULL)";
        return false;
    }
    
    if ( ! caretMappableDataFile->isMappedWithLabelTable()) {
        errorMessageOut = "File is not mapped with a label table.";
        return false;
    }
    
    if ((mapIndex < 0)
        || (mapIndex >= caretMappableDataFile->getNumberOfMaps())) {
        errorMessageOut = ("Map index="
                           + AString::number(mapIndex)
                           + " is invalid for file "
                           + caretMappableDataFile->getFileNameNoPath());
        return false;
    }
    
    if (labelName.isEmpty()) {
        errorMessageOut = "Label name is empty.";
        return false;
    }
    
    const GiftiLabelTable* labelTable = caretMappableDataFile->getMapLabelTable(mapIndex);
    CaretAssert(labelTable);
    if (labelTable != NULL) {
        const int64_t labelKey = labelTable->getLabelKeyFromName(labelName);
        if (labelKey < 0) {
            errorMessageOut = ("Label "
                               + labelName
                               + " not found in label table.");
            return false;
        }
        
        const LabelFile* labelFile = dynamic_cast<const LabelFile*>(caretMappableDataFile);
        const CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<const CiftiBrainordinateLabelFile*>(caretMappableDataFile);
        if (labelFile != NULL) {
            const StructureEnum::Enum structure = labelFile->getStructure();
            int64_t surfaceNumberOfNodes = labelFile->getNumberOfNodes();
            
            std::vector<int32_t> nodeIndices;
            labelFile->getNodeIndicesWithLabelKey(mapIndex, labelKey, nodeIndices);
            
            if (nodeIndices.empty()) {
                errorMessageOut = ("No vertices found for label "
                                   + labelName);
                return false;
            }
            
            std::vector<int64_t> nodeIndices64(nodeIndices.begin(),
                                               nodeIndices.end());
            setSurfaceNodes(structure,
                            surfaceNumberOfNodes,
                            nodeIndices64);
            
            return true;
        }
        else if (ciftiLabelFile != NULL) {
            bool haveBrainordinatesFlag = false;
            EventSurfaceStructuresValidGet structureNumberOfNodesEvent;
            EventManager::get()->sendEvent(structureNumberOfNodesEvent.getPointer());
            std::map<StructureEnum::Enum, int32_t> structNodes = structureNumberOfNodesEvent.getStructuresAndNumberOfNodes();
            for (std::map<StructureEnum::Enum, int32_t>::iterator structNodeIter = structNodes.begin();
                 structNodeIter != structNodes.end();
                 structNodeIter++) {
                const StructureEnum::Enum structure = structNodeIter->first;
                const int32_t surfaceNumberOfNodes = structNodeIter->second;
                std::vector<int32_t> nodeIndices;
                ciftiLabelFile->getNodeIndicesWithLabelKey(structure,
                                                           surfaceNumberOfNodes,
                                                           mapIndex,
                                                           labelKey,
                                                           nodeIndices);
                if ( ! nodeIndices.empty()) {
                    std::vector<int64_t> nodeIndices64(nodeIndices.begin(),
                                                       nodeIndices.end());
                    setSurfaceNodes(structure,
                                    surfaceNumberOfNodes,
                                    nodeIndices64);
                    haveBrainordinatesFlag = true;
                }
            }
            
            std::vector<float> voxelsXYZ;
            ciftiLabelFile->getVoxelCoordinatesWithLabelKey(mapIndex,
                                                            labelKey,
                                                            voxelsXYZ);
            if ( ! voxelsXYZ.empty()) {
                m_voxelXYZ.insert(m_voxelXYZ.end(),
                                  voxelsXYZ.begin(),
                                  voxelsXYZ.end());
                
                ciftiLabelFile->getVoxelSpacing(m_voxelSize[0], m_voxelSize[1], m_voxelSize[2]);
                m_voxelSize[0] = std::fabs(m_voxelSize[0]);
                m_voxelSize[1] = std::fabs(m_voxelSize[1]);
                m_voxelSize[2] = std::fabs(m_voxelSize[2]);
                
                haveBrainordinatesFlag = true;

            }
            
            if (haveBrainordinatesFlag) {
                return true;
            }
            
            errorMessageOut = ("No brainordinates found for label "
                               + labelName);
            return false;
        }
    }
    
    errorMessageOut = (caretMappableDataFile->getFileNameNoPath()
                       + " is not a label type file or not recognized as label file (programming error).");
    return false;
}


/**
 * Set the region of interest to the brainordinates in the given named parcel
 * from the parcels used for loading data in the given CIFTI file.
 *
 * @param ciftiMappableDataFile
 *     The CIFTI file.
 * @param mapIndex
 *     Index of the map.
 * @param parcelName
 *     Name of the parcel.
 * @param errorMessageOut
 *     Will contain error message upon exit.
 * @return
 *     True if successful. If failure, false is returned and errorMessageOut
 *     will contain cause of failure.
 */
bool
BrainordinateRegionOfInterest::setWithCiftiParcelLoadingBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                                                       const int32_t mapIndex,
                                                                       const AString& parcelName,
                                                                       AString& errorMessageOut)
{
    clear();
    
    errorMessageOut.clear();
    
    if (ciftiMappableDataFile == NULL) {
        errorMessageOut = "File is not valid (NULL)";
        return false;
    }
    
    if ((mapIndex < 0)
        || (mapIndex >= ciftiMappableDataFile->getNumberOfMaps())) {
        errorMessageOut = ("Map index="
                           + AString::number(mapIndex)
                           + " is invalid for file "
                           + ciftiMappableDataFile->getFileNameNoPath());
        return false;
    }
    
    const CiftiParcelsMap* ciftiParcelsMap = ciftiMappableDataFile->getCiftiParcelsMapForLoading();
    
    return setWithCiftiParcelBrainordinates(ciftiMappableDataFile,
                                            ciftiParcelsMap,
                                            parcelName,
                                            errorMessageOut);
}

/**
 * Set the region of interest to the brainordinates in the given named parcel
 * from the parcels used for mapping data in the given CIFTI file.
 *
 * @param ciftiMappableDataFile
 *     The CIFTI file.
 * @param mapIndex
 *     Index of the map.
 * @param parcelName
 *     Name of the parcel.
 * @param errorMessageOut
 *     Will contain error message upon exit.
 * @return
 *     True if successful. If failure, false is returned and errorMessageOut
 *     will contain cause of failure.
 */
bool
BrainordinateRegionOfInterest::setWithCiftiParcelMappingBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                                                       const int32_t mapIndex,
                                                                       const AString& parcelName,
                                                                       AString& errorMessageOut)
{
    clear();
    
    errorMessageOut.clear();
    
    if (ciftiMappableDataFile == NULL) {
        errorMessageOut = "File is not valid (NULL)";
        return false;
    }
    
    if ((mapIndex < 0)
        || (mapIndex >= ciftiMappableDataFile->getNumberOfMaps())) {
        errorMessageOut = ("Map index="
                           + AString::number(mapIndex)
                           + " is invalid for file "
                           + ciftiMappableDataFile->getFileNameNoPath());
        return false;
    }
    
    const CiftiParcelsMap* ciftiParcelsMap = ciftiMappableDataFile->getCiftiParcelsMapForBrainordinateMapping();
    
    return setWithCiftiParcelBrainordinates(ciftiMappableDataFile,
                                            ciftiParcelsMap,
                                            parcelName,
                                            errorMessageOut);
}

/**
 * Set the region of interest to the brainordinates in the given named parcel
 * from the given CIFTI parcels map in the given CIFTI file.
 *
 * @param ciftiMappableDataFile
 *     The CIFTI file.
 * @param ciftiParcelsMap
 *     The CIFTI Parcels Map.
 * @param parcelName
 *     Name of the parcel.
 * @param errorMessageOut
 *     Will contain error message upon exit.
 * @return
 *     True if successful. If failure, false is returned and errorMessageOut
 *     will contain cause of failure.
 */
bool
BrainordinateRegionOfInterest::setWithCiftiParcelBrainordinates(const CiftiMappableDataFile* ciftiMappableDataFile,
                                                                const CiftiParcelsMap* ciftiParcelsMap,
                                                                const AString& parcelName,
                                                                AString& errorMessageOut)
{
    if (parcelName.isEmpty()) {
        errorMessageOut = "Parcel name is empty.";
        return false;
    }
    
    if (ciftiParcelsMap == NULL) {
        errorMessageOut = ("No parcels map in "
                           + ciftiMappableDataFile->getFileNameNoPath());
        return false;
    }
    
    const int64_t parcelIndex = ciftiParcelsMap->getIndexFromNumberOrName(parcelName);
    if (parcelIndex < 0) {
        errorMessageOut = ("Parcel name="
                           + parcelName
                           + " not found in parcels map for file "
                           + ciftiMappableDataFile->getFileNameNoPath());
        return false;
    }
    
    
    const std::vector<CiftiParcelsMap::Parcel>& allParcels = ciftiParcelsMap->getParcels();
    CaretAssertVectorIndex(allParcels, parcelIndex);
    
    const CiftiParcelsMap::Parcel& parcel = allParcels[parcelIndex];
    
    for (std::map<StructureEnum::Enum, std::set<int64_t> >::const_iterator iter = parcel.m_surfaceNodes.begin();
         iter != parcel.m_surfaceNodes.end();
         iter++) {
        const StructureEnum::Enum structure = iter->first;
        const std::set<int64_t>& nodeSet = iter->second;
        
        int64_t surfaceNumberOfNodes = ciftiParcelsMap->getSurfaceNumberOfNodes(structure);
        std::vector<int64_t> nodeVector(nodeSet.begin(),
                                        nodeSet.end());
        
        setSurfaceNodes(structure,
                        surfaceNumberOfNodes,
                        nodeVector);
    }
    
    if (ciftiParcelsMap->hasVolumeData()) {
        const VolumeSpace& volumeSpace = ciftiParcelsMap->getVolumeSpace();
        
        const std::set<VoxelIJK>& voxelSetIJK = parcel.m_voxelIndices;
        for (std::set<VoxelIJK>::iterator voxelIter = voxelSetIJK.begin();
             voxelIter != voxelSetIJK.end();
             voxelIter++) {
            const VoxelIJK& voxelIJK = *voxelIter;
            float xyz[3];
            volumeSpace.indexToSpace(voxelIJK.m_ijk,
                                     xyz);
            m_voxelXYZ.push_back(xyz[0]);
            m_voxelXYZ.push_back(xyz[1]);
            m_voxelXYZ.push_back(xyz[2]);
        }
        
        if ( ! m_voxelXYZ.empty()) {
            VolumeSpace::OrientTypes orientation[3];
            float origin[3];
            volumeSpace.getOrientAndSpacingForPlumb(orientation,
                                                    m_voxelSize,
                                                    origin);
            m_voxelSize[0] = std::fabs(m_voxelSize[0]);
            m_voxelSize[1] = std::fabs(m_voxelSize[1]);
            m_voxelSize[2] = std::fabs(m_voxelSize[2]);
            
            /*
             * Voxel sizes must be non-negative
             */
            CaretAssert(m_voxelSize[0] >= 0.0);
            CaretAssert(m_voxelSize[1] >= 0.0);
            CaretAssert(m_voxelSize[2] >= 0.0);
        }
    }
    
    return true;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
BrainordinateRegionOfInterest::toString() const
{
    return "BrainordinateRegionOfInterest";
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
BrainordinateRegionOfInterest::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainordinateRegionOfInterest",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
BrainordinateRegionOfInterest::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

