
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
#include "SelectionManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemSurfaceTriangle.h"
#include "SelectionItemVoxel.h"
#include "IdentificationTextGenerator.h"
#include "Surface.h"

using namespace caret;

/**
 * \class SelectionManager
 * \brief Manages identification.
 *
 * Manages identification.
 */


/**
 * Constructor.
 */
SelectionManager::SelectionManager()
: CaretObject()
{
    m_surfaceBorderIdentification = new SelectionItemBorderSurface();
    m_surfaceFocusIdentification = new SelectionItemFocusSurface();
    m_volumeFocusIdentification = new SelectionItemFocusVolume();
    m_surfaceNodeIdentification = new SelectionItemSurfaceNode();
    m_surfaceNodeIdentificationSymbol = new SelectionItemSurfaceNodeIdentificationSymbol();
    m_surfaceTriangleIdentification = new SelectionItemSurfaceTriangle();
    m_voxelIdentification = new SelectionItemVoxel();
    
    m_allSelectionItems.push_back(m_surfaceBorderIdentification);
    m_allSelectionItems.push_back(m_surfaceFocusIdentification);
    m_allSelectionItems.push_back(m_surfaceNodeIdentification);
    m_allSelectionItems.push_back(m_surfaceNodeIdentificationSymbol);
    m_allSelectionItems.push_back(m_surfaceTriangleIdentification);
    m_allSelectionItems.push_back(m_voxelIdentification);
    m_allSelectionItems.push_back(m_volumeFocusIdentification);
    
    m_surfaceSelectedItems.push_back(m_surfaceNodeIdentification);
    m_surfaceSelectedItems.push_back(m_surfaceTriangleIdentification);
    
    m_layeredSelectedItems.push_back(m_surfaceBorderIdentification);
    m_layeredSelectedItems.push_back(m_surfaceFocusIdentification);
    
    m_volumeSelectedItems.push_back(m_voxelIdentification);
    m_volumeSelectedItems.push_back(m_volumeFocusIdentification);
    
    m_idTextGenerator = new IdentificationTextGenerator();
    
    m_lastSelectedItem = NULL;
    
    reset();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
}

/**
 * Destructor.
 */
SelectionManager::~SelectionManager()
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
    
    if (m_lastSelectedItem != NULL) {
        delete m_lastSelectedItem;
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
SelectionManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL) {
        EventIdentificationSymbolRemoval* removeIdEvent =
        dynamic_cast<EventIdentificationSymbolRemoval*>(event);
        CaretAssert(removeIdEvent);
        
        /*
         * Remove last event since all ID symbols being removed
         */
        setLastSelectedItem(NULL);
    }
}
/**
 * Filter selections to arbitrate between triangle/node
 * and to remove any selections behind another selection.
 *
 * @param applySelectionBackgroundFiltering
 *    If true (which is in most cases), if there are multiple items
 *    selected, those items "behind" other items are not reported.
 *    For example, suppose a focus is selected and there is a node
 *    the focus.  If this parameter is true, the node will NOT be
 *    selected.  If this parameter is false, the node will be
 *    selected.
 */
void 
SelectionManager::filterSelections(const bool applySelectionBackgroundFiltering)
{
    AString logText;
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        if (item->isValid()) {
            logText += ("\n" + item->toString() + "\n");
        }
    }
    CaretLogFine("Selected Items BEFORE filtering: " + logText);
    
    SelectionItemSurfaceTriangle* triangleID = m_surfaceTriangleIdentification;
    SelectionItemSurfaceNode* nodeID = m_surfaceNodeIdentification;
    
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
                CaretLogFine("Switched vertex to triangle nearest vertex ."
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
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
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
SelectionManager::clearDistantSelections()
{
    std::vector<std::vector<SelectionItem*>* > itemGroups;
    /*
     * Make layers items slightly closer since they are 
     * often pasted onto the surface.
     */
    for (std::vector<SelectionItem*>::iterator iter = m_layeredSelectedItems.begin();
         iter != m_layeredSelectedItems.end();
         iter++) {
        SelectionItem* item = *iter;
        item->setScreenDepth(item->getScreenDepth()* 0.99);
    }

    
    itemGroups.push_back(&m_layeredSelectedItems);
    itemGroups.push_back(&m_surfaceSelectedItems);
    itemGroups.push_back(&m_volumeSelectedItems);
    
    std::vector<SelectionItem*>* minDepthGroup = NULL;
    double minDepth = std::numeric_limits<double>::max();
    for (std::vector<std::vector<SelectionItem*>* >::iterator iter = itemGroups.begin();
         iter != itemGroups.end();
         iter++) {
        std::vector<SelectionItem*>* group = *iter;
        SelectionItem* minDepthItem =
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
        for (std::vector<std::vector<SelectionItem*>* >::iterator iter = itemGroups.begin();
             iter != itemGroups.end();
             iter++) {
            std::vector<SelectionItem*>* group = *iter;
            if (group != minDepthGroup) {
                for (std::vector<SelectionItem*>::iterator iter = group->begin();
                     iter != group->end();
                     iter++) {
                    SelectionItem* item = *iter;
                    item->reset();
                }
            }
        }
    }
}

/**
 * Reset all selected items except for the given selected item.
 * @param selectedItem
 *    SelectedItem that is NOT reset.
 */
void 
SelectionManager::clearOtherSelectedItems(SelectionItem* selectedItem)
{
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        if (item != selectedItem) {
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
SelectionItem* 
SelectionManager::getMinimumDepthFromMultipleSelections(std::vector<SelectionItem*> items) const
{
    double minDepth = std::numeric_limits<double>::max();
    
    SelectionItem* minDepthItem = NULL;
    
    for (std::vector<SelectionItem*>::iterator iter = items.begin();
         iter != items.end();
         iter++) {
        SelectionItem* item = *iter;
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
SelectionManager::getIdentificationText(const BrowserTabContent* browserTabContent,
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
SelectionManager::reset()
{
    for (std::vector<SelectionItem*>::iterator iter = m_allSelectionItems.begin();
         iter != m_allSelectionItems.end();
         iter++) {
        SelectionItem* item = *iter;
        item->reset();
    }
    
    for (std::vector<SelectionItemSurfaceNode*>::iterator iter = m_additionalSurfaceNodeIdentifications.begin();
         iter != m_additionalSurfaceNodeIdentifications.end();
         iter++) {
        delete *iter;
    }
    m_additionalSurfaceNodeIdentifications.clear();
}

/**
 * @return Identification for surface node.
 */
SelectionItemSurfaceNode* 
SelectionManager::getSurfaceNodeIdentification()
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const SelectionItemSurfaceNode* 
SelectionManager::getSurfaceNodeIdentification() const
{
    return m_surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const SelectionItemSurfaceNodeIdentificationSymbol* 
SelectionManager::getSurfaceNodeIdentificationSymbol() const
{
    return m_surfaceNodeIdentificationSymbol;
}

/**
 * @return Identification for surface node.
 */
SelectionItemSurfaceNodeIdentificationSymbol* 
SelectionManager::getSurfaceNodeIdentificationSymbol()
{
    return m_surfaceNodeIdentificationSymbol;
}

/**
 * @return Identification for surface triangle.
 */
SelectionItemSurfaceTriangle* 
SelectionManager::getSurfaceTriangleIdentification()
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for surface triangle.
 */
const SelectionItemSurfaceTriangle* 
SelectionManager::getSurfaceTriangleIdentification() const
{
    return m_surfaceTriangleIdentification;
}

/**
 * @return Identification for voxels.
 */
const SelectionItemVoxel* 
SelectionManager::getVoxelIdentification() const
{
    return m_voxelIdentification;
}

/**
 * @return Identification for voxels.
 */
SelectionItemVoxel* 
SelectionManager::getVoxelIdentification()
{
    return m_voxelIdentification;
}

/**
 * @return Identification for borders.
 */
SelectionItemBorderSurface* 
SelectionManager::getSurfaceBorderIdentification()
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for borders.
 */
const SelectionItemBorderSurface* 
SelectionManager::getSurfaceBorderIdentification() const
{
    return m_surfaceBorderIdentification;
}

/**
 * @return Identification for foci.
 */
SelectionItemFocusSurface* 
SelectionManager::getSurfaceFocusIdentification()
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
const SelectionItemFocusSurface* 
SelectionManager::getSurfaceFocusIdentification() const
{
    return m_surfaceFocusIdentification;
}

/**
 * @return Identification for foci.
 */
SelectionItemFocusVolume*
SelectionManager::getVolumeFocusIdentification()
{
    return m_volumeFocusIdentification;
}

/**
 * @return Identification for foci.
 */
const SelectionItemFocusVolume*
SelectionManager::getVolumeFocusIdentification() const
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
SelectionManager::addAdditionalSurfaceNodeIdentification(Surface* surface,
                                                              const int32_t nodeIndex,
                                                              bool isContralateralIdentification)
{
    if (surface != m_surfaceNodeIdentification->getSurface()) {
        SelectionItemSurfaceNode* nodeID = new SelectionItemSurfaceNode();
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
SelectionManager::getNumberOfAdditionalSurfaceNodeIdentifications() const
{
    return m_additionalSurfaceNodeIdentifications.size();
}

/**
 * Get an additional selected surface node.
 * @param indx
 *   Index of the identification information.
 */
SelectionItemSurfaceNode* 
SelectionManager::getAdditionalSurfaceNodeIdentification(const int32_t indx)
{
    CaretAssertVectorIndex(m_additionalSurfaceNodeIdentifications, indx);
    return m_additionalSurfaceNodeIdentifications[indx];
}

/**
 * Get an additional selected surface node.
 * @param indx
 *   Index of the identification information.
 */
const SelectionItemSurfaceNode* 
SelectionManager::getAdditionalSurfaceNodeIdentification(const int32_t indx) const
{
    CaretAssertVectorIndex(m_additionalSurfaceNodeIdentifications, indx);
    return m_additionalSurfaceNodeIdentifications[indx];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionManager::toString() const
{
    return "SelectionManager";
}

/**
 * @return The last selected item (may be NULL).
 */
const SelectionItem*
SelectionManager::getLastSelectedItem() const
{
    return m_lastSelectedItem;
}

/**
 * Set the last selected item to the given item.
 *
 * @param lastItem
 *     The last item that was selected;
 */
void
SelectionManager::setLastSelectedItem(const SelectionItem* lastItem)
{
    if (m_lastSelectedItem != NULL) {
        delete m_lastSelectedItem;
    }
    m_lastSelectedItem = NULL;
    
    if (lastItem != NULL) {
        const SelectionItemSurfaceNode* nodeID = dynamic_cast<const SelectionItemSurfaceNode*>(lastItem);
        const SelectionItemVoxel* voxelID = dynamic_cast<const SelectionItemVoxel*>(lastItem);
        if (nodeID != NULL) {
            m_lastSelectedItem = new SelectionItemSurfaceNode(*nodeID);
        }
        else if (voxelID != NULL) {
            m_lastSelectedItem = new SelectionItemVoxel(*voxelID);
        }
        else {
            CaretAssertMessage(0,
                               ("Unsupported last ID type" + lastItem->toString()));
        }
    }
}


