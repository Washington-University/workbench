
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __CIFTI_PARCEL_REORDERING_DECLARE__
#include "CiftiParcelReordering.h"
#undef __CIFTI_PARCEL_REORDERING_DECLARE__

#include "CaretAssert.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelsMap.h"
#include "ElapsedTimer.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "SceneClass.h"
#include "ScenePrimitiveArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelReordering 
 * \brief Contains a parcel reordering.
 * \ingroup Files
 */

/**
 * Constructs an invalid instance.
 */
CiftiParcelReordering::CiftiParcelReordering()
: CaretObject()
{
    m_sourceParcelLabelFile         = NULL;
    m_sourceParcelLabelFileMapIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_sourceParcelLabelFileMapIndex",
                          &m_sourceParcelLabelFileMapIndex);
}

/**
 * Destructor.
 */
CiftiParcelReordering::~CiftiParcelReordering()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CiftiParcelReordering::CiftiParcelReordering(const CiftiParcelReordering& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperCiftiParcelReordering(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CiftiParcelReordering&
CiftiParcelReordering::operator=(const CiftiParcelReordering& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCiftiParcelReordering(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CiftiParcelReordering::copyHelperCiftiParcelReordering(const CiftiParcelReordering& obj)
{
    m_sourceParcelLabelFile         = obj.m_sourceParcelLabelFile;
    m_sourceParcelLabelFileMapIndex = obj.m_sourceParcelLabelFileMapIndex;
    m_reorderedParcelIndices        = obj.m_reorderedParcelIndices;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CiftiParcelReordering::toString() const
{
    return "CiftiParcelReordering";
}

/**
 * Equality operator that compares this instance to another instance.
 *
 * @param parcelReordering
 *     Compared to "this"
 * @return 
 *     True if this instance and the other instance are "equal", else false.
 */
bool
CiftiParcelReordering::operator==(const CiftiParcelReordering& obj) const
{
    if ((m_sourceParcelLabelFile            == obj.m_sourceParcelLabelFile)
        && (m_sourceParcelLabelFileMapIndex == obj.m_sourceParcelLabelFileMapIndex)) {
        return true;
    }
    
    return false;
}

/**
 * Does this parcel reordering key match?
 *
 * @param sourceParcelLabelFile
 *    The source parcel label file for the reordering.
 * @param sourceParcelLabelFileMapIndex
 *    The map index from the source parcel map file.
 * @return
 *    True if this parcel reordering is for the given file and map indices,
 *    else false.
 */
bool
CiftiParcelReordering::isMatch(const CiftiParcelLabelFile* sourceParcelLabelFile,
             const int32_t sourceParcelLabelFileMapIndex) const
{
    if ((sourceParcelLabelFile            == m_sourceParcelLabelFile)
        && (sourceParcelLabelFileMapIndex == m_sourceParcelLabelFileMapIndex)) {
        return true;
    }
    
    return false;
}

/**
 * @ Is this parcel reordering valid?
 */
bool
CiftiParcelReordering::isValid() const
{
    if (m_reorderedParcelIndices.empty()) {
        return false;
    }
    
    return true;
}

///**
// * Create the parcel reordering.
// *
// * @param sourceParcelLabelFile
// *    Parcel label file used to create the reordering
// * @param sourceParcelLabelFileMapIndex
// *    Index of map in parcel label file used to create reordering
// * @param targetParcelsMap
// *    Parcels map for which a reordering is created.
// * @param errorMessageOut
// *    Error message if there was a problem creating the
// *    reordering.
// * @return
// *    True if the reordering was successfully created, else false.
// */
//bool
//CiftiParcelReordering::createReordering(const CiftiParcelLabelFile* sourceParcelLabelFile,
//                                        const int32_t sourceParcelLabelFileMapIndex,
//                                        const CiftiParcelsMap& targetParcelsMap,
//                                        AString& errorMessageOut)
//{
//    m_reorderedParcelIndices.clear();
//    
//    errorMessageOut = "";
//    
//    if (sourceParcelLabelFile != NULL) {
//        if ((sourceParcelLabelFileMapIndex < 0)
//            || (sourceParcelLabelFileMapIndex >= sourceParcelLabelFile->getNumberOfMaps())) {
//            errorMessageOut.appendWithNewLine("Source Parcel Label File map index="
//                                              + AString::number(sourceParcelLabelFileMapIndex)
//                                              + " is invalid.");
//        }
//    }
//    else {
//        errorMessageOut.appendWithNewLine("Source Parcel Label File is invalid.");
//    }
//    
//    /*
//     * The target parcels are those whose indices will be reordered
//     */
//    const std::vector<CiftiParcelsMap::Parcel>& allTargetParcels = targetParcelsMap.getParcels();
//    const int32_t numTargetParcels = static_cast<int32_t>(allTargetParcels.size());
//    if (numTargetParcels <= 0) {
//        errorMessageOut.appendWithNewLine("Parcels map that is to be reordered is empty.");
//    }
//    
//    /*
//     * These source parcels are those used to create the reordering
//     */
//    const CiftiParcelsMap* sourceParcelsMap = sourceParcelLabelFile->getCiftiParcelsMapForBrainordinateMapping();
//    CaretAssert(sourceParcelsMap);
//    const std::vector<CiftiParcelsMap::Parcel>& allSourceParcels = sourceParcelsMap->getParcels();
//    const int32_t numSourceParcels = static_cast<int32_t>(allSourceParcels.size());
//    if (numSourceParcels <= 0) {
//        errorMessageOut.appendWithNewLine("Parcels map from that Parcel Label File is used to reorder the parcels is empty.");
//    }
//    
//    if ( ! errorMessageOut.isEmpty()) {
//        return false;
//    }
//
//    /*
//     * Tracks parcels that are in the reordering so that they can be
//     * skipped and save time by avoiding parcel comparisons
//     */
//    std::vector<bool> targetParcelRemapped(numTargetParcels,
//                                           false);
//    
//    /*
//     * Loop through the source parcels used to create the reordering
//     */
//    for (int32_t iSource = 0;
//         iSource < numSourceParcels;
//         iSource++) {
//        CaretAssertVectorIndex(allSourceParcels,
//                               iSource);
//        const CiftiParcelsMap::Parcel& sourceParcel = allSourceParcels[iSource];
//        
//        /*
//         * Loop through target parcels to find those that
//         * have the exact same mapping by using the
//         * parcel equality operator.
//         */
//        for (int32_t iTarget = 0;
//             iTarget < numTargetParcels;
//             iTarget++) {
//            CaretAssertVectorIndex(targetParcelRemapped, iTarget);
//            if ( ! targetParcelRemapped[iTarget]) {
//                CaretAssertVectorIndex(allTargetParcels, iTarget);
//                if (allTargetParcels[iTarget] == sourceParcel) {
//                    m_reorderedParcelIndices.push_back(iTarget);
//
//                    targetParcelRemapped[iTarget] = true;
//                }
//            }
//        }
//    }
//    
//    if (m_reorderedParcelIndices.empty()) {
//        return false;
//    }
//    
//    m_sourceParcelLabelFile = const_cast<CiftiParcelLabelFile*>(sourceParcelLabelFile);
//    m_sourceParcelLabelFileMapIndex = sourceParcelLabelFileMapIndex;
//    
//    std::cout << "New parcel reorder: ";
//    for (std::vector<int32_t>::iterator iter = m_reorderedParcelIndices.begin();
//         iter != m_reorderedParcelIndices.end();
//         iter++) {
//        std::cout << " " << *iter;
//    }
//    std::cout << std::endl;
//    
//    return true;
//}

/**
 * Create the parcel reordering.
 *
 * @param sourceParcelLabelFile
 *    Parcel label file used to create the reordering
 * @param sourceParcelLabelFileMapIndex
 *    Index of map in parcel label file used to create reordering
 * @param targetParcelsMap
 *    Parcels map for which a reordering is created.
 * @param errorMessageOut
 *    Error message if there was a problem creating the
 *    reordering.
 * @return
 *    True if the reordering was successfully created, else false.
 */
bool
CiftiParcelReordering::createReordering(const CiftiParcelLabelFile* sourceParcelLabelFile,
                                        const int32_t sourceParcelLabelFileMapIndex,
                                        const CiftiParcelsMap& targetParcelsMap,
                                        AString& errorMessageOut)
{
    ElapsedTimer timer;
    timer.start();
    
    m_reorderedParcelIndices.clear();
    
    errorMessageOut = "";
    
    if (sourceParcelLabelFile != NULL) {
        if ((sourceParcelLabelFileMapIndex < 0)
            || (sourceParcelLabelFileMapIndex >= sourceParcelLabelFile->getNumberOfMaps())) {
            errorMessageOut.appendWithNewLine("Source Parcel Label File map index="
                                              + AString::number(sourceParcelLabelFileMapIndex)
                                              + " is invalid.");
        }
    }
    else {
        errorMessageOut.appendWithNewLine("Source Parcel Label File is invalid.");
    }
    
    /*
     * The target parcels are those whose indices will be reordered
     */
    const std::vector<CiftiParcelsMap::Parcel>& allTargetParcels = targetParcelsMap.getParcels();
    const int32_t numTargetParcels = static_cast<int32_t>(allTargetParcels.size());
    if (numTargetParcels <= 0) {
        errorMessageOut.appendWithNewLine("Parcels map that is to be reordered is empty.");
    }
    
    /*
     * These source's parcels map.
     */
    const CiftiParcelsMap* sourceParcelsMap = sourceParcelLabelFile->getCiftiParcelsMapForBrainordinateMapping();
    if (sourceParcelsMap == NULL) {
        errorMessageOut.appendWithNewLine("No parcels map in source Parcel Label File.");
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    /*
     * Reorder the indices for the Parcels Map in the selected map 
     * of the Parcel Labels File.
     */
    std::vector<int32_t> reorderedSourceParcelIndices;
    if ( ! sourceParcelLabelFile->getReorderedParcelIndicesFromMap(sourceParcelLabelFileMapIndex,
                                                                   reorderedSourceParcelIndices,
                                                                   errorMessageOut)) {
        return false;
    }
    
    const std::vector<CiftiParcelsMap::Parcel>& allSourceParcels = sourceParcelsMap->getParcels();
    const int32_t numberOfSourceParcels = static_cast<int32_t>(allSourceParcels.size());
    if (numberOfSourceParcels <= 0) {
        errorMessageOut.appendWithNewLine("No parcels in source Parcel Label File.");
        return  false;
    }
    
    /*
     * One a target parcel is remapped, it no never needs to 
     * be tested again.
     */
    std::vector<bool> targetParcelRemapped(numTargetParcels,
                                           false);
    
    /*
     * Loop through the source parcels used to create the reordering
     */
    const int32_t numReorderedSourceParcelIndices = static_cast<int32_t>(reorderedSourceParcelIndices.size());
    for (int32_t iSource = 0;
         iSource < numReorderedSourceParcelIndices;
         iSource++) {
        CaretAssertVectorIndex(reorderedSourceParcelIndices,
                               iSource);
        const int32_t sourceParcelIndex = reorderedSourceParcelIndices[iSource];
        CaretAssertVectorIndex(allSourceParcels, sourceParcelIndex);
        const CiftiParcelsMap::Parcel& sourceParcel = allSourceParcels[sourceParcelIndex];

        /*
         * Loop through target parcels to find those that
         * have the exact same mapping by using the
         * parcel's approximate match method which
         * compares the brainordinate mapping of the parcels.
         */
        for (int32_t iTarget = 0;
             iTarget < numTargetParcels;
             iTarget++) {
            CaretAssertVectorIndex(targetParcelRemapped, iTarget);
            if ( ! targetParcelRemapped[iTarget]) {
                CaretAssertVectorIndex(allTargetParcels, iTarget);
                if (allTargetParcels[iTarget].approximateMatch(sourceParcel)) {
                    m_reorderedParcelIndices.push_back(iTarget);

                    targetParcelRemapped[iTarget] = true;
                }
            }
        }
    }
    
    const int32_t matchCount = std::count(targetParcelRemapped.begin(),
                                     targetParcelRemapped.end(),
                                     true);
    const int32_t notMatchCount = numTargetParcels - matchCount;
    if (notMatchCount > 0) {
        errorMessageOut.appendWithNewLine("Failed to match "
                                          + AString::number(notMatchCount)
                                          + " of "
                                          + AString::number(numTargetParcels));
        return false;
    }
    
    //std::cout << "Time to match parcels was: " << timer.getElapsedTimeSeconds() << " seconds." << std::endl;
    
    m_sourceParcelLabelFile         = const_cast<CiftiParcelLabelFile*>(sourceParcelLabelFile);
    m_sourceParcelLabelFileMapIndex = sourceParcelLabelFileMapIndex;
    
    return true;
}

/**
 * @return The reordered indices of the parcels.
 */
std::vector<int32_t>
CiftiParcelReordering::getReorderedParcelIndices() const
{
    return m_reorderedParcelIndices;
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
CiftiParcelReordering::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiParcelReordering",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_sourceParcelLabelFile != NULL) {
        sceneClass->addPathName("m_sourceParcelLabelFile",
                                m_sourceParcelLabelFile->getFileName());
        if ( ! m_reorderedParcelIndices.empty()) {
            sceneClass->addIntegerArray("m_reorderedParcelIndices",
                                        &m_reorderedParcelIndices[0],
                                        m_reorderedParcelIndices.size());
        }
    }
    
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
CiftiParcelReordering::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sourceParcelLabelFile = NULL;
    m_reorderedParcelIndices.clear();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const ScenePathName* parcelLabelPathName = sceneClass->getPathName("m_sourceParcelLabelFile");
    if (parcelLabelPathName != NULL) {
        
    }
    
    const ScenePrimitiveArray* reorderArray = sceneClass->getPrimitiveArray("m_reorderedParcelIndices");
    if (reorderArray != NULL) {
        const int32_t numberOfElements = reorderArray->getNumberOfArrayElements();
        if (numberOfElements > 0) {
            m_reorderedParcelIndices.reserve(numberOfElements);
            for (int32_t i = 0; i < numberOfElements; i++) {
                m_reorderedParcelIndices.push_back(reorderArray->integerValue(i));
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

