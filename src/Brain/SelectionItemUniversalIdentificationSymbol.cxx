
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

#define __SELECTION_ITEM_UNIVERSAL_IDENTIFICATION_SYMBOL_DECLARE__
#include "SelectionItemUniversalIdentificationSymbol.h"
#undef __SELECTION_ITEM_UNIVERSAL_IDENTIFICATION_SYMBOL_DECLARE__

#include "Surface.h"

using namespace caret;

/**
 * \class SelectionItemUniversalIdentificationSymbol
 * \brief Selected node symbol
 *
 * Information about the selected universal symbol.
 */


/**
 * Constructor.
 */
SelectionItemUniversalIdentificationSymbol::SelectionItemUniversalIdentificationSymbol()
: SelectionItem(SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL)
{
//    this->surface = NULL;
//    this->nodeNumber = -1;
//    this->surfaceNumberOfNodes = -1;
//    this->structure = StructureEnum::INVALID;
}

/**
 * Destructor.
 */
SelectionItemUniversalIdentificationSymbol::~SelectionItemUniversalIdentificationSymbol()
{
    
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemUniversalIdentificationSymbol::reset()
{
    SelectionItem::reset();
//    this->surface = NULL;
//    this->surfaceNumberOfNodes = -1;
//    this->structure = StructureEnum::INVALID;
//    this->nodeNumber = -1;
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemUniversalIdentificationSymbol::isValid() const
{
    if (getIdentifiedItemUniqueIdentifier() >= 0) {
        return true;
    }
    return false;
//    
//    return ((this->nodeNumber >= 0)
//            && (this->surfaceNumberOfNodes > 0)
//            && (this->structure != StructureEnum::INVALID));
}

///**
// * @return Surface containing selected node.
// */
//const Surface* 
//SelectionItemUniversalIdentificationSymbol::getSurface() const
//{
//    return this->surface;
//}
//
///**
// * @return Surface containing selected node.
// */
//Surface* 
//SelectionItemUniversalIdentificationSymbol::getSurface()
//{
//    return this->surface;
//}
//
///**
// * Set the surface containing the selected node.
// * @param surface
// *    New value for surface.
// *
// */
//void 
//SelectionItemUniversalIdentificationSymbol::setSurface(Surface* surface)
//{
//    this->surface = surface;
//}
//
///**
// * return Number of selected node.
// */
//int32_t 
//SelectionItemUniversalIdentificationSymbol::getNodeNumber() const
//{
//    return this->nodeNumber;
//}
//
///**
// * Set node number that was selected.
// * @param nodeNumber
// *    New value for node.
// */
//void 
//SelectionItemUniversalIdentificationSymbol::setNodeNumber(const int32_t nodeNumber)
//{
//    this->nodeNumber = nodeNumber;
//}
//
/////**
//// * @return number of nodes in surface
//// */
////int32_t
////SelectionItemUniversalIdentificationSymbol::getSurfaceNumberOfNodes() const
////{
////    return this->surfaceNumberOfNodes;
////}
////
///**
// * Set the number of nodes in the surface
// * @param numberOfNodes
// *    Number of nodes in surface
// */
//void
//SelectionItemUniversalIdentificationSymbol::setSurfaceNumberOfNodes(const int32_t numberOfNodes)
//{
//    this->surfaceNumberOfNodes = numberOfNodes;
//}
//
///**
// * @return The structure
// */
//StructureEnum::Enum
//SelectionItemUniversalIdentificationSymbol::getStructure() const
//{
//    return this->structure;
//}
//
//
///**
// * Set the number of nodes in the surface
// * @param structure
// *    Ths structure
// * @param surfaceNumberOfNodes
// *    Number of nodes in surface
// * @param nodeNumber
// *    New value for node.
// */
//void
//SelectionItemUniversalIdentificationSymbol::set(const StructureEnum::Enum structure,
//                                                  const int32_t surfaceNumberOfNodes,
//                                                  const int32_t nodeNumber)
//{
//    this->structure = structure;
//    this->surfaceNumberOfNodes = surfaceNumberOfNodes;
//    this->nodeNumber = nodeNumber;
//}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemUniversalIdentificationSymbol::toString() const
{
    AString text = SelectionItem::toString();
//    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
//    text += "Vertex: " + AString::number(this->nodeNumber) + "\n";
//    text += "Number of Vertices: " + AString::number(this->surfaceNumberOfNodes) + "\n";
    return text;
}
