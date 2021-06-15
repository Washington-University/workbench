
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
#include "CaretUndoStack.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelMedia.h"
#include "ViewingTransformationsMedia.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
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

    m_redoAction = WuQtUtilities::createAction("Redo",
                                               "Redo ToolTip",
                                               this,
                                               this,
                                               SLOT(redoActionTriggered()));
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(m_redoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(redoToolButton);
    
    m_undoAction = WuQtUtilities::createAction("Undo",
                                               "Undo ToolTip",
                                               this,
                                               this,
                                               SLOT(undoActionTriggered()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(m_undoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(highResToolButton);
    layout->addSpacing(10);
    layout->addWidget(redoToolButton);
    layout->addWidget(undoToolButton);
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
    m_browserTabContent = browserTabContent;

    m_redoAction->setEnabled(false);
    m_undoAction->setEnabled(false);
    
    if (browserTabContent != NULL) {
        ModelMedia* mediaModel = browserTabContent->getDisplayedMediaModel();
        if (mediaModel != NULL) {
            m_highResolutionAction->setChecked(mediaModel->isHighResolutionSelectionEnabled(browserTabContent->getTabNumber()));
        }
        
        CaretUndoStack* undoStack = getUndoStack();
        if (undoStack != NULL) {
            m_redoAction->setEnabled(undoStack->canRedo());
            m_redoAction->setToolTip(undoStack->redoText());
            
            m_undoAction->setEnabled(undoStack->canUndo());
            m_undoAction->setToolTip(undoStack->undoText());
        }
    }
}

/**
 * @return Undo stack for this tab or NULL if not valid
 */
CaretUndoStack*
BrainBrowserWindowToolBarImageResolution::getUndoStack()
{
    CaretUndoStack* undoStack(NULL);
    if (m_browserTabContent != NULL) {
        if (m_browserTabContent->isMediaDisplayed()) {
            ViewingTransformationsMedia* mediaTransform = dynamic_cast<ViewingTransformationsMedia*>(m_browserTabContent->getViewingTransformation());
            if (mediaTransform != NULL) {
                undoStack = mediaTransform->getRedoUndoStack();
            }
        }
    }
    return undoStack;
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

/**
 * Gets called when the redo action is triggered
 */
void
BrainBrowserWindowToolBarImageResolution::redoActionTriggered()
{
    CaretUndoStack* undoStack = getUndoStack();
    AString errorMessage;
    if ( ! undoStack->redo(errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    updateContent(m_browserTabContent);
}

/**
 * Gets called when the undo action is triggered
 */
void
BrainBrowserWindowToolBarImageResolution::undoActionTriggered()
{
    CaretUndoStack* undoStack = getUndoStack();
    AString errorMessage;
    if ( ! undoStack->undo(errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    updateContent(m_browserTabContent);
}
