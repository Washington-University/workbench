
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

#define __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#undef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineColorOrTabModel 
 * \brief Model for selection of Color or Tab for Surface Outline
 *
 * Allows selection of a color or a browser tab for volume surface
 * outline.  If a color is selected, the surface outline is drawn
 * in that color.  If a browser tab is selected, the surface outline
 * is drawn in the current coloring for the selected surface using
 * the coloring assigned to the surface in the selected browser tab.
 *
 * Note: Only valid browser tabs are available for selection.
 */

/**
 * Constructor.
 */
VolumeSurfaceOutlineColorOrTabModel::VolumeSurfaceOutlineColorOrTabModel()
: CaretObject()
{
    this->selectedItem = NULL;
    this->previousSelectedItemIndex = -1;
    
    std::vector<CaretColorEnum::Enum> allColors;
    CaretColorEnum::getAllEnums(allColors, 
                                0);
    for (std::vector<CaretColorEnum::Enum>::iterator iter = allColors.begin();
         iter != allColors.end();
         iter++) {
        Item* item = new Item(*iter);
        this->colorItems.push_back(item);
    }
    
    this->browserTabItems.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                                 NULL);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        Item* item = new Item(i);
        this->browserTabItems[i] = item;
    }
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineColorOrTabModel::~VolumeSurfaceOutlineColorOrTabModel()
{
    for (std::vector<Item*>::iterator iter = this->colorItems.begin();
         iter != this->colorItems.end();
         iter++) {
        Item* item = *iter;
        delete item;
    }
    this->colorItems.clear();
    
    for (std::vector<Item*>::iterator iter = this->browserTabItems.begin();
         iter != this->browserTabItems.end();
         iter++) {
        Item* item = *iter;
        delete item;
    }
    this->browserTabItems.clear();
}

/**
 * Copy the given volume surface outline color or tab model.
 * @param modelToCopy
 *    Model that is copied.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::copyVolumeSurfaceOutlineColorOrTabModel(VolumeSurfaceOutlineColorOrTabModel* modelToCopy)
{
    switch (this->getSelectedItem()->getItemType()) {
        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
            this->setColor(modelToCopy->getSelectedItem()->getColor());
            break;
        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
            this->setBrowserTabIndex(modelToCopy->getSelectedItem()->getBrowserTabIndex());
            break;
    }
}

/**
 * @return All of the valid items for this model.
 */
std::vector<VolumeSurfaceOutlineColorOrTabModel::Item*> 
VolumeSurfaceOutlineColorOrTabModel::getValidItems()
{
    std::vector<Item*> items;

    /*
     * Limit to valid tabs
     */
    for (std::vector<Item*>::iterator iter = this->browserTabItems.begin();
         iter != this->browserTabItems.end();
         iter++) {
        Item* item = *iter;
        if (item->isValid()) {
            items.push_back(item);
        }        
    }
    
    /*
     * All color items are valid
     */
    for (std::vector<Item*>::iterator iter = this->colorItems.begin();
         iter != this->colorItems.end();
         iter++) {
        Item* item = *iter;
        items.push_back(item);
    }
    
    return items;    
}

/**
 * @return Pointer to selected item (NULL if selection
 * is invalid.
 */
VolumeSurfaceOutlineColorOrTabModel::Item* 
VolumeSurfaceOutlineColorOrTabModel::getSelectedItem()
{
    std::vector<Item*> allItems = this->getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    bool foundSelctedItem = false;
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i] == this->selectedItem) {
            foundSelctedItem = true;
            this->previousSelectedItemIndex = i;
            break;
        }
    }
    
    if (foundSelctedItem == false) {
        this->selectedItem = NULL;
    }
    
    if (this->selectedItem == NULL) {
        if (this->previousSelectedItemIndex >= 0) {
            if (this->previousSelectedItemIndex >= numItems) {
                this->previousSelectedItemIndex = numItems - 1;
            }
        }
        else {
            if (numItems > 0) {
                this->previousSelectedItemIndex = 0;
            }
        }

        if (this->previousSelectedItemIndex >= 0) {
            this->selectedItem = allItems[this->previousSelectedItemIndex];
        }
    }
    
    return this->selectedItem;
}

/**
 * Set the selected item.
 * @param item
 *   New selected item.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::setSelectedItem(Item* item)
{
    this->selectedItem = item;
    std::vector<Item*> allItems = this->getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i] == this->selectedItem) {
            this->previousSelectedItemIndex = i;
            break;
        }
    }
}

/**
 * Set the selection to the given color.
 * @param color
 *   Color that is to be selected.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::setColor(const CaretColorEnum::Enum color)
{
    std::vector<Item*> allItems = this->getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_COLOR) {
            if (allItems[i]->getColor() == color) {
                this->setSelectedItem(allItems[i]);
                break;
            }
        }
    }
}

/**
 * Set the selection to the given browser tab.
 * @param browserTabIndex
 *    Index of browser tab for selection.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::setBrowserTabIndex(const int32_t browserTabIndex)
{
    std::vector<Item*> allItems = this->getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_BROWSER_TAB) {
            if (allItems[i]->getBrowserTabIndex() == browserTabIndex) {
                this->setSelectedItem(allItems[i]);
                break;
            }
        }
    }
}


//======================================================================

/**
 * \class caret::VolumeSurfaceOutlineColorOrTabModel::Item 
 * \brief An item in VolumeSurfaceOutlineColorOrTabModel.
 *
 * At this time, item is either a color or a browser
 * tab index.
 */

/**
 * Constructor for a Caret Color.
 * @param color
 *   The caret color.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::Item(const CaretColorEnum::Enum color)
{
    this->color = color;
    this->browserTabIndex = 0;
    this->itemType = ITEM_TYPE_COLOR;
}

/**
 * Constructor for a browser tab.
 * @param browserTabIndex
 *   Index of browser tab.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::Item(const int32_t browserTabIndex)
{
    this->color = CaretColorEnum::CLASS;
    this->browserTabIndex = browserTabIndex;
    this->itemType = ITEM_TYPE_BROWSER_TAB;
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::~Item()
{
    
}

/**
 * @return Is this item valid?
 */
bool 
VolumeSurfaceOutlineColorOrTabModel::Item::isValid() const
{
    bool valid = false;
    
    switch (this->itemType) {
        case ITEM_TYPE_BROWSER_TAB:
            if (this->getBrowserTabContent() != NULL) {
                valid = true;
            }
            break;
        case ITEM_TYPE_COLOR:
            valid = true;
            break;
    }
    
    return valid;
}

/**
 * @return  Name of this item.
 */
AString
VolumeSurfaceOutlineColorOrTabModel::Item::getName()
{
    AString name = "PROGRAM ERROR";
    
    switch(this->itemType) {
        case ITEM_TYPE_BROWSER_TAB:
            name = ("Tab "
                    + AString::number(this->getBrowserTabContent()->getTabNumber() + 1));
            break;
        case ITEM_TYPE_COLOR:
            name = CaretColorEnum::toGuiName(this->color);
            break;
    }
    
    return name;
}

/**
 * @return Type of item.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::ItemType 
VolumeSurfaceOutlineColorOrTabModel::Item::getItemType() const
{
    return this->itemType;
    
}

/**
 * @return Pointer to browser tab in this item or NULL
 * if this item does NOT contain a browser tab.
 */
BrowserTabContent* 
VolumeSurfaceOutlineColorOrTabModel::Item::getBrowserTabContent()
{
    EventBrowserTabGet getTabEvent(this->browserTabIndex);
    EventManager::get()->sendEvent(getTabEvent.getPointer());
    
    BrowserTabContent* tabContent = getTabEvent.getBrowserTab();
    
    return tabContent;
}

/**
 * @return Pointer to browser tab in this item or NULL
 * if this item does NOT contain a browser tab.
 */
const BrowserTabContent* 
VolumeSurfaceOutlineColorOrTabModel::Item::getBrowserTabContent() const
{
    EventBrowserTabGet getTabEvent(this->browserTabIndex);
    EventManager::get()->sendEvent(getTabEvent.getPointer());
    
    BrowserTabContent* tabContent = getTabEvent.getBrowserTab();
    
    return tabContent;
}

/**
 * @return Index of browser tab in this item.  This will always
 * return an integer greater than or equal to zero.  Use isItemValid()
 * to ensure this item is valid.
 */
int32_t 
VolumeSurfaceOutlineColorOrTabModel::Item::getBrowserTabIndex() const
{
    return this->browserTabIndex;
}

/**
 * @return Enumerated type for color in this item.  Returned
 * value is undefined if a color is NOT in this item.
 */
CaretColorEnum::Enum 
VolumeSurfaceOutlineColorOrTabModel::Item::getColor()
{
    return this->color;    
}

