
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_DECLARE__
#include "BrainBrowserWindowToolBarChartOrientation.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ORIENTATION_DECLARE__

#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "ModelChart.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartOrientation 
 * \brief Toolbar component for chart orientation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarChartOrientation::BrainBrowserWindowToolBarChartOrientation(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_orientationResetToolButtonAction = WuQtUtilities::createAction("Reset",
                                                                         "Reset the view to remove any panning or zooming",
                                                                         this,
                                                                         this,
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));

    const QString customToolTip = ("Pressing the \"Custom\" button displays a dialog for creating and editing orientations.\n"
                                   "Note that custom orientations are stored in your Workbench's preferences and thus\n"
                                   "will be availble in any concurrent or future instances of Workbench.");
    m_customViewAction = WuQtUtilities::createAction("Custom",
                                                         customToolTip,
                                                         this,
                                                         this,
                                                         SLOT(customViewActionTriggered()));

    
    QToolButton* orientationResetToolButton = new QToolButton();
    orientationResetToolButton->setDefaultAction(m_orientationResetToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(orientationResetToolButton);
    
    m_orientationCustomViewSelectToolButton = new QToolButton();
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
BrainBrowserWindowToolBarChartOrientation::~BrainBrowserWindowToolBarChartOrientation()
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
BrainBrowserWindowToolBarChartOrientation::updateContent(BrowserTabContent* /*browserTabContent*/)
{
}

/**
 * Called when orientation reset button is pressed.
 */
void
BrainBrowserWindowToolBarChartOrientation::orientationResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->resetView();
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when custom view is triggered and displays Custom View Menu.
 */
void
BrainBrowserWindowToolBarChartOrientation::customViewActionTriggered()
{
    m_parentToolBar->customViewActionTriggered();
//    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
//    prefs->readCustomViews();
//    const std::vector<std::pair<AString, AString> > customViewNameAndComments = prefs->getCustomViewNamesAndComments();
//    
//    QMenu menu;
//    
//    QAction* editAction = menu.addAction("Create and Edit...");
//    editAction->setToolTip("Add and delete Custom Views.\n"
//                           "Edit model transformations.");
//    
//    const int32_t numViews = static_cast<int32_t>(customViewNameAndComments.size());
//    if (numViews > 0) {
//        menu.addSeparator();
//    }
//    for (int32_t i = 0; i < numViews; i++) {
//        QAction* action = menu.addAction(customViewNameAndComments[i].first);
//        action->setToolTip(WuQtUtilities::createWordWrappedToolTipText(customViewNameAndComments[i].second));
//    }
//    
//    QAction* selectedAction = menu.exec(QCursor::pos());
//    if (selectedAction != NULL) {
//        if (selectedAction == editAction) {
//            BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
//            GuiManager::get()->processShowCustomViewDialog(bbw);
//        }
//        else {
//            const AString customViewName = selectedAction->text();
//            
//            ModelTransform modelTransform;
//            if (prefs->getCustomView(customViewName, modelTransform)) {
//                BrowserTabContent* btc = this->getTabContentFromSelectedTab();
//                btc->setTransformationsFromModelTransform(modelTransform);
//                this->updateGraphicsWindow();
//                this->updateOtherYokedWindows();
//            }
//        }
//    }
}

