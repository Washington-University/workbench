
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

#define __ANNOTATION_LINE_ARROW_TIPS_WIDGET_DECLARE__
#include "AnnotationLineArrowTipsWidget.h"
#undef __ANNOTATION_LINE_ARROW_TIPS_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QToolButton>

#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationLineArrowTipsWidget 
 * \brief Widget for enabling/disabling line arrow tips
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationLineArrowTipsWidget::AnnotationLineArrowTipsWidget(const int32_t browserWindowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* label = new QLabel("Line");
    
    const QSize toolButtonSize(24, 24); //(18, 18);
    
    
    
    QToolButton* endArrowToolButton = new QToolButton();
    m_endArrowAction = new QAction(this);
    m_endArrowAction->setCheckable(true);
    m_endArrowAction->setToolTip("Show arrow at line's end coordinate");
    m_endArrowAction->setIcon(QIcon(createArrowPixmap(endArrowToolButton, ArrowType::DOWN)));
    QObject::connect(m_endArrowAction, &QAction::triggered,
                     this, &AnnotationLineArrowTipsWidget::endArrowTipActionToggled);
                     //this, [=] { this->endArrowTipActionToggled(); });
    endArrowToolButton->setDefaultAction(m_endArrowAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(endArrowToolButton);
    
    
    QToolButton* startArrowToolButton = new QToolButton();
    m_startArrowAction = new QAction(this);
    m_startArrowAction->setCheckable(true);
    m_startArrowAction->setToolTip("Show arrow at line's start coordinate");
    m_startArrowAction->setIcon(QIcon(createArrowPixmap(startArrowToolButton, ArrowType::UP)));
    QObject::connect(m_startArrowAction, &QAction::triggered,
                     this, &AnnotationLineArrowTipsWidget::startArrowTipActionToggled);
                     //this, [=] { this->startArrowTipActionToggled(); });
    startArrowToolButton->setDefaultAction(m_startArrowAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(startArrowToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    gridLayout->addWidget(label,
                          0, 0, Qt::AlignHCenter);
    gridLayout->addWidget(startArrowToolButton,
                          1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(endArrowToolButton,
                          2, 0, Qt::AlignHCenter);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationLineArrowTipsWidget::~AnnotationLineArrowTipsWidget()
{
}

/**
 * Update with the given line annotation.
 *
 * @param annotationLines
 */
void
AnnotationLineArrowTipsWidget::updateContent(std::vector<AnnotationLine*>& annotationLines)
{
    m_annotations.clear();
    m_annotations.insert(m_annotations.end(),
                         annotationLines.begin(),
                         annotationLines.end());
    
    AnnotationLine* line = NULL;
    if ( ! annotationLines.empty()) {
        line = annotationLines[0];
    }

    bool allStartOnFlag = true;
    bool allEndOnFlag   = true;
    
    const int32_t numLines = static_cast<int32_t>(annotationLines.size());
    for (int32_t i = 0; i < numLines; i++) {
        CaretAssertVectorIndex(annotationLines, i);
        if ( ! annotationLines[i]->isDisplayStartArrow()) {
            allStartOnFlag = false;
        }
        if ( ! annotationLines[i]->isDisplayEndArrow()) {
            allEndOnFlag = false;
        }
    }

    if (numLines <= 0) {
        allStartOnFlag = false;
        allEndOnFlag   = false;
    }
    
    m_startArrowAction->setChecked(allStartOnFlag);
    m_endArrowAction->setChecked(allEndOnFlag);
    
    if (numLines > 0) {
        setEnabled(true);
        
        AnnotationLine::setUserDefaultDisplayStartArrow(m_startArrowAction->isChecked());
        AnnotationLine::setUserDefaultDisplayEndArrow(m_endArrowAction->isChecked());
    }
    else {
        setEnabled(false);
    }
}


/**
 * Gets called when the line arrow start buttons is toggled.
 */
void
AnnotationLineArrowTipsWidget::startArrowTipActionToggled()
{
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowStart(m_startArrowAction->isChecked(),
                                           m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
        AnnotationLine::setUserDefaultDisplayStartArrow(m_startArrowAction->isChecked());
}

/**
 * Gets called when the line arrow end buttons is toggled.
 */
void
AnnotationLineArrowTipsWidget::endArrowTipActionToggled()
{
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeLineArrowEnd(m_endArrowAction->isChecked(),
                                         m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();

    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
        AnnotationLine::setUserDefaultDisplayEndArrow(m_endArrowAction->isChecked());
}

/**
 * Create a pixmap for the given arrow type type.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param arrowType
 *    The arrow type.
 * @return
 *    Pixmap with icon for the given arrow type.
 */
QPixmap
AnnotationLineArrowTipsWidget::createArrowPixmap(const QWidget* widget,
                          const ArrowType arrowType)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget, pixmap);
    
    const bool fillShapeFlag = false;
    if (fillShapeFlag) {
        QBrush brush = painter->brush();
        brush.setColor(painter->pen().color());
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);
    }
    
    const float percentage = 0.10f;
    const float left   = width  * percentage;
    const float right  = width  * (1.0 - percentage);
    const float bottom = height * percentage;
    const float top    = height * (1.0 - percentage);
    const float centerX = width * 0.5;
    QPolygonF triangle;
    switch (arrowType) {
        case ArrowType::DOWN:
            triangle.push_back(QPointF(right, top));
            triangle.push_back(QPointF(left, top));
            triangle.push_back(QPointF(centerX, bottom));
            break;
        case ArrowType::UP:
            triangle.push_back(QPointF(left, bottom));
            triangle.push_back(QPointF(right, bottom));
            triangle.push_back(QPointF(centerX, top));
            break;
    }
    painter->drawPolygon(triangle);
    
    return pixmap;
    
//    switch (annotationType) {
//        case AnnotationTypeEnum::BOX:
//            painter->drawRect(1, 1, width - 2, height - 2);
//            break;
//        case AnnotationTypeEnum::COLOR_BAR:
//            CaretAssertMessage(0, "No pixmap for colorbar as user does not create them like other annotations");
//            break;
//        case AnnotationTypeEnum::IMAGE:
//        {
//            const int blueAsGray = qGray(25,25,255);
//            QColor skyColor(blueAsGray, blueAsGray, blueAsGray);
//            
//            /*
//             * Background (sky)
//             */
//            painter->fillRect(pixmap.rect(), skyColor);
//            
//            const int greenAsGray = qGray(0, 255, 0);
//            QColor terrainColor(greenAsGray, greenAsGray, greenAsGray);
//            
//            /*
//             * Terrain
//             */
//            painter->setBrush(terrainColor);
//            painter->setPen(terrainColor);
//            const int w14 = width * 0.25;
//            const int h23 = height * 0.667;
//            const int h34 = height * 0.75;
//            QPolygon terrain;
//            terrain.push_back(QPoint(1, height - 1));
//            terrain.push_back(QPoint(width - 1, height - 1));
//            terrain.push_back(QPoint(width - 1, h23));
//            terrain.push_back(QPoint(w14 * 3, h34));
//            terrain.push_back(QPoint(w14 * 2, h23));
//            terrain.push_back(QPoint(w14, h34));
//            terrain.push_back(QPoint(1, h23));
//            terrain.push_back(QPoint(1, height - 1));
//            painter->drawPolygon(terrain);
//            
//            const int yellowAsGray = qGray(255, 255, 0);
//            QColor sunColor(yellowAsGray, yellowAsGray, yellowAsGray);
//            
//            /*
//             * Sun
//             */
//            painter->setBrush(sunColor);
//            painter->setPen(sunColor);
//            const int radius = width * 0.25;
//            painter->drawEllipse(width * 0.33, height * 0.33, radius, radius);
//        }
//            break;
//        case AnnotationTypeEnum::LINE:
//            painter->drawLine(1, height - 1, width - 1, 1);
//            break;
//        case AnnotationTypeEnum::OVAL:
//            painter->drawEllipse(1, 1, width - 1, height - 1);
//            break;
//        case AnnotationTypeEnum::TEXT:
//        {
//            QFont font = painter->font();
//            font.setPixelSize(20);
//            painter->setFont(font);
//            painter->drawText(pixmap.rect(),
//                              (Qt::AlignCenter),
//                              "A");
//        }
//            break;
//    }
//    
//    return pixmap;
//    
}


