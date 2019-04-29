
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

#define __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#undef __VOLUME_SURFACE_OUTLINE_COLOR_OR_TAB_MODEL_DECLARE__

#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "SceneClass.h"

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
    m_selectedItem = NULL;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        Item* item = new Item(i);
        m_allItems.push_back(item);
    }
    
    std::vector<CaretColorEnum::Enum> allColors;
    CaretColorEnum::getColorEnums(allColors);
    for (std::vector<CaretColorEnum::Enum>::iterator iter = allColors.begin();
         iter != allColors.end();
         iter++) {
        Item* item = new Item(*iter);
        m_allItems.push_back(item);
    }
    
    for (std::vector<Item*>::iterator iter = m_allItems.begin();
         iter != m_allItems.end();
         iter++) {
        Item* item = *iter;
        if (item->isValid()) {
            m_selectedItem = const_cast<Item*>(item);
            break;
        }
    }
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineColorOrTabModel::~VolumeSurfaceOutlineColorOrTabModel()
{
    for (std::vector<Item*>::iterator iter = m_allItems.begin();
         iter != m_allItems.end();
         iter++) {
        Item* item = *iter;
        delete item;
    }
    m_allItems.clear();
}

/**
 * Copy the given volume surface outline color or tab model.
 * @param modelToCopy
 *    Model that is copied.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::copyVolumeSurfaceOutlineColorOrTabModel(VolumeSurfaceOutlineColorOrTabModel* modelToCopy)
{
    Item* otherItem = modelToCopy->getSelectedItem();
    switch (otherItem->getItemType()) {
        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_COLOR:
            setColor(otherItem->getColor());
            break;
        case VolumeSurfaceOutlineColorOrTabModel::Item::ITEM_TYPE_BROWSER_TAB:
            setBrowserTabIndex(otherItem->getBrowserTabIndex());
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
     * Return all valid items
     */
    for (std::vector<Item*>::iterator iter = m_allItems.begin();
         iter != m_allItems.end();
         iter++) {
        Item* item = *iter;
        if (item->isValid()) {
            items.push_back(item);
        }        
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
    const int32_t numItems = static_cast<int32_t>(m_allItems.size());
    
    /*
     * Make sure selected item is valid
     */
    int32_t itemIndex = -1;
    if (m_selectedItem != NULL) {
        for (int32_t i = 0; i < numItems; i++) {
            if (m_allItems[i] == m_selectedItem) {
                if (m_allItems[i]->isValid() == false) {
                    /*
                     * Selected item is invalid
                     */
                    m_selectedItem = NULL;
                }
                itemIndex = i;
                
                break;
            }
        }
    }
    
    if (m_selectedItem == NULL) {
        
        /*
         * Choose the previous valid item
         */
        if (itemIndex >= 0) {
            for (int iBack = (itemIndex - 1); iBack >= 0; iBack--) {
                if (m_allItems[iBack]->isValid()) {
                    m_selectedItem = m_allItems[iBack];
                    break;
                }
            }
        }
        
        if (m_selectedItem == NULL) {
            /*
             * Choose first valid item
             */
            for (int i = 0; i < numItems; i++) {
                if (m_allItems[i]->isValid()) {
                    m_selectedItem = m_allItems[i];
                    break;
                }
            }
        }
    }
    
    return m_selectedItem;
}

/**
 * Set the selected item.
 * @param item
 *   New selected item.
 */
void
VolumeSurfaceOutlineColorOrTabModel::setSelectedItem(const Item* item)
{
    std::vector<Item*> allItems = getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (item->equals(*allItems[i])) {
            m_selectedItem = allItems[i];
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
    std::vector<Item*> allItems = getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_COLOR) {
            if (allItems[i]->getColor() == color) {
                setSelectedItem(allItems[i]);
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
    std::vector<Item*> allItems = getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_BROWSER_TAB) {
            if (allItems[i]->getBrowserTabIndex() == browserTabIndex) {
                setSelectedItem(allItems[i]);
                break;
            }
        }
    }
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
VolumeSurfaceOutlineColorOrTabModel::saveToScene(const SceneAttributes* sceneAttributes,
                                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSurfaceOutlineColorOrTabModel",
                                            1);
    
    if (m_selectedItem != NULL) {
        sceneClass->addChild(m_selectedItem->saveToScene(sceneAttributes,
                                                         "m_selectedItem"));
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
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    Item item(0);
    item.restoreFromScene(sceneAttributes,
                          sceneClass->getClass("m_selectedItem"));
    setSelectedItem(&item);
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
    m_color = color;
    m_browserTabIndex = 0;
    m_itemType = ITEM_TYPE_COLOR;
}

/**
 * Constructor for a browser tab.
 * @param browserTabIndex
 *   Index of browser tab.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::Item(const int32_t browserTabIndex)
{
    m_color = CaretColorEnum::BLACK;
    m_browserTabIndex = browserTabIndex;
    m_itemType = ITEM_TYPE_BROWSER_TAB;
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::~Item()
{
    
}

/**
 * Copy constructor.
 *
 * @param
 *    Item that is copied.
 */
VolumeSurfaceOutlineColorOrTabModel::Item::Item(const Item& item)
: SceneableInterface(item)
{
    m_color = item.m_color;
    m_browserTabIndex = item.m_browserTabIndex;
    m_itemType = item.m_itemType;
}

/**
 * Is this item equal to another item?
 *
 * @param item
 *    Item for comparison.
 * @return 
 *    True if items are equal, else false.
 */
bool
VolumeSurfaceOutlineColorOrTabModel::Item::equals(const Item& item) const
{
    if (m_itemType == item.m_itemType) {
        switch (m_itemType) {
            case ITEM_TYPE_BROWSER_TAB:
                if (m_browserTabIndex == item.m_browserTabIndex) {
                    return true;
                }
                break;
            case ITEM_TYPE_COLOR:
                if (m_color == item.m_color) {
                    return true;
                }
                break;
        }
    }
    
    return false;
}


/**
 * @return Is this item valid?
 */
bool 
VolumeSurfaceOutlineColorOrTabModel::Item::isValid() const
{
    bool valid = false;
    
    switch (m_itemType) {
        case ITEM_TYPE_BROWSER_TAB:
            if (getBrowserTabContent() != NULL) {
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
    
    switch(m_itemType) {
        case ITEM_TYPE_BROWSER_TAB:
        {
//            BrowserTabContent* btc = getBrowserTabContent();
//            if (btc != NULL) {
//                name = ("Tab "
//                        + AString::number(btc->getTabNumber() + 1));
//            }
            name = ("Tab "
                    + AString::number(m_browserTabIndex + 1));
        }
            break;
        case ITEM_TYPE_COLOR:
            name = CaretColorEnum::toGuiName(m_color);
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
    return m_itemType;
    
}

/**
 * @return Pointer to browser tab in this item or NULL
 * if this item does NOT contain a browser tab.
 */
BrowserTabContent* 
VolumeSurfaceOutlineColorOrTabModel::Item::getBrowserTabContent()
{
    EventBrowserTabGet getTabEvent(m_browserTabIndex);
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
    EventBrowserTabGet getTabEvent(m_browserTabIndex);
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
    return m_browserTabIndex;
}

/**
 * @return Enumerated type for color in this item.  Returned
 * value is undefined if a color is NOT in this item.
 */
CaretColorEnum::Enum 
VolumeSurfaceOutlineColorOrTabModel::Item::getColor()
{
    return m_color;    
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
VolumeSurfaceOutlineColorOrTabModel::Item::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                       const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSurfaceOutlineColorOrTabModel::Item",
                                            1);
    
    sceneClass->addInteger("m_browserTabIndex", 
                           m_browserTabIndex);
    sceneClass->addEnumeratedType<CaretColorEnum, CaretColorEnum::Enum>("m_color", 
                                                                        m_color);
    switch (m_itemType) {
        case ITEM_TYPE_COLOR:
            sceneClass->addString("m_itemType",
                                  "ITEM_TYPE_COLOR");
            break;
        case ITEM_TYPE_BROWSER_TAB:
            sceneClass->addString("m_itemType",
                                  "ITEM_TYPE_BROWSER_TAB");
            break;
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
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
VolumeSurfaceOutlineColorOrTabModel::Item::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                            const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_browserTabIndex = sceneClass->getIntegerValue("m_browserTabIndex");
    m_color = sceneClass->getEnumeratedTypeValue<CaretColorEnum, CaretColorEnum::Enum>("m_color", CaretColorEnum::BLUE);
    const AString itemTypeName = sceneClass->getStringValue("m_itemType",
                                                            "ITEM_TYPE_COLOR");
    if (itemTypeName == "ITEM_TYPE_BROWSER_TAB") {
        m_itemType = ITEM_TYPE_BROWSER_TAB;
    }
    else if (itemTypeName == "ITEM_TYPE_COLOR") {
        m_itemType = ITEM_TYPE_COLOR;
    }
    else {
        CaretAssert(0);
    }
}

