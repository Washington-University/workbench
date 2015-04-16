
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

#include <cmath>

#define __ANNOTATION_TEXT_ALIGNMENT_WIDGET_DECLARE__
#include "AnnotationTextAlignmentWidget.h"
#undef __ANNOTATION_TEXT_ALIGNMENT_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QToolButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;


/**
 * \class caret::AnnotationTextAlignmentWidget 
 * \brief Widget for adjusting annotation alignment.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param parent
 *     The parent widget.
 */
AnnotationTextAlignmentWidget::AnnotationTextAlignmentWidget(QWidget* parent)
: QWidget(parent)
{
    m_annotationText = NULL;
    
    QLabel* horizontalLabel = new QLabel("Text Horizontal");
    QToolButton* leftAlignToolButton   = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::LEFT);
    QToolButton* centerAlignToolButton = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::CENTER);
    QToolButton* rightAlignToolButton  = createHorizontalAlignmentToolButton(AnnotationTextAlignHorizontalEnum::RIGHT);
    
    m_horizontalAlignActionGroup = new QActionGroup(this);
    m_horizontalAlignActionGroup->setExclusive(true);
    m_horizontalAlignActionGroup->addAction(leftAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(centerAlignToolButton->defaultAction());
    m_horizontalAlignActionGroup->addAction(rightAlignToolButton->defaultAction());
    QObject::connect(m_horizontalAlignActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(horizontalAlignmentActionSelected(QAction*)));
    
    
    QHBoxLayout* horizontalAlignLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(horizontalAlignLayout, 2, 0);
    horizontalAlignLayout->addWidget(leftAlignToolButton);
    horizontalAlignLayout->addWidget(centerAlignToolButton);
    horizontalAlignLayout->addWidget(rightAlignToolButton);
    
    QLabel* verticalLabel = new QLabel("Text Vertical");
    QToolButton* topAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::TOP);
    QToolButton* middleAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::MIDDLE);
    QToolButton* bottomAlignToolButton = createVerticalAlignmentToolButton(AnnotationTextAlignVerticalEnum::BOTTOM);
    
    m_verticalAlignActionGroup = new QActionGroup(this);
    m_verticalAlignActionGroup->setExclusive(true);
    m_verticalAlignActionGroup->addAction(topAlignToolButton->defaultAction());
    m_verticalAlignActionGroup->addAction(middleAlignToolButton->defaultAction());
    m_verticalAlignActionGroup->addAction(bottomAlignToolButton->defaultAction());
    QObject::connect(m_verticalAlignActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(verticalAlignmentActionSelected(QAction*)));
    
    QHBoxLayout* verticalAlignLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(verticalAlignLayout, 2, 0);
    verticalAlignLayout->addWidget(topAlignToolButton);
    verticalAlignLayout->addWidget(middleAlignToolButton);
    verticalAlignLayout->addWidget(bottomAlignToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(horizontalLabel,
                          0, 0,
                          Qt::AlignHCenter);
    gridLayout->addLayout(horizontalAlignLayout,
                          1, 0);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, 1,
                          2, 1);
    gridLayout->addWidget(verticalLabel,
                          0, 2,
                          Qt::AlignHCenter);
    gridLayout->addLayout(verticalAlignLayout,
                          1, 2);
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationTextAlignmentWidget::~AnnotationTextAlignmentWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationTextAlignmentWidget::updateContent(AnnotationText* annotationText)
{
    m_annotationText = annotationText;
    
    if (m_annotationText != NULL) {
    }
    else {
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationTextAlignmentWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Gets called when a horizontal alignment selection is made.
 * 
 * @param action
 *     Action that was selected.
 */
void
AnnotationTextAlignmentWidget::horizontalAlignmentActionSelected(QAction* action)
{
    CaretAssert(action);
    const AnnotationTextAlignHorizontalEnum::Enum align = static_cast<AnnotationTextAlignHorizontalEnum::Enum>(action->data().toInt());
}

/**
 * Gets called when a vertical alignment selection is made.
 *
 * @param action
 *     Action that was selected.
 */
void
AnnotationTextAlignmentWidget::verticalAlignmentActionSelected(QAction* action)
{
    CaretAssert(action);
    const AnnotationTextAlignVerticalEnum::Enum align = static_cast<AnnotationTextAlignVerticalEnum::Enum>(action->data().toInt());
}

/**
 * Create a tool button for the given horizontal alignment.
 * The tool button will contain an action with the appropriate
 * icon and tooltip.
 *
 * @param horizontalAlignment
 *     The horizontal alignment.
 */
QToolButton*
AnnotationTextAlignmentWidget::createHorizontalAlignmentToolButton(const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment)
{
    QString toolTipText;
    switch (horizontalAlignment) {
        case AnnotationTextAlignHorizontalEnum::CENTER:
            toolTipText = "Align Center";
            break;
        case AnnotationTextAlignHorizontalEnum::LEFT:
            toolTipText = "Align Left";
            break;
        case AnnotationTextAlignHorizontalEnum::RIGHT:
            toolTipText = "Align Right";
            break;
    }
    
    QToolButton* toolButton = new QToolButton();
    QPixmap pixmap = createHorizontalAlignmentPixmap(toolButton,
                                                     horizontalAlignment);
    
    QAction* action = new QAction(this);
    action->setCheckable(true);
    action->setData((int)horizontalAlignment);
    action->setToolTip(toolTipText);
    action->setIcon(QIcon(pixmap));
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(pixmap.size());
    
    return toolButton;
}

/**
 * Create a tool button for the given vertical alignment.
 * The tool button will contain an action with the appropriate
 * icon and tooltip.
 *
 * @param verticalAlignment
 *     The vertical alignment.
 */
QToolButton*
AnnotationTextAlignmentWidget::createVerticalAlignmentToolButton(const AnnotationTextAlignVerticalEnum::Enum verticalAlignment)
{
    QString toolTipText;
    switch (verticalAlignment) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            toolTipText = "Align Bottom";
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            toolTipText = "Align Middle";
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            toolTipText = "Align Top";
            break;
    }
    
    QToolButton* toolButton = new QToolButton();
    QPixmap pixmap = createVerticalAlignmentPixmap(toolButton,
                                                   verticalAlignment);
    
    QAction* action = new QAction(this);
    action->setCheckable(true);
    action->setData((int)verticalAlignment);
    action->setToolTip(toolTipText);
    action->setIcon(QIcon(pixmap));
    toolButton->setDefaultAction(action);
    toolButton->setIconSize(pixmap.size());
    
    return toolButton;
}


/**
 * Create a horizontal alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground, 
 *    the palette from the given widget is used.
 * @param horizontalAlignment
 *    The horizontal alignment.
 * @return
 *    Pixmap with icon for the given horizontal alignment.
 */
QPixmap
AnnotationTextAlignmentWidget::createHorizontalAlignmentPixmap(const QWidget* widget,
                                                     const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment)
{
    CaretAssert(widget);
    
    /*
     * Get the widget's background and foreground color
     */
    const QPalette palette = widget->palette();
    const QPalette::ColorRole backgroundRole = widget->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QPalette::ColorRole foregroundRole = widget->foregroundRole();
    const QBrush foregroundBrush = palette.brush(foregroundRole);
    const QColor foregroundColor = foregroundBrush.color();
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
              static_cast<int>(height));
    
    /*
     * Create a painter and fill the pixmap with
     * the background color
     */
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,
                          true);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.fillRect(pixmap.rect(), backgroundColor);
    
    /*
     * Draw lines (rectangle) around the perimeter of
     * the pixmap
     */
    painter.setPen(foregroundColor);
    
    const qreal margin          = width * 0.05;
    const qreal longLineLength  = width - (margin * 2.0);
    const qreal shortLineLength = width / 2.0;
    const qreal yStep = std::round(height / 6.0);
    
    for (int32_t i = 1; i <= 5; i++) {
        const qreal lineLength = (((i % 2) == 0)
                                  ? shortLineLength
                                  : longLineLength);
        const qreal y = yStep * i;
        
        qreal xStart = 0.0;
        qreal xEnd   = width;
        
        switch (horizontalAlignment) {
            case AnnotationTextAlignHorizontalEnum::CENTER:
                xStart = (width - lineLength) / 2.0;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::LEFT:
                xStart = margin;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::RIGHT:
                xEnd   = width - margin;
                xStart = xEnd - lineLength;
                break;
        }

        painter.drawLine(QLineF(xStart,
                                y,
                                xEnd,
                                y));
    }
    
    return pixmap;
}

/**
 * Create a vertical alignment pixmap.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param verticalAlignment
 *    The vertical alignment.
 * @return
 *    Pixmap with icon for the given vertical alignment.
 */
QPixmap
AnnotationTextAlignmentWidget::createVerticalAlignmentPixmap(const QWidget* widget,
                                                         const AnnotationTextAlignVerticalEnum::Enum verticalAlignment)
{
    CaretAssert(widget);
    
    /*
     * Get the widget's background and foreground color
     */
    const QPalette palette = widget->palette();
    const QPalette::ColorRole backgroundRole = widget->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QPalette::ColorRole foregroundRole = widget->foregroundRole();
    const QBrush foregroundBrush = palette.brush(foregroundRole);
    const QColor foregroundColor = foregroundBrush.color();
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    
    /*
     * Create a painter and fill the pixmap with
     * the background color
     */
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,
                          true);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.fillRect(pixmap.rect(), backgroundColor);
    
    /*
     * Draw lines (rectangle) around the perimeter of
     * the pixmap
     */
    painter.setPen(foregroundColor);
    
    const qreal margin          = width * 0.05;
    const qreal longLineLength  = width - (margin * 2.0);
    const qreal shortLineLength = width / 2.0;
    const qreal yStep = std::round(height / 6.0);
    
    for (int32_t i = 1; i <= 3; i++) {
        const qreal lineLength = (((i % 2) == 0)
                                  ? shortLineLength
                                  : longLineLength);
        
        int32_t iOffset = i;
        switch (verticalAlignment) {
            case AnnotationTextAlignVerticalEnum::BOTTOM:
                iOffset += 2;
                break;
            case AnnotationTextAlignVerticalEnum::MIDDLE:
                iOffset += 1;
                break;
            case AnnotationTextAlignVerticalEnum::TOP:
                break;
        }
        const qreal y = yStep * iOffset;
        
        const qreal xStart = margin;
        const qreal xEnd   = xStart + lineLength;

        
        painter.drawLine(QLineF(xStart,
                                y,
                                xEnd,
                                y));
    }
    return pixmap;
}

