
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

#define __ANNOTATION_FINISH_CANCEL_WIDGET_DECLARE__
#include "AnnotationFinishCancelWidget.h"
#undef __ANNOTATION_FINISH_CANCEL_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

#include "Annotation.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventAnnotationDrawingFinishCancel.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationFinishCancelWidget
 * \brief Widget for finish and cancel buttons
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param orientation
 *    Orientation for the widets
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *    The browser window index
 * @param parent
 *    The parent widget.
 */
AnnotationFinishCancelWidget::AnnotationFinishCancelWidget(const Qt::Orientation orientation,
                                                           const UserInputModeEnum::Enum userInputMode,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    m_finishAction = new QAction("Finish");
    QToolButton* finishToolButton = new QToolButton();
    finishToolButton->setDefaultAction(m_finishAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(finishToolButton);
    QObject::connect(m_finishAction, &QAction::triggered,
                     this, &AnnotationFinishCancelWidget::finishActionTriggered);
    
    m_cancelAction = WuQtUtilities::createAction("Cancel",
                                               "Cancel drawing new annotation",
                                               this,
                                               this,
                                               SLOT(cancelActionTriggered()));
    m_cancelAction->setAutoRepeat(false);
    QToolButton* cancelToolButton = new QToolButton();
    cancelToolButton->setDefaultAction(m_cancelAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(cancelToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    switch (orientation) {
        case Qt::Horizontal:
            gridLayout->addWidget(finishToolButton,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  0, 1, Qt::AlignHCenter);
            break;
        case Qt::Vertical:
            gridLayout->addWidget(finishToolButton,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  1, 0, Qt::AlignHCenter);
            break;
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationFinishCancelWidget::~AnnotationFinishCancelWidget()
{
}

/**
 * Update with the selected annotations.
 *
 * @param annotations
 *     The selected annotations
 */
void
AnnotationFinishCancelWidget::updateContent(const std::vector<Annotation*>& annotations)
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    const Annotation* annotation(annMan->getAnnotationBeingDrawnInWindow(m_browserWindowIndex));
//    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack(m_userInputMode);
//
    AString cancelToolTip;
    AString finishToolTip;
    bool cancelEnabledFlag(false);
    bool finishEnabledFlag(false);
    if (annotation != NULL) {
        cancelEnabledFlag = true;
        
        cancelToolTip = ("Cancel drawing "
                         + AnnotationTypeEnum::toGuiName(annotation->getType()));
        finishToolTip = ("Finish drawing "
                         + AnnotationTypeEnum::toGuiName(annotation->getType()));
        
        const int32_t numCoords(annotation->getNumberOfCoordinates());
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                break;
            case AnnotationTypeEnum::OVAL:
                break;
            case AnnotationTypeEnum::POLYGON:
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::POLYLINE:
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
    }
    m_finishAction->setEnabled(finishEnabledFlag);
    m_finishAction->setToolTip(finishToolTip);

    m_cancelAction->setEnabled(cancelEnabledFlag);
    m_cancelAction->setToolTip(cancelToolTip);

    m_finishAction->setEnabled(false);
    m_cancelAction->setEnabled(false);
    
    setEnabled(( ! annotations.empty())
               || m_finishAction->isEnabled()
               || m_cancelAction->isEnabled());
}


/**
 * Gets called when the finish action is triggered
 */
void
AnnotationFinishCancelWidget::finishActionTriggered()
{
//    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
//    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack(m_userInputMode);
//
//    AString errorMessage;
//    if ( ! undoStack->redoInWindow(m_browserWindowIndex,
//                                   errorMessage)) {
//        WuQMessageBox::errorOk(this,
//                               errorMessage);
//    }
    
    EventAnnotationDrawingFinishCancel finishEvent(EventAnnotationDrawingFinishCancel::Mode::FINISH,
                                                   m_browserWindowIndex);
    EventManager::get()->sendEvent(finishEvent.getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the cancel action is triggered
 */
void
AnnotationFinishCancelWidget::cancelActionTriggered()
{
//    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
//    CaretUndoStack* undoStack = annMan->getCommandRedoUndoStack(m_userInputMode);
//
//    AString errorMessage;
//    if ( ! undoStack->undoInWindow(m_browserWindowIndex,
//                                   errorMessage)) {
//        WuQMessageBox::errorOk(this,
//                               errorMessage);
//    }
    
    EventAnnotationDrawingFinishCancel cancelEvent(EventAnnotationDrawingFinishCancel::Mode::CANCEL,
                                                   m_browserWindowIndex);
    EventManager::get()->sendEvent(cancelEvent.getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

