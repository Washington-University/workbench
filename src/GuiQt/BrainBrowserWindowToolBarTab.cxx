
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__
#include "BrainBrowserWindowToolBarTab.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ClippingPlanesWidget.h"
#include "EnumComboBoxTemplate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"
#include "YokingGroupEnum.h"
#include "WuQDataEntryDialog.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::BrainBrowserWindowToolBarTab 
 * \brief Tab section of Browser Window Toolbar.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of browser window.
 * @param toolBarLockWindowAndAllTabAspectRatioButton
 *     Button for locking window and all tab aspect ratio.
 * @param parentToolBar
 *     Parent toolbar.
 * @param objectNamePrefix
       Prefix for naming objects
 */
BrainBrowserWindowToolBarTab::BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                                           QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                                           BrainBrowserWindowToolBar* parentToolBar,
                                                           const QString& objectNamePrefix)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar),
m_lockWindowAndAllTabAspectButton(toolBarLockWindowAndAllTabAspectRatioButton)
{
    m_yokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_yokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    m_yokingGroupComboBox->getWidget()->setStatusTip("Select a yoking group (linked views)");
    m_yokingGroupComboBox->getWidget()->setToolTip(("Select a yoking group (linked views).\n"
                                                    "Models yoked to a group are displayed in the same view.\n"
                                                    "Surface Yoking is applied to Surface, Surface Montage\n"
                                                    "and Whole Brain.  Volume Yoking is applied to Volumes."));
    QComboBox* encapComboBox = m_yokingGroupComboBox->getComboBox();
    encapComboBox->setObjectName(objectNamePrefix
                                 + ":Tab:YokingGroup");
    WuQMacroManager::instance()->addMacroSupportToObject(encapComboBox,
                                                         "Select yoking group");
    
    m_yokeToLabel = new QLabel("Yoking:");
    QObject::connect(m_yokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokeToGroupComboBoxIndexChanged()));
    
    QIcon shadingIcon;
    const bool shadingIconValid = WuQtUtilities::loadIcon(":/ToolBar/lighting.png",
                                                          shadingIcon);
    const AString lightToolTip = WuQtUtilities::createWordWrappedToolTipText("Enables shading on surfaces.  Shading affects "
                                                                             "palette colors (but not by much) and in rare circumstances it "
                                                                             "may be helpful to turn shading off.");

    m_lightingAction = new QAction(this);
    m_lightingAction->setCheckable(true);
    if (shadingIconValid) {
        m_lightingAction->setIcon(shadingIcon);
    }
    else {
        m_lightingAction->setText("L");
    }
    m_lightingAction->setToolTip(lightToolTip);
    m_lightingAction->setObjectName(objectNamePrefix
                                    + ":Tab:EnableShading");
    QObject::connect(m_lightingAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxChecked);
    WuQMacroManager::instance()->addMacroSupportToObject(m_lightingAction,
                                                         "Enable shading");

    QToolButton* lightingToolButton = new QToolButton();
    lightingToolButton->setDefaultAction(m_lightingAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(lightingToolButton);

    QIcon clippingIcon;
    const bool clippingIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/clipping.png",
                            clippingIcon);
    
    m_clippingPlanesAction = new QAction(this);
    m_clippingPlanesAction->setCheckable(true);
    if (clippingIconValid) {
        m_clippingPlanesAction->setIcon(clippingIcon);
    }
    else {
        m_clippingPlanesAction->setText("C");
    }
    m_clippingPlanesAction->setToolTip("Enable clipping planes, click arrow to edit");
    m_clippingPlanesAction->setMenu(createClippingPlanesMenu());
    m_clippingPlanesAction->setObjectName(objectNamePrefix
                                          + ":Tab:ClippingPlanes");
    WuQMacroManager::instance()->addMacroSupportToObject(m_clippingPlanesAction,
                                                         "Enable clipping planes");
    QObject::connect(m_clippingPlanesAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarTab::clippingPlanesActionToggled);
    
    QToolButton* clippingPlanesToolButton = new QToolButton();
    clippingPlanesToolButton->setDefaultAction(m_clippingPlanesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(clippingPlanesToolButton);

    m_macroRecordingLabel = new QLabel("");
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    int32_t row(0);
    layout->addWidget(m_yokeToLabel,
                      row, 0, 1, 3, Qt::AlignLeft);
    row++;
    layout->addWidget(m_yokingGroupComboBox->getWidget(),
                      row, 0, 1, 3, Qt::AlignLeft);
    row++;
    layout->addWidget(m_lockWindowAndAllTabAspectButton,
                      row, 0, 1, 3, Qt::AlignLeft);
    row++;
    layout->addWidget(lightingToolButton,
                      row, 0);
    layout->addWidget(clippingPlanesToolButton,
                      row, 1);
    row++;
    layout->addWidget(m_macroRecordingLabel,
                      row, 0, 1, 3, Qt::AlignLeft);
    
    addToWidgetGroup(m_yokeToLabel);
    addToWidgetGroup(m_yokingGroupComboBox->getWidget());
    addToWidgetGroup(lightingToolButton);
    addToWidgetGroup(m_macroRecordingLabel);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarTab::~BrainBrowserWindowToolBarTab()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarTab::updateContent(BrowserTabContent* browserTabContent)
{
    if (browserTabContent == NULL) {
        return;
    }
    
    blockAllSignals(true);
    
    bool chartFlag = false;
    switch (browserTabContent->getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
    
    if (chartFlag) {
        m_yokeToLabel->setText("Chart Yoking:");
        m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getChartModelYokingGroup());
    }
    else {
        m_yokeToLabel->setText("Yoking:");
        m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getBrainModelYokingGroup());
    }
    
    m_lightingAction->setChecked(browserTabContent->isLightingEnabled());
    
    m_macroRecordingLabel->setText("");
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            m_macroRecordingLabel->setText("<html><font color=red>Macro</font></html>");
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    m_clippingPlanesAction->setChecked(browserTabContent->isClippingPlanesEnabled());
    
    blockAllSignals(false);
}

/**
 * Called when lighting checkbox is checked by user
 *
 * @param checked
 *     New status of lighting.
 */
void
BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxChecked(bool checked)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    browserTabContent->setLightingEnabled(checked);
    this->updateGraphicsWindow();
}

/**
 * Called when window yoke to tab combo box is selected.
 */
void
BrainBrowserWindowToolBarTab::yokeToGroupComboBoxIndexChanged()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    bool chartFlag = false;
    switch (browserTabContent->getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
    
    YokingGroupEnum::Enum yokingGroup = m_yokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>();
    if (chartFlag) {
        browserTabContent->setChartModelYokingGroup(yokingGroup);
    }
    else {
        browserTabContent->setBrainModelYokingGroup(yokingGroup);
    }
    
    m_parentToolBar->updateToolBarComponents(browserTabContent);

    this->updateGraphicsWindow();
}

/**
 * @return Instance of the clipping menu
 */
QMenu*
BrainBrowserWindowToolBarTab::createClippingPlanesMenu()
{
    m_clippingPlanesWidget = new ClippingPlanesWidget();
    QWidgetAction* clippingPlanesAction = new QWidgetAction(this);
    clippingPlanesAction->setDefaultWidget(m_clippingPlanesWidget);
    
    QMenu* menu = new QMenu(this);
    menu->addAction(clippingPlanesAction);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarTab::clippingPlanesMenuAboutToShow);
    return menu;
}

/**
 * Called when clipping planes action is toggled
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarTab::clippingPlanesActionToggled(bool checked)
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        browserTabContent->setClippingPlanesEnabled(checked);
        updateContent(browserTabContent);
        updateGraphicsWindow();
    }
}

/**
 * Called when clipping menu is about to show
 */
void
BrainBrowserWindowToolBarTab::clippingPlanesMenuAboutToShow()
{
    int32_t tabIndex = -1;
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        tabIndex = browserTabContent->getTabNumber();
    }

    m_clippingPlanesWidget->updateContent(tabIndex);
}

