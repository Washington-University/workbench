
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

#define __CIFTI_PARCEL_REORDERING_MODEL_DECLARE__
#include "CiftiParcelReorderingModel.h"
#undef __CIFTI_PARCEL_REORDERING_MODEL_DECLARE__

#include "CaretAssert.h"
#include "CiftiMappableDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelsMap.h"
#include "CiftiXML.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePathName.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelReorderingModel 
 * \brief Controls reordering of parcels for a CIFTI data file.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiParcelReorderingModel::CiftiParcelReorderingModel(const CiftiMappableDataFile* parentCiftiMappableDataFile)
: CaretObject(),
m_parentCiftiMappableDataFile(parentCiftiMappableDataFile)
{
    CaretAssert(parentCiftiMappableDataFile);
    
    switch (parentCiftiMappableDataFile->getDataFileType()) {
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            break;
        default:
            CaretAssert(0);
    }
    
    m_parcelReorderingEnabledStatus = false;
    m_selectedParcelLabelFile = NULL;
    m_selectedParcelLabelFileMapIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_selectedParcelLabelFileMapIndex",
                          &m_selectedParcelLabelFileMapIndex);
    m_sceneAssistant->add("m_parcelReorderingEnabledStatus",
                          &m_parcelReorderingEnabledStatus);
}

/**
 * Destructor.
 */
CiftiParcelReorderingModel::~CiftiParcelReorderingModel()
{
    delete m_sceneAssistant;
    
    clearCiftiParcelReordering();
}

/**
 * Clear all parcel reordering.
 */
void
CiftiParcelReorderingModel::clearCiftiParcelReordering()
{
    for (std::vector<CiftiParcelReordering*>::iterator iter = m_parcelReordering.begin();
         iter != m_parcelReordering.end();
         iter++) {
        delete *iter;
    }
    m_parcelReordering.clear();
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CiftiParcelReorderingModel::CiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj)
: CaretObject(obj),
SceneableInterface(obj)

{
    this->copyHelperCiftiParcelReorderingModel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CiftiParcelReorderingModel&
CiftiParcelReorderingModel::operator=(const CiftiParcelReorderingModel& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCiftiParcelReorderingModel(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CiftiParcelReorderingModel::copyHelperCiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj)
{
    std::vector<CiftiParcelLabelFile*> compatibleParcelLabelFiles;
    CiftiParcelLabelFile* selectedParcelLabelFile;
    int32_t selectedParcelLabelFileMapIndex;
    bool enabledStatus;
    obj.getSelectedParcelLabelFileAndMapForReordering(compatibleParcelLabelFiles,
                                                      selectedParcelLabelFile,
                                                  selectedParcelLabelFileMapIndex,
                                                  enabledStatus);
    setSelectedParcelLabelFileAndMapForReordering(selectedParcelLabelFile,
                                                  selectedParcelLabelFileMapIndex,
                                                  enabledStatus);
}

/**
 * (1) Since the user may load/unload files at will, any current
 * selection requests will need to validate against available
 * parcel label files.
 *
 * (2) The parcel label files must test for compatibility by 
 * matching their CiftiParcelMaps.
 *
 * @return Parcel label files that contain at least one map and
 *         contain compatible CiftiParcelMaps.
 */
std::vector<CiftiParcelLabelFile*>
CiftiParcelReorderingModel::getParcelLabelFiles() const
{
    EventCaretMappableDataFilesGet parcelLabelFilesEvent(DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL);
    EventManager::get()->sendEvent(parcelLabelFilesEvent.getPointer());
    
    std::vector<CaretMappableDataFile*> mappableFiles;
    parcelLabelFilesEvent.getAllFiles(mappableFiles);
    
    std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
    for (std::vector<CaretMappableDataFile*>::iterator iter = mappableFiles.begin();
         iter != mappableFiles.end();
         iter++) {
        CaretMappableDataFile* cmdf = *iter;
        if (cmdf->getNumberOfMaps() > 0) {
            CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<CiftiParcelLabelFile*>(*iter);
            CaretAssert(parcelLabelFile);
            
            std::map<CiftiParcelLabelFile*, bool>::iterator fileStatusIter = m_parcelLabelFileCompatibilityStatus.find(parcelLabelFile);
            if (fileStatusIter != m_parcelLabelFileCompatibilityStatus.end()) {
                const bool compatibleFlag = fileStatusIter->second;
                if (compatibleFlag) {
                    /*
                     * File was previously verified for parcel compatibility.
                     */
                    parcelLabelFiles.push_back(parcelLabelFile);
                }
            }
            else {
                const CiftiParcelsMap* parcelsMap = parcelLabelFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_COLUMN);
                CaretAssert(parcelsMap);
                
                bool testAlongRow    = false;
                bool testAlongColumn = false;
                switch (m_parentCiftiMappableDataFile->getDataFileType()) {
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                        testAlongColumn = true;
                        testAlongRow    = true;
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                        testAlongColumn = true;
                        break;
                    case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                        testAlongColumn = true;
                        break;
                    default:
                        CaretAssert(0);
                }
                int32_t passCount = 0;
                int32_t failCount = 0;
                
                if (testAlongColumn) {
                    if (m_parentCiftiMappableDataFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_COLUMN)->approximateMatch(*parcelsMap)) {
                        ++passCount;
                    }
                    else {
                        ++failCount;
                    }
                }
                
                if (testAlongRow) {
                    if (m_parentCiftiMappableDataFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_ROW)->approximateMatch(*parcelsMap)) {
                        ++passCount;
                    }
                    else {
                        ++failCount;
                    }
                }
                
                bool compatibleFlag = false;
                if ((passCount > 0)
                    && (failCount <= 0)) {
                    parcelLabelFiles.push_back(parcelLabelFile);
                    
                    /*
                     * NOTE: Since file was found to be compatible it will
                     * always be compatible.
                     */
                    compatibleFlag = true;
                }
                /*
                 * NOTE: Compatiblity status will never change so cache it
                 * to avoid retesting.
                 */
                m_parcelLabelFileCompatibilityStatus.insert(std::make_pair(parcelLabelFile,
                                                                           compatibleFlag));
            }
        }
    }
    
    return parcelLabelFiles;
}

/**
 * Validate the selected parcel label file and map.
 *
 * @param optionalParcelLabelFilesOut
 *    If not NULL, the matching parcel labels files are inserted into this.
 */
void
CiftiParcelReorderingModel::validateSelectedParcelLabelFileAndMap(std::vector<CiftiParcelLabelFile*>* optionalParcelLabelFilesOut) const
{
    std::vector<CiftiParcelLabelFile*> parcelLabelFiles = getParcelLabelFiles();
    bool foundFile = false;
    for (std::vector<CiftiParcelLabelFile*>::iterator iter = parcelLabelFiles.begin();
         iter != parcelLabelFiles.end();
         iter++) {
        CiftiParcelLabelFile* plf = *iter;
        if (m_selectedParcelLabelFile != NULL) {
            if (m_selectedParcelLabelFile == plf) {
                foundFile = true;
                break;
            }
        }
    }
    
    if (foundFile) {
        const int32_t numMaps = m_selectedParcelLabelFile->getNumberOfMaps();
        if (m_selectedParcelLabelFileMapIndex >= numMaps) {
            m_selectedParcelLabelFileMapIndex = numMaps - 1;
        }
        else if (m_selectedParcelLabelFileMapIndex < 0) {
            m_selectedParcelLabelFileMapIndex = 0;
        }
    }
    
    if ( ! foundFile) {
        if ( ! parcelLabelFiles.empty()) {
            m_selectedParcelLabelFile = parcelLabelFiles[0];
            m_selectedParcelLabelFileMapIndex = 0;
        }
        else {
            m_selectedParcelLabelFile         = NULL;
            m_selectedParcelLabelFileMapIndex = -1;
            m_parcelReorderingEnabledStatus   = false;
        }
    }
    
    if (optionalParcelLabelFilesOut != NULL) {
        *optionalParcelLabelFilesOut = parcelLabelFiles;
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CiftiParcelReorderingModel::toString() const
{
    return "CiftiParcelReorderingModel";
}

/**
 * Get the selected parcel label file used for reordering of parcels.
 *
 * @param parcelLabelFilesOut
 *    The compatible parcel label files.
 * @param selectedParcelLabelFileOut
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndexOut
 *    Map index in the selected parcel label file.
 * @param enabledStatusOut
 *    Enabled status of reordering.
 */
void
CiftiParcelReorderingModel::getSelectedParcelLabelFileAndMapForReordering(std::vector<CiftiParcelLabelFile*>& parcelLabelFilesOut,
                                                                          CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                                          int32_t& selectedParcelLabelFileMapIndexOut,
                                                                          bool& enabledStatusOut) const
{
    validateSelectedParcelLabelFileAndMap(&parcelLabelFilesOut);

    selectedParcelLabelFileOut = m_selectedParcelLabelFile;
    selectedParcelLabelFileMapIndexOut = m_selectedParcelLabelFileMapIndex;
    enabledStatusOut = m_parcelReorderingEnabledStatus;
}

/**
 * Set the selected parcel label file used for reordering of parcels.
 *
 * @param selectedParcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @param enabledStatus
 *    Enabled status of reordering.
 */
void
CiftiParcelReorderingModel::setSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* selectedParcelLabelFile,
                                                                          const int32_t selectedParcelLabelFileMapIndex,
                                                                          const bool enabledStatus)
{
    m_selectedParcelLabelFile         = selectedParcelLabelFile;
    m_selectedParcelLabelFileMapIndex = selectedParcelLabelFileMapIndex;
    m_parcelReorderingEnabledStatus   = enabledStatus;
}

/**
 * Get the parcel reordering for the given map index that was created using
 * the given parcel label file and its map index.
 *
 * @param parcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 * @param parcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
const CiftiParcelReordering*
CiftiParcelReorderingModel::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                 const int32_t parcelLabelFileMapIndex) const
{
    for (std::vector<CiftiParcelReordering*>::const_iterator iter = m_parcelReordering.begin();
         iter != m_parcelReordering.end();
         iter++) {
        const CiftiParcelReordering* parcelReordering = *iter;
        
        if (parcelReordering->isMatch(parcelLabelFile,
                                      parcelLabelFileMapIndex)) {
            return parcelReordering;
        }
    }
    
    return NULL;
}

/**
 * Get the parcel reordering for the given map index that was created using
 * the given parcel label file and its map index.
 *
 * @param parcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 * @param parcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
CiftiParcelReordering*
CiftiParcelReorderingModel::getParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                const int32_t parcelLabelFileMapIndex)
{
    for (std::vector<CiftiParcelReordering*>::iterator iter = m_parcelReordering.begin();
         iter != m_parcelReordering.end();
         iter++) {
        CiftiParcelReordering* parcelReordering = *iter;
        
        if (parcelReordering->isMatch(parcelLabelFile,
                                      parcelLabelFileMapIndex)) {
            return parcelReordering;
        }
    }
    
    return NULL;
}

/**
 * Create the parcel reordering for the parent file's parcels map index using
 * the given parcel label file and its map index.
 *
 * @param parcelLabelFile
 *    The selected parcel label file used for reordering the parcels.
 * @param parcelLabelFileMapIndex
 *    Map index in the selected parcel label file.
 * @param errorMessageOut
 *    Error message output.  Will only be non-empty if NULL is returned.
 * @return
 *    Pointer to parcel reordering or NULL if not found.
 */
bool
CiftiParcelReorderingModel::createParcelReordering(const CiftiParcelLabelFile* parcelLabelFile,
                                                   const int32_t parcelLabelFileMapIndex,
                                                   AString& errorMessageOut)
{
    if (getParcelReordering(parcelLabelFile,
                            parcelLabelFileMapIndex) != NULL) {
        return true;
    }
    
    const CiftiParcelsMap* ciftiParcelsMap = m_parentCiftiMappableDataFile->getCiftiParcelsMapForBrainordinateMapping();
    CaretAssert(ciftiParcelsMap);
    
    CiftiParcelReordering* parcelReordering = new CiftiParcelReordering();
    if (parcelReordering->createReordering(parcelLabelFile,
                                           parcelLabelFileMapIndex,
                                           *ciftiParcelsMap,
                                           errorMessageOut)) {
        m_parcelReordering.push_back(parcelReordering);
        return true;
    }
    
    return false;
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
CiftiParcelReorderingModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    validateSelectedParcelLabelFileAndMap(NULL);
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiParcelReorderingModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_selectedParcelLabelFile != NULL) {
        sceneClass->addPathName("m_selectedParcelLabelFile",
                                m_selectedParcelLabelFile->getFileName());
    }

    /*
     * NOTE: the parcel reorderings are not saved to the scene.
     * When the scene is restored, a parcel reordering is created
     * for the restored parcel label file and map index.
     */
    
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
CiftiParcelReorderingModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_selectedParcelLabelFile = NULL;
    clearCiftiParcelReordering();
    
    const AString parcelLabelFileName = sceneClass->getPathNameValue("m_selectedParcelLabelFile");
    if ( ! parcelLabelFileName.isEmpty()) {
        std::vector<CiftiParcelLabelFile*> parcelLabelFiles = getParcelLabelFiles();
        for (std::vector<CiftiParcelLabelFile*>::iterator iter = parcelLabelFiles.begin();
             iter != parcelLabelFiles.end();
             iter++) {
            CiftiParcelLabelFile* plf = *iter;
            if (plf->getFileName() == parcelLabelFileName) {
                m_selectedParcelLabelFile = plf;
                break;
            }
        }
    }
    
    validateSelectedParcelLabelFileAndMap(NULL);

    /*
     * If there is a valid selected parcel label file,
     * create the reordering for it.
     */
    if (m_selectedParcelLabelFile != NULL) {
        if (m_selectedParcelLabelFileMapIndex >= 0) {
            AString errorMessage;
            if ( ! createParcelReordering(m_selectedParcelLabelFile,
                                          m_selectedParcelLabelFileMapIndex,
                                          errorMessage)) {
                sceneAttributes->addToErrorMessage(errorMessage);
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

