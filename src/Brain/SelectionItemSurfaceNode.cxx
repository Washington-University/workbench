
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

#define __SELECTION_ITEM_SURFACE_NODE_DECLARE__
#include "SelectionItemSurfaceNode.h"
#undef __SELECTION_ITEM_SURFACE_NODE_DECLARE__

#include "Surface.h"

using namespace caret;

/**
 * \class SelectionItemSurfaceNode
 * \brief Selected node.
 *
 * Information about the selected node.
 */


/**
 * Constructor.
 */
SelectionItemSurfaceNode::SelectionItemSurfaceNode()
: SelectionItem(SelectionItemDataTypeEnum::SURFACE_NODE)
{
    m_surface = NULL;
    m_contralateralFlag = false;
    m_nodeNumber = -1;
}

/**
 * Destructor.
 */
SelectionItemSurfaceNode::~SelectionItemSurfaceNode()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemSurfaceNode::SelectionItemSurfaceNode(const SelectionItemSurfaceNode& obj)
: SelectionItem(obj)
{
    copyHelperSelectionItemSurfaceNode(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
SelectionItemSurfaceNode&
SelectionItemSurfaceNode::operator=(const SelectionItemSurfaceNode& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        copyHelperSelectionItemSurfaceNode(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItemSurfaceNode::copyHelperSelectionItemSurfaceNode(const SelectionItemSurfaceNode& idItem)
{
    m_surface = idItem.m_surface;
    m_nodeNumber = idItem.m_nodeNumber;
    m_contralateralFlag = idItem.m_contralateralFlag;
}

/**
 * Reset the selection item. 
 */
void 
SelectionItemSurfaceNode::reset()
{
    SelectionItem::reset();
    m_surface = NULL;
    m_nodeNumber = -1;
    m_contralateralFlag = false;
}

/**
 * @return Is m_ selected item valid?
 */
bool 
SelectionItemSurfaceNode::isValid() const
{
    return ((m_surface != NULL) && (m_nodeNumber >= 0));
}

/**
 * @return Surface containing selected node.
 */
const Surface* 
SelectionItemSurfaceNode::getSurface() const
{
    return m_surface;
}

/**
 * @return Surface containing selected node.
 */
Surface* 
SelectionItemSurfaceNode::getSurface()
{
    return m_surface;
}

/**
 * Set the surface containing the selected node.
 * @param surface
 *    New value for surface.
 *
 */
void 
SelectionItemSurfaceNode::setSurface(Surface* surface)
{
    m_surface = surface;
}

/**
 * return Number of selected node.
 */
int32_t 
SelectionItemSurfaceNode::getNodeNumber() const
{
    return m_nodeNumber;
}

/**
 * Set node number that was selected.
 * @param nodeNumber
 *    New value for node.
 */
void 
SelectionItemSurfaceNode::setNodeNumber(const int32_t nodeNumber)
{
    m_nodeNumber = nodeNumber;
}

///**
// * @return Is m_ a contralateral identification?
// */
//bool 
//SelectionItemSurfaceNode::isContralateral() const
//{
//    return m_contralateralFlag;
//}
//
///**
// * Set contralateral identification status.
// * @param status
// *    New status.
// */
//void 
//SelectionItemSurfaceNode::setContralateral(const bool status)
//{
//    m_contralateralFlag = status;
//}

/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString 
SelectionItemSurfaceNode::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((m_surface != NULL) ? m_surface->getFileNameNoPath() : "INVALID") + "\n");
    text += "Vertex: " + AString::number(m_nodeNumber) + "\n";
    if (isValid() && (m_surface != NULL)) {
        text += "Coordinate: " + AString::fromNumbers(m_surface->getCoordinate(m_nodeNumber), 3, ", ") + "\n";
    }
    text += "Contralateral: " + AString::fromBool(m_contralateralFlag) + "\n";
    return text;
}

/**
 * @return Information about brainordinate for this selection item.
 * Not all subclasses support this function in which case an empty string is returned.
 */
AString
SelectionItemSurfaceNode::getBrainordinateInformation() const
{
    if (isValid()) {
        if (m_surface != NULL) {
            AString txt(StructureEnum::toGuiName(m_surface->getStructure())
                        + " Vertex: "
                        + AString::number(m_nodeNumber));
            return txt;
        }
    }
    return "";
}


