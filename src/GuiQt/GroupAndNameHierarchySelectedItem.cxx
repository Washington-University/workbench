
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
#include "GroupAndNameHierarchySelectedItem.h"
#undef __CLASS_AND_NAME_HIERARCHY_SELECTED_ITEM_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::ClassAndNameHierarchySelectionInfo
 * \brief Provides information about item that was selected.
 */

/**
 * Constructor for ClassAndNameHierarchyModel
 * @param classAndNameHierarchyModel
 *   The class name hierarchy model.
 */
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(GroupAndNameHierarchyModel* classAndNameHierarchyModel)
{
    this->initialize(ITEM_TYPE_HIERARCHY_MODEL);
    this->classAndNameHierarchyModel = classAndNameHierarchyModel;
}

/**
 * Constructor for ClassDisplayGroupSelector
 * @param classDisplayGroupSelector
 *   The class display group selector.
 */
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(GroupAndNameHierarchyGroup* classDisplayGroupSelector)
{
    this->initialize(ITEM_TYPE_CLASS);
    this->classDisplayGroupSelector = classDisplayGroupSelector;    
}

/**
 * Constructor for NameDisplayGroupSelector
 * @param nameDisplayGroupSelector
 *   The name display group selector.
 */
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(GroupAndNameHierarchyName* nameDisplayGroupSelector)
{
    this->initialize(ITEM_TYPE_NAME);
    this->nameDisplayGroupSelector = nameDisplayGroupSelector;
}


/**
 * Destructor.
 */
GroupAndNameHierarchySelectedItem::~GroupAndNameHierarchySelectedItem()
{
}

/**
 * Initialize this instance.
 * @param itemType
 *    Type of item contained in this instance.
 */
void 
GroupAndNameHierarchySelectedItem::initialize(const ItemType itemType)
{
    this->itemType = itemType;
    this->classAndNameHierarchyModel = NULL;
    this->classDisplayGroupSelector  = NULL;
    this->nameDisplayGroupSelector   = NULL;
}


/**
 * @return ItemType of the selected item.
 */
GroupAndNameHierarchySelectedItem::ItemType 
GroupAndNameHierarchySelectedItem::getItemType() const 
{ 
    return this->itemType; 
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyModel* 
GroupAndNameHierarchySelectedItem::getClassAndNameHierarchyModel()
{
    CaretAssert(this->itemType == ITEM_TYPE_HIERARCHY_MODEL);
    return this->classAndNameHierarchyModel;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyGroup*
GroupAndNameHierarchySelectedItem::getClassDisplayGroupSelector()
{
    CaretAssert(this->itemType == ITEM_TYPE_CLASS);
    return this->classDisplayGroupSelector;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyName*
GroupAndNameHierarchySelectedItem::getNameDisplayGroupSelector()
{
    CaretAssert(this->itemType == ITEM_TYPE_NAME);
    return this->nameDisplayGroupSelector;
}

