
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __IDENTIFICATION_MANAGER_DECLARE__
#include "IdentificationManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ConnectivityLoaderManager.h"
#include "IdentifiedItemNode.h"
#include "SceneClassAssistant.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::IdentificationManager 
 * \brief Manages identified items.
 */

/**
 * Constructor.
 */
IdentificationManager::IdentificationManager(Brain* brain)
: SceneableInterface()
{
    m_brain = brain;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_contralateralIdentificationEnabled = false;
    m_volumeIdentificationEnabled = true;
    m_identificationSymbolColor = CaretColorEnum::GREEN;
    m_identificationContralateralSymbolColor = CaretColorEnum::BLUE;
    m_identifcationSymbolSize = 3.5;
    m_identifcationMostRecentSymbolSize = m_identifcationSymbolSize * 2.0;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                          &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add("m_volumeIdentificationEnabled",
                          &m_volumeIdentificationEnabled);
    
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
 */
void
IdentificationManager::addIdentifiedItem(IdentifiedItem* item)
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
    ConnectivityLoaderManager* clm = m_brain->getConnectivityLoaderManager();
    QList<TimeLine> surfaceTimeLines;
    clm->getSurfaceTimeLines(surfaceTimeLines);
    
//    for (QList<TimeLine>::iterator iter = surfaceTimeLines.begin();
//         iter != surfaceTimeLines.end();
//         iter++) {
//        const TimeLine& tl = *iter;
//        if (tl.structure == structure) {
//            
//        }
//    }
    
    std::vector<IdentifiedItemNode> nodeItems;
    
    for (std::list<IdentifiedItem*>::const_iterator iter = m_identifiedItems.begin();
         iter != m_identifiedItems.end();
         iter++) {
        const IdentifiedItem* item = *iter;
        const IdentifiedItemNode* nodeItem = dynamic_cast<const IdentifiedItemNode*>(item);
        if (nodeItem != NULL) {
            if ((nodeItem->getStructure() == structure)
                || (nodeItem->getContralateralStructure() == structure)) {
                if (nodeItem->getSurfaceNumberOfNodes() == surfaceNumberOfNodes) {
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
                    
                    nodeItems.push_back(nodeID);
                }
            }
        }
    }
    
    return nodeItems;
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
                    break;
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
 * @return Status of volume identification.
 */
bool
IdentificationManager::isVolumeIdentificationEnabled() const
{
    return m_volumeIdentificationEnabled;
}

/**
 * Set status of volume identification.
 * @param
 *    New status.
 */
void
IdentificationManager::setVolumeIdentificationEnabled(const bool enabled)
{
    m_volumeIdentificationEnabled = enabled;
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
                }
                else if (className == "IdentifiedItemNode") {
                    IdentifiedItemNode* item = new IdentifiedItemNode();
                    item->restoreFromScene(sceneAttributes, sc);
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
}
