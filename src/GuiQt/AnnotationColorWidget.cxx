
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
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretColorEnumMenu.h"
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
AnnotationColorWidget::AnnotationColorWidget(QWidget* parent)
: QWidget(parent)
{
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
    
    /*
     * Layout widgets
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(m_foregroundToolButton);
    layout->addWidget(backgroundToolButton);
    
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationColorWidget::~AnnotationColorWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
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
    std::cout << "Background: " << qPrintable(CaretColorEnum::toGuiName(caretColor)) << std::endl;
    const float* rgb = CaretColorEnum::toRGB(caretColor);
    QPixmap pm(10, 10);
    pm.fill(QColor::fromRgbF(rgb[0],
                             rgb[1],
                             rgb[2]));
    QIcon icon(pm);
    
    m_backgroundColorAction->setIcon(icon);
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
    std::cout << "Foreground: " << qPrintable(CaretColorEnum::toGuiName(caretColor)) << std::endl;
  
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
    QPixmap pm(10, 10);
    
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
    painter.drawRect(0, 0, 9, 9);
    painter.drawRect(1, 1, 7, 7);
    
    m_foregroundColorAction->setIcon(QIcon(pm));
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

