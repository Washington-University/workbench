
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
 *  but WITHOUT ANY WARRANTY; without even the implied warranty ofFeatureModel
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __FEATURE_ITEM_MODEL_DECLARE__
#include "FeatureItemModel.h"
#undef __FEATURE_ITEM_MODEL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "FeatureItem.h"
#include "FeatureItemGroup.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;


    
/**
 * \class caret::FeatureItemModel 
 * \brief Model that contains FeatureItems
 * \ingroup Files
 */

/**
 * Constructor.
 */
FeatureItemModel::FeatureItemModel()
: QStandardItemModel()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    

    const bool getCheckBoxChangedNotificationFlag(false);
    if (getCheckBoxChangedNotificationFlag) {
        QObject::connect(this, &FeatureItemModel::itemChanged,
                         [=](QStandardItem* item) {
            std::cout << "Data changed: " << item->text() << std::endl;
        });
    }
}

/**
 * Destructor.
 */
FeatureItemModel::~FeatureItemModel()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
FeatureItemModel::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
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
FeatureItemModel::addFeature(QList<QStandardItem*>& featureAndProperties)
{
    for (const auto& qsi : featureAndProperties) {
        CaretAssert(qsi);
    }
    CaretAssertVectorIndex(featureAndProperties, 0);
    FeatureItem* featureItem(dynamic_cast<FeatureItem*>(featureAndProperties[0]));
    CaretAssert(featureItem);
    
    AString errorMessage;
    
    if (columnCount() > 1) {
        if (featureAndProperties.size() != columnCount()) {
            errorMessage = ("Model contains "
                            + AString::number(columnCount())
                            + " columns but new feature with unique id="
                            + AString::number(featureItem->getUniqueID())
                            + " contains "
                            + AString::number(featureAndProperties.size())
                            + " columns.  Feature has been discarded.");
        }
    }
    
    if (errorMessage.isEmpty()) {
        const uint64_t groupID(featureItem->getGroupID());
        
        FeatureItemGroup* featureItemGroup(NULL);
        const auto groupIter(m_groupIdToFeatureGroupMap.find(groupID));
        if (groupIter != m_groupIdToFeatureGroupMap.end()) {
            /*
             * Add to existing group
             */
            featureItemGroup = groupIter->second;
        }
        else {
            /*
             * Add to new group
             */
            featureItemGroup = new FeatureItemGroup(groupID);
            invisibleRootItem()->appendRow(featureItemGroup);
            m_groupIdToFeatureGroupMap.insert(std::make_pair(groupID,
                                                             featureItemGroup));
        }
        
        FunctionResult result(featureItemGroup->addFeature(featureAndProperties));
        if (result.isError()) {
            errorMessage = result.getErrorMessage();
        }
    }

    if ( ! errorMessage.isEmpty()) {
        /*
         * If there is an error, featureAndProperties were
         * not added to the model so destroy them.
         */
        for (auto& fp : featureAndProperties) {
            delete fp;
        }
        CaretLogWarning(errorMessage);
    }
    
    return FunctionResult(errorMessage,
                          errorMessage.isEmpty());
}

/**
 * @return All feature groups in this model
 */
std::vector<const FeatureItemGroup*>
FeatureItemModel::getAllFeatureGroups() const
{
    std::vector<const FeatureItemGroup*> allFeatureGroups;
    
    for (const auto iter : m_groupIdToFeatureGroupMap) {
        allFeatureGroups.push_back(iter.second);
    }
    
    return allFeatureGroups;
}

/**
 * @return FeatureItemGroup with given group ID or NULL if not found
 * @param groupID
 *    The group ID
 */
FeatureItemGroup*
FeatureItemModel::getFeatureItemGroupWithID(const uint64_t groupID)
{
    FeatureItemGroup* featureGroupOut(NULL);
    
    const auto groupIter(m_groupIdToFeatureGroupMap.find(groupID));
    if (groupIter != m_groupIdToFeatureGroupMap.end()) {
        featureGroupOut = groupIter->second;
        CaretAssert(featureGroupOut);
    }
    return featureGroupOut;
}


/**
 * @return FeatureItem with given group and unique ID or NULL if not found
 * @param groupID
 *    Group ID of feature
 * @param uniqueID
 *    Unique ID of feature
 * @return
 *    FeatureItem with filename or NULL if not found
 */
FeatureItem*
FeatureItemModel::getFeatureWithGroupAndUniqueID(const uint64_t groupID,
                                                 const uint64_t uniqueID)
{
    FeatureItem* featureItemOut(NULL);
    
    FeatureItemGroup* featureItemGroup(getFeatureItemGroupWithID(groupID));
    if (featureItemGroup != NULL) {
        featureItemOut = featureItemGroup->getFeatureWithUniqueID(uniqueID);
    }
    
    return featureItemOut;
}


/**
 * Set checked status of all feature items in this file
 * @param checked
 *    If true, display all.
 */
void
FeatureItemModel::setCheckedStatusOfAllItems(const bool checked)
{
    QStandardItem* rootItem(invisibleRootItem());
    const int32_t numChildren(rootItem->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* childItem(rootItem->child(iRow));
        FeatureItemGroup* groupItem(dynamic_cast<FeatureItemGroup*>(childItem));
        groupItem->setAllChildrenChecked(checked);
        CaretAssert(groupItem);
        groupItem->setCheckState(checked
                                 ? Qt::Checked
                                 : Qt::Unchecked);
    }

    updateCheckedStateOfAllItems();
}

/**
 * Update the checked state of all items
 */
void
FeatureItemModel::updateCheckedStateOfAllItems()
{
    QStandardItem* rootItem(invisibleRootItem());
    const int32_t numChildren(rootItem->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* childItem(rootItem->child(iRow));
        FeatureBase* featureBase(dynamic_cast<FeatureBase*>(childItem));
        CaretAssert(featureBase);
        featureBase->setCheckStateFromChildren();
    }
}

/**
 * Set the header labels for the model
 * @param horizontalHeaderLabels
 *    Labels for the horizontal header
 */
void
FeatureItemModel::setHeaderLabels(const QStringList& horizontalHeaderLabels)
{
    /*
     * Note: Cannot set vertical labels in a tree model (must be a table model)
     */
    setHorizontalHeaderLabels(horizontalHeaderLabels);
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
FeatureItemModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    /*
     * cannot use feature filename as key since features stored in
     * a sharded file that contains may features.  Maybe use
     * shardIndex of feature
     */
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FeatureItemModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    SceneObjectMapIntegerKey* groupSceneMap(new SceneObjectMapIntegerKey("groupSceneMap",
                                                                         SceneObjectDataTypeEnum::SCENE_CLASS));
    
    for (const auto& groupIter : m_groupIdToFeatureGroupMap) {
        const uint64_t groupID(groupIter.first);
        const AString groupClassName("FeatureGroup_"
                                     + AString::number(groupID));
        std::cout << "Saving to scene: " << groupClassName << std::endl;
        groupSceneMap->addClass(groupID,
                                groupIter.second->saveToScene(sceneAttributes,
                                                              groupClassName));
    }
    
    sceneClass->addChild(groupSceneMap);

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
FeatureItemModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const SceneObjectMapIntegerKey* groupSceneMap(sceneClass->getMapIntegerKey("groupSceneMap"));
    if (groupSceneMap != NULL) {
        const std::vector<int32_t> allGroupIDs(groupSceneMap->getKeys());
        for (const int32_t& groupID : allGroupIDs) {
            const SceneClass* groupScene(groupSceneMap->classValue(groupID));
            if (groupScene != NULL) {
                FeatureItemGroup* featureItemGroup(getFeatureItemGroupWithID(groupID));
                if (featureItemGroup != NULL) {
                    featureItemGroup->restoreFromScene(sceneAttributes,
                                                       groupScene);
                }
                else {
                    CaretLogWarning("Failed to find Group with ID="
                                    + AString::number(groupID)
                                    + " for restoring scene.");
                }
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    updateCheckedStateOfAllItems();
}

