
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_DECLARE__
#include "TileTabGridRowColumnWidgets.h"
#undef __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>

#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "TileTabsConfigurationDialog.h"
#include "TileTabsGridRowColumnElement.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::TileTabGridRowColumnWidgets 
 * \brief Contains info about a tile tabs grid row or column
 * \ingroup GuiQt
 */


/**
 * Constructor.
 *
 * @param tileTabsConfigurationDialog
 *    The tile tabs configuration dialog.
 * @param rowColumnType
 *    'Row' or 'Column'
 * @param index
 *    Index of the row/column
 * @param gridLayout
 *    Gridlayout for widgets
 * @param parent
 *    Parent QObject
 */
TileTabGridRowColumnWidgets::TileTabGridRowColumnWidgets(TileTabsConfigurationDialog* tileTabsConfigurationDialog,
                                             const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                                             const int32_t index,
                                             QGridLayout* gridLayout,
                                             QObject* parent)
: QObject(parent),
m_tileTabsConfigurationDialog(tileTabsConfigurationDialog),
m_rowColumnType(rowColumnType),
m_index(index),
m_element(NULL)
{
    m_indexLabel = new QLabel(QString::number(m_index + 1));
    m_indexLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    const AString rowColText((rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::ROW)
                             ? "Row"
                             : "Column");
    const AString contructionToolTip(WuQtUtilities::createWordWrappedToolTipText("Delete, Duplicate, or Move "
                                                                                 + rowColText));
    const AString contentToolTip(WuQtUtilities::createWordWrappedToolTipText("Content of the "
                                                                             + rowColText
                                                                             + ": Spacer (empty space for Annotations) "
                                                                             "or Tabs (Browser Tabs)"));
    const AString typeToolTip(WuQtUtilities::createWordWrappedToolTipText("Type of Stretching: Percent or Weight"));
    const AString stretchToolTip(WuQtUtilities::createWordWrappedToolTipText("Value of Stretching Percentage [0, 100] or Stretching Weight"));
    
    /*
     * Construction Tool Button
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionAction = WuQtUtilities::createAction("M",
                                                       "Add/Move/Remove",
                                                       this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton);
    QObject::connect(constructionMenu, &QMenu::aboutToShow,
                     this, &TileTabGridRowColumnWidgets::constructionMenuAboutToShow);
    QObject::connect(constructionMenu, &QMenu::triggered,
                     this, &TileTabGridRowColumnWidgets::constructionMenuTriggered);
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    m_constructionToolButton->setFixedWidth(m_constructionToolButton->sizeHint().width());
    m_constructionToolButton->setToolTip(contructionToolTip);
    
    /*
     * Content type combo box
     */
    m_contentTypeComboBox = new EnumComboBoxTemplate(this);
    m_contentTypeComboBox->setup<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>();
    QObject::connect(m_contentTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &TileTabGridRowColumnWidgets::contentTypeActivated);
    m_contentTypeComboBox->getComboBox()->setFixedWidth(m_contentTypeComboBox->getComboBox()->sizeHint().width());
    m_contentTypeComboBox->getComboBox()->setToolTip(contentToolTip);
    
    /*
     * Stretch type combo box
     */
    m_stretchTypeComboBox = new EnumComboBoxTemplate(this);
    m_stretchTypeComboBox->setup<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>();
    QObject::connect(m_stretchTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &TileTabGridRowColumnWidgets::stretchTypeActivated);
    m_stretchTypeComboBox->getComboBox()->setFixedWidth(m_stretchTypeComboBox->getComboBox()->sizeHint().width());
    m_stretchTypeComboBox->getComboBox()->setToolTip(typeToolTip);
    
    /*
     * Stretch value spin box
     */
    m_stretchValueSpinBox = new QDoubleSpinBox();
    m_stretchValueSpinBox->setKeyboardTracking(false);
    m_stretchValueSpinBox->setRange(0.0, 1000.0);
    m_stretchValueSpinBox->setDecimals(2);
    m_stretchValueSpinBox->setSingleStep(0.1);
    QObject::connect(m_stretchValueSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &TileTabGridRowColumnWidgets::stretchValueChanged);
    m_stretchValueSpinBox->setFixedWidth(m_stretchValueSpinBox->sizeHint().width());
    m_stretchValueSpinBox->setToolTip(stretchToolTip);
    
    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout);
    const int32_t rowIndex(gridLayout->rowCount());
    int32_t columnIndex(0);
    m_gridLayoutGroup->addWidget(m_indexLabel, rowIndex, columnIndex++, Qt::AlignRight);
    m_gridLayoutGroup->addWidget(m_constructionToolButton, rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_contentTypeComboBox->getWidget(), rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_stretchTypeComboBox->getWidget(), rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_stretchValueSpinBox, rowIndex, columnIndex++);
}

/**
 * Destructor.
 */
TileTabGridRowColumnWidgets::~TileTabGridRowColumnWidgets()
{
    
}

/**
 * @return The construction menu.
 *
 * @param toolButton
 *    The parent toolbutton.
 */
QMenu*
TileTabGridRowColumnWidgets::createConstructionMenu(QToolButton* toolButton)
{
    const AString deleteText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Delete this Column"
                             : "Delete this Row");
    
    const AString duplicateAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                     ? "Duplicate this Column to Right"
                                     : "Duplicate this Row Below");
    
    const AString duplicateBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                      ? "Duplicate this Column to Left"
                                      : "Duplicate this Row Above");
    
    const AString insertSpacerAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                        ? "Insert Spacer Column to Right"
                                        : "Insert Spacer Row Below");
    const AString insertSpacerBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                         ? "Insert Spacer Column to Left"
                                         : "Insert Spacer Row Above");
    const AString moveAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                ? "Move this Column to Right"
                                : "Move this Row Down");
    
    const AString moveBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                 ? "Move this Column to Left"
                                 : "Move this Row Up");
    
    m_menuDeleteAction = new QAction(deleteText);
    m_menuDeleteAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DELETE_IT));
    
    m_menuDuplicateAfterAction = new QAction(duplicateAfterText);
    m_menuDuplicateAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DUPLICATE_AFTER));
    
    m_menuDuplicateBeforeAction = new QAction(duplicateBeforeText);
    m_menuDuplicateBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DUPLICATE_BEFORE));
    
    m_insertSpacerAfterAction = new QAction(insertSpacerAfterText);
    m_insertSpacerAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER));
    
    m_insertSpacerBeforeAction = new QAction(insertSpacerBeforeText);
    m_insertSpacerBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_BEFORE));
    
    m_menuMoveAfterAction = new QAction(moveAfterText);
    m_menuMoveAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::MOVE_AFTER));
    
    m_menuMoveBeforeAction = new QAction(moveBeforeText);
    m_menuMoveBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::MOVE_BEFORE));
    
    QMenu* menu = new QMenu(toolButton);
    menu->addAction(m_menuDuplicateBeforeAction);
    menu->addAction(m_menuDuplicateAfterAction);
    menu->addSeparator();
    menu->addAction(m_insertSpacerBeforeAction);
    menu->addAction(m_insertSpacerAfterAction);
    menu->addSeparator();
    menu->addAction(m_menuMoveBeforeAction);
    menu->addAction(m_menuMoveAfterAction);
    menu->addSeparator();
    menu->addAction(m_menuDeleteAction);
    
    return menu;
}

/**
 * Update with the given row/column element.
 */
void
TileTabGridRowColumnWidgets::updateContent(TileTabsGridRowColumnElement* element)
{
    m_element = element;
    const bool showFlag(m_element != NULL);
    
    if (showFlag) {
        m_contentTypeComboBox->setSelectedItem<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>(element->getContentType());
        m_stretchTypeComboBox->setSelectedItem<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>(element->getStretchType());
        QSignalBlocker valueBlocker(m_stretchValueSpinBox);
        switch (m_element->getStretchType()) {
            case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                m_stretchValueSpinBox->setRange(0.0, 100.0);
                m_stretchValueSpinBox->setSingleStep(1.0);
                m_stretchValueSpinBox->setValue(m_element->getPercentStretch());
                m_stretchValueSpinBox->setSuffix("%");
                break;
            case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
                m_stretchValueSpinBox->setRange(0.0, 1000.0);
                m_stretchValueSpinBox->setSingleStep(0.1);
                m_stretchValueSpinBox->setValue(m_element->getWeightStretch());
                m_stretchValueSpinBox->setSuffix("");
                break;
        }
    }
    
    m_gridLayoutGroup->setVisible(showFlag);
}

/**
 * Called when an item is selected from the construction menu
 *
 * @param action
 *     Action that was selected.
 */
void
TileTabGridRowColumnWidgets::constructionMenuTriggered(QAction* action)
{
    if (action != NULL) {
        const EventTileTabsGridConfigurationModification::Operation operation
        = static_cast<EventTileTabsGridConfigurationModification::Operation>(action->data().toInt());
        
        /*
         * This switch is here so that it will cause a compilation error
         * if the operations are changed.
         */
        switch (operation) {
            case EventTileTabsGridConfigurationModification::Operation::DELETE_IT:
                break;
            case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_BEFORE:
                break;
            case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_BEFORE:
                break;
            case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::MOVE_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::MOVE_BEFORE:
                break;
        }
        
        EventTileTabsGridConfigurationModification modification(m_tileTabsConfigurationDialog->getCustomTileTabsGridConfiguration(), 
                                                                m_index,
                                                                m_rowColumnType,
                                                                operation);
        emit modificationRequested(modification);
    }
    
}

/**
 * Called when construction menu is about to show.
 */
void
TileTabGridRowColumnWidgets::constructionMenuAboutToShow()
{
    const TileTabsLayoutGridConfiguration* config = m_tileTabsConfigurationDialog->getCustomTileTabsGridConfiguration();
    if (config != NULL) {
        int32_t numItems(-1);
        switch (m_rowColumnType) {
            case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
                numItems = config->getNumberOfColumns();
                break;
            case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
                numItems = config->getNumberOfRows();
                break;
        }
        
        m_menuDeleteAction->setEnabled(numItems > 1);
        m_menuDuplicateAfterAction->setEnabled(numItems >= 1);
        m_menuDuplicateBeforeAction->setEnabled(numItems >= 1);
        m_menuMoveAfterAction->setEnabled((numItems > 1)
                                          && (m_index < (numItems - 1)));
        m_menuMoveBeforeAction->setEnabled((numItems > 1)
                                           && (m_index > 0));
    }
}


/**
 * Called when content type combo box changed.
 */
void
TileTabGridRowColumnWidgets::contentTypeActivated()
{
    if (m_element != NULL) {
        m_element->setContentType(m_contentTypeComboBox->getSelectedItem<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>());
        emit itemChanged();
    }
}

/**
 * Called when stretch type combo box changed.
 */
void
TileTabGridRowColumnWidgets::stretchTypeActivated()
{
    if (m_element != NULL) {
        m_element->setStretchType(m_stretchTypeComboBox->getSelectedItem<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>());
        emit itemChanged();
    }
}

/**
 * Called when stretch value changed.
 */
void
TileTabGridRowColumnWidgets::stretchValueChanged(double)
{
    if (m_element != NULL) {
        switch (m_element->getStretchType()) {
            case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                m_element->setPercentStretch(m_stretchValueSpinBox->value());
                break;
            case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
                m_element->setWeightStretch(m_stretchValueSpinBox->value());
                break;
        }
        emit itemChanged();
    }
}

