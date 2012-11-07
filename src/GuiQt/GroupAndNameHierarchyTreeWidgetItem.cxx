
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
    this->initialize(displayGroup,
                     tabIndex,
                     ITEM_TYPE_HIERARCHY_MODEL,
                     classAndNameHierarchyModel->getName(),
                     NULL);
    this->classAndNameHierarchyModel = classAndNameHierarchyModel;

    /*
     * Loop through each class
     */
    const std::vector<int32_t> classKeysVector = classAndNameHierarchyModel->getAllGroupKeysSortedByName();
    for (std::vector<int32_t>::const_iterator classIter = classKeysVector.begin();
         classIter != classKeysVector.end();
         classIter++) {
        const int32_t classKey = *classIter;
        GroupAndNameHierarchyGroup* classSelector = classAndNameHierarchyModel->getGroupSelectorForGroupKey(classKey);
        CaretAssert(classSelector);
        
        GroupAndNameHierarchyTreeWidgetItem* classItem = new GroupAndNameHierarchyTreeWidgetItem(displayGroup,
                                                                                             tabIndex,
                                                                                             classSelector);
        addChildItem(classItem);
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
    this->initialize(displayGroup,
                     tabIndex,
                     ITEM_TYPE_CLASS,
                     classDisplayGroupSelector->getName(),
                     classDisplayGroupSelector->getIconColorRGBA());
    this->classDisplayGroupSelector = classDisplayGroupSelector;
    
    /*
     * Names in the model
     */
    const std::vector<int32_t> nameKeysVector = this->classDisplayGroupSelector->getAllNameKeysSortedByName();
    for (std::vector<int32_t>::const_iterator nameIter = nameKeysVector.begin();
         nameIter != nameKeysVector.end();
         nameIter++) {
        const int32_t nameKey = *nameIter;
        GroupAndNameHierarchyName* nameSelector = this->classDisplayGroupSelector->getNameSelectorWithKey(nameKey);
        CaretAssert(nameSelector);
        
            GroupAndNameHierarchyTreeWidgetItem* nameInfo =
            new GroupAndNameHierarchyTreeWidgetItem(displayGroup,
                                                  tabIndex,
                                                  nameSelector);
            this->addChildItem(nameInfo);
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
    this->initialize(displayGroup,
                     tabIndex,
                     ITEM_TYPE_NAME,
                     nameDisplayGroupSelector->getName(),
                     nameDisplayGroupSelector->getIconColorRGBA());
    this->nameDisplayGroupSelector = nameDisplayGroupSelector;
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
    std::cout << "Deleting " << qPrintable(this->text(TREE_COLUMN)) << std::endl;
}

/**
 * Initialize this instance.
 * @param itemType
 *    Type of item contained in this instance.
 */
void 
GroupAndNameHierarchyTreeWidgetItem::initialize(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex,
                                              const ItemType itemType,
                                              const QString text,
                                              const float* iconColorRGBA)
{
    std::cout << "Creating GroupAndNameHierarchyTreeWidgetItem: " << qPrintable(text) << std::endl;
    
    m_displayGroup = displayGroup;
    m_tabIndex = tabIndex;
    this->itemType = itemType;
    this->classAndNameHierarchyModel = NULL;
    this->classDisplayGroupSelector  = NULL;
    this->nameDisplayGroupSelector   = NULL;
    m_hasChildren = false;

    switch (this->itemType) {
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

    this->setText(TREE_COLUMN, text);
    
    Qt::ItemFlags itemFlags = (Qt::ItemIsSelectable
                              | Qt::ItemIsUserCheckable
                              | Qt::ItemIsEnabled);
    if (m_hasChildren) {
        itemFlags |= Qt::ItemIsTristate;
    }
    
    if (iconColorRGBA != NULL) {
        if (iconColorRGBA[3] > 0.0) {
            QPixmap pm(10, 10);
            pm.fill(QColor::fromRgbF(iconColorRGBA[0],
                                     iconColorRGBA[1],
                                     iconColorRGBA[2]));
            QIcon icon(pm);
            this->setIcon(TREE_COLUMN, icon);
        }
    }
}

void
GroupAndNameHierarchyTreeWidgetItem::updateSelections()
{
    GroupAndNameCheckStateEnum::Enum checkState = GroupAndNameCheckStateEnum::UNCHECKED;
    
    bool expandedStatus = false;
    switch (this->itemType) {
        case ITEM_TYPE_CLASS:
            checkState = this->classDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = this->classDisplayGroupSelector->isExpanded(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            checkState = this->classAndNameHierarchyModel->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = this->classAndNameHierarchyModel->isExpanded(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_NAME:
            checkState = this->nameDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            expandedStatus = false;
            break;
    }
    
    Qt::CheckState qtCheckState = toQCheckState(checkState);
    this->setCheckState(TREE_COLUMN,
                        qtCheckState);
    
    if (m_hasChildren) {
        this->setExpanded(expandedStatus);
        for (std::vector<GroupAndNameHierarchyTreeWidgetItem*>::iterator iter = m_children.begin();
             iter != m_children.end();
             iter++) {
            GroupAndNameHierarchyTreeWidgetItem* item = *iter;
            item->updateSelections();
        }
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
    this->addChild(child);
    
    
    std::cout << "Added child: " << qPrintable(child->text(TREE_COLUMN)) << std::endl;
}

/**
 * @return ItemType of the selected item.
 */
GroupAndNameHierarchyTreeWidgetItem::ItemType 
GroupAndNameHierarchyTreeWidgetItem::getItemType() const 
{ 
    return this->itemType; 
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyModel* 
GroupAndNameHierarchyTreeWidgetItem::getClassAndNameHierarchyModel()
{
    CaretAssert(this->itemType == ITEM_TYPE_HIERARCHY_MODEL);
    return this->classAndNameHierarchyModel;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyGroup*
GroupAndNameHierarchyTreeWidgetItem::getClassDisplayGroupSelector()
{
    CaretAssert(this->itemType == ITEM_TYPE_CLASS);
    return this->classDisplayGroupSelector;
}

/**
 * @return The class name and hierarchy model.  NULL
 * if this instance contains another type of data.
 */
GroupAndNameHierarchyName*
GroupAndNameHierarchyTreeWidgetItem::getNameDisplayGroupSelector()
{
    CaretAssert(this->itemType == ITEM_TYPE_NAME);
    return this->nameDisplayGroupSelector;
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
    switch (this->itemType) {
        case ITEM_TYPE_CLASS:
            this->classDisplayGroupSelector->setExpanded(m_displayGroup,
                                                         m_tabIndex,
                                                         expanded);
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            this->classAndNameHierarchyModel->setExpanded(m_displayGroup,
                                                          m_tabIndex,
                                                          expanded);
            break;
        case ITEM_TYPE_NAME:
            break;
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
    switch (this->itemType) {
        case ITEM_TYPE_CLASS:
            this->classDisplayGroupSelector->setSelected(m_displayGroup,
                                                         m_tabIndex,
                                                         selected);
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            this->classAndNameHierarchyModel->setSelected(m_displayGroup,
                                                          m_tabIndex,
                                                          selected);
            break;
        case ITEM_TYPE_NAME:
            this->nameDisplayGroupSelector->setSelected(m_displayGroup,
                                                        m_tabIndex,
                                                        selected);
            break;
    }
}


