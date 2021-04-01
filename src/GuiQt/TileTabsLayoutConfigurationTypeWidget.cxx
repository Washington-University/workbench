
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_DECLARE__
#include "TileTabsLayoutConfigurationTypeWidget.h"
#undef __TILE_TABS_LAYOUT_CONFIGURATION_TYPE_WIDGET_DECLARE__

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowContent.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "TileTabsConfigurationDialog.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "WuQDataEntryDialog.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::TileTabsLayoutConfigurationTypeWidget 
 * \brief Widget for selection of the tile tabs configuration type
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parentType
 *    Type of parent for this widget
 * @param parent
 *    Optional parent widget
 */
TileTabsLayoutConfigurationTypeWidget::TileTabsLayoutConfigurationTypeWidget(const ParentType parentType,
                                                                             QWidget* parent)
: QWidget(parent),
m_parentType(parentType),
m_windowIndex(-1)
{
    /*
     * Button group keeps radio buttons mutually exclusive
     */
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);
    
    QGridLayout* layout = new QGridLayout(this);
    int32_t layoutRow(0);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 6, 0);
    
    std::vector<TileTabsLayoutConfigurationTypeEnum::Enum> layoutTypeEnums;
    TileTabsLayoutConfigurationTypeEnum::getAllEnums(layoutTypeEnums);
    
    for (const auto layoutType : layoutTypeEnums) {
        QRadioButton* rb = new QRadioButton(TileTabsLayoutConfigurationTypeEnum::toGuiName(layoutType));
        rb->setToolTip(TileTabsLayoutConfigurationTypeEnum::toGuiToolTip(layoutType));
        QObject::connect(rb, &QRadioButton::clicked,
                         [=]() { layoutTypeRadioButtonClicked(layoutType); });
        m_buttonGroup->addButton(rb,
                                 TileTabsLayoutConfigurationTypeEnum::toIntegerCode(layoutType));
        
        switch (layoutType) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                layout->addWidget(rb,
                                  layoutRow, 0, 1, 5, Qt::AlignLeft);
                layoutRow++;
                m_automaticGridRowsColumnsLabel = new QLabel("(11 Rows, 22 Cols)");
                layout->addWidget(m_automaticGridRowsColumnsLabel, layoutRow, 1, 1, 4);
                layoutRow++;
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            {
                layout->addWidget(rb,
                                  layoutRow, 0, 1, 5, Qt::AlignLeft);
                layoutRow++;
                m_customGridColumnsSpinBox = new QSpinBox();
                m_customGridColumnsSpinBox->setRange(1, 99);
                m_customGridColumnsSpinBox->setToolTip("Number of columns in custom grid");
                QObject::connect(m_customGridColumnsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                                 this, &TileTabsLayoutConfigurationTypeWidget::customGridColumnSpinBoxValueChanged);
                
                m_customGridRowsSpinBox = new QSpinBox();
                m_customGridRowsSpinBox->setToolTip("Number of rows in custom grid");
                m_customGridRowsSpinBox->setRange(1, 99);
                QObject::connect(m_customGridRowsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                                 this, &TileTabsLayoutConfigurationTypeWidget::customGridRowSpinBoxValueChanged);
                
                layout->addWidget(m_customGridRowsSpinBox, layoutRow, 1, 1, 2);
                layout->addWidget(m_customGridColumnsSpinBox, layoutRow, 3, 1, 2);
                layoutRow++;
            }
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            {
                layout->addWidget(rb,
                                  layoutRow, 0, 1, 3, Qt::AlignLeft);
                m_manualConfigurationSetButton = createManualConfigurationSetToolButton();
                layout->addWidget(m_manualConfigurationSetButton,
                                  layoutRow, 3, 1, 2, Qt::AlignLeft);
                layoutRow++;
            }
                break;
        }
    }
    
    CaretAssert(m_customGridColumnsSpinBox);
    CaretAssert(m_customGridRowsSpinBox);
}

/**
 * Destructor.
 */
TileTabsLayoutConfigurationTypeWidget::~TileTabsLayoutConfigurationTypeWidget()
{
}

/**
 * Called when a layout type is selected
 * @param layoutType
 *    The layout type
 */
void
TileTabsLayoutConfigurationTypeWidget::layoutTypeRadioButtonClicked(const TileTabsLayoutConfigurationTypeEnum::Enum layoutType)
{
    BrowserWindowContent* bwc(getBrowserWindowContent());

    switch (layoutType) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            const TileTabsLayoutGridConfiguration* gridConfig = bwc->getCustomGridTileTabsConfiguration();
            if (gridConfig != NULL) {
                if ( ! TileTabsConfigurationDialog::warnIfGridConfigurationTooSmallDialog(m_windowIndex,
                                                                                          gridConfig,
                                                                                          this)) {
                    return;
                }
            }
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            break;
    }
    
    bwc->setTileTabsConfigurationMode(layoutType);
    
    updateGraphicsAndUserInterface();
}

/**
 * Called when custom grid columns is changed
 * @param columns
 *    New number of colums
 */
void
TileTabsLayoutConfigurationTypeWidget::customGridColumnSpinBoxValueChanged(const int columns)
{
    customGridRowsColumnsChanged(m_customGridRowsSpinBox->value(), columns);
}

/**
 * Called when custom grid rows is changed
 * @param rows
 *    New number of rows
 */
void
TileTabsLayoutConfigurationTypeWidget::customGridRowSpinBoxValueChanged(const int rows)
{
    customGridRowsColumnsChanged(rows, m_customGridColumnsSpinBox->value());
}

/**
 * Called to update rows/columns in the custom grid configuration
 * @param rows
 *    Number of rows
 * @param columns
 *    Number of columns
 */
void
TileTabsLayoutConfigurationTypeWidget::customGridRowsColumnsChanged(const int32_t rows,
                                                                    const int32_t columns)
{
    BrowserWindowContent* bwc(getBrowserWindowContent());
    TileTabsLayoutGridConfiguration* customGridConfiguration = bwc->getCustomGridTileTabsConfiguration();
    CaretAssert(customGridConfiguration);
    if (customGridConfiguration != NULL) {
        customGridConfiguration->setNumberOfRows(rows);
        customGridConfiguration->setNumberOfColumns(columns);
        
        updateGraphicsAndUserInterface();
    }
}

/**
 * Called to update the content in this widget
 * @param windowIndex
 *    Index of the window
 */
void
TileTabsLayoutConfigurationTypeWidget::updateContent(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
    CaretAssert(m_windowIndex >= 0);
    
    BrainBrowserWindow* browserWindow(GuiManager::get()->getBrowserWindowByWindowIndex(m_windowIndex));
    if (browserWindow == NULL) {
        /*
         * Browser window may be under construction
         */
        return;
    }
    BrowserWindowContent* bwc(browserWindow->getBrowerWindowContent());
    const TileTabsLayoutConfigurationTypeEnum::Enum layoutType(bwc->getTileTabsConfigurationMode());
    const int32_t integerCode(TileTabsLayoutConfigurationTypeEnum::toIntegerCode(layoutType));
    
    QAbstractButton* button = m_buttonGroup->button(integerCode);
    CaretAssert(button);
    QRadioButton* radioButton(qobject_cast<QRadioButton*>(button));
    CaretAssert(radioButton);
    
    QSignalBlocker blocker(m_buttonGroup);
    radioButton->setChecked(true);
    
    std::vector<BrowserTabContent*> windowTabs;
    browserWindow->getAllTabContent(windowTabs);
    int32_t autoColumnCount(0), autoRowCount(0);
    const int32_t windowTabCount(windowTabs.size());
    TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(windowTabCount,
                                                                      autoRowCount,
                                                                      autoColumnCount);
    const AString autoLabelText(AString::number(autoRowCount)
                                + " Rows, "
                                + AString::number(autoColumnCount)
                                + " Cols");
    m_automaticGridRowsColumnsLabel->setText(autoLabelText);
    
    TileTabsLayoutGridConfiguration* customGridConfiguration = bwc->getCustomGridTileTabsConfiguration();
    CaretAssert(customGridConfiguration);
    if (customGridConfiguration != NULL) {
        QSignalBlocker rowsBlocker(m_customGridRowsSpinBox);
        QSignalBlocker columnsBlocker(m_customGridColumnsSpinBox);
        int32_t configRowCount(0), configColCount(0);
        if (customGridConfiguration->isCustomDefaultFlag()) {
            TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(windowTabCount,
                                                                              configRowCount,
                                                                              configColCount);
        }
        else {
            configColCount = customGridConfiguration->getNumberOfColumns();
            configRowCount = customGridConfiguration->getNumberOfRows();
        }
        m_customGridColumnsSpinBox->setValue(configColCount);
        m_customGridRowsSpinBox->setValue(configRowCount);
    }
}

/**
 * @return A pointer to the custom tile tabs configuration.
 */
TileTabsLayoutGridConfiguration*
TileTabsLayoutConfigurationTypeWidget::getCustomTileTabsGridConfiguration()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    TileTabsLayoutGridConfiguration* configuration = browserWindowContent->getCustomGridTileTabsConfiguration();
    CaretAssert(configuration);
    return configuration;
}

/**
 * @return Browser window (could be NULL when GUI is being created)
 */
BrainBrowserWindow*
TileTabsLayoutConfigurationTypeWidget::getBrowserWindow()
{
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_windowIndex));
    return bbw;
}


/**
 * @return Point to browser window containing this widget
 */
BrowserWindowContent*
TileTabsLayoutConfigurationTypeWidget::getBrowserWindowContent()
{
    std::unique_ptr<EventBrowserWindowContent> eventWindowContent(EventBrowserWindowContent::getWindowContent(m_windowIndex));
    EventManager::get()->sendEvent(eventWindowContent->getPointer());
    BrowserWindowContent* bwc(eventWindowContent->getBrowserWindowContent());
    CaretAssert(bwc);
    return bwc;
}

/**
 * Update the graphics and the user interface
 */
void
TileTabsLayoutConfigurationTypeWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_windowIndex).getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

QToolButton*
TileTabsLayoutConfigurationTypeWidget::createManualConfigurationSetToolButton()
{
    m_setManualToAutomaticGridActionText = "Set Bounds of Tabs from Automatic Grid";
    m_setManualToCustomGridActionText    = "Set Bounds of Tabs from Custom Grid";
    m_setManualToGridColumnsActionText   = "Set Bounds of Tabs from Grid...";
    
    const QString toolTipText("<html>"
                              "Set (replace) the bounds of all tabs using the bounds created by a grid configuration.  After "
                              "choosing one of the selections, the user may edit individual tab bounds as desired."
                              "<ul>"
                              "<li><b>" + m_setManualToAutomaticGridActionText + "</b> - Tab bounds will be identical to the Automatic Grid"
                              "<li><b>" + m_setManualToCustomGridActionText + "</b> - Tab bounds will be identical to the Custom Grid (note that a "
                              "custom grid may not display all tabs but all tabs will be in the manual configuration)"
                              "<li><b>" + m_setManualToGridColumnsActionText + "</b> - Using a dialog, the user is prompted to enter the number of columns in the grid and wb_view will "
                              "set the number of rows so that the grid contains all tabs.  Tabs bounds are then set using the grid"
                              "</ul>"
                              "</html>");
    
    QToolButton* toolButton = new QToolButton();
    toolButton->setText("Set...");
    toolButton->setToolTip(toolTipText);
    QObject::connect(toolButton, &QToolButton::clicked,
                     this, &TileTabsLayoutConfigurationTypeWidget::manualConfigurationSetToolButtonClicked);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    return toolButton;
}

/**
 * Called when manual configuration set tool button is clicked
 */
void
TileTabsLayoutConfigurationTypeWidget::manualConfigurationSetToolButtonClicked()
{
    QMenu menu(this);
    QAction* setAutomaticGridAction = menu.addAction(m_setManualToAutomaticGridActionText);
    QAction* setCustomGridAction    = menu.addAction(m_setManualToCustomGridActionText);
    QAction* setColumnsAction       = menu.addAction(m_setManualToGridColumnsActionText);
    
    QAction* selectedAction = menu.exec(m_manualConfigurationSetButton->mapToGlobal(QPoint(0,0)));
    if (selectedAction == setColumnsAction) {
        manualConfigurationSetMenuColumnsItemTriggered();
    }
    else if (selectedAction == setAutomaticGridAction) {
        manualConfigurationSetMenuFromAutomaticItemTriggered();
    }
    else if (selectedAction == setCustomGridAction) {
        manualConfigurationSetMenuFromCustomItemTriggered();
    }
    else if (selectedAction != NULL) {
        CaretAssertMessage(0, "Has a new action been added but not processed?");
    }
    
//    CaretAssertToDoWarning();

//    updateContent();
//    signalConfigurationChanged();
//    //    updateDialog();
}

/**
 * Called when manual configuration set columns menu item triggered
 */
void
TileTabsLayoutConfigurationTypeWidget::manualConfigurationSetMenuColumnsItemTriggered()
{
    const BrainBrowserWindow* window = getBrowserWindow();
    CaretAssert(window);
    
    std::vector<BrowserTabContent*> allTabContent;
    window->getAllTabContent(allTabContent);
    const int32_t numberOfTabs = static_cast<int32_t>(allTabContent.size());
    
    int32_t defaultNumberOfRows(1);
    int32_t defaultNumberOfColumns(1);
    TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(numberOfTabs,
                                                                      defaultNumberOfRows,
                                                                      defaultNumberOfColumns);
    
    WuQDataEntryDialog ded("Create Manual Layout from Grid",
                           m_manualConfigurationSetButton);
    QSpinBox* rowsSpinBox    = ded.addSpinBox("Rows",
                                              defaultNumberOfRows);
    rowsSpinBox->setMinimum(1);
    rowsSpinBox->setMaximum(100);
    rowsSpinBox->setSingleStep(1);
    
    QSpinBox* columnsSpinBox = ded.addSpinBox("Columns",
                                              defaultNumberOfColumns);
    columnsSpinBox->setMinimum(1);
    columnsSpinBox->setMaximum(100);
    columnsSpinBox->setSingleStep(1);
    
    const bool wrapTextFlag(false);
    ded.setTextAtTop("Set rows and columns of Grid:",
                     wrapTextFlag);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const int32_t numberOfRows    = rowsSpinBox->value();
        CaretAssert(numberOfRows >= 1);
        const int32_t numberOfColumns = columnsSpinBox->value();
        CaretAssert(numberOfColumns >= 1);
        
        std::unique_ptr<TileTabsLayoutGridConfiguration> gridConfig(TileTabsLayoutGridConfiguration::newInstanceCustomGrid());
        gridConfig->setNumberOfRows(numberOfRows);
        gridConfig->setNumberOfColumns(numberOfColumns);
        
        TileTabsConfigurationDialog::loadIntoManualConfiguration(gridConfig.get(),
                                                                 m_windowIndex,
                                                                 m_manualConfigurationSetButton);
        updateContent(m_windowIndex);
    }
}

/**
 * Called when manual configuration set from automatic grid menu item is triggered
 */
void
TileTabsLayoutConfigurationTypeWidget::manualConfigurationSetMenuFromAutomaticItemTriggered()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    if (browserWindowContent != NULL) {
        TileTabsLayoutGridConfiguration* gridConfiguration = browserWindowContent->getAutomaticGridTileTabsConfiguration();
        CaretAssert(gridConfiguration);
        
        TileTabsConfigurationDialog::loadIntoManualConfiguration(gridConfiguration,
                                                                 m_windowIndex,
                                                                 m_manualConfigurationSetButton);
        updateContent(m_windowIndex);
    }
}

/**
 * Called when manual configuration set from custom grid menu item is triggered
 */
void
TileTabsLayoutConfigurationTypeWidget::manualConfigurationSetMenuFromCustomItemTriggered()
{
    TileTabsLayoutGridConfiguration* gridConfiguration = getCustomTileTabsGridConfiguration();
    CaretAssert(gridConfiguration);
    if (gridConfiguration->isCustomDefaultFlag()) {
        /*
         * If we are here the user has not yet selected the custom grid
         * and it defaults to the automatic configuration when selected.
         * So, in this case, use the automatic grid.
         */
        manualConfigurationSetMenuFromAutomaticItemTriggered();
    }
    else {
        TileTabsConfigurationDialog::loadIntoManualConfiguration(gridConfiguration,
                                                                 m_windowIndex,
                                                                 m_manualConfigurationSetButton);
        updateContent(m_windowIndex);
    }
}

