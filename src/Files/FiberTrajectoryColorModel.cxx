
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __FIBER_TRAJECTORY_COLOR_MODEL_DECLARE__
#include "FiberTrajectoryColorModel.h"
#undef __FIBER_TRAJECTORY_COLOR_MODEL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::FiberTrajectoryColorModel 
 * \brief Combines Fiber Coloring Type and Caret Color enumerated types.
 * \ingroup Files
 *
 * Fiber trajectories may be colored using either the fiber coloring
 * type or a caret color.  This model allows these types to be 
 * treated as a single type.
 */

/**
 * Constructor.
 */
FiberTrajectoryColorModel::FiberTrajectoryColorModel()
: CaretObject()
{
    m_selectedItem = NULL;
    
    Item* item = new Item();
    m_allItems.push_back(item);
    
    std::vector<CaretColorEnum::Enum> allColors;
    CaretColorEnum::getAllEnums(allColors);
    for (std::vector<CaretColorEnum::Enum>::iterator iter = allColors.begin();
         iter != allColors.end();
         iter++) {
        Item* item = new Item(*iter);
        m_allItems.push_back(item);
    }

    if (m_allItems.empty()) {
        CaretAssert(0);
    }
    else {
        m_selectedItem = m_allItems[0];
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
}

/**
 * Destructor.
 */
FiberTrajectoryColorModel::~FiberTrajectoryColorModel()
{
    for (std::vector<Item*>::iterator iter = m_allItems.begin();
         iter != m_allItems.end();
         iter++) {
        Item* item = *iter;
        delete item;
    }
    m_allItems.clear();
    
    delete m_sceneAssistant;
}

/**
 * @return All items in this model.
 */
std::vector<FiberTrajectoryColorModel::Item*>
FiberTrajectoryColorModel::getValidItems()
{
    std::vector<FiberTrajectoryColorModel::Item*> items = m_allItems;
    return m_allItems;
}

/**
 * @return Pointer to selected item.
 */
FiberTrajectoryColorModel::Item*
FiberTrajectoryColorModel::getSelectedItem()
{
    if (m_selectedItem == NULL) {
        if (m_allItems.empty() == false) {
            m_selectedItem = m_allItems[0];
        }
    }
    
    return m_selectedItem;
}

/**
 * @return Pointer to selected item.
 */
const FiberTrajectoryColorModel::Item*
FiberTrajectoryColorModel::getSelectedItem() const
{
    FiberTrajectoryColorModel* nonConstThis = const_cast<FiberTrajectoryColorModel*>(this);
    const Item* item = nonConstThis->getSelectedItem();
    return item;
}

/**
 * Set the selected item.
 * @param item
 *   New selected item.
 */
void
FiberTrajectoryColorModel::setSelectedItem(const Item* item)
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
 * Set the selection to the given caret color.
 * @param color
 *   Color that is to be selected.
 */
void
FiberTrajectoryColorModel::setCaretColor(const CaretColorEnum::Enum color)
{
    std::vector<Item*> allItems = getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_CARET_COLOR) {
            if (allItems[i]->getCaretColor() == color) {
                setSelectedItem(allItems[i]);
                break;
            }
        }
    }
}

/**
 * @return Is the fiber coloring type selected?
 */
bool
FiberTrajectoryColorModel::isFiberOrientationColoringTypeSelected() const
{
    if (m_selectedItem->getItemType() == Item::ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE) {
        return true;
    }
    return false;
}


/**
 * Set the selection to the given fiber coloring type..
 * @param fiberColorType
 *   Fiber coloring type that is to be selected.
 */
void
FiberTrajectoryColorModel::setFiberOrientationColoringTypeSelected()
{
    std::vector<Item*> allItems = getValidItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        if (allItems[i]->getItemType() == Item::ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE) {
            setSelectedItem(allItems[i]);
            break;
        }
    }
}

/**
 * Copy the selection from the other model.
 *
 * @param other
 *    The other model.
 */
void
FiberTrajectoryColorModel::copy(const FiberTrajectoryColorModel& other) {
    const Item* otherItem = other.getSelectedItem();
    switch (otherItem->getItemType()) {
        case Item::ITEM_TYPE_CARET_COLOR:
            setCaretColor(otherItem->getCaretColor());
            break;
        case Item::ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
            setFiberOrientationColoringTypeSelected();
            break;
    }
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
FiberTrajectoryColorModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FiberTrajectoryColorModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (m_selectedItem != NULL) {
        sceneClass->addChild(m_selectedItem->saveToScene(sceneAttributes,
                                                         "m_selectedItem"));
    }
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
FiberTrajectoryColorModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    Item item;
    item.restoreFromScene(sceneAttributes,
                          sceneClass->getClass("m_selectedItem"));
    
    setSelectedItem(&item);
}


/* ========================================================================== */

/**
 * Constructor for a caret color.
 *
 * @param caretColor
 *    The caret color enum.
 */
FiberTrajectoryColorModel::Item::Item(const CaretColorEnum::Enum caretColor)
{
    m_caretColor = caretColor;
    m_itemType = ITEM_TYPE_CARET_COLOR;
    
    initializeAtEndOfConstruction();
}

/**
 * Constructor for a fiber coloring type.
 *
 * @param fiberColoringType
 *    The fiber coloring type.
 */
FiberTrajectoryColorModel::Item::Item()
{
    m_caretColor = CaretColorEnum::BLACK;
    m_itemType = ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE;
    
    initializeAtEndOfConstruction();
}

/**
 * Destructor.
 */
FiberTrajectoryColorModel::Item::~Item()
{
    delete m_sceneAssistant;
}

/**
 * Initialize at the end of construction.
 */
void
FiberTrajectoryColorModel::Item::initializeAtEndOfConstruction()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_caretColor",
                                                                &m_caretColor);
}

/**
 * Equality test for Item.
 *
 * @param item
 *     Item for comparison.
 * @return
 *     True if the same, else false.
 */
bool
FiberTrajectoryColorModel::Item::equals(const Item& item) const
{
    if (m_itemType == item.m_itemType) {
        switch (m_itemType) {
            case ITEM_TYPE_CARET_COLOR:
                if (m_caretColor == item.m_caretColor) {
                    return true;
                }
                break;
            case ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
                return true;
                break;
        }
    }
    
    return false;
}

/**
 * @return Name of the item.
 */
AString
FiberTrajectoryColorModel::Item::getName() const
{
    AString name = "PROGRAM_ERROR";
    switch (m_itemType) {
        case ITEM_TYPE_CARET_COLOR:
            name = CaretColorEnum::toGuiName(m_caretColor);
            break;
        case ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
            name = "Fiber";
            break;
    }
    
    return name;
}

/**
 * @return Type of the item.
 */
FiberTrajectoryColorModel::Item::ItemType
FiberTrajectoryColorModel::Item::getItemType() const
{
    return m_itemType;
}

/**
 * @return The caret color for this item.
 */
CaretColorEnum::Enum
FiberTrajectoryColorModel::Item::getCaretColor() const
{
    return m_caretColor;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
FiberTrajectoryColorModel::Item::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FiberTrajectoryColorModel::Item",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    switch (m_itemType) {
        case ITEM_TYPE_CARET_COLOR:
            sceneClass->addString("m_itemType",
                                  "ITEM_TYPE_CARET_COLOR");
            break;
        case ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE:
            sceneClass->addString("m_itemType",
                                  "ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE");
            break;
    }
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
FiberTrajectoryColorModel::Item::restoreFromScene(const SceneAttributes* sceneAttributes,
                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    const AString itemTypeName = sceneClass->getStringValue("m_itemType",
                                                            "ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE");
    
    m_itemType = ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE;
    if (itemTypeName == "ITEM_TYPE_CARET_COLOR") {
        m_itemType = ITEM_TYPE_CARET_COLOR;
    }
    else if (itemTypeName == "ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE") {
        m_itemType = ITEM_TYPE_FIBER_ORIENTATION_COLORING_TYPE;
    }
    else {
        CaretLogWarning(("Unrecognized value: "
                        + itemTypeName));
    }
}

