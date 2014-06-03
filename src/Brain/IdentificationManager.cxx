
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __IDENTIFICATION_MANAGER_DECLARE__
#include "IdentificationManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "IdentifiedItemNode.h"
#include "SceneClassAssistant.h"
#include "SceneClass.h"
#include "ScenePrimitive.h"

using namespace caret;


    
/**
 * \class caret::IdentificationManager 
 * \brief Manages identified items.
 */

/**
 * Constructor.
 */
IdentificationManager::IdentificationManager()
: SceneableInterface()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_contralateralIdentificationEnabled = false;
    m_identificationSymbolColor = CaretColorEnum::WHITE;
    m_identificationContralateralSymbolColor = CaretColorEnum::LIME;
    m_identifcationSymbolSize = 6.0;
    m_identifcationMostRecentSymbolSize = 10.0;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                          &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add("m_identifcationSymbolSize",
                          &m_identifcationSymbolSize);
    
    m_sceneAssistant->add("m_identifcationMostRecentSymbolSize",
                          &m_identifcationMostRecentSymbolSize);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                &m_identificationSymbolColor);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationContralateralSymbolColor",
                                                                &m_identificationContralateralSymbolColor);

    removeAllIdentifiedItems();
}

/**
 * Destructor.
 */
IdentificationManager::~IdentificationManager()
{
    removeAllIdentifiedItems();
    
    delete m_sceneAssistant;
}

/**
 * Add an identified item.
 * @param item
 *    Identified item that is added.
 *    NOTE: Takes ownership of this item and will delete, at the appropriate time.
 *    If item is a node and contralateral identification is enabled, the contralateral
 *    structure will be set in the node item.
 */
void
IdentificationManager::addIdentifiedItem(IdentifiedItem* item)
{
    CaretAssert(item);
    
    IdentifiedItemNode* nodeItem = dynamic_cast<IdentifiedItemNode*>(item);
    if (nodeItem != NULL) {
        if (m_contralateralIdentificationEnabled) {
            const StructureEnum::Enum contralateralStructure = StructureEnum::getContralateralStructure(nodeItem->getStructure());
            nodeItem->setContralateralStructure(contralateralStructure);
        }
    }
    
    addIdentifiedItemPrivate(item);
}

/**
 * Add an identified item.
 * @param item
 *    Identified item that is added.
 *    NOTE: Takes ownership of this item and will delete, at the appropriate time.
 */
void
IdentificationManager::addIdentifiedItemPrivate(IdentifiedItem* item)
{
    CaretAssert(item);
    m_mostRecentIdentifiedItem = item;
    
    m_identifiedItems.push_back(item);
}

/**
 * @return String containing identification text for information window.
 */
AString
IdentificationManager::getIdentificationText() const
{
    AString text;
    
    for (std::list<IdentifiedItem*>::const_iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        const IdentifiedItem* item = *iter;
        if (text.isEmpty() == false) {
            text += "<P></P>";
        }
        text += item->getText();
    }

    return text;
}

/**
 * Remove all identification text.
 */
void
IdentificationManager::removeIdentificationText()
{
    for (std::list<IdentifiedItem*>::iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        IdentifiedItem* item = *iter;
        item->clearText();
    }
}

/**
 * Get identified nodes for the surface with the given structure and
 * number of nodes.
 *
 * @param structure
 *    The structure
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 */
std::vector<IdentifiedItemNode>
IdentificationManager::getNodeIdentifiedItemsForSurface(const StructureEnum::Enum structure,
                                                        const int32_t surfaceNumberOfNodes) const
{
    std::vector<IdentifiedItemNode> nodeItemsOut;
    
    for (std::list<IdentifiedItem*>::const_iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        const IdentifiedItem* item = *iter;
        const IdentifiedItemNode* nodeItem = dynamic_cast<const IdentifiedItemNode*>(item);
        if (nodeItem != NULL) {
            if (nodeItem->getSurfaceNumberOfNodes() == surfaceNumberOfNodes) {
                bool useIt = false;
                if (nodeItem->getStructure() == structure) {
                    useIt = true;
                }
                else if (nodeItem->getContralateralStructure() == structure) {
                    useIt = true;
                }
                if (useIt) {
                    IdentifiedItemNode nodeID(*nodeItem);
                    
                    const float* symbolRGB = CaretColorEnum::toRGB(m_identificationSymbolColor);
                    nodeID.setSymbolRGB(symbolRGB);
                    const float* contralateralSymbolRGB = CaretColorEnum::toRGB(m_identificationContralateralSymbolColor);
                    nodeID.setContralateralSymbolRGB(contralateralSymbolRGB);
                    if (item == m_mostRecentIdentifiedItem) {
                        nodeID.setSymbolSize(m_identifcationMostRecentSymbolSize);
                    }
                    else {
                        nodeID.setSymbolSize(m_identifcationSymbolSize);
                    }
                    nodeItemsOut.push_back(nodeID);
                }
            }
        }
    }

    return nodeItemsOut;
}

/**
 * Remove any identification for the node in the surface with the given
 * structure and number of nodes.
 *
 * @param structure
 *    The structure
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param nodeIndex
 *    Index of the node.
 */
void
IdentificationManager::removeIdentifiedNodeItem(const StructureEnum::Enum structure,
                                                const int32_t surfaceNumberOfNodes,
                                                const int32_t nodeIndex)
{
    for (std::list<IdentifiedItem*>::iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        IdentifiedItem* item = *iter;
        const IdentifiedItemNode* node = dynamic_cast<const IdentifiedItemNode*>(item);
        if (node != NULL) {
            if ((node->getStructure() == structure)
                || (node->getContralateralStructure() == structure)) {
                if ((node->getSurfaceNumberOfNodes() == surfaceNumberOfNodes)
                    && (node->getNodeIndex() == nodeIndex)) {
                    m_identifiedItems.erase(iter);
                    delete item;
                    return;
                }
            }
        }
    }
}

/**
 * Remove all identified items. 
 */
void
IdentificationManager::removeAllIdentifiedItems()
{
    for (std::list<IdentifiedItem*>::iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        IdentifiedItem* item = *iter;
        delete item;
    }
    
    m_identifiedItems.clear();
    
    m_mostRecentIdentifiedItem = NULL;
}

/**
 * Remove all identified nodes.
 */
void
IdentificationManager::removeAllIdentifiedNodes()
{
    std::list<IdentifiedItem*> itemsToKeep;
    
    for (std::list<IdentifiedItem*>::iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        IdentifiedItem* item = *iter;
        IdentifiedItemNode* nodeItem = dynamic_cast<IdentifiedItemNode*>(item);
        if (nodeItem != NULL) {
            if (m_mostRecentIdentifiedItem == nodeItem) {
                m_mostRecentIdentifiedItem = NULL;
            }
            delete nodeItem;
        }
        else {
            itemsToKeep.push_back(item);
        }
    }
    
    m_identifiedItems = itemsToKeep;
}

/**
 * @return Status of contralateral identification.
 */
bool
IdentificationManager::isContralateralIdentificationEnabled() const
{
    return m_contralateralIdentificationEnabled;
}

/**
 * Set status of contralateral identification.
 * @param
 *    New status.
 */
void
IdentificationManager::setContralateralIdentificationEnabled(const bool enabled)
{
    m_contralateralIdentificationEnabled = enabled;
}

/**
 * @return The size of the identification symbol
 */
float
IdentificationManager::getIdentificationSymbolSize() const
{
    return m_identifcationSymbolSize;
}

/**
 * Set the size of the identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setIdentificationSymbolSize(const float symbolSize)
{
    m_identifcationSymbolSize = symbolSize;
}

/**
 * @return The size of the most recent identification symbol
 */
float
IdentificationManager::getMostRecentIdentificationSymbolSize() const
{
    return m_identifcationMostRecentSymbolSize;
}

/**
 * Set the size of the most recent identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setMostRecentIdentificationSymbolSize(const float symbolSize)
{
    m_identifcationMostRecentSymbolSize = symbolSize;
}

/**
 * @return The color of the identification symbol.
 */
CaretColorEnum::Enum
IdentificationManager::getIdentificationSymbolColor() const
{
    return m_identificationSymbolColor;
}

/**
 * Set the color of the identification symbol.
 * @param color
 *    New color.
 */
void
IdentificationManager::setIdentificationSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationSymbolColor = color;
}

/**
 * @return The color of the contralateral identification symbol.
 */
CaretColorEnum::Enum
IdentificationManager::getIdentificationContralateralSymbolColor() const
{
    return m_identificationContralateralSymbolColor;
}

/**
 * Set the color of the contralateral identification symbol.
 * @param color
 *    New color.
 */
void
IdentificationManager::setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationContralateralSymbolColor = color;
}


/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
IdentificationManager::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentificationManager",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    for (std::list<IdentifiedItem*>::iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        IdentifiedItem* item = *iter;
        sceneClass->addClass(item->saveToScene(sceneAttributes,
                                               "identifiedItem"));
    }
    
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
IdentificationManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    removeAllIdentifiedItems();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const int32_t numChildren = sceneClass->getNumberOfObjects();
    for (int32_t i = 0; i < numChildren; i++) {
        const SceneObject* so = sceneClass->getObjectAtIndex(i);
        if (so->getName() == "identifiedItem") {
            const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
            if (sc != NULL) {
                const AString className = sc->getClassName();
                if (className == "IdentifiedItem") {
                    IdentifiedItem* item = new IdentifiedItem();
                    item->restoreFromScene(sceneAttributes, sc);
                    if (item->isValid()) {
                        addIdentifiedItemPrivate(item);
                    }
                    else {
                        delete item;
                    }
                }
                else if (className == "IdentifiedItemNode") {
                    IdentifiedItemNode* item = new IdentifiedItemNode();
                    item->restoreFromScene(sceneAttributes, sc);
                    if (item->isValid()) {
                        addIdentifiedItemPrivate(item);
                    }
                    else {
                        delete item;
                    }
                }
                else {
                    const AString msg = ("IdentifiedItem from scene is invalid.  "
                                         "Has a new IdentifiedItem type been added?  "
                                         "Class name=" + className);
                    CaretAssertMessage(0,
                                       msg);
                    CaretLogSevere(msg);
                }
            }
        }
    }
    
    /*
     * "m_volumeIdentificationEnabled" was removed when volume identification
     * was made a tab property.  If this item is present in the scene,
     * update volume ID status in all tabs.
     */
    const ScenePrimitive* idPrimitive = sceneClass->getPrimitive("m_volumeIdentificationEnabled");
    if (idPrimitive != NULL) {
        const bool volumeID = sceneClass->getBooleanValue("m_volumeIdentificationEnabled");
        
        EventBrowserTabGetAll allTabs;
        EventManager::get()->sendEvent(allTabs.getPointer());
        std::vector<BrowserTabContent*> tabContent = allTabs.getAllBrowserTabs();
        
        for (std::vector<BrowserTabContent*>::iterator iter = tabContent.begin();
             iter != tabContent.end();
             iter++) {
            BrowserTabContent* btc = *iter;
            btc->setIdentificationUpdatesVolumeSlices(volumeID);
        }
    }
}
