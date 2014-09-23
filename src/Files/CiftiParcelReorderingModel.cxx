
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
#include "CiftiParcelLabelFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePathName.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelReorderingModel 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Files
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
CiftiParcelReorderingModel::CiftiParcelReorderingModel()
: CaretObject()
{
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
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CiftiParcelReorderingModel::CiftiParcelReorderingModel(const CiftiParcelReorderingModel& obj)
: CaretObject(obj)
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
    CiftiParcelLabelFile* selectedParcelLabelFile;
    int32_t selectedParcelLabelFileMapIndex;
    bool enabledStatus;
    obj.getSelectedParcelLabelFileAndMapForReordering(selectedParcelLabelFile,
                                                  selectedParcelLabelFileMapIndex,
                                                  enabledStatus);
    setSelectedParcelLabelFileAndMapForReordering(selectedParcelLabelFile,
                                                  selectedParcelLabelFileMapIndex,
                                                  enabledStatus);
}

/**
 * @return Parcel label files that contain at least one map.
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
            CiftiParcelLabelFile* plf = dynamic_cast<CiftiParcelLabelFile*>(*iter);
            CaretAssert(plf);
            parcelLabelFiles.push_back(plf);
        }
    }
    
    return parcelLabelFiles;
}

/**
 * Validate the selected parcel label file and map.
 */
void
CiftiParcelReorderingModel::validateSelectedParcelLabelFileAndMap() const
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
 * @param selectedParcelLabelFileOut
 *    The selected parcel label file used for reordering the parcels.
 *    May be NULL!
 * @param selectedParcelLabelFileMapIndexOut
 *    Map index in the selected parcel label file.
 * @param enabledStatusOut
 *    Enabled status of reordering.
 */
void
CiftiParcelReorderingModel::getSelectedParcelLabelFileAndMapForReordering(CiftiParcelLabelFile* &selectedParcelLabelFileOut,
                                                                          int32_t& selectedParcelLabelFileMapIndexOut,
                                                                          bool& enabledStatusOut) const
{
    validateSelectedParcelLabelFileAndMap();

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
                                                               int32_t& selectedParcelLabelFileMapIndex,
                                                               bool& enabledStatus)
{
    m_selectedParcelLabelFile         = selectedParcelLabelFile;
    m_selectedParcelLabelFileMapIndex = selectedParcelLabelFileMapIndex;
    m_parcelReorderingEnabledStatus   = enabledStatus;
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
    validateSelectedParcelLabelFileAndMap();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CiftiParcelReorderingModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_selectedParcelLabelFile != NULL) {
        sceneClass->addPathName("m_selectedParcelLabelFile",
                                m_selectedParcelLabelFile->getFileName());
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
CiftiParcelReorderingModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const AString parcelLabelFileName = sceneClass->getPathNameValue("m_selectedParcelLabelFile");
    
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
    
    validateSelectedParcelLabelFileAndMap();

    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

