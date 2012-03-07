
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

#define __CLASS_AND_NAME_HIERARCHY_SELECTED_ITEM_DECLARE__
#include "ClassAndNameHierarchySelectedItem.h"
#undef __CLASS_AND_NAME_HIERARCHY_SELECTED_ITEM_DECLARE__

#include "BorderFile.h"
#include "GiftiLabel.h"

using namespace caret;


    
/**
 * \class caret::ClassAndNameHierarchySelectionInfo
 * \brief Provides information about item that was selected.
 */

/**
 * Constructor.
 * @param itemType 
 *     The type of the itemData.
 * @param itemData
 *     Pointer to the item corresponding to itemType.
 */
ClassAndNameHierarchySelectedItem::ClassAndNameHierarchySelectedItem(const ItemType itemType,
                                                                     void* itemData)
: CaretObject()
{
    this->itemType = itemType;
    this->itemData = itemData;
}

/**
 * Destructor.
 */
ClassAndNameHierarchySelectedItem::~ClassAndNameHierarchySelectedItem()
{
    
}

/**
 * @return ItemType of the selected item.
 */
ClassAndNameHierarchySelectedItem::ItemType 
ClassAndNameHierarchySelectedItem::getItemType() const 
{ 
    return this->itemType; 
}

/**
 * @return Border file that was selected.  NULL if item
 * type is not a border file.
 */
ClassAndNameHierarchyModel* 
ClassAndNameHierarchySelectedItem::getClassAndNameHierarchyModel() const
{
    ClassAndNameHierarchyModel* hierarchyModel = NULL;
    switch (this->itemType) {
        case ITEM_TYPE_HIERARCHY_MODEL:
            hierarchyModel = (ClassAndNameHierarchyModel*)this->itemData;
            break;
        case ITEM_TYPE_CLASS:
            break;
        case ITEM_TYPE_NAME:
            break;
    }
    return hierarchyModel;
}

/**
 * @return Gifti Label that was selected.  NULL if item
 * type is not a Gifti Label.
 */
GiftiLabel* 
ClassAndNameHierarchySelectedItem::getDataAsGiftiLabel() const
{
    GiftiLabel* giftiLabel = NULL;
    switch (this->itemType) {
        case ITEM_TYPE_HIERARCHY_MODEL:
            break;
        case ITEM_TYPE_CLASS:
        case ITEM_TYPE_NAME:
            giftiLabel = (GiftiLabel*)this->itemData;
            break;
    }
    return giftiLabel;
}
