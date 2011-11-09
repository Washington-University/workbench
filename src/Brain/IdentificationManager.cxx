
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

#define __IDENTIFICATION_MANAGER_DECLARE__
#include "IdentificationManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "IdentificationItemSurfaceNode.h"
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
    this->surfaceNodeIdentification = new IdentificationItemSurfaceNode();
    this->surfaceTriangleIdentification = new IdentificationItemSurfaceTriangle();
    this->voxelIdentification = new IdentificationItemVoxel();
    
    this->allIdentificationItems.push_back(this->surfaceNodeIdentification);
    this->allIdentificationItems.push_back(this->surfaceTriangleIdentification);
    this->allIdentificationItems.push_back(this->voxelIdentification);
    
    this->surfaceSelectedItems.push_back(this->surfaceNodeIdentification);
    this->surfaceSelectedItems.push_back(this->surfaceTriangleIdentification);
    
    //this->layeredSelectedItems.push_back();
    
    this->volumeSelectedItems.push_back(this->voxelIdentification);
    
    this->idTextGenerator = new IdentificationTextGenerator();
}

/**
 * Destructor.
 */
IdentificationManager::~IdentificationManager()
{
    delete this->surfaceNodeIdentification;
    this->surfaceNodeIdentification = NULL;
    delete this->surfaceTriangleIdentification;
    this->surfaceTriangleIdentification = NULL;
    delete this->voxelIdentification;
    this->voxelIdentification = NULL;
    delete this->idTextGenerator;
    this->idTextGenerator = NULL;
}

/**
 * Filter selections to arbitrate between triangle/node
 * and to remove any selections behind another selection.
 */
void 
IdentificationManager::filterSelections()
{
    AString logText;
    for (std::vector<IdentificationItem*>::iterator iter = this->allIdentificationItems.begin();
         iter != this->allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        if (item->isValid()) {
            logText += ("\n" + item->toString() + "\n");
        }
    }
    CaretLogFine("Selected Items BEFORE filtering: " + logText);
    
    IdentificationItemSurfaceTriangle* triangleID = this->surfaceTriangleIdentification;
    IdentificationItemSurfaceNode* nodeID = this->surfaceNodeIdentification;
    
    //
    // If both a node and triangle are found
    //
    if ((nodeID->getNodeNumber() >= 0) &&
        (triangleID->getTriangleNumber() >= 0)) {
        //
        // Is node further from user than triangle?
        //
        double depthDiff = this->surfaceNodeIdentification->getScreenDepth()
        - triangleID->getScreenDepth();
        if (depthDiff > 0.00001) {
            //
            // Do not use node
            //
            this->surfaceNodeIdentification->reset();
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
        if (this->surfaceNodeIdentification->getNodeNumber() < 0) {
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
    
    this->clearDistantSelections();
    
    logText = "";
    for (std::vector<IdentificationItem*>::iterator iter = this->allIdentificationItems.begin();
         iter != this->allIdentificationItems.end();
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
    for (std::vector<IdentificationItem*>::iterator iter = this->layeredSelectedItems.begin();
         iter != this->layeredSelectedItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        item->setScreenDepth(item->getScreenDepth()* 0.99);
    }

    
    itemGroups.push_back(&this->layeredSelectedItems);
    itemGroups.push_back(&this->surfaceSelectedItems);
    itemGroups.push_back(&this->volumeSelectedItems);
    
    std::vector<IdentificationItem*>* minDepthGroup = NULL;
    double minDepth = std::numeric_limits<double>::max();
    for (std::vector<std::vector<IdentificationItem*>* >::iterator iter = itemGroups.begin();
         iter != itemGroups.end();
         iter++) {
        std::vector<IdentificationItem*>* group = *iter;
        IdentificationItem* minDepthItem =
        this->getMinimumDepthFromMultipleSelections(*group);
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
 */
AString 
IdentificationManager::getIdentificationText(const BrowserTabContent* browserTabContent) const
{
    const AString text = this->idTextGenerator->createIdentificationText(this, browserTabContent);
    return text;
}

/**
 * Reset all identification.
 */
void 
IdentificationManager::reset()
{
    for (std::vector<IdentificationItem*>::iterator iter = this->allIdentificationItems.begin();
         iter != this->allIdentificationItems.end();
         iter++) {
        IdentificationItem* item = *iter;
        item->reset();
    }
}

/**
 * @return Identification for surface node.
 */
IdentificationItemSurfaceNode* 
IdentificationManager::getSurfaceNodeIdentification()
{
    return this->surfaceNodeIdentification;
}

/**
 * @return Identification for surface node.
 */
const IdentificationItemSurfaceNode* 
IdentificationManager::getSurfaceNodeIdentification() const
{
    return this->surfaceNodeIdentification;
}

/**
 * @return Identification for surface triangle.
 */
IdentificationItemSurfaceTriangle* 
IdentificationManager::getSurfaceTriangleIdentification()
{
    return this->surfaceTriangleIdentification;
}

/**
 * @return Identification for surface triangle.
 */
const IdentificationItemSurfaceTriangle* 
IdentificationManager::getSurfaceTriangleIdentification() const
{
    return this->surfaceTriangleIdentification;
}

/**
 * @return Identification for voxels.
 */
const IdentificationItemVoxel* 
IdentificationManager::getVoxelIdentification() const
{
    return this->voxelIdentification;
}

/**
 * @return Identification for voxels.
 */
IdentificationItemVoxel* 
IdentificationManager::getVoxelIdentification()
{
    return this->voxelIdentification;
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
