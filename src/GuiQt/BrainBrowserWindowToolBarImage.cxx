
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_DECLARE__
#include "BrainBrowserWindowToolBarImage.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_DECLARE__

#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelMedia.h"
#include "ViewingTransformationsMedia.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarImage
 * \brief Image Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarImage::BrainBrowserWindowToolBarImage(BrainBrowserWindowToolBar* parentToolBar,
                                                                                   const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    m_redoAction = WuQtUtilities::createAction("Redo",
                                               "Redo ToolTip",
                                               this,
                                               this,
                                               SLOT(redoActionTriggered()));
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(m_redoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(redoToolButton);
    m_redoAction->setObjectName(parentObjectName + ":Redo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_redoAction, "Redo Image View");
    
    m_undoAction = WuQtUtilities::createAction("Undo",
                                               "Undo ToolTip",
                                               this,
                                               this,
                                               SLOT(undoActionTriggered()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(m_undoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    m_undoAction->setObjectName(parentObjectName + ":Undo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_undoAction, "Undo Image View");

    QLabel* modeLabel(new QLabel("Coord"));
    m_mediaDisplayCoordinateModeEnumComboBox = new EnumComboBoxTemplate(this);
    std::vector<MediaDisplayCoordinateModeEnum::Enum> supportedModes;
    supportedModes.push_back(MediaDisplayCoordinateModeEnum::PIXEL);
    supportedModes.push_back(MediaDisplayCoordinateModeEnum::PLANE);
    m_mediaDisplayCoordinateModeEnumComboBox->setupWithItems<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>(supportedModes);
    QObject::connect(m_mediaDisplayCoordinateModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarImage::mediaDisplayCoordinateModeEnumComboBoxItemActivated);
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setObjectName(parentObjectName
                                                                         + ":mediaDisplayModeComboBox");
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setToolTip("Coordinate Display Mode");
    WuQMacroManager::instance()->addMacroSupportToObject(m_mediaDisplayCoordinateModeEnumComboBox->getWidget(), "Set media coordinate mode for display");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(redoToolButton, 0, Qt::AlignHCenter);
    layout->addWidget(undoToolButton, 0, Qt::AlignHCenter);
    layout->addSpacing(5);
    layout->addWidget(modeLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_mediaDisplayCoordinateModeEnumComboBox->getWidget(), 0, Qt::AlignHCenter);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarImage::~BrainBrowserWindowToolBarImage()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarImage::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    m_redoAction->setEnabled(false);
    m_undoAction->setEnabled(false);
    
    if (browserTabContent != NULL) {
        CaretUndoStack* undoStack = getUndoStack();
        if (undoStack != NULL) {
            m_redoAction->setEnabled(undoStack->canRedo());
            m_redoAction->setToolTip(undoStack->redoText());
            
            m_undoAction->setEnabled(undoStack->canUndo());
            m_undoAction->setToolTip(undoStack->undoText());
        }
        
        const MediaDisplayCoordinateModeEnum::Enum mediaDisplayMode(browserTabContent->getMediaDisplayCoordinateMode());
        m_mediaDisplayCoordinateModeEnumComboBox->setSelectedItem<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>(mediaDisplayMode);
    }
    
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setEnabled(browserTabContent != NULL);
}

/**
 * @return Undo stack for this tab or NULL if not valid
 */
CaretUndoStack*
BrainBrowserWindowToolBarImage::getUndoStack()
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
 * Gets called when the redo action is triggered
 */
void
BrainBrowserWindowToolBarImage::redoActionTriggered()
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
BrainBrowserWindowToolBarImage::undoActionTriggered()
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

/**
 * Called when media coordinate display mode is changed
 */
void
BrainBrowserWindowToolBarImage::mediaDisplayCoordinateModeEnumComboBoxItemActivated()
{
    if (m_browserTabContent != NULL) {
        const MediaDisplayCoordinateModeEnum::Enum mode(m_mediaDisplayCoordinateModeEnumComboBox->getSelectedItem<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>());
        m_browserTabContent->setMediaDisplayCoordinateMode(mode);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}
