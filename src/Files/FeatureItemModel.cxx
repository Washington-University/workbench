
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
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
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
FeatureItemModel::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Add a feature and its properties
 * @param uniqueID
 *    Unique ID of feature
 * @param featureAndProperties
 *    The feature is first and then the properties
 */
void
FeatureItemModel::addFeature(const uint64_t uniqueID,
                             const QList<QStandardItem*>& featureAndProperties)
{
    if (m_uniqueIdToRowMap.find(uniqueID) != m_uniqueIdToRowMap.end()) {
        CaretLogWarning("Feature with uniqueID="
                        + AString::number(uniqueID)
                        + " exists.  Feature and properties not added to model.");
        for (auto& fp : featureAndProperties) {
            delete fp;
        }
    }
    m_uniqueIdToRowMap.insert(std::make_pair(uniqueID,
                                             rowCount()));
    appendRow(featureAndProperties);
}

/**
 * @return Number of features
 */
int32_t
FeatureItemModel::getNumberOfFeatures() const
{
    return rowCount();
}

/**
 * @return Feature at the given index or NULL if not found
 * @param index
 *    Index of feature
 * @return
 *    Feature at index or NULL if not found
 */
FeatureItem*
FeatureItemModel::getFeatureAtIndex(const int32_t index)
{
    const int32_t column(0);
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* standardItem(item(index, column));
    CaretAssert(standardItem);
    FeatureItem* featureItem(dynamic_cast<FeatureItem*>(standardItem));
    CaretAssert(featureItem);
    return featureItem;
}

/**
 * @return Feature at the given index or NULL if not found (const method)
 * @param index
 *    Index of feature
 * @return
 *    Feature at index or NULL if not found
 */
const FeatureItem*
FeatureItemModel::getFeatureAtIndex(const int32_t index) const
{
    const int32_t column(0);
    CaretAssert((index >= 0)
                && (index < rowCount()));
    const QStandardItem* standardItem(item(index, column));
    CaretAssert(standardItem);
    const FeatureItem* featureItem(dynamic_cast<const FeatureItem*>(standardItem));
    CaretAssert(featureItem);
    return featureItem;
}

/**
 * @return FeatureItem with given unique ID or NULL if not found
 * @param uniqueID
 *    Unique ID of feature
 * @return
 *    FeatureItem with filename or NULL if not found
 */
FeatureItem*
FeatureItemModel::getFeatureWithUniqueID(const uint64_t uniqueID)
{
    FeatureItem* featureItem(NULL);
    const auto iter(m_uniqueIdToRowMap.find(uniqueID));
    if (iter != m_uniqueIdToRowMap.end()) {
        const int32_t rowIndex(iter->second);
        featureItem = getFeatureAtIndex(rowIndex);
    }
    return featureItem;
}


/**
 * Set display status of all feature items in this file
 * @param displayStatus
 *    If true, display all.
 */
void
FeatureItemModel::setAllFeaturesDisplayed(const bool displayStatus)
{
    const int32_t column(0);
    const Qt::CheckState checkState(displayStatus
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    const int32_t num(rowCount());
    for (int32_t i = 0; i < num; i++) {
        item(i, column)->setCheckState(checkState);
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
    setHorizontalHeaderLabels(horizontalHeaderLabels);
    
    QStringList verticalHeaderLabels;
    const int32_t num(getNumberOfFeatures());
    for (int32_t i = 0; i < num; i++) {
        verticalHeaderLabels.push_back(getFeatureAtIndex(i)->getUniqueIdAsString());
    }
    setVerticalHeaderLabels(verticalHeaderLabels);
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
    
    SceneObjectMapIntegerKey* featureMap(new SceneObjectMapIntegerKey("featureItemMap",
                                                                    SceneObjectDataTypeEnum::SCENE_CLASS));
    const int32_t num(getNumberOfFeatures());
    for (int32_t i = 0; i < num; i++) {
        FeatureItem* featureItem(getFeatureAtIndex(i));
        const AString className("FeatureItem"
                                + AString::number(i));
        featureMap->addClass(featureItem->getUniqueID(),
                             featureItem->saveToScene(sceneAttributes,
                                                      className));
    }
    
    sceneClass->addChild(featureMap);
    
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
    
    const SceneObjectMapIntegerKey* featureMap = sceneClass->getMapIntegerKey("featureItemMap");
    if (featureMap != NULL) {
        const std::vector<int32_t> allKeys(featureMap->getKeys());
        for (const int32_t& key : allKeys) {
            const SceneClass* sc(featureMap->classValue(key));
            if (sc != NULL) {
                FeatureItem* featureItem(getFeatureWithUniqueID(key));
                if (featureItem != NULL) {
                    featureItem->restoreFromScene(sceneAttributes,
                                                  sc);
                }
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
}

