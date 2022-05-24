
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MPR_DECLARE__
#include "BrainBrowserWindowToolBarVolumeMPR.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MPR_DECLARE__

#include <QGridLayout>
#include <QLabel>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarVolumeMPR
 * \brief Image Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarVolumeMPR::BrainBrowserWindowToolBarVolumeMPR(BrainBrowserWindowToolBar* parentToolBar,
                                                                                   const QString& /*parentObjectName*/)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarVolumeMPR::~BrainBrowserWindowToolBarVolumeMPR()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarVolumeMPR::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    bool enabledFlag(false);
    if (browserTabContent != NULL) {
    }
    
    setEnabled(enabledFlag);
}

