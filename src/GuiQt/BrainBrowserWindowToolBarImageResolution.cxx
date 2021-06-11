
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_RESOLUTION_DECLARE__
#include "BrainBrowserWindowToolBarImageResolution.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_RESOLUTION_DECLARE__

#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "ModelMedia.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarImageResolution
 * \brief Image Resolution Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarImageResolution::BrainBrowserWindowToolBarImageResolution(BrainBrowserWindowToolBar* parentToolBar,
                                                                       const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QString toolTip("If a rectangular selection is made using the mouse while this is enabled, "
                    "a new high-resolution image is created containing the selected region.");
    m_highResolutionAction = new QAction();
    m_highResolutionAction->setCheckable(true);
    m_highResolutionAction->setText("High Res");
    WuQtUtilities::setWordWrappedToolTip(m_highResolutionAction, toolTip);
    QObject::connect(m_highResolutionAction, &QAction::toggled,
                     this, &BrainBrowserWindowToolBarImageResolution::highResolutionActionToggled);

    QToolButton* highResToolButton = new QToolButton();
    highResToolButton->setDefaultAction(m_highResolutionAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(highResToolButton);

    highResToolButton->setObjectName(parentObjectName
                                     + ":BrainBrowserWindowToolBarImageResolution:HighResToolButton");
    WuQMacroManager::instance()->addMacroSupportToObject(highResToolButton,
                                                         "Enable high-resolution image selection");

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(highResToolButton);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarImageResolution::~BrainBrowserWindowToolBarImageResolution()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarImageResolution::updateContent(BrowserTabContent* browserTabContent)
{
    if (browserTabContent != NULL) {
        ModelMedia* mediaModel = browserTabContent->getDisplayedMediaModel();
        if (mediaModel != NULL) {
            m_highResolutionAction->setChecked(mediaModel->isHighResolutionSelectionEnabled(browserTabContent->getTabNumber()));
        }
    }
}

/**
 * Called when high resolution action toggled
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarImageResolution::highResolutionActionToggled(bool checked)
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelMedia* mediaModel = browserTabContent->getDisplayedMediaModel();
        if (mediaModel != NULL) {
            mediaModel->setHighResolutionSelectionEnabled(browserTabContent->getTabNumber(),
                                                          checked);
        }
    }
}

