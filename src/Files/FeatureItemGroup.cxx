
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_ITEM_GROUP_DECLARE__
#include "FeatureItemGroup.h"
#undef __FEATURE_ITEM_GROUP_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FeatureItem.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::FeatureItemGroup 
 * \brief Groups features together in a model
 * \ingroup Files
 */

/**
 * @return The invalid relationship ID
 */
uint64_t
FeatureItemGroup::getDefaultGroupID()
{
    /*
     * Note use the signed 32-bit maximum integer value for
     * the default group.  OpenGL identification does not support
     * 64-bit values.
     */
    const int32_t maxSigned32(std::numeric_limits<int32_t>::max());
    const uint64_t unsigned64(static_cast<uint64_t>(maxSigned32));
    return unsigned64;
}


/**
 * Constructor.
 */
FeatureItemGroup::FeatureItemGroup(const uint64_t groupID)
: FeatureBase(FeatureBase::BaseType::FEATURE_GROUP),
m_groupID(groupID)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    AString groupName((m_groupID == std::numeric_limits<uint64_t>::max())
                      ? "Default"
                      : ("Group " + AString::number(m_groupID)));
    setText(groupName);
    
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    
    setCheckable(true);
    setCheckState(Qt::Checked);
}

/**
 * Destructor.
 */
FeatureItemGroup::~FeatureItemGroup()
{
}

/**
 * @return The group ID*/
uint64_t
FeatureItemGroup::getGroupID() const
{
    return m_groupID;
}

/**
 * Add a feature to this model.  If there is
 * a feature already in the model with the matching unique ID, the
 * featureAndProperties are NOT added and are destroyed.
 *
 * @param featureAndProperties
 *    The feature is first and then the properties.  Caller MUST NOT
 *    reference featureAndProperties after calling this function as
 *    they could be destroyed immediately or at a later time.
 * @return
 *   A FunctionResult with success or failure.
 */
FunctionResult
FeatureItemGroup::addFeature(QList<QStandardItem*>& featureAndProperties)
{

    AString errorMessage;
    CaretAssertVectorIndex(featureAndProperties, 0);
    FeatureItem* featureItem(dynamic_cast<FeatureItem*>(featureAndProperties[0]));
    CaretAssert(featureItem);
    
    const uint64_t uniqueID(featureItem->getUniqueID());
    const auto result(m_uniqueIdToFeatureMap.insert(std::make_pair(uniqueID,
                                                                   featureItem)));
    if (result.second) {
        appendRow(featureAndProperties);
        
        /*
         * This is lazy initialized and contains all features.
         * We could add the features here but featues could
         * be deleted in the future.
         */
        m_allFeatures.clear();
    }
    else {
        errorMessage = ("Feature with uniqueID="
                        + AString::number(uniqueID)
                        + " exists in model with groupID="
                        + AString::number(m_groupID)
                        + ".  Feature has been discarded.");
        FeatureBase::destroyFeatureAndProperties(featureAndProperties);
    }
    
    return FunctionResult(errorMessage,
                          errorMessage.isEmpty());
    
}

/**
 * @return All features in this group
 */
const std::vector<const FeatureItem*>&
FeatureItemGroup::getAllFeatures() const
{
    if (m_allFeatures.empty()) {
        m_allFeatures.reserve(m_uniqueIdToFeatureMap.size());
        for (const auto& iter : m_uniqueIdToFeatureMap) {
            m_allFeatures.push_back(iter.second);
        }
    }
    return m_allFeatures;
}

/**
 * @return FeatureItem with given unique ID or NULL if not found
 * @param uniqueID
 *    Unique ID of feature
 * @return
 *    FeatureItem with filename or NULL if not found
 */
FeatureItem*
FeatureItemGroup::getFeatureWithUniqueID(const uint64_t uniqueID)
{
    FeatureItem* featureItem(NULL);
    const auto iter(m_uniqueIdToFeatureMap.find(uniqueID));
    if (iter != m_uniqueIdToFeatureMap.end()) {
        featureItem = iter->second;
    }
    return featureItem;
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
FeatureItemGroup::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FeatureItemGroup",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    const bool checkedFlag(checkState() == Qt::Checked);
    sceneClass->addBoolean("checkedFlag",
                           checkedFlag);

    SceneObjectMapIntegerKey* featureSceneMap(new SceneObjectMapIntegerKey("featureSceneMap",
                                                                         SceneObjectDataTypeEnum::SCENE_CLASS));
    
    for (const auto& featureIter : m_uniqueIdToFeatureMap) {
        const uint64_t featureID(featureIter.first);
        const AString featureClassName("FeatureItem_"
                                     + AString::number(featureID));
        featureSceneMap->addClass(featureID,
                                  featureIter.second->saveToScene(sceneAttributes,                                                                  featureClassName));
    }

    sceneClass->addChild(featureSceneMap);
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
FeatureItemGroup::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const bool defaultValue(true);
    const bool checkedFlag(sceneClass->getBooleanValue("checkedFlag",
                                                       defaultValue));
    if (checkedFlag) {
        setCheckState(Qt::Checked);
    }
    else {
        setCheckState(Qt::Unchecked);
    }

    const SceneObjectMapIntegerKey* featureSceneMap(sceneClass->getMapIntegerKey("featureSceneMap"));
    if (featureSceneMap != NULL) {
        const std::vector<int32_t> allFeatureIDs(featureSceneMap->getKeys());
        for (const int32_t& uniqueID : allFeatureIDs) {
            const SceneClass* featureScene(featureSceneMap->classValue(uniqueID));
            if (featureScene != NULL) {
                FeatureItem* featureItem(getFeatureWithUniqueID(uniqueID));
                if (featureItem != NULL) {
                    featureItem->restoreFromScene(sceneAttributes,
                                                  featureScene);
                }
                else {
                    CaretLogWarning("Failed to find Feature with Unique ID="
                                    + AString::number(uniqueID)
                                    + " for restoring scene.");
                }
            }
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
FeatureItemGroup::toString() const
{
    AString txt("id=" +
                AString::number(m_groupID));
    return txt;
}

