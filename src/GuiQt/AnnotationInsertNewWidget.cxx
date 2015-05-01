
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

#define __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__
#include "AnnotationInsertNewWidget.h"
#undef __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "EventAnnotation.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::AnnotationInsertNewWidget
 * \brief Widget for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     Parent of this widget.
 */
AnnotationInsertNewWidget::AnnotationInsertNewWidget(const int32_t browserWindowIndex,
                                                     QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* insertLabel = new QLabel("Insert");
    
    QWidget* textToolButton = createTextToolButton();
    QWidget* shapeToolButton = createShapeToolButton();
    
    WuQtUtilities::matchWidgetHeights(textToolButton,
                                      shapeToolButton);
    
    QHBoxLayout* toolButtonLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(toolButtonLayout, 2, 0);
    toolButtonLayout->addStretch();
    toolButtonLayout->addWidget(textToolButton);
    toolButtonLayout->addWidget(shapeToolButton);
    toolButtonLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(insertLabel, 0, Qt::AlignHCenter);
    layout->addLayout(toolButtonLayout);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationInsertNewWidget::~AnnotationInsertNewWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationInsertNewWidget::receiveEvent(Event* event)
{
    //    if (event->getEventType() == EventTypeEnum::) {
    //        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
    //        CaretAssert(eventName);
    //
    //        event->setEventProcessed();
    //    }
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationInsertNewWidget::updateContent(Annotation* annotation)
{
    
    //    if (m_annotation != NULL) {
    //        m_coordinateSpaceComboBox->setSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(m_annotation->getCoordinateSpace());
    //        m_typeComboBox->setSelectedItem<AnnotationTypeEnum, AnnotationTypeEnum::Enum>(m_annotation->getType());
    //    }
}

/**
 * Gets called when the coordinate space is changed.
 */
void
AnnotationInsertNewWidget::coordinateSpaceEnumChanged()
{
    //    if (m_annotation != NULL) {
    //        m_annotation->setCoordinateSpace(m_coordinateSpaceComboBox->getSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>());
    //    }
}

/**
 * @return Create the text tool button.
 */
QWidget*
AnnotationInsertNewWidget::createTextToolButton()
{
    m_textToolButtonAction = WuQtUtilities::createAction("A",
                                                         "Create a text annotation",
                                                         this,
                                                         this,
                                                         SLOT(textActionTriggered()));
    
    QToolButton* toolButton = new QToolButton();
    toolButton->setDefaultAction(m_textToolButtonAction);
    return toolButton;
}

/**
 * @return Create the shape tool button.
 */
QWidget*
AnnotationInsertNewWidget::createShapeToolButton()
{
    QMenu* shapeMenu = new QMenu();
    QObject::connect(shapeMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(shapeMenuActionTriggered(QAction*)));
    
    std::vector<AnnotationTypeEnum::Enum> allTypes;
    AnnotationTypeEnum::getAllEnums(allTypes);
    
    QAction* boxAction = NULL;
    for (std::vector<AnnotationTypeEnum::Enum>::iterator iter = allTypes.begin();
         iter != allTypes.end();
         iter++) {
        const AnnotationTypeEnum::Enum annType = *iter;
        
        bool useTypeFlag = false;
        switch (annType) {
            case AnnotationTypeEnum::ARROW:
                useTypeFlag = true;
                break;
            case AnnotationTypeEnum::BOX:
                useTypeFlag = true;
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                useTypeFlag = true;
                break;
            case AnnotationTypeEnum::OVAL:
                useTypeFlag = true;
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
        
        if (useTypeFlag) {
            QAction* typeAction = shapeMenu->addAction(AnnotationTypeEnum::toGuiName(annType));
            typeAction->setIcon(createShapePixmap(shapeMenu,
                                                  annType));
            typeAction->setData((int)AnnotationTypeEnum::toIntegerCode(annType));
            
            if (annType == AnnotationTypeEnum::BOX) {
                boxAction = typeAction;
            }
        }
    }
    
    m_shapeToolButtonAction = WuQtUtilities::createAction("Shape",
                                                          "Create the selected shape annotation\n"
                                                          "Click right side fo button to change shape.",
                                                          this,
                                                          this,
                                                          SLOT(shapeActionTriggered()));
    m_shapeToolButtonAction->setData((int)-1);
    m_shapeToolButtonAction->setMenu(shapeMenu);
    
    QToolButton* toolButton = new QToolButton();
    toolButton->setDefaultAction(m_shapeToolButtonAction);

    /*
     * Initialize the shape tool button action to the BOX action.
     */
    CaretAssert(boxAction);
    m_shapeToolButtonAction->blockSignals(true);
    m_shapeToolButtonAction->setIcon(boxAction->icon());
    m_shapeToolButtonAction->setData(boxAction->data());
    m_shapeToolButtonAction->setText("");
    m_shapeToolButtonAction->blockSignals(false);
    
    return toolButton;
}

/**
 * Create an annotation with the given type.
 *
 * @param annotationType
 *    Type of annotation.
 */
void
AnnotationInsertNewWidget::createAnnotationWithType(const AnnotationTypeEnum::Enum annotationType)
{
    std::cout << "Create Shape Type: " << qPrintable(AnnotationTypeEnum::toName(annotationType)) << std::endl;
    EventManager::get()->sendEvent(EventAnnotation().setModeCreateNewAnnotationType(annotationType).getPointer());
}

/**
 * Gets called when the text tool button's action is triggered.
 *
 * @param action
 *     The selected action.
 */
void
AnnotationInsertNewWidget::textActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::TEXT);
}

/**
 * Gets called when the shape tool button's action is triggered.
 *
 * @param action
 *     The selected action.
 */
void
AnnotationInsertNewWidget::shapeActionTriggered()
{
    const int32_t shapeID = m_shapeToolButtonAction->data().toInt();
    if (shapeID >= 0) {
        const AnnotationTypeEnum::Enum annType = AnnotationTypeEnum::fromIntegerCode(shapeID, NULL);
        createAnnotationWithType(annType);
    }
}

/**
 * Gets called when an item is selected from the shape tool
 * button's menu.
 *
 * @param action
 *     The selected action.
 */
void
AnnotationInsertNewWidget::shapeMenuActionTriggered(QAction* action)
{
    CaretAssert(action);
    
    const int32_t typeID = action->data().toInt();
    const AnnotationTypeEnum::Enum annType = AnnotationTypeEnum::fromIntegerCode(typeID, NULL);
    
    m_shapeToolButtonAction->setIcon(action->icon());
    m_shapeToolButtonAction->setData(action->data());
    m_shapeToolButtonAction->setText("");
    
    createAnnotationWithType(annType);
}

/**
 * Create a pixmap for the given annotation shape type.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param annotationType
 *    The annotation type.
 * @return
 *    Pixmap with icon for the given annotation type.
 */
QPixmap
AnnotationInsertNewWidget::createShapePixmap(const QWidget* widget,
                                             const AnnotationTypeEnum::Enum annotationType)
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
    const float width  = 16.0;
    const float height = 16.0;
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
    
    /**
     * NOTE: ORIGIN is in TOP LEFT corner of pixmap.
     */
    switch (annotationType) {
        case AnnotationTypeEnum::ARROW:
            painter.drawLine(1, height - 1, width - 1, 1);
            painter.drawLine(width - 1, 1, width - (width / 3), 1);
            painter.drawLine(width - 1, 1, width - 1, 1 + (height / 3));
            break;
        case AnnotationTypeEnum::BOX:
            painter.drawRect(1, 1, width - 2, height - 2);
            break;
        case AnnotationTypeEnum::IMAGE:
            CaretAssertMessage(0, "IMAGE is not a shape.");
            break;
        case AnnotationTypeEnum::LINE:
            painter.drawLine(1, height - 1, width - 1, 1);
            break;
        case AnnotationTypeEnum::OVAL:
            painter.drawEllipse(1, 1, width - 1, height - 1);
            break;
        case AnnotationTypeEnum::TEXT:
            CaretAssertMessage(0, "TEXT is not a shape.");
            break;
    }
    
    return pixmap;
}

