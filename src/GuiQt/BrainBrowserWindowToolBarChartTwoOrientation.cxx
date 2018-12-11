
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTATION_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoOrientation.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ORIENTATION_DECLARE__

#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "SessionManager.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartTwoOrientation 
 * \brief Toolbar component for chart orientation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarChartTwoOrientation::BrainBrowserWindowToolBarChartTwoOrientation(BrainBrowserWindowToolBar* parentToolBar,
                                                                                           const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    const QString objectNamePrefix(parentObjectName
                                   + ":ChartOrientation");
    
    QToolButton* orientationResetToolButton = new QToolButton();
    m_orientationResetToolButtonAction = WuQtUtilities::createAction("Reset",
                                                                         "Reset the view to remove any panning or zooming",
                                                                         orientationResetToolButton,
                                                                         this,
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));
    m_orientationResetToolButtonAction->setObjectName(objectNamePrefix
                                                      + ":ResetButton");
    WuQMacroManager::instance()->addMacroSupportToObject(m_orientationResetToolButtonAction);

    const QString customToolTip = ("Pressing the \"Custom\" button displays a dialog for creating and editing orientations.\n"
                                   "Note that custom orientations are stored in your Workbench's preferences and thus\n"
                                   "will be availble in any concurrent or future instances of Workbench.");
    m_orientationCustomViewSelectToolButton = new QToolButton();
    m_customViewAction = WuQtUtilities::createAction("Custom",
                                                         customToolTip,
                                                         m_orientationCustomViewSelectToolButton,
                                                         this,
                                                         SLOT(customViewActionTriggered()));
    m_customViewAction->setObjectName(objectNamePrefix
                                      + ":ShowCustomViewDialog");
    WuQMacroManager::instance()->addMacroSupportToObject(m_customViewAction);

    
    orientationResetToolButton->setDefaultAction(m_orientationResetToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(orientationResetToolButton);
    
    m_orientationCustomViewSelectToolButton->setDefaultAction(m_customViewAction);
    m_orientationCustomViewSelectToolButton->setSizePolicy(QSizePolicy::Minimum,
                                                               QSizePolicy::Fixed);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_orientationCustomViewSelectToolButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(orientationResetToolButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_orientationCustomViewSelectToolButton, 0, Qt::AlignHCenter);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoOrientation::~BrainBrowserWindowToolBarChartTwoOrientation()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartTwoOrientation::updateContent(BrowserTabContent* /*browserTabContent*/)
{
}

/**
 * Called when orientation reset button is pressed.
 */
void
BrainBrowserWindowToolBarChartTwoOrientation::orientationResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->resetView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when custom view is triggered and displays Custom View Menu.
 */
void
BrainBrowserWindowToolBarChartTwoOrientation::customViewActionTriggered()
{
    m_parentToolBar->customViewActionTriggered();
}

