
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

#include "AnnotationManager.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationRedoUndoWidget 
 * \brief Widget for redo and undo buttons
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationRedoUndoWidget::AnnotationRedoUndoWidget(const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* titleLabel = new QLabel("Oops");
    
    m_redoAction = WuQtUtilities::createAction("Redo",
                                               "Redo ToolTip",
                                               this,
                                               this,
                                               SLOT(redoActionTriggered()));
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(m_redoAction);
    
    m_undoAction = WuQtUtilities::createAction("Undo",
                                               "Undo ToolTip",
                                               this,
                                               this,
                                               SLOT(undoActionTriggered()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(m_undoAction);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(titleLabel,
                          0, 0, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(redoToolButton,
                          1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(undoToolButton,
                          2, 0, Qt::AlignHCenter);
    
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
 * Update with the given line annotation.
 *
 * @param annotationLine
 */
void
AnnotationRedoUndoWidget::updateContent()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();

    m_redoAction->setEnabled(undoStack->canRedo());
    m_redoAction->setToolTip(undoStack->redoText());
    
    m_undoAction->setEnabled(undoStack->canUndo());
    m_undoAction->setToolTip(undoStack->undoText());
}


/**
 * Gets called when the redo action is triggered
 */
void
AnnotationRedoUndoWidget::redoActionTriggered()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();
    undoStack->redoInWindow(m_browserWindowIndex);

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the undo action is triggered
 */
void
AnnotationRedoUndoWidget::undoActionTriggered()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();
    undoStack->undoInWindow(m_browserWindowIndex);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

