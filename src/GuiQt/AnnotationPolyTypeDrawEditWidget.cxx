
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
#include <QActionGroup>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
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
#include "WuQMessageBox.h"
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
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *    The browser window index
 * @param parent
 *    The parent widget.
 */
AnnotationPolyTypeDrawEditWidget::AnnotationPolyTypeDrawEditWidget(UserInputModeAnnotations* userInputModeAnnotations,
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
    
    m_cancelAction = new QAction("Cancel");
    m_cancelAction->setToolTip("Cancel drawing and discard changes");
    QObject::connect(m_cancelAction, &QAction::triggered,
                     this, &AnnotationPolyTypeDrawEditWidget::cancelActionTriggered);
    
    QToolButton* cancelToolButton = new QToolButton();
    cancelToolButton->setDefaultAction(m_cancelAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(cancelToolButton);
    
    WuQtUtilities::matchWidgetWidths(m_finishToolButton,
                                     cancelToolButton);
    
    QToolButton* eraseLastCoordinateToolButton = new QToolButton();
    m_eraseLastCoordinateAction = new QAction("Erase");
    m_eraseLastCoordinateAction->setToolTip("<html>"
                                            "Remove the last coordinate that was "
                                            "added in DRAW MODE"
                                            "</html>");
    QObject::connect(m_eraseLastCoordinateAction, &QAction::triggered,
                     this, &AnnotationPolyTypeDrawEditWidget::eraseLastCoordinateActionTriggered);
    eraseLastCoordinateToolButton->setDefaultAction(m_eraseLastCoordinateAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(eraseLastCoordinateToolButton);

    /*
     * Draw button
     */
    const QString drawToolTip("<html>"
                              "DRAW MODE - add new coordinates"
                              "<ul>"
                              "<li>Click the mouse to add one coordinate to the end of "
                              "the poly shape"
                              "<li>Drag the mouse to add a series of "
                              "coordinates to the end of the poly shape"
                              "<li>Using both clicks and drags is allowed"
                              "</ul>"
                              "</html>");
    m_drawCoordinatesAction = new QAction("Draw");
    m_drawCoordinatesAction->setCheckable(true);
    m_drawCoordinatesAction->setToolTip(drawToolTip);
    QObject::connect(m_drawCoordinatesAction, &QAction::triggered, this,
                     &AnnotationPolyTypeDrawEditWidget::drawCoordinatesActionTriggered);
    
    QToolButton* drawCoordinatesToolButton(new QToolButton());
    drawCoordinatesToolButton->setDefaultAction(m_drawCoordinatesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(drawCoordinatesToolButton);
    
    /*
     * Delete button
     */
    const QString deleteToolTip("<html>"
                                "EDIT MODE - REMOVE coordinates"
                                "<ul>"
                                "<li>Move the mouse over a coordinate"
                                "<li>The cursor becomes an 'X'"
                                "<li>Click the mouse to remove the coordinate"
                                "</ul>"
                                "</html>");
    m_removeCoordinatesAction = new QAction("Remove");
    m_removeCoordinatesAction->setCheckable(true);
    m_removeCoordinatesAction->setToolTip(deleteToolTip);
    QObject::connect(m_removeCoordinatesAction, &QAction::triggered, this,
                     &AnnotationPolyTypeDrawEditWidget::removeCoordinatesActionTriggered);
    
    QToolButton* removeCoordinatesToolButton(new QToolButton());
    removeCoordinatesToolButton->setDefaultAction(m_removeCoordinatesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(removeCoordinatesToolButton);
    
    /*
     * Insert button
     */
    const QString insertToolTip("<html>"
                                "EDIT MODE - INSERT coordinates"
                                "<ul>"
                                "<li>Move mouse over a line"
                                "<li>The cursor becomes a 'plus' symbol"
                                "<li>Click the mouse to insert a coordinate"
                                "</ul>"
                                "</html>");
    m_insertCoordinatesAction = new QAction("Insert");
    m_insertCoordinatesAction->setCheckable(true);
    m_insertCoordinatesAction->setToolTip(insertToolTip);
    QObject::connect(m_insertCoordinatesAction, &QAction::triggered, this,
                     &AnnotationPolyTypeDrawEditWidget::insertCoordinatesActionTriggered);
    
    QToolButton* insertCoordinatesToolButton(new QToolButton());
    insertCoordinatesToolButton->setDefaultAction(m_insertCoordinatesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(insertCoordinatesToolButton);
    
    /*
     * Move One Coordinate button
     */
    AString annMoveToolTip;
    if (m_userInputMode == UserInputModeEnum::Enum::ANNOTATIONS) {
        annMoveToolTip = ("<br>"
                          "MOVE all coordinates in polyline/polygon "
                          "(only available after shape has been 'finished')"
                          "<ul>"
                          "<li>Move mouse over a line in the shape"
                          "<li>The cursor becomes a 'four arrows' symbol"
                          "<li>Hold down the mouse and drag the entire "
                          "shape to its new location"
                          "</ul>");
    }
    const QString moveToolTip("<html>"
                              "EDIT MODE - MOVE coordinate under mouse"
                              "<ul>"
                              "<li>Move mouse over a coordinate"
                              "<li>The cursor becomes a 'two arrows' symbol"
                              "<li>Hold down the mouse and drag the coordinate "
                              "to its new location"
                              "</ul>"
                              + annMoveToolTip
                              + "<html>");
    m_moveOneCoordinateAction = new QAction("Move");
    m_moveOneCoordinateAction->setCheckable(true);
    m_moveOneCoordinateAction->setToolTip(moveToolTip);
    QObject::connect(m_moveOneCoordinateAction, &QAction::triggered, this,
                     &AnnotationPolyTypeDrawEditWidget::moveOneCoordinateActionTriggered);
    
    QToolButton* moveOneCoordinateToolButton(new QToolButton());
    moveOneCoordinateToolButton->setDefaultAction(m_moveOneCoordinateAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(moveOneCoordinateToolButton);
    
    m_moveTwoCoordinatesAction = NULL;
    QToolButton* moveTwoCoordinatesToolButton(NULL);

    if (m_userInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING) {
        /*
         * Move Two Coordinates button
         */
        const QString moveTwoToolTip("<html>"
                                     "EDIT MODE - MOVE BOTH coordinate under mouse "
                                     "and its corresponding coordinate at the other "
                                     "end of the polyhedron"
                                     "<ul>"
                                     "<li>Move mouse over a coordinate"
                                     "<li>The cursor becomes a 'two arrows' symbol"
                                     "<li>Hold down the mouse and drag the coordinate "
                                     "(and coordinate at other end) to the new locations"
                                     "</ul>"
                                     "<html>");
        m_moveTwoCoordinatesAction = new QAction("Move2");
        m_moveTwoCoordinatesAction->setCheckable(true);
        m_moveTwoCoordinatesAction->setToolTip(moveTwoToolTip);
        QObject::connect(m_moveTwoCoordinatesAction, &QAction::triggered, this,
                         &AnnotationPolyTypeDrawEditWidget::moveTwoCoordinatesActionTriggered);
        
        moveTwoCoordinatesToolButton = new QToolButton();
        moveTwoCoordinatesToolButton->setDefaultAction(m_moveTwoCoordinatesAction);
        WuQtUtilities::setToolButtonStyleForQt5Mac(moveTwoCoordinatesToolButton);
    }
    
    /*
     * Keep buttons mutually exclusive
     */
    QActionGroup* actionGroup(new QActionGroup(this));
    actionGroup->setExclusive(true);
    actionGroup->addAction(m_drawCoordinatesAction);
    actionGroup->addAction(m_removeCoordinatesAction);
    actionGroup->addAction(m_insertCoordinatesAction);
    actionGroup->addAction(m_moveOneCoordinateAction);
    if (m_moveTwoCoordinatesAction != NULL) {
        actionGroup->addAction(m_moveTwoCoordinatesAction);
    }
    
    /*
     * Keep some buttons same width
     */
    WuQtUtilities::matchWidgetWidths(drawCoordinatesToolButton,
                                     eraseLastCoordinateToolButton);
    WuQtUtilities::matchWidgetWidths(removeCoordinatesToolButton,
                                     moveOneCoordinateToolButton);
    if (moveTwoCoordinatesToolButton != NULL) {
        WuQtUtilities::matchWidgetWidths(insertCoordinatesToolButton,
                                         moveTwoCoordinatesToolButton);
    }

    QLabel* drawingLabel(new QLabel("Drawing"));
    QFont font(drawingLabel->font());
    font.setPointSizeF(font.pointSizeF() * 0.8);
    drawingLabel->setFont(font);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    if (m_userInputMode == UserInputModeEnum::Enum::ANNOTATIONS) {
        /*
         * Not sure why this is needed for Annotations toolbar (prevents buttons
         * overlapping vertical line on left) but not samples toolbar
         */
        QMargins margins(gridLayout->contentsMargins());
        margins.setLeft(3);
        gridLayout->setContentsMargins(margins);
    }
    
    /**
     * Adds space between buttons
     */
    gridLayout->setHorizontalSpacing(1);
    gridLayout->setColumnMinimumWidth(1, 3);
    gridLayout->setColumnMinimumWidth(4, 3);
    
    int32_t row(0);
    gridLayout->addWidget(drawingLabel,
                          row, 2, 1, 2, Qt::AlignHCenter);
    ++row;
    
    gridLayout->addWidget(drawCoordinatesToolButton,
                          row, 0, Qt::AlignHCenter);
    gridLayout->addWidget(removeCoordinatesToolButton,
                          row, 2, Qt::AlignHCenter);
    gridLayout->addWidget(insertCoordinatesToolButton,
                          row, 3, Qt::AlignHCenter);
    gridLayout->addWidget(m_finishToolButton,
                          row, 5, Qt::AlignHCenter);
    ++row;
    gridLayout->addWidget(eraseLastCoordinateToolButton,
                          row, 0, Qt::AlignHCenter);
    gridLayout->addWidget(moveOneCoordinateToolButton,
                          row, 2, Qt::AlignHCenter);
    if (moveTwoCoordinatesToolButton != NULL) {
        gridLayout->addWidget(moveTwoCoordinatesToolButton,
                              row, 3, Qt::AlignHCenter);
    }
    gridLayout->addWidget(cancelToolButton,
                          row, 5, Qt::AlignHCenter);

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
 * Create the backspace icon in a pixmap
 */
QPixmap
AnnotationPolyTypeDrawEditWidget::createBackspacePixmap(QWidget* widget) const
{
    const QString backspaceCharacter("X");
    CaretAssert(backspaceCharacter.length() == 1);
    
    /*
     * Use font metrics to position the character
     * Origin is in top left
     */
    QFont font(widget->font());
    font.setPointSize(16);
    QFontMetrics fontMetrics(font);
    const QRect fontBoundsRect(fontMetrics.boundingRect(backspaceCharacter));
    const int32_t charLeft(fontBoundsRect.left());
    const int32_t charRight(fontBoundsRect.right());
    const int32_t drawLeft(-(charLeft + charRight) / 2);
    const int32_t charTop(fontBoundsRect.top());
    const int32_t charBottom(fontBoundsRect.bottom());
    const int32_t drawTop(-(charTop + charBottom) / 2);
    const int32_t backspaceCharX(drawLeft + 1);
    const int32_t backspaceCharY(drawTop);
    
    /*
     * Create a pixmap to draw into with origin in the center
     */
    const float pixmapSize = 24.0;
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap,
                                                                                            static_cast<uint32_t>(WuQtUtilities::PixMapCreationOptions::TransparentBackground));
    /*
     * Create the backspace key outline in a polygon
     * Origin is in top left
     */
    const int32_t top(11);
    const int32_t left(-11);
    const int32_t leftTwo(-2);
    const int32_t right(12);
    const int32_t bottom(-11);
    QPolygon keyOutlinePolygon;
    keyOutlinePolygon.push_back(QPoint(left, 0));
    keyOutlinePolygon.push_back(QPoint(leftTwo, top));
    keyOutlinePolygon.push_back(QPoint(right, top));
    keyOutlinePolygon.push_back(QPoint(right, bottom));
    keyOutlinePolygon.push_back(QPoint(leftTwo, bottom));

    /*
     * Draw the polygon and the character
     */
    QPen pen(painter->pen());
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawPolygon(keyOutlinePolygon);
    painter->setFont(font);
    painter->drawText(QPoint(backspaceCharX,
                             backspaceCharY),
                      backspaceCharacter);

    return pixmap;
}

/**
 * Update the widget
 */
void
AnnotationPolyTypeDrawEditWidget::updateContent()
{
    std::vector<UserInputModeAnnotations::PolyTypeDrawEditOperation> availableOperations;
    const Annotation* selectedAnnotation(NULL);
    m_userInputModeAnnotations->getEnabledPolyTypeDrawEditOperations(availableOperations,
                                                                     selectedAnnotation);
    
    AString cancelToolTip;
    AString finishToolTip;
    bool cancelEnabledFlag(false);
    bool finishEnabledFlag(false);
    bool eraseLastEnabledFlag(false);
    bool drawCoordinatesEnabledFlag(false);
    bool drawCoordinatesCheckedFlag(false);
    bool deleteCoordinatesEnabledFlag(false);
    bool deleteCoordinatesCheckedFlag(false);
    bool insertCoordinatesEnabledFlag(false);
    bool insertCoordinatesCheckedFlag(false);
    bool moveOneCoordinateEnabledFlag(false);
    bool moveOneCoordinateCheckedFlag(false);
    bool moveTwoCoordinatesEnabledFlag(false);
    bool moveTwoCoordinatesCheckedFlag(false);
    
    for (const auto operation : availableOperations) {
        switch (operation) {
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::CANCEL_NEW_ANNOTATION:
                cancelEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::REMOVE_COORDINATE:
                deleteCoordinatesEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::DRAW_NEW_COORDINATE:
                drawCoordinatesEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::ERASE_LAST_COORDINATE:
                eraseLastEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::FINISH_NEW_ANNOTATION:
                finishEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::INSERT_COORDINATE:
                insertCoordinatesEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_ONE_COORDINATE:
                moveOneCoordinateEnabledFlag = true;
                break;
            case UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_TWO_COORDINATES:
                moveTwoCoordinatesEnabledFlag = true;
                break;
        }
    }
    m_annotationNumberOfCoordinates = 0;

    if (selectedAnnotation != NULL) {
        cancelToolTip = ("Cancel drawing "
                         + AnnotationTypeEnum::toGuiName(selectedAnnotation->getType()));
        finishToolTip = ("Finish drawing "
                         + AnnotationTypeEnum::toGuiName(selectedAnnotation->getType()));
        
        int32_t numCoords(selectedAnnotation->getNumberOfCoordinates());
        switch (selectedAnnotation->getType()) {
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
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                numCoords /= 2;  /* Coordinates are pairs */
                break;
            case AnnotationTypeEnum::POLYLINE:
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
        m_annotationNumberOfCoordinates = numCoords;
    }
    
    switch (m_userInputModeAnnotations->getPolyTypeDrawEditOperation()) {
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::CANCEL_NEW_ANNOTATION:
            /* Not a toggle button  */
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::REMOVE_COORDINATE:
            deleteCoordinatesCheckedFlag = true;
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::DRAW_NEW_COORDINATE:
            drawCoordinatesCheckedFlag = true;
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::ERASE_LAST_COORDINATE:
            /* Not a toggle button  */
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::FINISH_NEW_ANNOTATION:
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::INSERT_COORDINATE:
            insertCoordinatesCheckedFlag = true;
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_ONE_COORDINATE:
            moveOneCoordinateCheckedFlag = true;
            break;
        case UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_TWO_COORDINATES:
            moveTwoCoordinatesCheckedFlag = true;;
            break;
    }

    if (finishEnabledFlag) {
        m_finishToolButton->setStyleSheet(m_finishToolButtonStyleSheetEnabled);
    }
    else {
        m_finishToolButton->setStyleSheet(m_finishToolButtonStyleSheetDisabled);
    }
    
    if (m_drawCoordinatesAction != NULL) {
        m_drawCoordinatesAction->setEnabled(drawCoordinatesEnabledFlag);
        m_drawCoordinatesAction->setChecked(drawCoordinatesCheckedFlag);
    }
    if (m_removeCoordinatesAction != NULL) {
        m_removeCoordinatesAction->setEnabled(deleteCoordinatesEnabledFlag);
        m_removeCoordinatesAction->setChecked(deleteCoordinatesCheckedFlag);
    }
    if (m_insertCoordinatesAction != NULL) {
        m_insertCoordinatesAction->setEnabled(insertCoordinatesEnabledFlag);
        m_insertCoordinatesAction->setChecked(insertCoordinatesCheckedFlag);
    }
    if (m_moveOneCoordinateAction != NULL) {
        m_moveOneCoordinateAction->setEnabled(moveOneCoordinateEnabledFlag);
        m_moveOneCoordinateAction->setChecked(moveOneCoordinateCheckedFlag);
    }
    if (m_moveTwoCoordinatesAction != NULL) {
        m_moveTwoCoordinatesAction->setEnabled(moveTwoCoordinatesEnabledFlag);
        m_moveTwoCoordinatesAction->setChecked(moveTwoCoordinatesCheckedFlag);
    }

    m_finishAction->setEnabled(finishEnabledFlag);
    m_finishAction->setToolTip(finishToolTip);

    m_cancelAction->setEnabled(cancelEnabledFlag);
    m_cancelAction->setToolTip(cancelToolTip);
    
    m_eraseLastCoordinateAction->setEnabled(eraseLastEnabledFlag);
    
    setEnabled(finishEnabledFlag
               || cancelEnabledFlag
               || eraseLastEnabledFlag
               || drawCoordinatesEnabledFlag
               || deleteCoordinatesEnabledFlag
               || insertCoordinatesEnabledFlag
               || moveOneCoordinateEnabledFlag
               || moveTwoCoordinatesEnabledFlag);
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
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputMode,
                                                      m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const Annotation* annotation(annDrawEvent.getAnnotation());
    if (annotation != NULL) {
        if (annotation->getNumberOfCoordinates() > 0) {
            const AString msg("Continue and discard partially drawn "
                              + AnnotationTypeEnum::toGuiName(annotation->getType())
                              + "?");
            if (WuQMessageBox::warningYesNo(this, msg)) {
                /* Yes, discard */
            }
            else {
                return;
            }
        }
    }

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
 * Called when the delete coordinate action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::removeCoordinatesActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setPolyTypeDrawEditOperation(UserInputModeAnnotations::PolyTypeDrawEditOperation::REMOVE_COORDINATE);
    }
    
    updateContent();
}

/**
 * Called when the insert coordinates action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::insertCoordinatesActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setPolyTypeDrawEditOperation(UserInputModeAnnotations::PolyTypeDrawEditOperation::INSERT_COORDINATE);
    }
    
    updateContent();
}

/**
 * Called when the move coordinate action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::moveOneCoordinateActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setPolyTypeDrawEditOperation(UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_ONE_COORDINATE);
    }
    
    updateContent();
}

/**
 * Called when the move two coordinates action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::moveTwoCoordinatesActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setPolyTypeDrawEditOperation(UserInputModeAnnotations::PolyTypeDrawEditOperation::MOVE_TWO_COORDINATES);
    }
    
    updateContent();
}

/**
 * Called when the draw coordinates action is triggered (by user)
 * @param checked
 *    New checked status
 */
void
AnnotationPolyTypeDrawEditWidget::drawCoordinatesActionTriggered(bool checked)
{
    CaretAssert(m_userInputModeAnnotations);
    if (checked) {
        m_userInputModeAnnotations->setPolyTypeDrawEditOperation(UserInputModeAnnotations::PolyTypeDrawEditOperation::DRAW_NEW_COORDINATE);
    }
    
    updateContent();
}
