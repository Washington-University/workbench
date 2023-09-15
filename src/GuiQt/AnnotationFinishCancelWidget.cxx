
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
#include "EventAnnotationGetBeingDrawnInWindow.h"
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
    m_finishToolButton = new QToolButton();
    m_finishToolButton->setDefaultAction(m_finishAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_finishToolButton);
    QObject::connect(m_finishAction, &QAction::triggered,
                     this, &AnnotationFinishCancelWidget::finishActionTriggered);
    
    m_finishToolButtonStyleSheetDisabled = m_finishToolButton->styleSheet();
    
    /*
     * Used to make button background green when finish button is enabled
     */
    m_finishToolButtonStyleSheetEnabled = ("background-color: rgb(0, 255, 0)");

    m_cancelAction = WuQtUtilities::createAction("Cancel",
                                               "Cancel drawing new annotation",
                                               this,
                                               this,
                                               SLOT(cancelActionTriggered()));

    QToolButton* cancelToolButton = new QToolButton();
    cancelToolButton->setDefaultAction(m_cancelAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(cancelToolButton);
    
    WuQtUtilities::matchWidgetWidths(m_finishToolButton,
                                     cancelToolButton);
    
    m_eraseLastCoordinateAction = new QAction("X");
    m_eraseLastCoordinateAction->setToolTip("Remove the last poly coordinate");
    QObject::connect(m_eraseLastCoordinateAction, &QAction::triggered,
                     this, &AnnotationFinishCancelWidget::eraseLastCoordinateActionTriggered);
    QToolButton* eraseLastCoordinateToolButton = new QToolButton();
    eraseLastCoordinateToolButton->setDefaultAction(m_eraseLastCoordinateAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(eraseLastCoordinateToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    switch (orientation) {
        case Qt::Horizontal:
            gridLayout->addWidget(new QLabel("Drawing"),
                                  0, 0, 1, 2, Qt::AlignHCenter);
            gridLayout->addWidget(m_finishToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  2, 0, Qt::AlignHCenter);
            gridLayout->addWidget(eraseLastCoordinateToolButton,
                                  1, 1, 2, 1, Qt::AlignCenter);
            break;
        case Qt::Vertical:
            gridLayout->addWidget(m_finishToolButton,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(eraseLastCoordinateToolButton,
                                  2, 0, Qt::AlignHCenter);
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
AnnotationFinishCancelWidget::updateContent(const std::vector<Annotation*>& /*annotations*/)
{
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const Annotation* annotation(annDrawEvent.getAnnotation());

    AString cancelToolTip;
    AString finishToolTip;
    bool cancelEnabledFlag(false);
    bool finishEnabledFlag(false);
    bool eraseLastEnabledFlag(false);
    m_annotationNumberOfCoordinates = 0;
    if (annotation != NULL) {
        cancelEnabledFlag = true;
        
        cancelToolTip = ("Cancel drawing "
                         + AnnotationTypeEnum::toGuiName(annotation->getType()));
        finishToolTip = ("Finish drawing "
                         + AnnotationTypeEnum::toGuiName(annotation->getType()));
        
        int32_t numCoords(annotation->getNumberOfCoordinates());
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
                eraseLastEnabledFlag = (numCoords > 0);
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                switch (m_userInputMode) {
                    case UserInputModeEnum::Enum::INVALID:
                        break;
                    case UserInputModeEnum::Enum::ANNOTATIONS:
                        break;
                    case UserInputModeEnum::Enum::BORDERS:
                        break;
                    case UserInputModeEnum::Enum::FOCI:
                        break;
                    case UserInputModeEnum::Enum::IMAGE:
                        break;
                    case UserInputModeEnum::Enum::SAMPLES_EDITING:
                        /*
                         * When drawing samples, there are pairs of coordinates
                         * with one on the near volume slice and the other on
                         * the far volume slice.  Since the each coordinate added
                         * by the use is doubled, reduce the number of coordinates
                         * by 2.
                         */
                        numCoords /= 2;
                        break;
                    case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
                        break;
                    case UserInputModeEnum::Enum::VIEW:
                        break;
                    case UserInputModeEnum::Enum::VOLUME_EDIT:
                        break;
                }
                eraseLastEnabledFlag = (numCoords > 0);
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::POLYLINE:
                eraseLastEnabledFlag = (numCoords > 0);
                finishEnabledFlag = (numCoords >= 3);
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
        m_annotationNumberOfCoordinates = numCoords;
    }
    
    if (finishEnabledFlag) {
        m_finishToolButton->setStyleSheet(m_finishToolButtonStyleSheetEnabled);
    }
    else {
        m_finishToolButton->setStyleSheet(m_finishToolButtonStyleSheetDisabled);
    }
    
    m_finishAction->setEnabled(finishEnabledFlag);
    m_finishAction->setToolTip(finishToolTip);

    m_cancelAction->setEnabled(cancelEnabledFlag);
    m_cancelAction->setToolTip(cancelToolTip);
    
    m_eraseLastCoordinateAction->setEnabled(eraseLastEnabledFlag);
    
    setEnabled(m_eraseLastCoordinateAction->isEnabled()
               || m_finishAction->isEnabled()
               || m_cancelAction->isEnabled());
}


/**
 * Gets called when the finish action is triggered
 */
void
AnnotationFinishCancelWidget::finishActionTriggered()
{
    m_finishAction->blockSignals(true);
    
    EventAnnotationDrawingFinishCancel finishEvent(EventAnnotationDrawingFinishCancel::Mode::FINISH,
                                                   m_browserWindowIndex,
                                                   m_userInputMode);
    EventManager::get()->sendEvent(finishEvent.getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    m_finishAction->blockSignals(false);
}

/**
 * Gets called when the cancel action is triggered
 */
void
AnnotationFinishCancelWidget::cancelActionTriggered()
{
    EventAnnotationDrawingFinishCancel cancelEvent(EventAnnotationDrawingFinishCancel::Mode::CANCEL,
                                                   m_browserWindowIndex,
                                                   m_userInputMode);
    EventManager::get()->sendEvent(cancelEvent.getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the erase last coordinate action is triggered
 */
void
AnnotationFinishCancelWidget::eraseLastCoordinateActionTriggered()
{
    if (m_annotationNumberOfCoordinates > 1) {
        EventAnnotationDrawingFinishCancel eraseEvent(EventAnnotationDrawingFinishCancel::Mode::ERASE_LAST_COORDINATE,
                                                       m_browserWindowIndex,
                                                       m_userInputMode);
        EventManager::get()->sendEvent(eraseEvent.getPointer());
    }
    else if (m_annotationNumberOfCoordinates == 1) {
        EventAnnotationDrawingFinishCancel restartEvent(EventAnnotationDrawingFinishCancel::Mode::RESTART_DRAWING,
                                                      m_browserWindowIndex,
                                                      m_userInputMode);
        EventManager::get()->sendEvent(restartEvent.getPointer());
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
