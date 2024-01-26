
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_REDO_UNDO_WIDGET_DECLARE__
#include "AnnotationRedoUndoWidget.h"
#undef __ANNOTATION_REDO_UNDO_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "UserInputModeAnnotations.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationRedoUndoWidget 
 * \brief Widget for redo and undo buttons
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *    The browser window index
 * @param parent
 *    The parent widget.
 */
AnnotationRedoUndoWidget::AnnotationRedoUndoWidget(const Qt::Orientation orientation,
                                                   UserInputModeAnnotations* userInputModeAnnotations,
                                                   const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_userInputModeAnnotations(userInputModeAnnotations),
m_userInputMode(userInputModeAnnotations->getUserInputMode()),
m_browserWindowIndex(browserWindowIndex)
{
    CaretAssert(m_userInputModeAnnotations);
    
    QLabel* titleLabel = new QLabel("Edit");

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
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    switch (orientation) {
        case Qt::Horizontal:
            gridLayout->addWidget(titleLabel,
                                  0, 0, 1, 2, Qt::AlignHCenter);
            gridLayout->addWidget(redoToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(undoToolButton,
                                  1, 1, Qt::AlignHCenter);
            break;
        case Qt::Vertical:
            gridLayout->addWidget(titleLabel,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(redoToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(undoToolButton,
                                  2, 0, Qt::AlignHCenter);
            break;
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
}

/**
 * Destructor.
 */
AnnotationRedoUndoWidget::~AnnotationRedoUndoWidget()
{
}

/**
 * Update with the selected annotations.
 *
 * @param annotations
 *     The selected annotations
 */
void
AnnotationRedoUndoWidget::updateContent(const std::vector<Annotation*>& annotations)
{
    m_selectedAnnotations = annotations;
    
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputMode,
                                                      m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const bool drawingAnnotationFlag(annDrawEvent.isAnnotationDrawingInProgress());
    
    CaretUndoStack* undoStack = m_userInputModeAnnotations->getUndoRedoStack();

    m_redoAction->setEnabled(undoStack->canRedo());
    m_redoAction->setToolTip(undoStack->redoText());
    
    m_undoAction->setEnabled(undoStack->canUndo());
    m_undoAction->setToolTip(undoStack->undoText());
    
    setEnabled(( ! annotations.empty())
               || m_redoAction->isEnabled()
               || m_undoAction->isEnabled());
}

/**
 * Gets called when the redo action is triggered
 */
void
AnnotationRedoUndoWidget::redoActionTriggered()
{
    CaretUndoStack* undoStack = m_userInputModeAnnotations->getUndoRedoStack();

    AString errorMessage;
    if ( ! undoStack->redoInWindow(m_browserWindowIndex,
                                   errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Gets called when the undo action is triggered
 */
void
AnnotationRedoUndoWidget::undoActionTriggered()
{
    CaretUndoStack* undoStack = m_userInputModeAnnotations->getUndoRedoStack();

    AString errorMessage;
    if ( ! undoStack->undoInWindow(m_browserWindowIndex,
                                   errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


