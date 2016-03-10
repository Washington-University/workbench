
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

#define __ANNOTATION_MENU_ARRANGE_DECLARE__
#include "AnnotationMenuArrange.h"
#undef __ANNOTATION_MENU_ARRANGE_DECLARE__

#include "Annotation.h"
#include "AnnotationArrangerInputs.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationSelectionInformation.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventAnnotationGrouping.h"
#include "EventGetBrainOpenGLTextRenderer.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuArrange 
 * \brief Menu for arranging annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget.
 * @param browserWindowIndex
 *     Index of the browser window.
 */
AnnotationMenuArrange::AnnotationMenuArrange(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QMenu(parent),
m_browserWindowIndex(browserWindowIndex)
{
    addAlignmentSelections();
    
    addSeparator();
    
    addDistributeSelections();

    addSeparator();
    
    addGroupingSelections();
    
    QObject::connect(this, SIGNAL(aboutToShow()),
                     this, SLOT(menuAboutToShow()));
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionTriggered(QAction*)));
}

/**
 * Destructor.
 */
AnnotationMenuArrange::~AnnotationMenuArrange()
{
}

/**
 * Add alignment options to the menu.
 */
void
AnnotationMenuArrange::addAlignmentSelections()
{
    std::vector<AnnotationAlignmentEnum::Enum> alignments;
    AnnotationAlignmentEnum::getAllEnums(alignments);
    
    for (std::vector<AnnotationAlignmentEnum::Enum>::iterator iter = alignments.begin();
         iter != alignments.end();
         iter++) {
        const AnnotationAlignmentEnum::Enum annAlign = *iter;
        const QString enumText = AnnotationAlignmentEnum::toGuiName(annAlign);
        const QString enumName = AnnotationAlignmentEnum::toName(annAlign);
        
        QPixmap pixmap = createAlignmentPixmap(this, annAlign);

        QAction* action = addAction(enumText);
        action->setIcon(pixmap);
        action->setData(enumName);
    }
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addDistributeSelections()
{
    std::vector<AnnotationDistributeEnum::Enum> distributes;
    AnnotationDistributeEnum::getAllEnums(distributes);
    
    for (std::vector<AnnotationDistributeEnum::Enum>::iterator iter = distributes.begin();
         iter != distributes.end();
         iter++) {
        const AnnotationDistributeEnum::Enum annDist = *iter;
        const QString enumText = AnnotationDistributeEnum::toGuiName(annDist);
        const QString enumName = AnnotationDistributeEnum::toName(annDist);
        
        QPixmap pixmap = createDistributePixmap(this, annDist);
        
        QAction* action = addAction(enumText);
        action->setIcon(pixmap);
        action->setData(enumName);
        
        //action->setEnabled(false);
    }
}

/**
 * Add distribution items to the menu.
 */
void
AnnotationMenuArrange::addGroupingSelections()
{
    m_groupAction = NULL;
    m_regroupAction  = NULL;
    m_ungroupAction  = NULL;
    
    std::vector<AnnotationGroupingModeEnum::Enum> groupings;
    AnnotationGroupingModeEnum::getAllEnums(groupings);
    
    for (std::vector<AnnotationGroupingModeEnum::Enum>::iterator iter = groupings.begin();
         iter != groupings.end();
         iter++) {
        const AnnotationGroupingModeEnum::Enum groupingMode = *iter;
        const QString enumText = AnnotationGroupingModeEnum::toGuiName(groupingMode);
        const QString enumName = AnnotationGroupingModeEnum::toName(groupingMode);
        
        //        QPixmap pixmap = createDistributePixmap(this, annDist);
        
        QAction* action = addAction(enumText);
        //        action->setIcon(pixmap);
        action->setData(enumName);
        
        switch (groupingMode) {
            case  AnnotationGroupingModeEnum::GROUP:
                m_groupAction = action;
                break;
            case AnnotationGroupingModeEnum::REGROUP:
                m_regroupAction = action;
                break;
            case AnnotationGroupingModeEnum::UNGROUP:
                m_ungroupAction = action;
                break;
        }
    }

    CaretAssert(m_groupAction);
    CaretAssert(m_regroupAction);
    CaretAssert(m_ungroupAction);
}

/*
 * Gets called when the menu is about to show
 * so that its menu items can be enabled/disabled.
 */
void
AnnotationMenuArrange::menuAboutToShow()
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    const AnnotationSelectionInformation* selectionInfo = annMan->getSelectionInformation(m_browserWindowIndex);
    CaretAssert(selectionInfo);
    
    m_groupAction->setEnabled(selectionInfo->isGroupingModeValid(AnnotationGroupingModeEnum::GROUP));
    m_regroupAction->setEnabled(selectionInfo->isGroupingModeValid(AnnotationGroupingModeEnum::REGROUP));
    m_ungroupAction->setEnabled(selectionInfo->isGroupingModeValid(AnnotationGroupingModeEnum::UNGROUP));
}


/**
 * Gets called when the user selects a menu item.
 */
void
AnnotationMenuArrange::menuActionTriggered(QAction* action)
{
    CaretAssert(action);
    
    const QString enumName = action->data().toString();
    
    bool validAlignmentFlag = false;
    const AnnotationAlignmentEnum::Enum annAlign = AnnotationAlignmentEnum::fromName(enumName,
                                                                                     &validAlignmentFlag);

    bool validDistributeFlag = false;
    const AnnotationDistributeEnum::Enum annDist = AnnotationDistributeEnum::fromName(enumName,
                                                                                       &validDistributeFlag);
    bool validGroupingFlag = false;
    const AnnotationGroupingModeEnum::Enum annGroup = AnnotationGroupingModeEnum::fromName(enumName,
                                                                                           &validGroupingFlag);

    if (validAlignmentFlag) {
        applyAlignment(annAlign);
    }
    else if (validDistributeFlag) {
        applyDistribute(annDist);
    }
    else if (validGroupingFlag) {
        applyGrouping(annGroup);
    }
    else {
        const AString msg("Unrecognized Enum name in Annotation Align Menu \""
                          + enumName
                          + "\"");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Apply alignment selection.
 *
 * @param alignment
 *     Selected alignment.
 */
void
AnnotationMenuArrange::applyAlignment(const AnnotationAlignmentEnum::Enum alignment)
{
//    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
//    CaretAssert(bbw);
    
    EventGetBrainOpenGLTextRenderer textRendererEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(textRendererEvent.getPointer());
    BrainOpenGLTextRenderInterface* textRenderer = textRendererEvent.getTextRenderer();
    if (textRenderer == NULL) {
        WuQMessageBox::errorOk(this, "Failed to get text renderer for window "
                               + QString::number(m_browserWindowIndex));
        return;
    }
    
    AnnotationArrangerInputs alignMod(textRenderer,
                                     m_browserWindowIndex);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->alignAnnotations(alignMod,
                                    alignment,
                                    errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Apply distribute selection.
 *
 * @param distribute
 *     Selected distribute.
 */
void
AnnotationMenuArrange::applyDistribute(const AnnotationDistributeEnum::Enum distribute)
{
//    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
//    CaretAssert(bbw);
    
    EventGetBrainOpenGLTextRenderer textRendererEvent(m_browserWindowIndex);
    EventManager::get()->sendEvent(textRendererEvent.getPointer());
    BrainOpenGLTextRenderInterface* textRenderer = textRendererEvent.getTextRenderer();
    if (textRenderer == NULL) {
        WuQMessageBox::errorOk(this, "Failed to get text renderer for window "
                               + QString::number(m_browserWindowIndex));
        return;
    }
    
    AnnotationArrangerInputs distributeMod(textRenderer,
                                           m_browserWindowIndex);
    
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->distributeAnnotations(distributeMod,
                                         distribute,
                                         errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Apply grouping selection.
 *
 * @param grouping
 *     Selected grouping.
 */
void
AnnotationMenuArrange::applyGrouping(const AnnotationGroupingModeEnum::Enum grouping)
{
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();

    const AnnotationSelectionInformation* selectionInfo = annMan->getSelectionInformation(m_browserWindowIndex);
    CaretAssert(selectionInfo);
    
    if ( ! selectionInfo->isGroupingModeValid(AnnotationGroupingModeEnum::GROUP)) {
        const QString msg("PROGRAM ERROR: AnnotationMenuArrange::applyGrouping "
                          "should not have been called.  Grouping is invalid.");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
        return;
    }
    
    std::vector<const AnnotationGroup*> groups = selectionInfo->getSelectedAnnotationGroups();
    std::vector<Annotation*> annotations = selectionInfo->getSelectedAnnotations();
    
    switch (grouping) {
        case AnnotationGroupingModeEnum::GROUP:
        {
            if (groups.size() != 1) {
                const QString msg("PROGRAM ERROR: AnnotationMenuArrange::applyGrouping "
                                  "should not have been called.  More than one selected group.");
                CaretAssertMessage(0, msg);
                CaretLogSevere(msg);
                return;
            }
            CaretAssertVectorIndex(groups, 0);
            const AnnotationGroup* annotationGroup = groups[0];
            
            EventAnnotationGrouping groupEvent;
            groupEvent.setModeGroupAnnotations(annotationGroup,
                                               annotations);
            EventManager::get()->sendEvent(groupEvent.getPointer());
            
            if (groupEvent.isError()) {
                WuQMessageBox::errorOk(this,
                                       groupEvent.getErrorMessage());
            }
            std::cout << "GROUPING EVENT SENT" << std::endl;
        }
            break;
        case AnnotationGroupingModeEnum::REGROUP:
            break;
        case AnnotationGroupingModeEnum::UNGROUP:
            break;
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Create an alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param alignment
 *    The alignment.
 * @return
 *    Pixmap with icon for the given alignment.
 */
QPixmap
AnnotationMenuArrange::createAlignmentPixmap(const QWidget* widget,
                                             const AnnotationAlignmentEnum::Enum alignment)
{
    CaretAssert(widget);
    
    const float pixmapSize = 24.0;
    const float halfPixmapSize  = pixmapSize / 2.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    const qreal minInset = 2;
    const qreal minInset2 = minInset * 2;
    const qreal minInset3 = minInset * 3;
    const qreal maxInset = pixmapSize - minInset;
    
    const QLineF horizontalLine(minInset, 0.0, maxInset, 0.0);
    const QLineF verticalLine(0.0, minInset, 0.0, maxInset);
    
    const qreal rectangleThickness = pixmapSize * 0.25;
    const qreal longRectangleLength = pixmapSize - (minInset * 4);
    const qreal shortRectangleLength = longRectangleLength * 0.60;
    const QRectF longVerticalRectangle(QPointF(0.0, 0.0),
                                      QPointF(rectangleThickness, longRectangleLength));
    const QRectF shortVerticalRectangle(QPointF(0.0, 0.0),
                                       QPointF(rectangleThickness, shortRectangleLength));
    const qreal shortVerticalRectangleOffset = pixmapSize - minInset2 - rectangleThickness;

    const QRectF longHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(longRectangleLength, rectangleThickness));
    const QRectF shortHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(shortRectangleLength, rectangleThickness));
    
    const qreal longRectangleSpace = pixmapSize - longRectangleLength;
    const qreal shortRectangleSpace = pixmapSize - shortRectangleLength;
    
    QBrush foregroundBrush = widget->palette().brush(widget->foregroundRole());
    QPen foregroundPen = painter->pen();
    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
    
    switch (alignment) {
        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
            drawLine(painter, horizontalLine, 0.0, minInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, minInset3);
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, minInset3);
            break;
        case AnnotationAlignmentEnum::ALIGN_CENTER:
            drawLine(painter, verticalLine, halfPixmapSize, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, (longRectangleSpace / 2.0), minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace / 2.0), shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_LEFT:
            drawLine(painter, verticalLine, minInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, minInset3, minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, minInset3, shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
            drawLine(painter, horizontalLine, 0.0, halfPixmapSize);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, (longRectangleSpace / 2.0));
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace / 2.0));
            break;
        case AnnotationAlignmentEnum::ALIGN_RIGHT:
            drawLine(painter, verticalLine, maxInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, (longRectangleSpace - minInset3), minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace - minInset3), shortVerticalRectangleOffset);
            break;
        case AnnotationAlignmentEnum::ALIGN_TOP:
            drawLine(painter, horizontalLine, 0.0, maxInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, (longRectangleSpace - minInset3));
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace - minInset3));
            break;
    }
    
    return pixmap;
}

/**
 * Create a distribute pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param distribute
 *    The distribute type.
 * @return
 *    Pixmap with icon for the given alignment.
 */
QPixmap
AnnotationMenuArrange::createDistributePixmap(const QWidget* widget,
                                             const AnnotationDistributeEnum::Enum distribute)
{
    CaretAssert(widget);
    
    const float pixmapSize = 24.0;
    const float halfPixmapSize  = pixmapSize / 2.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    const qreal minInset = 2;
    const qreal minInset2 = minInset * 2;
    const qreal minInset3 = minInset * 3;
    const qreal maxInset = pixmapSize - minInset;
    
    const QLineF horizontalLine(minInset, 0.0, maxInset, 0.0);
    const QLineF verticalLine(0.0, minInset, 0.0, maxInset);
    
    const qreal rectangleThickness = pixmapSize * 0.15;
    const qreal longRectangleLength = pixmapSize - (minInset3 * 2);
    const qreal shortRectangleLength = longRectangleLength * 0.50;
    const qreal midSizeRectangleLength = longRectangleLength * 0.75;
    const QRectF longVerticalRectangle(QPointF(0.0, 0.0),
                                       QPointF(rectangleThickness, longRectangleLength));
    const QRectF midSizeVerticalRectangle(QPointF(0.0, 0.0),
                                        QPointF(rectangleThickness, midSizeRectangleLength));
    const QRectF shortVerticalRectangle(QPointF(0.0, 0.0),
                                        QPointF(rectangleThickness, shortRectangleLength));
    const qreal shortVerticalRectangleOffset = halfPixmapSize - (rectangleThickness / 2.0);
    const qreal midSizeVerticalRectangleOffset = pixmapSize - minInset2 - rectangleThickness;
    
    const QRectF longHorizontalRectangle(QPointF(0.0, 0.0),
                                         QPointF(longRectangleLength, rectangleThickness));
    const QRectF midSizeHorizontalRectangle(QPointF(0.0, 0.0),
                                          QPointF(midSizeRectangleLength, rectangleThickness));
    const QRectF shortHorizontalRectangle(QPointF(0.0, 0.0),
                                          QPointF(shortRectangleLength, rectangleThickness));
    
    const qreal shortRectangleSpace = pixmapSize - shortRectangleLength;
    const qreal midSizeRectangleSpace = pixmapSize - midSizeRectangleLength;
    
    QBrush foregroundBrush = widget->palette().brush(widget->foregroundRole());
    QPen foregroundPen = painter->pen();
    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
    
    switch (distribute) {
        case AnnotationDistributeEnum::HORIZONTALLY:
            drawLine(painter, horizontalLine, 0.0, maxInset);
            drawLine(painter, horizontalLine, 0.0, minInset);
            drawRect(painter, foregroundColor, longVerticalRectangle, minInset2, minInset3);
            drawRect(painter, foregroundColor, shortVerticalRectangle, shortVerticalRectangleOffset, (shortRectangleSpace / 2.0));
            drawRect(painter, foregroundColor, midSizeVerticalRectangle, midSizeVerticalRectangleOffset, (midSizeRectangleSpace / 2.0));
            break;
        case AnnotationDistributeEnum::VERTICALLY:
            drawLine(painter, verticalLine, minInset, 0.0);
            drawLine(painter, verticalLine, maxInset, 0.0);
            drawRect(painter, foregroundColor, longHorizontalRectangle, minInset3, minInset2);
            drawRect(painter, foregroundColor, shortHorizontalRectangle, (shortRectangleSpace / 2.0), shortVerticalRectangleOffset);
            drawRect(painter, foregroundColor, midSizeHorizontalRectangle, (midSizeRectangleSpace / 2.0), midSizeVerticalRectangleOffset);
            break;
    }
    
    return pixmap;
}

/**
 * Draw a line at the given X,Y using the painter.
 *
 * @param painter
 *     The painter.
 * @param line
 *     The line
 * @param x
 *     Translate to X.
 * @param y
 *     Translate to Y.
 */
void
AnnotationMenuArrange::drawLine(QSharedPointer<QPainter>& painter,
                                const QLineF& line,
                                const qreal x,
                                const qreal y)
{
    painter->save();
    painter->translate(x, y);
    painter->drawLine(line);
    painter->restore();
}

/**
 * Draw a filled rectangle at the given X,Y using the painter and in the given color.
 *
 * @param painter
 *     The painter.
 * @param color
 *     Color of filled rectangle.
 * @param rectangle
 *     The rectangle
 * @param x
 *     Translate to X.
 * @param y
 *     Translate to Y.
 */
void
AnnotationMenuArrange::drawRect(QSharedPointer<QPainter>& painter,
                                const QColor& color,
                                const QRectF& rectangle,
                                const qreal x,
                                const qreal y)
{
    painter->save();
    painter->translate(x, y);
    painter->fillRect(rectangle,
                      color);
    painter->restore();
}

///**
// * Create an alignment pixmap.
// *
// * @param widget
// *    To color the pixmap with backround and foreground,
// *    the palette from the given widget is used.
// * @param horizontalAlignment
// *    The horizontal alignment.
// * @return
// *    Pixmap with icon for the given horizontal alignment.
// */
//QPixmap
//AnnotationMenuArrange::createAlignmentPixmap(const QWidget* widget,
//                                                     const AnnotationAlignmentEnum::Enum alignment)
//{
//    CaretAssert(widget);
//    
//    /*
//     * Create a small, square pixmap that will contain
//     * the foreground color around the pixmap's perimeter.
//     */
//    const float pixmapSize = 24.0;
//    
//    const float halfSize  = pixmapSize / 2.0;
//    
////    int32_t numLines = 5;
//    
////    if (m_smallLayoutFlag) {
////        width    = 12.0;
////        height   = 12.0;
////        numLines = 3;
////    }
//    QPixmap pixmap(static_cast<int>(pixmapSize),
//                   static_cast<int>(pixmapSize));
//    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
//                                                                                pixmap);
//    
//    const qreal margin          = pixmapSize * 0.05;
//    const qreal margin2 = margin * 2.0;
//    const qreal margin3 = margin * 3.0;
////    const qreal lineLength  = width - (margin * 2.0);
//    
//    /*
//     * Origin is at TOP !!!
//     */
//    const qreal xLeft   = margin;
//    const qreal xRight  = pixmapSize - margin;
//    const qreal yTop    = margin;
//    const qreal yBottom = pixmapSize - margin;
//    
//    const qreal lineStart = margin;
//    const qreal lineEnd   = pixmapSize - margin;
//    
//    const qreal verticalRectangleWidth = (pixmapSize * 0.25); //(width - (margin * 8.0)) / 2.0;
//    const qreal verticalRectangleHeight = pixmapSize - (margin * 6.0);
//    
//    QBrush foregroundBrush = widget->palette().brush(widget->foregroundRole());
//    QPen foregroundPen = painter->pen();
//    QColor foregroundColor = widget->palette().brush(widget->foregroundRole()).color();
//    
//    const qreal minInset = xLeft + margin2;
//    const qreal maxInset = xRight - margin2;
//    const qreal rectangleLongLength = (yBottom - margin3) - (yTop + margin);
//    
//    switch (alignment) {
//        case AnnotationAlignmentEnum::ALIGN_BOTTOM:
//            painter->drawLine(QLineF(lineStart, lineEnd,
//                                     lineEnd, lineEnd));
//////            painter->fillRect(QRectF(QPointF(minInset, yTop + margin),  // big rectangle
//////                                     QPointF((minInset + verticalRectangleWidth), yBottom - margin3)),
//////                              foregroundColor);
////            painter->fillRect(QRectF(QPointF(minInset, yTop + margin),  // big rectangle
////                                     QPointF((minInset + verticalRectangleWidth), yTop + margin + rectangleLongLength)),
////                              foregroundColor);
////            painter->fillRect(QRectF(QPointF(maxInset - verticalRectangleWidth, yTop + (pixmapSize * 0.3)),
////                                     QPointF(maxInset, yBottom - margin3)),   // small rectangle
////                              foregroundColor);
//            break;
//        case AnnotationAlignmentEnum::ALIGN_CENTER:
//            painter->drawLine(QLineF(halfSize, lineEnd,
//                                     halfSize, lineStart));
//            break;
//        case AnnotationAlignmentEnum::ALIGN_LEFT:
//            painter->drawLine(QLineF(lineStart, lineEnd,
//                                     lineStart, lineStart));
//            break;
//        case AnnotationAlignmentEnum::ALIGN_MIDDLE:
//            painter->drawLine(QLineF(lineStart, halfSize,
//                                     lineEnd, halfSize));
//            break;
//        case AnnotationAlignmentEnum::ALIGN_RIGHT:
//            painter->drawLine(QLineF(lineEnd, lineEnd,
//                                     lineEnd, lineStart));
//            break;
//        case AnnotationAlignmentEnum::ALIGN_TOP:
//            painter->drawLine(QLineF(lineStart, lineStart,
//                                     lineEnd, lineStart));
//            break;
//    }
//    
////    for (int32_t i = 1; i <= numLines; i++) {
////        const qreal lineLength = (((i % 2) == 0)
////                                  ? shortLineLength
////                                  : longLineLength);
////        const qreal y = yStep * i;
////        
////        qreal xStart = 0.0;
////        qreal xEnd   = width;
////        
////        switch (alignment) {
////            case AnnotationAlignmentEnum::ALIGN_BOTTOM:
////                break;
////            case AnnotationAlignmentEnum::ALIGN_CENTER:
////                break;
////            case AnnotationAlignmentEnum::ALIGN_LEFT:
////                break;
////            case AnnotationAlignmentEnum::ALIGN_MIDDLE:
////                break;
////            case AnnotationAlignmentEnum::ALIGN_RIGHT:
////                break;
////            case AnnotationAlignmentEnum::ALIGN_TOP:
////                break;
////        }
//////        switch (alignment) {
//////            case AnnotationTextAlignHorizontalEnum::CENTER:
//////                xStart = (width - lineLength) / 2.0;
//////                xEnd   = xStart + lineLength;
//////                break;
//////            case AnnotationTextAlignHorizontalEnum::LEFT:
//////                xStart = margin;
//////                xEnd   = xStart + lineLength;
//////                break;
//////            case AnnotationTextAlignHorizontalEnum::RIGHT:
//////                xEnd   = width - margin;
//////                xStart = xEnd - lineLength;
//////                break;
//////        }
////        
////        painter->drawLine(QLineF(xStart,
////                                 y,
////                                 xEnd,
////                                 y));
////    }
//    
//    return pixmap;
//}

