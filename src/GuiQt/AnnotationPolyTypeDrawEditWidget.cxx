
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

#define __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_DECLARE__
#include "AnnotationPolyTypeDrawEditWidget.h"
#undef __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_DECLARE__

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
#include "UserInputModeAnnotations.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationPolyTypeDrawEditWidget
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
AnnotationPolyTypeDrawEditWidget::AnnotationPolyTypeDrawEditWidget(const Qt::Orientation orientation,
                                                           UserInputModeAnnotations* userInputModeAnnotations,
                                                           const int32_t browserWindowIndex,
                                                           QWidget* parent)
: QWidget(parent),
m_userInputModeAnnotations(userInputModeAnnotations),
m_userInputMode(m_userInputModeAnnotations->getUserInputMode()),
m_browserWindowIndex(browserWindowIndex)
{
    CaretAssert(m_userInputModeAnnotations);
    
    m_finishAction = new QAction("Finish");
    m_finishToolButton = new QToolButton();
    m_finishToolButton->setDefaultAction(m_finishAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_finishToolButton);
    QObject::connect(m_finishAction, &QAction::triggered,
                     this, &AnnotationPolyTypeDrawEditWidget::finishActionTriggered);
    
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
                     this, &AnnotationPolyTypeDrawEditWidget::eraseLastCoordinateActionTriggered);
    QToolButton* eraseLastCoordinateToolButton = new QToolButton();
    eraseLastCoordinateToolButton->setDefaultAction(m_eraseLastCoordinateAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(eraseLastCoordinateToolButton);

    m_editVerticesAction = NULL;
    QToolButton* editVerticesToolButton(NULL);
    
    if (m_userInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING) {
        
        const QString moveToolTip("Click to enter coordinate moving mode.\n"
                                  "Move the mouse over a coordinate.  When cursor\n"
                                  "changes to 'two arrows', drag the mouse to\n"
                                  "move the coordinate.  Coordinates may be \n"
                                  "moved on any slice.  Click button again to \n"
                                  "return to drawing and finish the polyhedron.");
        m_editVerticesAction = new QAction("Move");
        m_editVerticesAction->setCheckable(true);
        m_editVerticesAction->setToolTip(moveToolTip);
        QObject::connect(m_editVerticesAction, &QAction::triggered, this,
                         &AnnotationPolyTypeDrawEditWidget::editVerticesActionTriggered);
        
        editVerticesToolButton = new QToolButton();
        editVerticesToolButton->setDefaultAction(m_editVerticesAction);
        WuQtUtilities::setToolButtonStyleForQt5Mac(editVerticesToolButton);
    }
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    switch (orientation) {
        case Qt::Horizontal:
            if (editVerticesToolButton != NULL) {
                gridLayout->addWidget(new QLabel("Drawing"),
                                      0, 0, 1, 4, Qt::AlignHCenter);
                gridLayout->addWidget(editVerticesToolButton,
                                      1, 3, Qt::AlignHCenter);
            }
            else {
                gridLayout->addWidget(new QLabel("Drawing"),
                                      0, 0, 1, 3, Qt::AlignHCenter);
            }
            
            gridLayout->addWidget(m_finishToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  1, 1, Qt::AlignHCenter);
            gridLayout->addWidget(eraseLastCoordinateToolButton,
                                  1, 2, Qt::AlignHCenter);
            break;
        case Qt::Vertical:
        {
            QLabel* drawingLabel(new QLabel("Drawing"));
            QFont font(drawingLabel->font());
            font.setPointSizeF(font.pointSizeF() * 0.8);
            drawingLabel->setFont(font);
            
            gridLayout->setVerticalSpacing(0);
            gridLayout->addWidget(drawingLabel,
                                  0, 0, 1, 2, Qt::AlignHCenter);
            gridLayout->addWidget(m_finishToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(cancelToolButton,
                                  2, 0, Qt::AlignHCenter);
            gridLayout->addWidget(eraseLastCoordinateToolButton,
                                  1, 1, Qt::AlignCenter);
            if (editVerticesToolButton != NULL) {
                gridLayout->addWidget(editVerticesToolButton,
                                      2, 1, Qt::AlignHCenter);
            }
        }
            break;
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationPolyTypeDrawEditWidget::~AnnotationPolyTypeDrawEditWidget()
{
}

/**
 * Update the widget
 */
void
AnnotationPolyTypeDrawEditWidget::updateContent()
{
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputMode,
                                                      m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const Annotation* annotation(annDrawEvent.getAnnotation());

    AString cancelToolTip;
    AString finishToolTip;
    bool finishEnabledFlag(false);
    bool eraseLastEnabledFlag(false);
    bool editVerticesEnabledFlag(false);
    bool editVerticesCheckedFlag(false);
    m_annotationNumberOfCoordinates = 0;
    if (annotation != NULL) {
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
                editVerticesEnabledFlag = (numCoords > 0);
                if (editVerticesEnabledFlag) {
                    switch (m_userInputModeAnnotations->getDrawingNewPolyTypeStereotaxicMode()) {
                        case UserInputModeAnnotations::ADD_NEW_VERTICES:
                            editVerticesCheckedFlag = false;
                            break;
                        case UserInputModeAnnotations::EDIT_VERTICES:
                            editVerticesCheckedFlag = true;
                            break;
                    }
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
    
    if (m_editVerticesAction != NULL) {
        m_editVerticesAction->setEnabled(editVerticesEnabledFlag);
        m_editVerticesAction->setChecked(editVerticesCheckedFlag);
    }
    
    m_finishAction->setEnabled(finishEnabledFlag);
    m_finishAction->setToolTip(finishToolTip);

    m_cancelAction->setEnabled(annDrawEvent.isAnnotationDrawingInProgress());
    m_cancelAction->setToolTip(cancelToolTip);
    
    m_eraseLastCoordinateAction->setEnabled(eraseLastEnabledFlag);
    
    if (editVerticesCheckedFlag) {
        m_cancelAction->setEnabled(false);
        m_eraseLastCoordinateAction->setEnabled(false);
        m_finishAction->setEnabled(false);
    }
    
    setEnabled(finishEnabledFlag
               || m_cancelAction->isEnabled()
               || eraseLastEnabledFlag
               || editVerticesEnabledFlag);
}


/**
 * Gets called when the finish action is triggered
 */
void
AnnotationPolyTypeDrawEditWidget::finishActionTriggered()
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
AnnotationPolyTypeDrawEditWidget::cancelActionTriggered()
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
AnnotationPolyTypeDrawEditWidget::eraseLastCoordinateActionTriggered()
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

/**
 * Called when the edit vertices action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::editVerticesActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setDrawingNewPolyTypeStereotaxicMode(UserInputModeAnnotations::DrawingNewPolyTypeStereotaxicMode::EDIT_VERTICES);
    }
    else {
        m_userInputModeAnnotations->setDrawingNewPolyTypeStereotaxicMode(UserInputModeAnnotations::DrawingNewPolyTypeStereotaxicMode::ADD_NEW_VERTICES);
    }
    
    updateContent();
}

