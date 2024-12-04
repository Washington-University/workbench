
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

#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_DECLARE__
#include "AnnotationTextSubstitutionLayer.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_DECLARE__

#include <algorithm>

#include "AnnotationTextSubstitutionFile.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionLayer 
 * \brief Model of row in annotation substitution layer
 * \ingroup Brain
 */

/**
 * Constructor.
 */
AnnotationTextSubstitutionLayer::AnnotationTextSubstitutionLayer()
: CaretObject(), EventListenerInterface(), SceneableInterface()
{
    m_enabled             = false;
    m_groupIdentifer      = "";
    m_mapYokingGroup      = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    m_substitutionFileRowIndex = 0;
    m_substitutionFileSelectionModel.reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileType(DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION));
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabled", &m_enabled);
    m_sceneAssistant->add("m_groupIdentifer", &m_groupIdentifer);
    m_sceneAssistant->add<MapYokingGroupEnum, MapYokingGroupEnum::Enum>("m_mapYokingGroup", &m_mapYokingGroup);
    m_sceneAssistant->add("m_substitutionFileRowIndex", &m_substitutionFileRowIndex);
    m_sceneAssistant->add("m_substitutionFileSelectionModel", "CaretDataFileSelectionModel", m_substitutionFileSelectionModel.get());
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionLayer::~AnnotationTextSubstitutionLayer()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationTextSubstitutionLayer::AnnotationTextSubstitutionLayer(const AnnotationTextSubstitutionLayer& obj)
: CaretObject(obj),
EventListenerInterface(),
SceneableInterface(obj)
{
    this->copyHelperAnnotationTextSubstitutionLayer(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationTextSubstitutionLayer&
AnnotationTextSubstitutionLayer::operator=(const AnnotationTextSubstitutionLayer& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationTextSubstitutionLayer(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationTextSubstitutionLayer::copyHelperAnnotationTextSubstitutionLayer(const AnnotationTextSubstitutionLayer& obj)
{
    m_enabled = obj.m_enabled;
    m_allGroupIdentifiers = obj.m_allGroupIdentifiers;
    m_groupIdentifer = obj.m_groupIdentifer;
    m_mapYokingGroup = obj.m_mapYokingGroup;
    m_substitutionFileRowIndex = obj.m_substitutionFileRowIndex;
    *m_substitutionFileSelectionModel = *obj.m_substitutionFileSelectionModel;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
AnnotationTextSubstitutionLayer::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        EventMapYokingSelectMap* mapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(mapEvent);
        
        if (mapEvent->getMapYokingGroup() == m_mapYokingGroup) {
            setSubstitutionFileRowIndex(mapEvent->getMapIndex());
        }
        
        event->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        EventMapYokingValidation* yokeEvent(dynamic_cast<EventMapYokingValidation*>(event));
        CaretAssert(yokeEvent);
        
        if (m_mapYokingGroup == yokeEvent->getMapYokingGroup()) {
            AnnotationTextSubstitutionFile* annTextSubsFile(getSelectedSubstitutionFile());
            if (annTextSubsFile != NULL) {
                if (m_mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
                    yokeEvent->addAnnotationTextSubstitutionFile(annTextSubsFile, m_mapYokingGroup);
                }
            }
        }
    }
}

/**
 * Update this instance
 * @param groupIDs
 *    Group identifiers found in all text annotations
 */
void
AnnotationTextSubstitutionLayer::updateContent(const std::vector<AString>& groupIDs)
{
    m_allGroupIdentifiers = groupIDs;
    m_allGroupIdentifiers.push_back(getNoGroupIdentifierText());
    if (std::find(m_allGroupIdentifiers.begin(),
                  m_allGroupIdentifiers.end(),
                  m_groupIdentifer) == m_allGroupIdentifiers.end()) {
        if (m_allGroupIdentifiers.empty()) {
            m_groupIdentifer = "";
        }
        else {
            CaretAssertVectorIndex(m_allGroupIdentifiers, 0);
            m_groupIdentifer = m_allGroupIdentifiers[0];
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationTextSubstitutionLayer::toString() const
{
    return "AnnotationTextSubstitutionLayer";
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
AnnotationTextSubstitutionLayer::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationTextSubstitutionLayer",
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
AnnotationTextSubstitutionLayer::restoreFromScene(const SceneAttributes* sceneAttributes,
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

/**
 * @return True if this layers enabled and has valid selections for text substitution
 */
bool
AnnotationTextSubstitutionLayer::isSubstitutionsValid() const
{
    if (m_enabled) {
        if ( ! m_groupIdentifer.isEmpty()) {
            if (m_substitutionFileSelectionModel->getSelectedFile() != NULL) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @return enabled status
 */
bool
AnnotationTextSubstitutionLayer::isEnabled() const
{
    return m_enabled;
}

/**
 * Set enabled status
 *
 * @param enabled
 *    New value for enabled status
 */
void
AnnotationTextSubstitutionLayer::setEnabled(const bool enabled)
{
    m_enabled = enabled;
}

/**
 * @return All available group identifiers
 */
const std::vector<AString>& 
AnnotationTextSubstitutionLayer::getAllGroupIdentifiers() const
{
    return m_allGroupIdentifiers;
}

/**
 * @return Group identifier in annotation substution text
 */
AString
AnnotationTextSubstitutionLayer::getGroupIdentifer() const
{
    return m_groupIdentifer;
}

/**
 * Set group identifier in annotation substution text
 *
 * @param groupIdentifer
 *    New value for group identifier in annotation substution text
 */
void
AnnotationTextSubstitutionLayer::setGroupIdentifer(const AString& groupIdentifer)
{
    m_groupIdentifer = groupIdentifer;
}

/**
 * @return The text for when no group is used
 */
AString
AnnotationTextSubstitutionLayer::getNoGroupIdentifierText()
{
    return "No-Group";
}


/**
 * @return map yoking group
 */
MapYokingGroupEnum::Enum
AnnotationTextSubstitutionLayer::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * Set map yoking group
 *
 * @param mapYokingGroup
 *    New value for map yoking group
 */
void
AnnotationTextSubstitutionLayer::setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup)
{
    m_mapYokingGroup = mapYokingGroup;
}

/**
 * @return selected row index in annotation substitution file
 */
int32_t
AnnotationTextSubstitutionLayer::getSubstitutionFileRowIndex() const
{
    return m_substitutionFileRowIndex;
}

/**
 * Set selected row index in annotation substitution file
 *
 * @param substitutionFileRowIndex
 *    New value for selected row index in annotation substitution file
 */
void
AnnotationTextSubstitutionLayer::setSubstitutionFileRowIndex(const int32_t substitutionFileRowIndex)
{
    m_substitutionFileRowIndex = substitutionFileRowIndex;
}

/**
 * @return Pointer to selected substitution file or NULL is not available
 */
AnnotationTextSubstitutionFile*
AnnotationTextSubstitutionLayer::getSelectedSubstitutionFile()
{
    AnnotationTextSubstitutionFile* asf(m_substitutionFileSelectionModel->getSelectedFileOfType<AnnotationTextSubstitutionFile>());
    return asf;
}

/**
 * @return Pointer to selected substitution file or NULL is not available
 */
const AnnotationTextSubstitutionFile*
AnnotationTextSubstitutionLayer::getSelectedSubstitutionFile() const
{
    const AnnotationTextSubstitutionFile* asf(m_substitutionFileSelectionModel->getSelectedFileOfType<const AnnotationTextSubstitutionFile>());
    return asf;
}

/**
 * @return annotation file selection model
 */
const CaretDataFileSelectionModel*
AnnotationTextSubstitutionLayer::getSubstitutionFileSelectionModel() const
{
    return m_substitutionFileSelectionModel.get();
}

/**
 * @return annotation file selection model
 */
CaretDataFileSelectionModel*
AnnotationTextSubstitutionLayer::getSubstitutionFileSelectionModel()
{
    return m_substitutionFileSelectionModel.get();
}


