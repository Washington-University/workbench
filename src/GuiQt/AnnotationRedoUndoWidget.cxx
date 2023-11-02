
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
#include "AnnotationPolyhedron.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
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
                                                   const UserInputModeEnum::Enum userInputMode,
                                                   const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* titleLabel = new QLabel("Edit");

    QToolButton* lockToolButton(NULL);
    switch (userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
        {
            const AString lockToolTip("<html>"
                                      "Lock the selected polyhedron.  Lock is enabled when there "
                                      "is one and only one polyhedron selected.  "
                                      "Once locked, no other "
                                      "polyhedrons can be selected.  "
                                      "This feature may be useful when polyhedrons overlap or are "
                                      "very close to each other.  "
                                      "Lock must be turned off "
                                      "to deselect the locked polyhedron or select a different polyhedron.<br><br>"
                                      "Lock is turned off by:"
                                      "<ul>"
                                      "<li> Clicking the <b>Lock</b> button"
                                      "<li> Changing to a different Mode"
                                      "<li> Cutting or Deleting the polyhedron"
                                      "<li> Deselect All or Select All is selected from the Edit or Right-Click Menu"
                                      "</ul>"
                                      "</html>");
            m_lockAction = new QAction(this);
            m_lockAction->setCheckable(true);
            m_lockAction->setText("Lock");
            m_lockAction->setToolTip(lockToolTip);
            QObject::connect(m_lockAction, &QAction::toggled,
                             this, &AnnotationRedoUndoWidget::lockActionToggled);

            lockToolButton = new QToolButton();
            lockToolButton->setDefaultAction(m_lockAction);
            WuQtUtilities::setToolButtonStyleForQt5Mac(lockToolButton);
        }
            break;
    }
    
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
            if (lockToolButton != NULL) {
                gridLayout->addWidget(lockToolButton,
                                      1, 2, Qt::AlignHCenter);
            }
            break;
        case Qt::Vertical:
            gridLayout->addWidget(titleLabel,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(redoToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(undoToolButton,
                                  2, 0, Qt::AlignHCenter);
            if (lockToolButton != NULL) {
                gridLayout->addWidget(lockToolButton,
                                      3, 0, Qt::AlignHCenter);
            }
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
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();

    m_redoAction->setEnabled(undoStack->canRedo());
    m_redoAction->setToolTip(undoStack->redoText());
    
    m_undoAction->setEnabled(undoStack->canUndo());
    m_undoAction->setToolTip(undoStack->undoText());
    
    if (m_lockAction != NULL) {
        m_lockAction->setEnabled(false);
        bool lockCheckedFlag(false);
        switch (m_userInputMode) {
            case UserInputModeEnum::Enum::ANNOTATIONS:
            case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            case UserInputModeEnum::Enum::BORDERS:
            case UserInputModeEnum::Enum::FOCI:
            case UserInputModeEnum::Enum::IMAGE:
            case UserInputModeEnum::Enum::INVALID:
            case UserInputModeEnum::Enum::VIEW:
            case UserInputModeEnum::Enum::VOLUME_EDIT:
                CaretAssert(0);
                break;
            case UserInputModeEnum::Enum::SAMPLES_EDITING:
            {
                lockCheckedFlag = Annotation::getSelectionLockedPolyhedronInWindow(m_browserWindowIndex);

                if (annotations.size() == 1) {
                    CaretAssertVectorIndex(annotations, 0);
                    CaretAssert(annotations[0]);
                    AnnotationPolyhedron* polyhedron(annotations[0]->castToPolyhedron());
                    if (polyhedron != NULL) {
                        m_lockAction->setEnabled(true);

                        lockCheckedFlag = Annotation::getSelectionLockedPolyhedronInWindow(m_browserWindowIndex);
                    }
                }
            }
                break;
        }
        QSignalBlocker blocker(m_lockAction);
        m_lockAction->setChecked(lockCheckedFlag);
    }
    setEnabled(( ! annotations.empty())
               || m_redoAction->isEnabled()
               || m_undoAction->isEnabled());
    
    if (drawingAnnotationFlag) {
        /*
         * Disable redo/undo when drawing an annotation since redo/undo
         * does not work on the annotation being drawn.
         */
        setEnabled(false);
    }
}

/**
 * Gets called when the redo action is triggered
 */
void
AnnotationRedoUndoWidget::redoActionTriggered()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();
    
    AString errorMessage;
    if ( ! undoStack->redoInWindow(m_browserWindowIndex,
                                   errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the undo action is triggered
 */
void
AnnotationRedoUndoWidget::undoActionTriggered()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack();
    
    AString errorMessage;
    if ( ! undoStack->undoInWindow(m_browserWindowIndex,
                                   errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the lock action is triggered
 */
void
AnnotationRedoUndoWidget::lockActionToggled(bool checked)
{
    if (checked) {
        switch (m_userInputMode) {
            case UserInputModeEnum::Enum::ANNOTATIONS:
            case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            case UserInputModeEnum::Enum::BORDERS:
            case UserInputModeEnum::Enum::FOCI:
            case UserInputModeEnum::Enum::IMAGE:
            case UserInputModeEnum::Enum::INVALID:
            case UserInputModeEnum::Enum::VIEW:
            case UserInputModeEnum::Enum::VOLUME_EDIT:
                CaretAssert(0);
                break;
            case UserInputModeEnum::Enum::SAMPLES_EDITING:
            {
                if (m_selectedAnnotations.size() == 1) {
                    CaretAssertVectorIndex(m_selectedAnnotations, 0);
                    CaretAssert(m_selectedAnnotations[0]);
                    AnnotationPolyhedron* polyhedron(m_selectedAnnotations[0]->castToPolyhedron());
                    if (polyhedron != NULL) {
                        if (polyhedron == Annotation::getSelectionLockedPolyhedronInWindow(m_browserWindowIndex)) {
                            Annotation::unlockPolyhedronInWindow(m_browserWindowIndex);
                        }
                        else {
                            Annotation::setSelectionLockedPolyhedronInWindow(m_browserWindowIndex,
                                                                             polyhedron);
                        }
                    }
                }
            }
                break;
        }
    }
    else {
        Annotation::setSelectionLockedPolyhedronInWindow(m_browserWindowIndex, NULL);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
