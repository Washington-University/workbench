
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
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"
#include "YokingGroupEnum.h"

using namespace caret;
    
/**
 * \class caret::BrainBrowserWindowToolBarTab 
 * \brief Tab section of Browser Window Toolbar.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarTab::BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                                           BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar)
{
    m_yokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_yokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    m_yokingGroupComboBox->getWidget()->setStatusTip("Select a yoking group (linked views)");
    m_yokingGroupComboBox->getWidget()->setToolTip(("Select a yoking group (linked views).\n"
                                                    "Models yoked to a group are displayed in the same view.\n"
                                                    "Surface Yoking is applied to Surface, Surface Montage\n"
                                                    "and Whole Brain.  Volume Yoking is applied to Volumes."));
    
    QLabel* yokeToLabel = new QLabel("Yoking:");
    QObject::connect(m_yokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokeToGroupComboBoxIndexChanged()));
    
    
    m_idSlicesAction = WuQtUtilities::createAction("Volume ID",
                                                   "If this item is selected AND a node or voxel\n"
                                                   "identification takes place in any tab/window, the\n"
                                                   "selected volume slices in this tab will move to\n"
                                                   "the location of the identified node or voxel.",
                                                   this,
                                                   this,
                                                   SLOT(volumeIdentificationToggled(bool)));
    m_idSlicesAction->setCheckable(true);
    QToolButton* idToolButton = new QToolButton();
    idToolButton->setDefaultAction(m_idSlicesAction);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addWidget(yokeToLabel);
    layout->addWidget(m_yokingGroupComboBox->getWidget());
    layout->addWidget(idToolButton, 0, Qt::AlignHCenter);
    
    addToWidgetGroup(yokeToLabel);
    addToWidgetGroup(m_yokingGroupComboBox->getWidget());
    addToWidgetGroup(idToolButton);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarTab::~BrainBrowserWindowToolBarTab()
{
}

/**
 * Called when volume identification action toggled.
 *
 * @param value
 *     New value.
 */
void
BrainBrowserWindowToolBarTab::volumeIdentificationToggled(bool value)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    browserTabContent->setIdentificationUpdatesVolumeSlices(value);
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
    
    m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getYokingGroup());
    m_idSlicesAction->setChecked(browserTabContent->isIdentificationUpdatesVolumeSlices());
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
    
    YokingGroupEnum::Enum yokingGroup = m_yokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>();
    browserTabContent->setYokingGroup(yokingGroup);
    
    m_parentToolBar->updateToolBarComponents(browserTabContent);

    this->updateGraphicsWindow();
}


