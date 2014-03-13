
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

#define __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__
#include "GroupAndNameHierarchyTreeWidgetItem.h"
#undef __CLASS_AND_NAME_HIERARCHY_TREE_WIDGET_ITEM_DECLARE__

#include "CaretAssert.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyModel.h"
#include "GroupAndNameHierarchyName.h"

using namespace caret;

/**
 * \class caret::ClassAndNameHierarchySelectionInfo
 * \brief Tree Widget Item for Class and Name Hierarchy
 * \ingroup GuiQt
 */

/**
 * Constructor for ClassAndNameHierarchyModel
 * @param classAndNameHierarchyModel
 *   The class name hierarchy model.
 */
GroupAndNameHierarchyTreeWidgetItem::GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyModel* classAndNameHierarchyModel)
: QTreeWidgetItem()
{
    CaretAssert(classAndNameHierarchyModel);
    initialize(classAndNameHierarchyModel,
               displayGroup,
                     tabIndex,
                     ITEM_TYPE_HIERARCHY_MODEL,
                     classAndNameHierarchyModel->getName(),
                     NULL);
    m_classAndNameHierarchyModel = classAndNameHierarchyModel;

    /*
     * Loop through each class
     */
    std::vector<GroupAndNameHierarchyItem*> classChildren =  m_classAndNameHierarchyModel->getChildren();
    for (std::vector<GroupAndNameHierarchyItem*>::iterator classIter = classChildren.begin();
         classIter != classChildren.end();
         classIter++) {
        GroupAndNameHierarchyItem* classItem = *classIter;
        CaretAssert(classItem);
        
        GroupAndNameHierarchyGroup* group = dynamic_cast<GroupAndNameHierarchyGroup*>(classItem);
        CaretAssert(group);
        GroupAndNameHierarchyTreeWidgetItem* groupItem = new GroupAndNameHierarchyTreeWidgetItem(displayGroup,
                                                                                             tabIndex,
                                                                                             group);
        addChildItem(groupItem);
    }
}

/**
 * Constructor for ClassDisplayGroupSelector
 * @param classDisplayGroupSelector
 *   The class display group selector.
 */
GroupAndNameHierarchyTreeWidgetItem::GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyGroup* classDisplayGroupSelector)
: QTreeWidgetItem()
{
    CaretAssert(classDisplayGroupSelector);
    initialize(classDisplayGroupSelector,
               displayGroup,
                     tabIndex,
                     ITEM_TYPE_CLASS,
                     classDisplayGroupSelector->getName(),
                     classDisplayGroupSelector->getIconColorRGBA());
    m_classDisplayGroupSelector = classDisplayGroupSelector;
    
    std::vector<GroupAndNameHierarchyItem*> nameChildren = m_classDisplayGroupSelector->getChildren();
    for (std::vector<GroupAndNameHierarchyItem*>::iterator nameIter = nameChildren.begin();
         nameIter != nameChildren.end();
         nameIter++) {
        GroupAndNameHierarchyItem* nameItem = *nameIter;
        CaretAssert(nameItem);
        
        GroupAndNameHierarchyName* name = dynamic_cast<GroupAndNameHierarchyName*>(nameItem);
        CaretAssert(name);
        GroupAndNameHierarchyTreeWidgetItem* nameTreeItem = new GroupAndNameHierarchyTreeWidgetItem(displayGroup,
                                                                                                 tabIndex,
                                                                                                 name);
        addChildItem(nameTreeItem);
    }
}

/**
 * Constructor for NameDisplayGroupSelector
 * @param nameDisplayGroupSelector
 *   The name display group selector.
 */
GroupAndNameHierarchyTreeWidgetItem::GroupAndNameHierarchyTreeWidgetItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyName* nameDisplayGroupSelector)
: QTreeWidgetItem()
{
    CaretAssert(nameDisplayGroupSelector);
    initialize(nameDisplayGroupSelector,
               displayGroup,
                     tabIndex,
                     ITEM_TYPE_NAME,
                     nameDisplayGroupSelector->getName(),
                     nameDisplayGroupSelector->getIconColorRGBA());
    m_nameDisplayGroupSelector = nameDisplayGroupSelector;
}

/**
 * Destructor.
 */
GroupAndNameHierarchyTreeWidgetItem::~GroupAndNameHierarchyTreeWidgetItem()
{
    /*
     * Note: Do not need to delete children since they are added to
     * Qt layouts which will delete them.
     */
}

/**
 * Initialize this instance.
 * @param itemType
 *    Type of item contained in this instance.
 */
void 
GroupAndNameHierarchyTreeWidgetItem::initialize(GroupAndNameHierarchyItem* groupAndNameHierarchyItem,
                                                const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex,
                                              const ItemType itemType,
                                              const QString text,
                                              const float* /*iconColorRGBA*/)
{
    m_groupAndNameHierarchyItem = groupAndNameHierarchyItem;
    m_classAndNameHierarchyModel = dynamic_cast<GroupAndNameHierarchyModel*>(groupAndNameHierarchyItem);
    m_classDisplayGroupSelector  = dynamic_cast<GroupAndNameHierarchyGroup*>(groupAndNameHierarchyItem);
    m_nameDisplayGroupSelector   = dynamic_cast<GroupAndNameHierarchyName*>(groupAndNameHierarchyItem);
    const int32_t count = (((m_classAndNameHierarchyModel != NULL) ? 1 : 0)
                           + ((m_classDisplayGroupSelector != NULL) ? 1 : 0)
                           + ((m_nameDisplayGroupSelector != NULL) ? 1 : 0));
    if (count != 1) {
        CaretAssertMessage(0,
                           "Invalid item added to group/name hierarchy tree.");
    }
    
    m_iconColorRGBA[0] = -1.0;
    m_iconColorRGBA[1] = -1.0;
    m_iconColorRGBA[2] = -1.0;
    m_iconColorRGBA[3] = -1.0;
    m_displayGroup = displayGroup;
    m_tabIndex = tabIndex;
    m_itemType = itemType;
//    m_classAndNameHierarchyModel = NULL;
//    m_classDisplayGroupSelector  = NULL;
//    m_nameDisplayGroupSelector   = NULL;
    m_hasChildren = false;

    switch (m_itemType) {
        case ITEM_TYPE_CLASS:
            m_hasChildren = true;
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            m_hasChildren = true;
            break;
        case ITEM_TYPE_NAME:
            m_hasChildren = false;
            break;
    }

    setText(TREE_COLUMN, text);
    
    /*Qt::ItemFlags itemFlags = (Qt::ItemIsSelectable
                              | Qt::ItemIsUserCheckable
                              | Qt::ItemIsEnabled);//*/
    if (m_hasChildren) {
    //    itemFlags |= Qt::ItemIsTristate;
    }
//    setFlags(itemFlags);   // NEW 11/14/12
    
//    if (iconColorRGBA != NULL) {
//        if (iconColorRGBA[3] > 0.0) {
//            QPixmap pm(10, 10);
//            pm.fill(QColor::fromRgbF(iconColorRGBA[0],
//                                     iconColorRGBA[1],
//                                     iconColorRGBA[2]));
//            QIcon icon(pm);
//            setIcon(TREE_COLUMN, icon);
//        }
//    }
    
    updateIconColorIncludingChildren();
}

/**
 * Update the selections in this and its children.
 * @param displayGroup
 *    Display group that is active.
 * @param tabIndex
 *    Index of tab that is displayed.
 */
void
GroupAndNameHierarchyTreeWidgetItem::updateSelections(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex)
{
    m_displayGroup = displayGroup;
    m_tabIndex     = tabIndex;
    GroupAndNameCheckStateEnum::Enum checkState = GroupAndNameCheckStateEnum::UNCHECKED;
    
    bool expandedStatus = false;
    switch (m_itemType) {
        case ITEM_TYPE_CLASS:
            CaretAssert(m_classDisplayGroupSelector);
            checkState = m_classDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = m_classDisplayGroupSelector->isExpandedToDisplayChildren(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            CaretAssert(m_classAndNameHierarchyModel);
            checkState = m_classAndNameHierarchyModel->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = m_classAndNameHierarchyModel->isExpandedToDisplayChildren(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_NAME:
            CaretAssert(m_nameDisplayGroupSelector);
            checkState = m_nameDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = false;
            break;
    }
    
    Qt::CheckState qtCheckState = toQCheckState(checkState);
    setCheckState(TREE_COLUMN,
                        qtCheckState);
    
    if (m_hasChildren) {
        setExpanded(expandedStatus);
        for (std::vector<GroupAndNameHierarchyTreeWidgetItem*>::iterator iter = m_children.begin();
             iter != m_children.end();
             iter++) {
            GroupAndNameHierarchyTreeWidgetItem* item = *iter;
            item->updateSelections(m_displayGroup, tabIndex);
        }
    }
}

/**
 * If this item's color has changed, update its icon.
 * Process all of its children.
 */
void
GroupAndNameHierarchyTreeWidgetItem::updateIconColorIncludingChildren()
{
    if (m_groupAndNameHierarchyItem != NULL) {
        const float* rgba = m_groupAndNameHierarchyItem->getIconColorRGBA();
        if (rgba != NULL) {
            if (rgba[3] > 0.0) {

                bool colorChanged = false;
                for (int32_t i = 0; i < 4; i++) {
                    if (m_iconColorRGBA[i] != rgba[i]) {
                        colorChanged = true;
                        break;
                    }
                }
                if (colorChanged) {
                    m_iconColorRGBA[0] = rgba[0];
                    m_iconColorRGBA[1] = rgba[1];
                    m_iconColorRGBA[2] = rgba[2];
                    m_iconColorRGBA[3] = rgba[3];
                    
                    QPixmap pm(10, 10);
                    pm.fill(QColor::fromRgbF(m_iconColorRGBA[0],
                                             m_iconColorRGBA[1],
                                             m_iconColorRGBA[2]));
                    QIcon icon(pm);
                    setIcon(TREE_COLUMN, icon);
                }
            }
        }
    }
    
    for (std::vector<GroupAndNameHierarchyTreeWidgetItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchyTreeWidgetItem* item = *iter;
        item->updateIconColorIncludingChildren();
    }
}


/**
 * Add a child.
 * @param child.
 *   The child.
 */
void
GroupAndNameHierarchyTreeWidgetItem::addChildItem(GroupAndNameHierarchyTreeWidgetItem* child)
{
    CaretAssert(child);
    m_children.push_back(child);
    addChild(child);
}

/**
 * @return ItemType of the selected item.
 */
GroupAndNameHierarchyTreeWidgetItem::ItemType 
GroupAndNameHierarchyTreeWidgetItem::getItemType() const 
{ 
    return m_itemType; 
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyModel* 
GroupAndNameHierarchyTreeWidgetItem::getClassAndNameHierarchyModel()
{
    CaretAssert(m_itemType == ITEM_TYPE_HIERARCHY_MODEL);
    return m_classAndNameHierarchyModel;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyGroup*
GroupAndNameHierarchyTreeWidgetItem::getClassDisplayGroupSelector()
{
    CaretAssert(m_itemType == ITEM_TYPE_CLASS);
    return m_classDisplayGroupSelector;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyName*
GroupAndNameHierarchyTreeWidgetItem::getNameDisplayGroupSelector()
{
    CaretAssert(m_itemType == ITEM_TYPE_NAME);
    return m_nameDisplayGroupSelector;
}

/**
 * Convert QCheckState to GroupAndNameCheckStateEnum
 * @param checkState
 *    The QCheckState
 * @return GroupAndNameCheckStateEnum converted from QCheckState
 */
GroupAndNameCheckStateEnum::Enum
GroupAndNameHierarchyTreeWidgetItem::fromQCheckState(const Qt::CheckState checkState)
{
    switch (checkState) {
        case Qt::Unchecked:
            return GroupAndNameCheckStateEnum::UNCHECKED;
            break;
        case Qt::PartiallyChecked:
            return GroupAndNameCheckStateEnum::PARTIALLY_CHECKED;
            break;
        case Qt::Checked:
            return GroupAndNameCheckStateEnum::CHECKED;
            break;
    }
    return GroupAndNameCheckStateEnum::UNCHECKED;
}

/**
 * Convert GroupAndNameCheckStateEnum to QCheckState
 * @param checkState
 *    The GroupAndNameCheckStateEnum
 * @return QCheckState converted from GroupAndNameCheckStateEnum converted.
 */
Qt::CheckState
GroupAndNameHierarchyTreeWidgetItem::toQCheckState(const GroupAndNameCheckStateEnum::Enum checkState)
{
    switch (checkState) {
        case GroupAndNameCheckStateEnum::CHECKED:
            return Qt::Checked;
            break;
        case GroupAndNameCheckStateEnum::PARTIALLY_CHECKED:
            return Qt::PartiallyChecked;
            break;
        case GroupAndNameCheckStateEnum::UNCHECKED:
            return Qt::Unchecked;
            break;
    }
    return Qt::Unchecked;
}

/**
 * Set the expanded status of the data in the model that is presented
 * by this item.
 * @param expanded
 *     Status of expansion.
 */
void
GroupAndNameHierarchyTreeWidgetItem::setModelDataExpanded(const bool expanded)
{
    GroupAndNameHierarchyItem* item = NULL;
    switch (m_itemType) {
        case ITEM_TYPE_CLASS:
            item = m_classDisplayGroupSelector;
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            item = m_classAndNameHierarchyModel;
            break;
        case ITEM_TYPE_NAME:
            item = m_nameDisplayGroupSelector;
            break;
    }
    
    if (item != NULL) {
        item->setExpandedToDisplayChildren(m_displayGroup,
                                           m_tabIndex,
                                           expanded);
    }
}

/**
 * Set the selected status of the data in the model that is presented
 * by this item.
 * @param selected
 *     Status of selection.
 */
void
GroupAndNameHierarchyTreeWidgetItem::setModelDataSelected(const bool selected)
{
    
    GroupAndNameHierarchyItem* item = NULL;
    switch (m_itemType) {
        case ITEM_TYPE_CLASS:
            item = m_classDisplayGroupSelector;
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            item = m_classAndNameHierarchyModel;
            break;
        case ITEM_TYPE_NAME:
            item = m_nameDisplayGroupSelector;
            break;
    }
    
    if (item != NULL) {
        const GroupAndNameCheckStateEnum::Enum existingCheckState = item->getCheckState(m_displayGroup,
                                                                                        m_tabIndex);
        
        switch (existingCheckState) {
            case GroupAndNameCheckStateEnum::CHECKED:
                break;
            case GroupAndNameCheckStateEnum::PARTIALLY_CHECKED:
                break;
            case GroupAndNameCheckStateEnum::UNCHECKED:
                break;
        }
        
        if (selected) {
            item->setSelected(m_displayGroup,
                              m_tabIndex,
                              true);
            item->setAncestorsSelected(m_displayGroup,
                                       m_tabIndex,
                                       true);
            item->setDescendantsSelected(m_displayGroup,
                                         m_tabIndex,
                                         true);
        }
        else {
            item->setSelected(m_displayGroup,
                              m_tabIndex,
                              false);
            item->setDescendantsSelected(m_displayGroup,
                                         m_tabIndex,
                                         false);
        }
    }
}


