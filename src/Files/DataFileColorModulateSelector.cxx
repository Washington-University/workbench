
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __DATA_FILE_COLOR_MODULATE_SELECTOR_DECLARE__
#include "DataFileColorModulateSelector.h"
#undef __DATA_FILE_COLOR_MODULATE_SELECTOR_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "SceneClassAssistant.h"
#include "SceneClass.h"
#include "VolumeFile.h"

using namespace caret;
    
/**
 * \class caret::DataFileColorModulateSelector 
 * \brief Items used to modulate a file's coloring with another file
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mapFile
 *    File that is modulated
 * @param mapIndex
 *    Index of map
 */
DataFileColorModulateSelector::DataFileColorModulateSelector(CaretMappableDataFile* mapFile)
: CaretObject()
{
    const bool excludeSelfFlag(true);
    m_selectionModel.reset(new CaretMappableDataFileAndMapSelectionModel(mapFile,
                                                                         excludeSelfFlag));
    
    m_sceneAssistant.reset(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabledFlag",
                          &m_enabledFlag);
}

/**
 * Destructor.
 */
DataFileColorModulateSelector::~DataFileColorModulateSelector()
{
}

/**
 * @return Pointer to selected mappable file selected, else NULL.
 */
const CaretMappableDataFile*
DataFileColorModulateSelector::getSelectedMapFile() const
{
    return m_selectionModel->getSelectedFile();
}

/**
 * @return Pointer to volume file if a volume file is selected, else NULL.
 */
const VolumeFile*
DataFileColorModulateSelector::getSelectedVolumeFile() const
{
    const VolumeFile* volumeFile(NULL);
    
    const CaretMappableDataFile* mapFile(getSelectedMapFile());
    if (mapFile != NULL) {
        volumeFile = dynamic_cast<const VolumeFile*>(mapFile);
    }
    
    return volumeFile;
}

/**
 * @return Index of map in selected file or negative if no file selected
 */
int32_t
DataFileColorModulateSelector::getSelectedMapIndex() const
{
    return m_selectionModel->getSelectedMapIndex();
}

/**
 * @return The selection model
 */
CaretMappableDataFileAndMapSelectionModel*
DataFileColorModulateSelector::getSelectionModel()
{
    return m_selectionModel.get();
}

/**
 * @return The selection model
 */
const CaretMappableDataFileAndMapSelectionModel*
DataFileColorModulateSelector::getSelectionModel() const
{
    return m_selectionModel.get();
}

/**
 * @return True if enabled
 */
bool
DataFileColorModulateSelector::isEnabled() const
{
    return m_enabledFlag;
}

/**
 * Set enabled status
 * @param enabled
 *    New enabled status
 */
void
DataFileColorModulateSelector::setEnabled(const bool enabled)
{
    m_enabledFlag = enabled;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
DataFileColorModulateSelector::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DataFileColorModulateSelector",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    sceneClass->addClass(m_selectionModel->saveToScene(sceneAttributes,
                                                       "m_selectionModel"));
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
DataFileColorModulateSelector::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    m_selectionModel->restoreFromScene(sceneAttributes,
                                       sceneClass->getClass("m_selectionModel"));
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DataFileColorModulateSelector::toString() const
{
    return "DataFileColorModulateSelector";
}

