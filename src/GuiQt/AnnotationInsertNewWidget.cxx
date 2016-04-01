
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
#include <QActionGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationMenuFileSelection.h"
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
    QToolButton* shapeBoxToolButton   = createShapeToolButton(AnnotationTypeEnum::BOX);
    QToolButton* shapeImageToolButton = createShapeToolButton(AnnotationTypeEnum::IMAGE);
    QToolButton* shapeLineToolButton  = createShapeToolButton(AnnotationTypeEnum::LINE);
    QToolButton* shapeOvalToolButton  = createShapeToolButton(AnnotationTypeEnum::OVAL);
    QToolButton* shapeTextToolButton  = createShapeToolButton(AnnotationTypeEnum::TEXT);
    
    QToolButton* tabSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::TAB);
    QToolButton* stereotaxicSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
    QToolButton* surfaceSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::SURFACE);
    QToolButton* windowSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::WINDOW);
    
    const bool smallButtonsFlag = false;
    if (smallButtonsFlag) {
        const int mw = 24;
        const int mh = 24;
        
        shapeBoxToolButton->setMaximumSize(mw, mh);
        shapeImageToolButton->setMaximumSize(mw, mh);
        shapeLineToolButton->setMaximumSize(mw, mh);
        shapeOvalToolButton->setMaximumSize(mw, mh);
        shapeTextToolButton->setMaximumSize(mw, mh);

        tabSpaceToolButton->setMaximumSize(mw, mh);
        stereotaxicSpaceToolButton->setMaximumSize(mw, mh);
        surfaceSpaceToolButton->setMaximumSize(mw, mh);
        windowSpaceToolButton->setMaximumSize(mw, mh);
    }
    
    m_spaceActionGroup = new QActionGroup(this);
    m_spaceActionGroup->addAction(tabSpaceToolButton->defaultAction());
    m_spaceActionGroup->addAction(stereotaxicSpaceToolButton->defaultAction());
    m_spaceActionGroup->addAction(surfaceSpaceToolButton->defaultAction());
    m_spaceActionGroup->addAction(windowSpaceToolButton->defaultAction());
    QObject::connect(m_spaceActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(spaceOrShapeActionTriggered()));
    
    m_shapeActionGroup = new QActionGroup(this);
    m_shapeActionGroup->addAction(shapeBoxToolButton->defaultAction());
    m_shapeActionGroup->addAction(shapeImageToolButton->defaultAction());
    m_shapeActionGroup->addAction(shapeLineToolButton->defaultAction());
    m_shapeActionGroup->addAction(shapeOvalToolButton->defaultAction());
    m_shapeActionGroup->addAction(shapeTextToolButton->defaultAction());
    QObject::connect(m_shapeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(spaceOrShapeActionTriggered()));
    
    QToolButton* fileSelectionToolButton = createFileSelectionToolButton();
    
    
    QLabel* fileLabel  = new QLabel("File");
    QLabel* spaceLabel = new QLabel("Space");
    QLabel* typeLabel  = new QLabel("Type");
    
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    
    const bool rowsLayoutFlag = true;
    if (rowsLayoutFlag) {
        QVBoxLayout* fileLayout = new QVBoxLayout();
        fileLayout->addWidget(fileLabel, 0, Qt::AlignHCenter);
        fileLayout->addWidget(fileSelectionToolButton, 0, Qt::AlignHCenter);
        fileLayout->addStretch();
        
        QLabel* insertLabel = new QLabel("Insert New");
        
        gridLayout->addWidget(insertLabel,
                              0, 0, 1, 8,
                              Qt::AlignHCenter);
        
        gridLayout->addLayout(fileLayout,
                              1, 0, 3, 1,
                              (Qt::AlignTop | Qt::AlignHCenter));
        
        gridLayout->setColumnMinimumWidth(1, 5);
        
        gridLayout->addWidget(spaceLabel,
                              1, 2, Qt::AlignLeft);
        gridLayout->addWidget(stereotaxicSpaceToolButton,
                              1, 3);
        gridLayout->addWidget(surfaceSpaceToolButton,
                              1, 4);
        gridLayout->addWidget(tabSpaceToolButton,
                              1, 5);
        gridLayout->addWidget(windowSpaceToolButton,
                              1, 6);

        gridLayout->setRowMinimumHeight(2, 2);
        
        gridLayout->addWidget(typeLabel,
                              3, 2, Qt::AlignLeft);
        gridLayout->addWidget(shapeBoxToolButton,
                              3, 3);
        gridLayout->addWidget(shapeImageToolButton,
                              3, 4);
        gridLayout->addWidget(shapeLineToolButton,
                              3, 5);
        gridLayout->addWidget(shapeOvalToolButton,
                              3, 6);
        gridLayout->addWidget(shapeTextToolButton,
                              3, 7);
    }
    else {
        QLabel* insertLabel = new QLabel("Insert New");
        
        gridLayout->addWidget(insertLabel,
                              0, 0, 1, 8, Qt::AlignHCenter);
        
        gridLayout->addWidget(fileLabel,
                              1, 0,
                              Qt::AlignHCenter);
        gridLayout->addWidget(fileSelectionToolButton,
                              2, 0, 2, 1,
                              (Qt::AlignTop | Qt::AlignHCenter));
        
        gridLayout->setColumnMinimumWidth(1, 15);
        gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                              1, 1, 3, 1,
                              Qt::AlignHCenter);
        
        gridLayout->addWidget(spaceLabel,
                              1, 2, Qt::AlignLeft);
        gridLayout->addWidget(stereotaxicSpaceToolButton,
                              1, 3);
        gridLayout->addWidget(surfaceSpaceToolButton,
                              1, 4);
        gridLayout->addWidget(tabSpaceToolButton,
                              1, 5);
        gridLayout->addWidget(windowSpaceToolButton,
                              1, 6);
        
        QSpacerItem* rowSpaceItem = new QSpacerItem(5, 5,
                                                    QSizePolicy::Fixed,
                                                    QSizePolicy::Fixed);
        gridLayout->addItem(rowSpaceItem,
                            2, 3, 1, 6);
        
        gridLayout->addWidget(typeLabel,
                              3, 2, Qt::AlignLeft);
        gridLayout->addWidget(shapeBoxToolButton,
                              3, 3);
        gridLayout->addWidget(shapeImageToolButton,
                              3, 4);
        gridLayout->addWidget(shapeLineToolButton,
                              3, 5);
        gridLayout->addWidget(shapeOvalToolButton,
                              3, 6);
        gridLayout->addWidget(shapeTextToolButton,
                              3, 7);
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
    /*
     * Default Space to Tab
     * Default Shape to Text
     * Do this before creating action groups to avoid
     * triggering signals.
     */
    m_spaceActionGroup->blockSignals(true);
    tabSpaceToolButton->defaultAction()->setChecked(true);
    m_spaceActionGroup->blockSignals(false);

    m_shapeActionGroup->blockSignals(true);
    shapeTextToolButton->defaultAction()->setChecked(true);
    m_shapeActionGroup->blockSignals(false);
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
    itemSelectedFromFileSelectionMenu();
}

/**
 * Gets called when an item is selected from the file selection menu.
 */
void
AnnotationInsertNewWidget::itemSelectedFromFileSelectionMenu()
{
    /*
     * Add a space so that the arrow is not
     */
    m_fileSelectionToolButtonAction->setText(m_fileSelectionMenu->getSelectedNameForToolButton());
}

/**
 * @return Create a file selection/menu toolbutton.
 */
QToolButton*
AnnotationInsertNewWidget::createFileSelectionToolButton()
{
    m_fileSelectionMenu = new AnnotationMenuFileSelection();
    QObject::connect(m_fileSelectionMenu, SIGNAL(menuItemSelected()),
                     this, SLOT(itemSelectedFromFileSelectionMenu()));
    
    m_fileSelectionToolButtonAction = new QAction(m_fileSelectionMenu->getSelectedNameForToolButton(),
                                                  this);
    m_fileSelectionToolButtonAction->setToolTip("Choose file for new annotation");
    m_fileSelectionToolButtonAction->setMenu(m_fileSelectionMenu);
    
    QToolButton* fileSelectionToolButton = new QToolButton();
    fileSelectionToolButton->setDefaultAction(m_fileSelectionToolButtonAction);
    fileSelectionToolButton->setFixedWidth(fileSelectionToolButton->sizeHint().width());
    
    return fileSelectionToolButton;
}

/**
 * @return Create the shape tool button for the given annotation type.
 * 
 * @param annotationType
 *     The annotation type.
 */
QToolButton*
AnnotationInsertNewWidget::createShapeToolButton(const AnnotationTypeEnum::Enum annotationType)
{
    const QString typeGuiName = AnnotationTypeEnum::toGuiName(annotationType);
    QToolButton* toolButton = new QToolButton();
    
    QAction* action = new QAction(createShapePixmap(toolButton,
                                                    annotationType),
                                  typeGuiName,
                                  this);
    
    action->setData(AnnotationTypeEnum::toIntegerCode(annotationType));
    
    action->setToolTip(typeGuiName
                       + " annotation");

    action->setCheckable(true);
    action->setChecked(false);
    
    toolButton->setDefaultAction(action);
    
    return toolButton;
}

/**
 * Called when a space or shape action triggered.
 */
void
AnnotationInsertNewWidget::spaceOrShapeActionTriggered()
{
    const QAction* spaceAction = m_spaceActionGroup->checkedAction();
    if (spaceAction == NULL) {
        WuQMessageBox::errorOk(this, "No space is selected.  Select a space.");
        return;
    }
    
    const QAction* shapeAction = m_shapeActionGroup->checkedAction();
    if (shapeAction == NULL) {
        WuQMessageBox::errorOk(this, "No shape is selected.  Select a shape.");
        return;
    }
    
    AnnotationFile* annotationFile = m_fileSelectionMenu->getSelectedAnnotationFile();
    CaretAssert(annotationFile);
    
    CaretAssert(spaceAction);
    const int spaceInt = spaceAction->data().toInt();
    bool spaceValidFlag = false;
    AnnotationCoordinateSpaceEnum::Enum annSpace = AnnotationCoordinateSpaceEnum::fromIntegerCode(spaceInt,
                                                                                               &spaceValidFlag);
    CaretAssert(spaceValidFlag);

    CaretAssert(shapeAction);
    const int shapeInt = shapeAction->data().toInt();
    bool shapeValidFlag = false;
    AnnotationTypeEnum::Enum annShape = AnnotationTypeEnum::fromIntegerCode(shapeInt,
                                                                            &shapeValidFlag);
    CaretAssert(shapeValidFlag);
    
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(annotationFile,
                                                                annSpace,
                                                                annShape).getPointer());
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
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssertMessage(0, "No pixmap for colorbar as user does not create them like other annotations");
            break;
        case AnnotationTypeEnum::IMAGE:
        {
            const int blueAsGray = qGray(25,25,255);
            QColor skyColor(blueAsGray, blueAsGray, blueAsGray);
            
            /*
             * Background (sky)
             */
            painter->fillRect(pixmap.rect(), skyColor);
            
            const int greenAsGray = qGray(0, 255, 0);
            QColor terrainColor(greenAsGray, greenAsGray, greenAsGray);
            
            /*
             * Terrain
             */
            painter->setBrush(terrainColor);
            painter->setPen(terrainColor);
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
            
            const int yellowAsGray = qGray(255, 255, 0);
            QColor sunColor(yellowAsGray, yellowAsGray, yellowAsGray);
            
            /*
             * Sun
             */
            painter->setBrush(sunColor);
            painter->setPen(sunColor);
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

/**
 * @return Create the space tool button for the given annotation space.
 *
 * @param annotationSpace
 *     The annotation space
 */
QToolButton*
AnnotationInsertNewWidget::createSpaceToolButton(const AnnotationCoordinateSpaceEnum::Enum annotationSpace)
{
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssertMessage(0, "Annotations in pixel space not supported.");
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    
    const bool useIconFlag = true;
    QToolButton* toolButton = new QToolButton();
    QAction* action = NULL;
    if (useIconFlag) {
        action = new QAction(createSpacePixmap(toolButton,
                                               annotationSpace),
                             AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace),
                             this);
    }
    else {
        action = new QAction(AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace),
                             this);
    }

    action->setData((int)AnnotationCoordinateSpaceEnum::toIntegerCode(annotationSpace));
    action->setToolTip(AnnotationCoordinateSpaceEnum::toToolTip(annotationSpace));
    toolButton->setDefaultAction(action);
    
    action->setCheckable(true);
    action->setChecked(false);
    
    return toolButton;
}

/**
 * Create a pixmap for the given annotation coordinate space
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param annotationSpace
 *    The annotation coordinate space.
 * @return
 *    Pixmap with icon for the given annotation coordinate space.
 */
QPixmap
AnnotationInsertNewWidget::createSpacePixmap(const QWidget* widget,
                                             const AnnotationCoordinateSpaceEnum::Enum annotationSpace)
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
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssertMessage(0, "Annotations in pixel space not supported.");
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    
    const QString letter = AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace);
    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    painter->drawText(pixmap.rect(),
                      (Qt::AlignCenter),
                      letter);
    
    return pixmap;
}

