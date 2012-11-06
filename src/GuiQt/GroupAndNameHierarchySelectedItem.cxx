
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

#include <QAction>
#include <QCheckBox>
#include <QToolButton>
#include <QVBoxLayout>

#define __CLASS_AND_NAME_HIERARCHY_SELECTED_ITEM_DECLARE__
#include "GroupAndNameHierarchySelectedItem.h"
#undef __CLASS_AND_NAME_HIERARCHY_SELECTED_ITEM_DECLARE__

#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyModel.h"
#include "GroupAndNameHierarchyName.h"
#include "WuQtUtilities.h"

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
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyModel* classAndNameHierarchyModel,
                                                                     QWidget* parent)
: QWidget(parent)
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
        
        GroupAndNameHierarchySelectedItem* classItem = new GroupAndNameHierarchySelectedItem(displayGroup,
                                                                                             tabIndex,
                                                                                             classSelector);
        addChild(classItem);
    }
}

/**
 * Constructor for ClassDisplayGroupSelector
 * @param classDisplayGroupSelector
 *   The class display group selector.
 */
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyGroup* classDisplayGroupSelector,
                                                                     QWidget* parent)
: QWidget(parent)
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
        
            GroupAndNameHierarchySelectedItem* nameInfo =
            new GroupAndNameHierarchySelectedItem(displayGroup,
                                                  tabIndex,
                                                  nameSelector);
            this->addChild(nameInfo);
    }    
}

/**
 * Constructor for NameDisplayGroupSelector
 * @param nameDisplayGroupSelector
 *   The name display group selector.
 */
GroupAndNameHierarchySelectedItem::GroupAndNameHierarchySelectedItem(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex,
                                                                     GroupAndNameHierarchyName* nameDisplayGroupSelector,
                                                                     QWidget* parent)
: QWidget(parent)
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
GroupAndNameHierarchySelectedItem::~GroupAndNameHierarchySelectedItem()
{
    /* 
     * Note: Do not need to delete children since they are added to
     * Qt layouts which will delete them.
     */
    std::cout << "Deleting " << qPrintable(m_checkBox->text()) << std::endl;
    if (m_childrenLayout != NULL) {
        std::cout << "Child Layout has " << m_childrenLayout->count() << std::endl;
    }
}

/**
 * Initialize this instance.
 * @param itemType
 *    Type of item contained in this instance.
 */
void 
GroupAndNameHierarchySelectedItem::initialize(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t tabIndex,
                                              const ItemType itemType,
                                              const QString text,
                                              const float* iconColorRGBA)
{
    std::cout << "Creating checkbox: " << qPrintable(text) << std::endl;
    
    m_displayGroup = displayGroup;
    m_tabIndex = tabIndex;
    this->itemType = itemType;
    this->classAndNameHierarchyModel = NULL;
    this->classDisplayGroupSelector  = NULL;
    this->nameDisplayGroupSelector   = NULL;

    m_checkBox = new QCheckBox(text);
    QObject::connect(m_checkBox, SIGNAL(stateChanged(int)),
                     this, SLOT(checkBoxStateChanged(int)));
    
    bool hasChildren = false;
    switch (this->itemType) {
        case ITEM_TYPE_CLASS:
            hasChildren = true;
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            hasChildren = true;
            break;
        case ITEM_TYPE_NAME:
            break;
    }
    m_checkBox->setTristate(hasChildren);    
    
    if (iconColorRGBA != NULL) {
        if (iconColorRGBA[3] > 0.0) {
            QPixmap pm(10, 10);
            pm.fill(QColor::fromRgbF(iconColorRGBA[0],
                                     iconColorRGBA[1],
                                     iconColorRGBA[2]));
            QIcon icon(pm);
            m_checkBox->setIcon(icon);
        }
    }

    m_expandCollapseAction = NULL;
    if (hasChildren) {
        m_expandCollapseAction = new QAction("E",
                                             this);
        m_expandCollapseAction->setCheckable(true);
        QObject::connect(m_expandCollapseAction, SIGNAL(triggered(bool)),
                         this, SLOT(expandCollapseActionTriggered(bool)));
    }
    
    QHBoxLayout* rowLayout = new QHBoxLayout();
    rowLayout->setContentsMargins(0, 0, 0, 0);
    if (m_expandCollapseAction != NULL) {
        QToolButton* expandCollapseToolButton = new QToolButton();
        expandCollapseToolButton->setDefaultAction(m_expandCollapseAction);
        rowLayout->addWidget(expandCollapseToolButton);
    }
    rowLayout->addWidget(m_checkBox);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    if (hasChildren) {
        layout->setContentsMargins(10, 2, 5, 0);
    }
    else {
        layout->setContentsMargins(10, 0, 5, 0);
    }
    layout->addLayout(rowLayout);
    
    m_childrenWidget = NULL;
    m_childrenLayout = NULL;
    if (hasChildren) {
        m_childrenWidget = new QWidget();
        m_childrenLayout = new QVBoxLayout(m_childrenWidget);
        m_childrenLayout->setContentsMargins(10, 0, 5, 0);
        m_childrenLayout->setSpacing(2);
        layout->addWidget(m_childrenWidget);
    }
    //layout->addStretch();
}

void
GroupAndNameHierarchySelectedItem::expandCollapseActionTriggered(bool status)
{
    m_childrenWidget->setVisible(status);
    std::cout << "Expand/Collapse" << status << std::endl;
}

void
GroupAndNameHierarchySelectedItem::updateSelections()
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
    m_checkBox->blockSignals(true);
    m_checkBox->setCheckState(qtCheckState);
    m_checkBox->setVisible(true);
    m_checkBox->blockSignals(false);
    
    if (m_childrenWidget != NULL) {
        if (m_childrenWidget->isVisible() != expandedStatus) {
            m_childrenWidget->setVisible(expandedStatus);
        }
        m_expandCollapseAction->setChecked(expandedStatus);
    }
    
    for (std::vector<GroupAndNameHierarchySelectedItem*>::iterator iter = m_children.begin();
         iter != m_children.end();
         iter++) {
        GroupAndNameHierarchySelectedItem* item = *iter;
        item->updateSelections();
    }
}

/**
 * Called when a checkbox state is changed.
 * @param state
 *    New state
 */
void
GroupAndNameHierarchySelectedItem::checkBoxStateChanged(int state)
{
    std::cout << qPrintable(m_checkBox->text())
    << " state: " << state << std::endl;

    const bool selected = (state != Qt::Unchecked);
    
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
    
    emit statusChanged();
}

/**
 * Called when status of a child is changed.
 */
void
GroupAndNameHierarchySelectedItem::childStatusWasChanged()
{
    GroupAndNameCheckStateEnum::Enum newCheckState = GroupAndNameCheckStateEnum::UNCHECKED;
    
    switch (this->itemType) {
        case ITEM_TYPE_CLASS:
            newCheckState = this->classDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_HIERARCHY_MODEL:
            newCheckState = this->classAndNameHierarchyModel->getCheckState(m_displayGroup, m_tabIndex);
            break;
        case ITEM_TYPE_NAME:
            newCheckState = this->nameDisplayGroupSelector->getCheckState(m_displayGroup, m_tabIndex);
            break;
    }
    
    GroupAndNameCheckStateEnum::Enum oldCheckState = fromQCheckState(m_checkBox->checkState());
    
    /*
     * Did my state changed due to a child changing its state
     */
//    if (newCheckState != oldCheckState) {
        Qt::CheckState qtCheckState = toQCheckState(newCheckState);
        m_checkBox->setCheckState(qtCheckState);
        emit statusChanged();
//    }
//    else {
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        if (m_displayGroup != DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().addToolBox().getPointer());
        }
//    }
}

/**
 * Add a child.
 * @param child.
 *   The child.
 */
void
GroupAndNameHierarchySelectedItem::addChild(GroupAndNameHierarchySelectedItem* child)
{
    CaretAssert(child);
    m_children.push_back(child);
    CaretAssert(m_childrenLayout);
    m_childrenLayout->addWidget(child); //child->m_checkBox);
    
    QObject::connect(child, SIGNAL(statusChanged()),
                     this, SLOT(childStatusWasChanged()));
    std::cout << "Added child: " << qPrintable(child->m_checkBox->text()) << std::endl;
}

/**
 * Return the layout containing the children
 */
QVBoxLayout*
GroupAndNameHierarchySelectedItem::getChildrenLayout()
{
    return m_childrenLayout;
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

/**
 * Convert QCheckState to GroupAndNameCheckStateEnum
 * @param checkState
 *    The QCheckState
 * @return GroupAndNameCheckStateEnum converted from QCheckState
 */
GroupAndNameCheckStateEnum::Enum
GroupAndNameHierarchySelectedItem::fromQCheckState(const Qt::CheckState checkState)
{
    switch (checkState) {
        case Qt::Unchecked:
            return GroupAndNameCheckStateEnum::CHECKED;
            break;
        case Qt::PartiallyChecked:
            return GroupAndNameCheckStateEnum::PARTIALLY_CHECKED;
            break;
        case Qt::Checked:
            return GroupAndNameCheckStateEnum::UNCHECKED;
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
GroupAndNameHierarchySelectedItem::toQCheckState(const GroupAndNameCheckStateEnum::Enum checkState)
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


