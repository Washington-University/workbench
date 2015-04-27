
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

#define __ANNOTATION_COLOR_WIDGET_DECLARE__
#include "AnnotationColorWidget.h"
#undef __ANNOTATION_COLOR_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationColorWidget 
 * \brief Widget for annotation color selection.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     Parent for this widget.
 */
AnnotationColorWidget::AnnotationColorWidget(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotation = NULL;
    
    QLabel* colorLabel = new QLabel("Color");
    
    const QSize toolButtonSize(24, 24);
    
    /*
     * Background color menu
     */
    m_backgroundColorMenu = new CaretColorEnumMenu();
    QObject::connect(m_backgroundColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(backgroundColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Background action and tool button
     */
    m_backgroundColorAction = new QAction("B",
                                          this);
    m_backgroundColorAction->setToolTip("Adjust the background color");
    m_backgroundColorAction->setMenu(m_backgroundColorMenu);
    QToolButton* backgroundToolButton = new QToolButton();
    backgroundToolButton->setDefaultAction(m_backgroundColorAction);
    backgroundToolButton->setIconSize(toolButtonSize);
    
    /*
     * Foreground color menu
     */
    m_foregroundColorMenu = new CaretColorEnumMenu();
    QObject::connect(m_foregroundColorMenu, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(foregroundColorSelected(const CaretColorEnum::Enum)));
    
    /*
     * Foreground color action and toolbutton
     */
    m_foregroundColorAction = new QAction("F",
                                          this);
    m_foregroundColorAction->setToolTip("Adjust the foreground color");
    m_foregroundColorAction->setMenu(m_foregroundColorMenu);
    m_foregroundToolButton = new QToolButton();
    m_foregroundToolButton->setDefaultAction(m_foregroundColorAction);
    m_foregroundToolButton->setIconSize(toolButtonSize);
    
    /*
     * Layout widgets
     */
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(colorLabel,
                      0, 0,
                      1, 2,
                      Qt::AlignHCenter);
    layout->addWidget(m_foregroundToolButton,
                      1, 0);
    layout->addWidget(backgroundToolButton,
                      1, 1);
    
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
    backgroundColorSelected(CaretColorEnum::WHITE);
    foregroundColorSelected(CaretColorEnum::BLACK);
}

/**
 * Destructor.
 */
AnnotationColorWidget::~AnnotationColorWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationColorWidget::updateContent(Annotation* annotation)
{
    m_annotation = annotation;
    
    if (m_annotation != NULL) {
    }
    else {
    }
}

/**
 * Gets called when the background color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationColorWidget::backgroundColorSelected(const CaretColorEnum::Enum caretColor)
{
    const float* rgb = CaretColorEnum::toRGB(caretColor);
    QPixmap pm(24, 24);
    pm.fill(QColor::fromRgbF(rgb[0],
                             rgb[1],
                             rgb[2]));
    QIcon icon(pm);
    
    m_backgroundColorAction->setIcon(icon);

    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
}

/**
 * Gets called when the foreground color is changed.
 *
 * @param caretColor
 *     Color that was selected.
 */
void
AnnotationColorWidget::foregroundColorSelected(const CaretColorEnum::Enum caretColor)
{
    /*
     * Get the toolbutton's background color
     */
    const QPalette tbPalette = m_foregroundToolButton->palette();
    const QPalette::ColorRole backgroundRole = m_foregroundToolButton->backgroundRole();
    const QBrush brush = tbPalette.brush(backgroundRole);
    const QColor toolButtonBackgroundColor = brush.color();

    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const int width  = 24;
    const int height = 24;
    QPixmap pm(width,
               height);
    
    /*
     * Create a painter and fill the pixmap with 
     * the background color
     */
    QPainter painter(&pm);
    painter.setBackgroundMode(Qt::OpaqueMode);
    painter.fillRect(pm.rect(), toolButtonBackgroundColor);

    /*
     * Draw lines (rectangle) around the perimeter of
     * the pixmap
     */
    const float* foregroundColor = CaretColorEnum::toRGB(caretColor);
    painter.setPen(QColor::fromRgbF(foregroundColor[0],
                                    foregroundColor[1],
                                    foregroundColor[2]));
//    painter.drawRect(0, 0, 9, 9);
//    painter.drawRect(1, 1, 7, 7);
//    painter.drawRect(0, 0, width - 1, height - 1);
    for (int32_t i = 0; i < 3; i++) {
        painter.drawRect(i, i,
                         width - 1 - i * 2, height - 1 - i * 2);
    }
    
    m_foregroundColorAction->setIcon(QIcon(pm));

    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());    
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationColorWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

