
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

#define __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL_DECLARE__
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#undef __SELECTION_ITEM_SURFACE_NODE_IDENTIFICATION_SYMBOL_DECLARE__

#include "Surface.h"

using namespace caret;

/**
 * \class SelectionItemSurfaceNodeIdentificationSymbol
 * \brief Selected node symbol
 *
 * Information about the selected node symbol.
 */


/**
 * Constructor.
 */
SelectionItemSurfaceNodeIdentificationSymbol::SelectionItemSurfaceNodeIdentificationSymbol()
: SelectionItem(SelectionItemDataTypeEnum::SURFACE_NODE_IDENTIFICATION_SYMBOL)
{
    this->surface = NULL;
    this->nodeNumber = -1;
    this->surfaceNumberOfNodes = -1;
    this->structure = StructureEnum::INVALID;
}

/**
 * Destructor.
 */
SelectionItemSurfaceNodeIdentificationSymbol::~SelectionItemSurfaceNodeIdentificationSymbol()
{
    
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemSurfaceNodeIdentificationSymbol::reset()
{
    SelectionItem::reset();
    this->surface = NULL;
    this->surfaceNumberOfNodes = -1;
    this->structure = StructureEnum::INVALID;
    this->nodeNumber = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemSurfaceNodeIdentificationSymbol::isValid() const
{
    return ((this->nodeNumber >= 0)
            && (this->surfaceNumberOfNodes > 0)
            && (this->structure != StructureEnum::INVALID));
}

/**
 * @return Surface containing selected node.
 */
const Surface* 
SelectionItemSurfaceNodeIdentificationSymbol::getSurface() const
{
    return this->surface;
}

/**
 * @return Surface containing selected node.
 */
Surface* 
SelectionItemSurfaceNodeIdentificationSymbol::getSurface()
{
    return this->surface;
}

/**
 * Set the surface containing the selected node.
 * @param surface
 *    New value for surface.
 *
 */
void 
SelectionItemSurfaceNodeIdentificationSymbol::setSurface(Surface* surface)
{
    this->surface = surface;
}

/**
 * return Number of selected node.
 */
int32_t 
SelectionItemSurfaceNodeIdentificationSymbol::getNodeNumber() const
{
    return this->nodeNumber;
}

/**
 * Set node number that was selected.
 * @param nodeNumber
 *    New value for node.
 */
void 
SelectionItemSurfaceNodeIdentificationSymbol::setNodeNumber(const int32_t nodeNumber)
{
    this->nodeNumber = nodeNumber;
}

/**
 * @return number of nodes in surface
 */
int32_t
SelectionItemSurfaceNodeIdentificationSymbol::getSurfaceNumberOfNodes() const
{
    return this->surfaceNumberOfNodes;
}

/**
 * Set the number of nodes in the surface
 * @param numberOfNodes
 *    Number of nodes in surface
 */
void
SelectionItemSurfaceNodeIdentificationSymbol::setSurfaceNumberOfNodes(const int32_t numberOfNodes)
{
    this->surfaceNumberOfNodes = numberOfNodes;
}

/**
 * @return The structure
 */
StructureEnum::Enum
SelectionItemSurfaceNodeIdentificationSymbol::getStructure() const
{
    return this->structure;
}


/**
 * Set the number of nodes in the surface
 * @param structure
 *    Ths structure
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface
 * @param nodeNumber
 *    New value for node.
 */
void
SelectionItemSurfaceNodeIdentificationSymbol::set(const StructureEnum::Enum structure,
                                                  const int32_t surfaceNumberOfNodes,
                                                  const int32_t nodeNumber)
{
    this->structure = structure;
    this->surfaceNumberOfNodes = surfaceNumberOfNodes;
    this->nodeNumber = nodeNumber;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemSurfaceNodeIdentificationSymbol::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
    text += "Vertex: " + AString::number(this->nodeNumber) + "\n";
    text += "Number of Vertices: " + AString::number(this->surfaceNumberOfNodes) + "\n";
    return text;
}
