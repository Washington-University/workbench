
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
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "Brain.h"
#include "EventAnnotationCreateNewType.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
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
    QLabel* deleteLabel = new QLabel("Delete");
    
    QWidget* shapeBoxToolButton   = createShapeToolButton(AnnotationTypeEnum::BOX);
    QWidget* shapeImageToolButton = createShapeToolButton(AnnotationTypeEnum::IMAGE);
    QWidget* shapeLineToolButton  = createShapeToolButton(AnnotationTypeEnum::LINE);
    QWidget* shapeOvalToolButton  = createShapeToolButton(AnnotationTypeEnum::OVAL);
    QWidget* shapeTextToolButton  = createShapeToolButton(AnnotationTypeEnum::TEXT);
    
    /*
     * Disable IMAGE button
     */
    shapeImageToolButton->setEnabled(false);
    
    m_deleteToolButton = createDeleteToolButton();
    
    QSpacerItem* spaceItem = new QSpacerItem(5, 10,
                                             QSizePolicy::Fixed,
                                             QSizePolicy::Fixed);
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    gridLayout->addWidget(insertLabel,
                          0, 0, 1, 3, Qt::AlignHCenter);
    gridLayout->addWidget(shapeBoxToolButton,
                          1, 0);
    gridLayout->addWidget(shapeImageToolButton,
                          1, 1);
    gridLayout->addWidget(shapeLineToolButton,
                          1, 2);
    gridLayout->addWidget(shapeOvalToolButton,
                          2, 0);
    gridLayout->addWidget(shapeTextToolButton,
                          2, 1);
    
    gridLayout->addItem(spaceItem,
                        0, 3);
    
    gridLayout->addWidget(deleteLabel,
                          0, 4, Qt::AlignHCenter);
    gridLayout->addWidget(m_deleteToolButton,
                          1, 4, 2, 1, (Qt::AlignHCenter
                                       | Qt::AlignTop));
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationInsertNewWidget::~AnnotationInsertNewWidget()
{
}

/**
 * Update the content.
 */
void
AnnotationInsertNewWidget::updateContent()
{
}

/**
 * @return Create the shape tool button for the given annotation type.
 * 
 * @param annotationType
 *     The annotation type.
 */
QWidget*
AnnotationInsertNewWidget::createShapeToolButton(const AnnotationTypeEnum::Enum annotationType)
{
    const QString typeGuiName = AnnotationTypeEnum::toGuiName(annotationType);
    QToolButton* toolButton = new QToolButton();
    QAction* action = new QAction(createShapePixmap(toolButton,
                                                    annotationType),
                                  typeGuiName,
                                  this);
    action->setToolTip("Create a "
                       + typeGuiName
                       + " annotation");

    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            QObject::connect(action, SIGNAL(triggered(bool)),
                             this, SLOT(shapeBoxActionTriggered()));
            break;
        case AnnotationTypeEnum::IMAGE:
            QObject::connect(action, SIGNAL(triggered(bool)),
                             this, SLOT(shapeImageActionTriggered()));
            break;
        case AnnotationTypeEnum::LINE:
            QObject::connect(action, SIGNAL(triggered(bool)),
                             this, SLOT(shapeLineActionTriggered()));
            break;
        case AnnotationTypeEnum::OVAL:
            QObject::connect(action, SIGNAL(triggered(bool)),
                             this, SLOT(shapeOvalActionTriggered()));
            break;
        case AnnotationTypeEnum::TEXT:
            QObject::connect(action, SIGNAL(triggered(bool)),
                             this, SLOT(shapeTextActionTriggered()));
            break;
    }
    
    toolButton->setDefaultAction(action);
    
    return toolButton;
}

/**
 * @return The delete tool button.
 */
QToolButton*
AnnotationInsertNewWidget::createDeleteToolButton()
{
    m_deleteToolButtonAction = WuQtUtilities::createAction("",
                                                         ("Delete the selected annotation\n"
                                                          "\n"
                                                          "Pressing the Delete key while an annotation\n"
                                                          "is selected will also delete an annotation\n"
                                                          "\n"
                                                          "Pressing the arrow will show a menu for\n"
                                                          "undeleting annotations"),
                                                         this,
                                                         this,
                                                         SLOT(deleteActionTriggered()));
    QToolButton* toolButton = new QToolButton();

    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(toolButton,
                                                                                pixmap);
    
    
    /* trash can */
    painter->drawLine(4, 6, 4, 22);
    painter->drawLine(4, 22, 20, 22);
    painter->drawLine(20, 22, 20, 6);
    
    /* trash can lines */
    painter->drawLine(12, 8, 12, 20);
    painter->drawLine(8,  8,  8, 20);
    painter->drawLine(16, 8, 16, 20);
    
    /* trash can lid and handle */
    painter->drawLine(2, 6, 22, 6);
    painter->drawLine(8, 6, 8, 2);
    painter->drawLine(8, 2, 16, 2);
    painter->drawLine(16, 2, 16, 6);
    

    m_deleteToolButtonAction->setIcon(QIcon(pixmap));

    toolButton->setIconSize(pixmap.size());
    toolButton->setDefaultAction(m_deleteToolButtonAction);
    
    return toolButton;
}

/**
 * Gets called when the delete action is triggered.
 */
void
AnnotationInsertNewWidget::deleteActionTriggered()
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    std::vector<Annotation*> selectedAnnotations = annotationManager->getSelectedAnnotations();
    if ( ! selectedAnnotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeDeleteAnnotations(selectedAnnotations);
        annotationManager->applyCommand(undoCommand);
        
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
//    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
//    std::vector<Annotation*> selectedAnnotations = annotationManager->getSelectedAnnotations();
//    if ( ! selectedAnnotations.empty()) {
//            /*
//             * Delete all selected annotations and update graphics and UI.
//             */
//            AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
//            annotationManager->deleteSelectedAnnotations();
//            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//
//        {
//        }
//    }
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
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(annotationType).getPointer());
}

/**
 * Gets called when box action is triggered.
 */
void
AnnotationInsertNewWidget::shapeBoxActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::BOX);
}

/**
 * Gets called when line action is triggered.
 */
void
AnnotationInsertNewWidget::shapeLineActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::LINE);
}

/**
 * Gets called when image action is triggered.
 */
void
AnnotationInsertNewWidget::shapeImageActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::IMAGE);
}

/**
 * Gets called when text action is triggered.
 */
void
AnnotationInsertNewWidget::shapeTextActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::TEXT);
}

/**
 * Gets called when oval action is triggered.
 */
void
AnnotationInsertNewWidget::shapeOvalActionTriggered()
{
    createAnnotationWithType(AnnotationTypeEnum::OVAL);
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
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    /**
     * NOTE: ORIGIN is in TOP LEFT corner of pixmap.
     */
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            painter->drawRect(1, 1, width - 2, height - 2);
            break;
        case AnnotationTypeEnum::IMAGE:
        {
            /*
             * Background (sky)
             */
            painter->fillRect(pixmap.rect(), QColor(25,25,255));
            
            /*
             * Terrain
             */
            painter->setBrush(QColor(0, 255, 0));
            painter->setPen(QColor(0, 255, 0));
            const int w14 = width * 0.25;
            const int h23 = height * 0.667;
            const int h34 = height * 0.75;
            QPolygon terrain;
            terrain.push_back(QPoint(1, height - 1));
            terrain.push_back(QPoint(width - 1, height - 1));
            terrain.push_back(QPoint(width - 1, h23));
            terrain.push_back(QPoint(w14 * 3, h34));
            terrain.push_back(QPoint(w14 * 2, h23));
            terrain.push_back(QPoint(w14, h34));
            terrain.push_back(QPoint(1, h23));
            terrain.push_back(QPoint(1, height - 1));
            painter->drawPolygon(terrain);
            
            /*
             * Sun
             */
            painter->setBrush(QColor(255, 255, 0));
            painter->setPen(QColor(255, 255, 0));
            const int radius = width * 0.25;
            painter->drawEllipse(width * 0.33, height * 0.33, radius, radius);
        }
            break;
        case AnnotationTypeEnum::LINE:
            painter->drawLine(1, height - 1, width - 1, 1);
            break;
        case AnnotationTypeEnum::OVAL:
            painter->drawEllipse(1, 1, width - 1, height - 1);
            break;
        case AnnotationTypeEnum::TEXT:
        {
            QFont font = painter->font();
            font.setPixelSize(20);
            painter->setFont(font);
            painter->drawText(pixmap.rect(),
                             (Qt::AlignCenter),
                             "A");
        }
            break;
    }
    
    return pixmap;
}

