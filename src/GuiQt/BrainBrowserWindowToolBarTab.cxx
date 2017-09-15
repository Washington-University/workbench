
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
 * @param windowAspectRatioLockedAction
 *     Action for locking window aspect ratio.
 * @param tabAspectRatioLockedAction
 *     Action for locking tab aspect ratio.
 * @param parentToolBar
 *     Parent toolbar.
 */
BrainBrowserWindowToolBarTab::BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                                           QAction* windowAspectRatioLockedAction,
                                                           QAction* tabAspectRatioLockedAction,
                                                           BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar),
m_tabAspectRatioLockedAction(tabAspectRatioLockedAction)
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
    
    m_windowAspectRatioLockedToolButton = new QToolButton();
    m_windowAspectRatioLockedToolButton->setDefaultAction(windowAspectRatioLockedAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_windowAspectRatioLockedToolButton);
    QObject::connect(m_windowAspectRatioLockedToolButton, &QToolButton::customContextMenuRequested,
                     this, &BrainBrowserWindowToolBarTab::windowAspectCustomContextMenuRequested);
    m_windowAspectRatioLockedToolButton->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_tabAspectRatioLockedToolButton = new QToolButton();
    m_tabAspectRatioLockedToolButton->setDefaultAction(tabAspectRatioLockedAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_tabAspectRatioLockedToolButton);
    QObject::connect(m_tabAspectRatioLockedToolButton, &QToolButton::customContextMenuRequested,
                     this, &BrainBrowserWindowToolBarTab::tabAspectCustomContextMenuRequested);
    m_tabAspectRatioLockedToolButton->setContextMenuPolicy(Qt::CustomContextMenu);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addWidget(m_yokeToLabel);
    layout->addWidget(m_yokingGroupComboBox->getWidget());
    layout->addSpacing(15);
    layout->addWidget(m_windowAspectRatioLockedToolButton);
    layout->addWidget(m_tabAspectRatioLockedToolButton);
    
    addToWidgetGroup(m_yokeToLabel);
    addToWidgetGroup(m_yokingGroupComboBox->getWidget());
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarTab::~BrainBrowserWindowToolBarTab()
{
}

/**
 * Called when context menu requested for window lock aspect button.
 *
 * @param pos
 *     Postion of mouse in parent widget.
 */
void
BrainBrowserWindowToolBarTab::tabAspectCustomContextMenuRequested(const QPoint& /*pos*/)
{
    BrowserTabContent* tabContent = m_parentToolBar->getTabContentFromSelectedTab();
    if (tabContent != NULL) {
        BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
        CaretAssert(window);
        
        float aspectRatio = getAspectRatioFromDialog("Set Tab Aspect Ratio",
                                                     tabContent->getAspectRatio(),
                                                     m_tabAspectRatioLockedToolButton);
        if (aspectRatio > 0.0) {
            window->setLockTabAspectStatusAndRatio(true, aspectRatio);
        }
    }
}

/**
 * Called when context menu requested for window lock aspect button.
 *
 * @param pos
 *     Postion of mouse in parent widget.
 */
void
BrainBrowserWindowToolBarTab::windowAspectCustomContextMenuRequested(const QPoint& /*pos*/)
{
    BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    CaretAssert(window);
    
    float aspectRatio = getAspectRatioFromDialog("Set Window Aspect Ratio",
                                                 window->getAspectRatio(),
                                                 m_windowAspectRatioLockedToolButton);
    if (aspectRatio > 0.0) {
        window->setLockWindowAspectStatusAndRatio(true, aspectRatio);
    }
}

/**
 * Get the new aspect ratio using a dialog.
 *
 * @param title
 *     Title for dialog.
 * @param aspectRatio
 *     Default value for aspect ratio
 * @param parent
 *     Parent for the dialog.
 */
float
BrainBrowserWindowToolBarTab::getAspectRatioFromDialog(const QString& title,
                                                       const float aspectRatio,
                                                       QWidget* parent) const
{
    float aspectRatioOut = -1.0;
    
    WuQDataEntryDialog ded(title,
                           parent);
    QDoubleSpinBox* ratioSpinBox = ded.addDoubleSpinBox("Aspect Ratio", aspectRatio);
    ratioSpinBox->setSingleStep(0.01);
    ratioSpinBox->setRange(ratioSpinBox->singleStep(), 100.0);
    ratioSpinBox->setDecimals(3);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        aspectRatioOut = ratioSpinBox->value();
    }
    
    return aspectRatioOut;
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
    m_tabAspectRatioLockedAction->blockSignals(true);
    m_tabAspectRatioLockedAction->setChecked(browserTabContent->isAspectRatioLocked());
    m_tabAspectRatioLockedAction->blockSignals(false);
    
    blockAllSignals(false);
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
