
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
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EnumComboBoxTemplate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"
#include "YokingGroupEnum.h"
#include "WuQDataEntryDialog.h"
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
 */
BrainBrowserWindowToolBarTab::BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                                           QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                                           BrainBrowserWindowToolBar* parentToolBar)
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
    
    m_yokeToLabel = new QLabel("Yoking:");
    QObject::connect(m_yokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokeToGroupComboBoxIndexChanged()));
    
    const AString lightToolTip = WuQtUtilities::createWordWrappedToolTipText("Enables shading on surfaces.  Shading affects "
                                                                             "palette colors (but not by much) and in rare circumstances it "
                                                                             "may be helpful to turn shading off.");
    m_lightingEnabledCheckBox = new QCheckBox("Shading");
    m_lightingEnabledCheckBox->setToolTip(lightToolTip);
    QObject::connect(m_lightingEnabledCheckBox, &QCheckBox::toggled,
                     this, &BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxToggled);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addWidget(m_yokeToLabel);
    layout->addWidget(m_yokingGroupComboBox->getWidget());
    layout->addWidget(m_lockWindowAndAllTabAspectButton);
    layout->addWidget(m_lightingEnabledCheckBox);
    
    addToWidgetGroup(m_yokeToLabel);
    addToWidgetGroup(m_yokingGroupComboBox->getWidget());
    addToWidgetGroup(m_lightingEnabledCheckBox);
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
    
    m_lightingEnabledCheckBox->setChecked(browserTabContent->isLightingEnabled());
    
    blockAllSignals(false);
}

/**
 * Called when lighting checkbox is toggled by user
 *
 * @param checked
 *     New status of lighting.
 */
void
BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxToggled(bool checked)
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
