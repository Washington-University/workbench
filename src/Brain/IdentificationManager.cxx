
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#ifdef CARET_OS_WINDOWS
#include <Windows.h>
#endif

#ifdef CARET_OS_MACOSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <cmath>
#include <limits>

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventIdentificationSymbolRemoval.h"

#define __IDENTIFICATION_MANAGER_DECLARE__
#include "IdentificationManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "IdentificationItemBorderSurface.h"
#include "IdentificationItemFocusSurface.h"
#include "IdentificationItemFocusVolume.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemSurfaceNodeIdentificationSymbol.h"
#include "IdentificationItemSurfaceTriangle.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationTextGenerator.h"
#include "Surface.h"

using namespace caret;

/**
 * \class IdentificationManager
 * \brief Manages identification.
 *
 * Manages identification.
 */


/**
 * Constructor.
 */
IdentificationManager::IdentificationManager()
: CaretObject()
{
    m_surfaceBorderIdentification = new IdentificationItemBorderSurface();
    m_surfaceFocusIdentification = new IdentificationItemFocusSurface();
    m_volumeFocusIdentification = new IdentificationItemFocusVolume();
    m_surfaceNodeIdentification = new IdentificationItemSurfaceNode();
    m_surfaceNodeIdentificationSymbol = new IdentificationItemSurfaceNodeIdentificationSymbol();
    m_surfaceTriangleIdentification = new IdentificationItemSurfaceTriangle();
    m_voxelIdentification = new IdentificationItemVoxel();
    
    m_allIdentificationItems.push_back(m_surfaceBorderIdentification);
    m_allIdentificationItems.push_back(m_surfaceFocusIdentification);
    m_allIdentificationItems.push_back(m_surfaceNodeIdentification);
    m_allIdentificationItems.push_back(m_surfaceNodeIdentificationSymbol);
    m_allIdentificationItems.push_back(m_surfaceTriangleIdentification);
    m_allIdentificationItems.push_back(m_voxelIdentification);
    m_allIdentificationItems.push_back(m_volumeFocusIdentification);
    
    m_surfaceSelectedItems.push_back(m_surfaceNodeIdentification);
    m_surfaceSelectedItems.push_back(m_surfaceTriangleIdentification);
    
    m_layeredSelectedItems.push_back(m_surfaceBorderIdentification);
    m_layeredSelectedItems.push_back(m_surfaceFocusIdentification);
    
    m_volumeSelectedItems.push_back(m_voxelIdentification);
    m_volumeSelectedItems.push_back(m_volumeFocusIdentification);
    
    m_idTextGenerator = new IdentificationTextGenerator();
    
    m_lastIdentifiedItem = NULL;
    
    reset();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
}

/**
 * Destructor.
 */
IdentificationManager::~IdentificationManager()
{
    reset();
    delete m_surfaceBorderIdentification;
    m_surfaceBorderIdentification = NULL;
    delete m_surfaceFocusIdentification;
    m_surfaceFocusIdentification = NULL;
    delete m_surfaceNodeIdentification;
    m_surfaceNodeIdentification = NULL;
    delete m_surfaceNodeIdentificationSymbol;
    m_surfaceNodeIdentificationSymbol = NULL;
    delete m_surfaceTriangleIdentification;
    m_surfaceTriangleIdentification = NULL;
    delete m_voxelIdentification;
    m_voxelIdentification = NULL;
    delete m_volumeFocusIdentification;
    m_volumeFocusIdentification = NULL;
    delete m_idTextGenerator;
    m_idTextGenerator = NULL;
    
    if (m_lastIdentifiedItem != NULL) {
        delete m_lastIdentifiedItem;
    }

    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   The event.
 */
void
IdentificationManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL) {
        EventIdentificationSymbolRemoval* removeIdEvent =
        dynamic_cast<EventIdentificationSymbolRemoval*>(event);
        CaretAssert(removeIdEvent);
        
        /*
         * Remove last event since all ID symbols being removed
         */
        setLastIdentifiedItem(NULL);
    }
}
/**
 * Filter selections to arbitrate between triangle/node
 * and to remove any selections behind another selection.
 *
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    identified, those items "behind" other items are not reported.
 *    For example, suppose a focus is identified and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    identified.  If this parameter is false, the node will be
 *    identified.
 */
void 
IdentificationManager::filterSelections(const bool applySelectionBackgroundFiltering)
{
    AString logText;
    for (std::vector<IdentificationItem*>::iterator iter = m_allIdentificationItems.begin();
         iter != m_allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        if (item->isValid()) {
            logText += ("\n" + item->toString() + "\n");
        }
    }
    CaretLogFine("Selected Items BEFORE filtering: " + logText);
    
    IdentificationItemSurfaceTriangle* triangleID = m_surfaceTriangleIdentification;
    IdentificationItemSurfaceNode* nodeID = m_surfaceNodeIdentification;
    
    //
    // If both a node and triangle are found
    //
    if ((nodeID->getNodeNumber() >= 0) &&
        (triangleID->getTriangleNumber() >= 0)) {
        //
        // Is node further from user than triangle?
        //
        double depthDiff = m_surfaceNodeIdentification->getScreenDepth()
        - triangleID->getScreenDepth();
        if (depthDiff > 0.00001) {
            //
            // Do not use node
            //
            m_surfaceNodeIdentification->reset();
        }
    }
    
    //
    // Have a triangle ?
    //
    const int32_t triangleNumber = triangleID->getTriangleNumber();
    if (triangleNumber >= 0) {
        //
        // If no node, use node in nearest triangle
        //
        if (m_surfaceNodeIdentification->getNodeNumber() < 0) {
            const int32_t nearestNode = triangleID->getNearestNodeNumber();
            if (nearestNode >= 0) {
                CaretLogFine("Switched node to triangle nearest node ."
                             + AString::number(nearestNode));
                nodeID->setNodeNumber(nearestNode);
                nodeID->setScreenDepth(triangleID->getScreenDepth());
                nodeID->setSurface(triangleID->getSurface());
                double xyz[3];
                triangleID->getNearestNodeScreenXYZ(xyz);
                nodeID->setScreenXYZ(xyz);
                triangleID->getNearestNodeModelXYZ(xyz);
                nodeID->setModelXYZ(xyz);
                nodeID->setBrain(triangleID->getBrain());
            }
        }
    }
    
    /*
     * See if node identification symbol is too far from selected node.
     * This may occur if the symbol is on the other side of the surface.
     */
    if ((m_surfaceNodeIdentificationSymbol->getNodeNumber() >= 0)
        && (m_surfaceNodeIdentification->getNodeNumber() >= 0)) {
        const double depthDiff = (m_surfaceNodeIdentificationSymbol->getScreenDepth()
                                  - m_surfaceNodeIdentification->getScreenDepth());
        if (depthDiff > 0.01) {
            m_surfaceNodeIdentificationSymbol->reset();
        }
        else {
            m_surfaceNodeIdentification->reset();
        }
    }
    
    if (applySelectionBackgroundFiltering) {
         clearDistantSelections();
    }
    
    logText = "";
    for (std::vector<IdentificationItem*>::iterator iter = m_allIdentificationItems.begin();
         iter != m_allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        if (item->isValid()) {
            logText += ("\n" + item->toString() + "\n");
        }
    }
    CaretLogFine("Selected Items AFTER filtering: " + logText);
}

/**
 * Examine the selection groups and manipulate them
 * so that there are not items selected in more
 * than one group.
 */
void 
IdentificationManager::clearDistantSelections()
{
    std::vector<std::vector<IdentificationItem*>* > itemGroups;
    /*
     * Make layers items slightly closer since they are 
     * often pasted onto the surface.
     */
    for (std::vector<IdentificationItem*>::iterator iter = m_layeredSelectedItems.begin();
         iter != m_layeredSelectedItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        item->setScreenDepth(item->getScreenDepth()* 0.99);
    }

    
    itemGroups.push_back(&m_layeredSelectedItems);
    itemGroups.push_back(&m_surfaceSelectedItems);
    itemGroups.push_back(&m_volumeSelectedItems);
    
    std::vector<IdentificationItem*>* minDepthGroup = NULL;
    double minDepth = std::numeric_limits<double>::max();
    for (std::vector<std::vector<IdentificationItem*>* >::iterator iter = itemGroups.begin();
         iter != itemGroups.end();
         iter++) {
        std::vector<IdentificationItem*>* group = *iter;
        IdentificationItem* minDepthItem =
        getMinimumDepthFromMultipleSelections(*group);
        if (minDepthItem != NULL) {
            double md = minDepthItem->getScreenDepth();
            if (md < minDepth) {
                minDepthGroup = group;
                minDepth = md;
            }
        }
    }
    
    if (minDepthGroup != NULL) {
        for (std::vector<std::vector<IdentificationItem*>* >::iterator iter = itemGroups.begin();
             iter != itemGroups.end();
             iter++) {
            std::vector<IdentificationItem*>* group = *iter;
            if (group != minDepthGroup) {
                for (std::vector<IdentificationItem*>::iterator iter = group->begin();
                     iter != group->end();
                     iter++) {
                    IdentificationItem* item = *iter;
                    item->reset();
                }
            }
        }
    }
}

/**
 * Reset all identified items except for the given identified item.
 * @param identifiedItem
 *    IdentifiedItem that is NOT reset.
 */
void 
IdentificationManager::clearOtherIdentifiedItems(IdentificationItem* identifiedItem)
{
    for (std::vector<IdentificationItem*>::iterator iter = m_allIdentificationItems.begin();
         iter != m_allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        if (item != identifiedItem) {
            item->reset();
        }
    }
    
}

/**
 * From the list of selectable items, find the item with the 
 * minimum depth.
 * @param items  List of selectable items.
 * @return  Reference to selectable item with the minimum depth
 * or NULL if no valid selectable items in the list.
 */
IdentificationItem* 
IdentificationManager::getMinimumDepthFromMultipleSelections(std::vector<IdentificationItem*> items) const
{
    double minDepth = std::numeric_limits<double>::max();
    
    IdentificationItem* minDepthItem = NULL;
    
    for (std::vector<IdentificationItem*>::iterator iter = items.begin();
         iter != items.end();
         iter++) {
        IdentificationItem* item = *iter;
        if (item->isValid()) {
            if (item->getScreenDepth() < minDepth) {
                minDepthItem = item;
                minDepth = item->getScreenDepth();
            }
        }
    }
    
    return minDepthItem;
}

/**
 * Get text describing the current identification data.
 * @param browserTabContent
 *    Tab content in which identification took place.
 * @param brain
 *    Brain containing the data.
 */
AString 
IdentificationManager::getIdentificationText(const BrowserTabContent* browserTabContent,
                                             const Brain* brain) const
{
    const AString text = m_idTextGenerator->createIdentificationText(this, 
                                                                         browserTabContent,
                                                                         brain);
    return text;
}

/**
 * Reset all identification.
 */
void 
IdentificationManager::reset()
{
    for (std::vector<IdentificationItem*>::iterator iter = m_allIdentificationItems.begin();
         iter != m_allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        item->reset();
    }
    
    for (std::vector<IdentificationItemSurfaceNode*>::iterator iter = m_additionalSurfaceNodeIdentifications.begin();
         iter != m_additionalSurfaceNodeIdentifications.end();
         iter++) {
        delete *iter;
    }
    m_additionalSurfaceNodeIdentifications.clear();
}

/**
 * @return Identification for surface node.
 */
IdentificationItemSurfaceNode* 
IdentificationManager::getSurfaceNodeIdentification()
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const IdentificationItemSurfaceNode* 
IdentificationManager::getSurfaceNodeIdentification() const
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const IdentificationItemSurfaceNodeIdentificationSymbol* 
IdentificationManager::getSurfaceNodeIdentificationSymbol() const
{
    return m_surfaceNodeIdentificationSymbol;
}

/**
 * @return Identification for surface node.
 */
IdentificationItemSurfaceNodeIdentificationSymbol* 
IdentificationManager::getSurfaceNodeIdentificationSymbol()
{
    return m_surfaceNodeIdentificationSymbol;
}

/**
 * @return Identification for surface triangle.
 */
IdentificationItemSurfaceTriangle* 
IdentificationManager::getSurfaceTriangleIdentification()
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for surface triangle.
 */
const IdentificationItemSurfaceTriangle* 
IdentificationManager::getSurfaceTriangleIdentification() const
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for voxels.
 */
const IdentificationItemVoxel* 
IdentificationManager::getVoxelIdentification() const
{
    return m_voxelIdentification;
}

/**
 * @return Identification for voxels.
 */
IdentificationItemVoxel* 
IdentificationManager::getVoxelIdentification()
{
    return m_voxelIdentification;
}

/**
 * @return Identification for borders.
 */
IdentificationItemBorderSurface* 
IdentificationManager::getSurfaceBorderIdentification()
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for borders.
 */
const IdentificationItemBorderSurface* 
IdentificationManager::getSurfaceBorderIdentification() const
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for foci.
 */
IdentificationItemFocusSurface* 
IdentificationManager::getSurfaceFocusIdentification()
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
const IdentificationItemFocusSurface* 
IdentificationManager::getSurfaceFocusIdentification() const
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
IdentificationItemFocusVolume*
IdentificationManager::getVolumeFocusIdentification()
{
    return m_volumeFocusIdentification;
}

/**
 * @return Identification for foci.
 */
const IdentificationItemFocusVolume*
IdentificationManager::getVolumeFocusIdentification() const
{
    return m_volumeFocusIdentification;
}

/**
 * Add an additional surface node identifications
 * typically made as a result of identification in a volume or
 * an contralateral identification.
 * @param surface
 *   Surface on which identification took place.
 * @param nodeIndex
 *   Index of surface node.
 * @param isContralateralIdentification
 *   True if contralateral identification.
 */
void 
IdentificationManager::addAdditionalSurfaceNodeIdentification(Surface* surface,
                                                              const int32_t nodeIndex,
                                                              bool isContralateralIdentification)
{
    if (surface != m_surfaceNodeIdentification->getSurface()) {
        IdentificationItemSurfaceNode* nodeID = new IdentificationItemSurfaceNode();
        nodeID->setSurface(surface);
        nodeID->setNodeNumber(nodeIndex);
        nodeID->setContralateral(isContralateralIdentification);
        m_additionalSurfaceNodeIdentifications.push_back(nodeID);
    }
}

/**
 * @return the number of additional surface node identifications
 * typically made as a result of identification in a volume or
 * an contralateral identification.
 */
int32_t 
IdentificationManager::getNumberOfAdditionalSurfaceNodeIdentifications() const
{
    return m_additionalSurfaceNodeIdentifications.size();
}

/**
 * Get an additional identified surface node.
 * @param indx
 *   Index of the identification information.
 */
IdentificationItemSurfaceNode* 
IdentificationManager::getAdditionalSurfaceNodeIdentification(const int32_t indx)
{
    CaretAssertVectorIndex(m_additionalSurfaceNodeIdentifications, indx);
    return m_additionalSurfaceNodeIdentifications[indx];
}

/**
 * Get an additional identified surface node.
 * @param indx
 *   Index of the identification information.
 */
const IdentificationItemSurfaceNode* 
IdentificationManager::getAdditionalSurfaceNodeIdentification(const int32_t indx) const
{
    CaretAssertVectorIndex(m_additionalSurfaceNodeIdentifications, indx);
    return m_additionalSurfaceNodeIdentifications[indx];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationManager::toString() const
{
    return "IdentificationManager";
}

/**
 * @return The last identified item (may be NULL).
 */
const IdentificationItem*
IdentificationManager::getLastIdentifiedItem() const
{
    return m_lastIdentifiedItem;
}

/**
 * Set the last identified item to the given item.
 *
 * @param lastItem
 *     The last item that was identified;
 */
void
IdentificationManager::setLastIdentifiedItem(const IdentificationItem* lastItem)
{
    if (m_lastIdentifiedItem != NULL) {
        delete m_lastIdentifiedItem;
    }
    m_lastIdentifiedItem = NULL;
    
    if (lastItem != NULL) {
        const IdentificationItemSurfaceNode* nodeID = dynamic_cast<const IdentificationItemSurfaceNode*>(lastItem);
        const IdentificationItemVoxel* voxelID = dynamic_cast<const IdentificationItemVoxel*>(lastItem);
        if (nodeID != NULL) {
            m_lastIdentifiedItem = new IdentificationItemSurfaceNode(*nodeID);
        }
        else if (voxelID != NULL) {
            m_lastIdentifiedItem = new IdentificationItemVoxel(*voxelID);
        }
        else {
            CaretAssertMessage(0,
                               ("Unsupported last ID type" + lastItem->toString()));
        }
    }
}


