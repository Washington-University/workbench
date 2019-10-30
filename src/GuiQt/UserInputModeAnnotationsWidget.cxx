
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

#define __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__
#include "UserInputModeAnnotationsWidget.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBackgroundTypeWidget.h"
#include "AnnotationBoundsWidget.h"
#include "AnnotationBrowserTab.h"
#include "AnnotationCoordinateCenterXYWidget.h"
#include "AnnotationCoordinateSpaceWidget.h"
#include "AnnotationCoordinateWidget.h"
#include "AnnotationColorWidget.h"
#include "AnnotationDeleteWidget.h"
#include "AnnotationFontWidget.h"
#include "AnnotationFormatWidget.h"
#include "AnnotationInsertNewWidget.h"
#include "AnnotationLine.h"
#include "AnnotationLineArrowTipsWidget.h"
#include "AnnotationManager.h"
#include "AnnotationNameWidget.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoWidget.h"
#include "AnnotationRotationWidget.h"
#include "AnnotationText.h"
#include "AnnotationTextAlignmentWidget.h"
#include "AnnotationTextEditorWidget.h"
#include "AnnotationTextOrientationWidget.h"
#include "AnnotationWidthHeightWidget.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventAnnotationCreateNewType.h"
#include "EventBrainReset.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "UserInputModeAnnotations.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::UserInputModeAnnotationsWidget 
 * \brief Toolbar widget for annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param inputModeAnnotations
 *    Annotation mode input processor.
 * @param browserWindowIndex
 *    Index of browser window using this widget.
 */
UserInputModeAnnotationsWidget::UserInputModeAnnotationsWidget(UserInputModeAnnotations* inputModeAnnotations,
                                                               const int32_t browserWindowIndex)
: QWidget(),
m_browserWindowIndex(browserWindowIndex),
m_inputModeAnnotations(inputModeAnnotations)
{
    CaretAssert(inputModeAnnotations);
    
    switch (inputModeAnnotations->getUserInputMode()) {
        case UserInputModeEnum::ANNOTATIONS:
            createAnnotationWidget();
            break;
        case UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING:
            createTileTabsEditingWidget();
            break;
        case UserInputModeEnum::BORDERS:
        case UserInputModeEnum::FOCI:
        case UserInputModeEnum::IMAGE:
        case UserInputModeEnum::INVALID:
        case UserInputModeEnum::VIEW:
        case UserInputModeEnum::VOLUME_EDIT:
            CaretAssert(0);
            return;
            break;
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
UserInputModeAnnotationsWidget::~UserInputModeAnnotationsWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
UserInputModeAnnotationsWidget::receiveEvent(Event* event)
{
    bool updateAnnotationWidgetsFlag = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainEvent);
        
        updateAnnotationWidgetsFlag = true;
        
        brainEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE) {
        EventAnnotationCreateNewType* annotationEvent = dynamic_cast<EventAnnotationCreateNewType*>(event);
        CaretAssert(annotationEvent);
        updateAnnotationWidgetsFlag = true;
        annotationEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* updateEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(updateEvent);
        updateAnnotationWidgetsFlag = true;
        updateEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        updateAnnotationWidgetsFlag = true;
    }
    else {
        return;
    }

    if (! updateAnnotationWidgetsFlag) {
        return;
    }
    
    updateWidget();
    
}

/**
 * Create annotation mode widget
 */
void
UserInputModeAnnotationsWidget::createTileTabsEditingWidget()
{
    m_nameWidget                 = new AnnotationNameWidget(m_inputModeAnnotations->getUserInputMode(),
                                                            m_browserWindowIndex);
    
    m_boundsWidget               = new AnnotationBoundsWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                              m_browserWindowIndex);
    
    m_coordinateCenterXYWidget   = new AnnotationCoordinateCenterXYWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                          AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                          AnnotationCoordinateCenterXYWidget::COORDINATE_ONE,
                                                                          m_browserWindowIndex);
    
    m_widthHeightWidget          = new AnnotationWidthHeightWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                   AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                   m_browserWindowIndex,
                                                                   Qt::Vertical);
    
    m_backgroundTypeWidget       = new AnnotationBackgroundTypeWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                      AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                      m_browserWindowIndex);
    
    m_formatWidget               = new AnnotationFormatWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);
    
    m_redoUndoWidget             = new AnnotationRedoUndoWidget(Qt::Vertical,
                                                                m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(2, 2, 2, 2);
    gridLayout->setVerticalSpacing(0);
    int column = 0;
    gridLayout->addWidget(m_nameWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_boundsWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_coordinateCenterXYWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_widthHeightWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_backgroundTypeWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_formatWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    gridLayout->addWidget(m_redoUndoWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    
    for (int32_t iCol = 0; iCol < column; iCol++) {
        gridLayout->setColumnStretch(iCol, 0);
    }
    gridLayout->setColumnStretch(column, 100);
    
    const int numRows = gridLayout->rowCount();
    gridLayout->setRowStretch(numRows, 100);
}

/*
 * Create tile tabs mode widget
 */
void
UserInputModeAnnotationsWidget::createAnnotationWidget()
{
    m_textEditorWidget           = new AnnotationTextEditorWidget(m_browserWindowIndex);
    
    m_lineArrowTipsWidget        = new AnnotationLineArrowTipsWidget(m_browserWindowIndex);
    
    m_fontWidget                 = new AnnotationFontWidget(AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                            m_browserWindowIndex);
    
    m_colorWidget                = new AnnotationColorWidget(AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                             m_browserWindowIndex);
    
    m_textAlignmentWidget        = new AnnotationTextAlignmentWidget(m_browserWindowIndex);
    
    m_textOrientationWidget      = new AnnotationTextOrientationWidget(m_browserWindowIndex);
    
    QLabel* coordinateSpaceLabel = new QLabel("Space");
    m_coordinateSpaceWidget      = new AnnotationCoordinateSpaceWidget(m_browserWindowIndex);
    
    m_coordinateOneWidget        = new AnnotationCoordinateWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                  AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                  AnnotationCoordinateWidget::COORDINATE_ONE,
                                                                  m_browserWindowIndex);
    
    m_coordinateTwoWidget        = new AnnotationCoordinateWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                  AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                  AnnotationCoordinateWidget::COORDINATE_TWO,
                                                                  m_browserWindowIndex);
    
    m_widthHeightWidget          = new AnnotationWidthHeightWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                   AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                   m_browserWindowIndex,
                                                                   Qt::Horizontal);
    
    m_rotationWidget             = new AnnotationRotationWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    m_formatWidget               = new AnnotationFormatWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);
    
    m_insertNewWidget            = new AnnotationInsertNewWidget(m_browserWindowIndex);
    
    m_deleteWidget               = new AnnotationDeleteWidget(m_browserWindowIndex);
    
    m_redoUndoWidget             = new AnnotationRedoUndoWidget(Qt::Horizontal,
                                                                m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    /*
     * Connect signals for setting a coordinate with the mouse.
     */
    QObject::connect(m_coordinateOneWidget, SIGNAL(signalSelectCoordinateWithMouse()),
                     this, SLOT(selectCoordinateOneWithMouse()));
    QObject::connect(m_coordinateTwoWidget, SIGNAL(signalSelectCoordinateWithMouse()),
                     this, SLOT(selectCoordinateTwoWithMouse()));
    
    /*
     * Layout top row of widgets
     */
    QWidget* topRowWidget = new QWidget();
    QHBoxLayout* topRowLayout = new QHBoxLayout(topRowWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(topRowLayout, 2, 0);
    topRowLayout->addWidget(m_colorWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_lineArrowTipsWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_textEditorWidget, 100, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_fontWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_textAlignmentWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_textOrientationWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_insertNewWidget, 0, Qt::AlignTop);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
//    topRowLayout->addWidget(m_deleteWidget, 0, Qt::AlignTop);
//    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
//    topRowLayout->addWidget(m_formatWidget, 0, Qt::AlignTop);
//    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
//    topRowLayout->addWidget(m_redoUndoWidget, 0, Qt::AlignTop);
    topRowLayout->addStretch();
    
    topRowWidget->setFixedHeight(topRowWidget->sizeHint().height());
    
    /*
     * Layout bottom row of widgets
     */
    QGridLayout* bottomRowLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 0);
    int32_t column(0);
    bottomRowLayout->addWidget(coordinateSpaceLabel, 0, column);
    bottomRowLayout->addWidget(m_coordinateSpaceWidget, 1, column, Qt::AlignHCenter);
    column++;
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    bottomRowLayout->addWidget(m_coordinateOneWidget, 0, column);
    bottomRowLayout->addWidget(m_coordinateTwoWidget, 1, column);
    column++;
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    bottomRowLayout->addWidget(m_widthHeightWidget, 0, column);
    bottomRowLayout->addWidget(m_rotationWidget, 1, column);
    column++;
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    bottomRowLayout->addWidget(m_deleteWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    bottomRowLayout->addWidget(m_formatWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, column, 2, 1);
    column++;
    bottomRowLayout->addWidget(m_redoUndoWidget, 0, column, 2, 1, Qt::AlignTop);
    column++;
    bottomRowLayout->setColumnStretch(column, 100);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(topRowWidget);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(bottomRowLayout);
}

/**
 * Select coordinate one with the mouse.
 */
void
UserInputModeAnnotationsWidget::selectCoordinateOneWithMouse()
{
    m_inputModeAnnotations->setMode(UserInputModeAnnotations::MODE_SET_COORDINATE_ONE);
}

/**
 * Select coordinate two with the mouse.
 */
void
UserInputModeAnnotationsWidget::selectCoordinateTwoWithMouse()
{
    m_inputModeAnnotations->setMode(UserInputModeAnnotations::MODE_SET_COORDINATE_TWO);
}


/**
 * Update the widget.
 */
void
UserInputModeAnnotationsWidget::updateWidget()
{
    /*
     * Show the proper widget
     */
    switch (m_inputModeAnnotations->getMode()) {
        case UserInputModeAnnotations::MODE_NEW_WITH_CLICK:
            break;
        case UserInputModeAnnotations::MODE_NEW_WITH_DRAG:
            break;
        case UserInputModeAnnotations::MODE_PASTE:
            break;
        case UserInputModeAnnotations::MODE_PASTE_SPECIAL:
            break;
        case UserInputModeAnnotations::MODE_SELECT:
            break;
        case UserInputModeAnnotations::MODE_SET_COORDINATE_ONE:
            break;
        case UserInputModeAnnotations::MODE_SET_COORDINATE_TWO:
            break;
    }
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    
    
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_browserWindowIndex);
    
    std::vector<AnnotationBrowserTab*> browserTabAnnotations;
    std::vector<AnnotationLine*> lineAnnotations;
    std::vector<AnnotationText*> textAnnotations;
    std::vector<AnnotationFontAttributesInterface*> fontStyleAnnotations;
    std::vector<AnnotationTwoDimensionalShape*> twoDimAnnotations;
    std::vector<AnnotationOneDimensionalShape*> oneDimAnnotations;
    
    for (std::vector<Annotation*>::iterator iter = selectedAnnotations.begin();
         iter != selectedAnnotations.end();
         iter++) {
        Annotation* ann = *iter;
        CaretAssert(ann);
        
        if (ann->getType() == AnnotationTypeEnum::BROWSER_TAB) {
            AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(ann);
            if (abt != NULL) {
                browserTabAnnotations.push_back(abt);
            }
        }
        
        AnnotationText* annText = dynamic_cast<AnnotationText*>(ann);
        if (annText != NULL) {
            textAnnotations.push_back(annText);
        }
        
        AnnotationFontAttributesInterface* annFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(ann);
        if (annFontStyle != NULL) {
            fontStyleAnnotations.push_back(annFontStyle);
        }
        
        AnnotationOneDimensionalShape* annOne = dynamic_cast<AnnotationOneDimensionalShape*>(ann);
        if (annOne != NULL) {
            oneDimAnnotations.push_back(annOne);
        }
        
        AnnotationTwoDimensionalShape* annTwo= dynamic_cast<AnnotationTwoDimensionalShape*>(ann);
        if (annTwo != NULL) {
            twoDimAnnotations.push_back(annTwo);
        }
        
        AnnotationLine* annLine = dynamic_cast<AnnotationLine*>(ann);
        if (annLine != NULL) {
            lineAnnotations.push_back(annLine);
        }
    }
    
    /*
     * Note: pointers are initialized to NULL in the header file
     */
    if (m_nameWidget != NULL) m_nameWidget->updateContent(selectedAnnotations);
    if (m_boundsWidget != NULL) m_boundsWidget->updateContent(browserTabAnnotations);
    if (m_coordinateSpaceWidget != NULL) m_coordinateSpaceWidget->updateContent(selectedAnnotations);
    if (m_fontWidget != NULL) m_fontWidget->updateContent(fontStyleAnnotations);
    if (m_textEditorWidget != NULL) m_textEditorWidget->updateContent(textAnnotations);
    if (m_colorWidget != NULL) m_colorWidget->updateContent(selectedAnnotations);
    if (m_lineArrowTipsWidget != NULL) m_lineArrowTipsWidget->updateContent(lineAnnotations);
    if (m_textAlignmentWidget != NULL) m_textAlignmentWidget->updateContent(textAnnotations);
    if (m_textOrientationWidget != NULL) m_textOrientationWidget->updateContent(textAnnotations);
    if (m_widthHeightWidget != NULL) m_widthHeightWidget->updateContent(twoDimAnnotations);
    if (m_rotationWidget != NULL) m_rotationWidget->updateContent(selectedAnnotations);
    if (m_insertNewWidget != NULL) m_insertNewWidget->updateContent();
    if (m_deleteWidget != NULL) m_deleteWidget->updateContent();
    
    Annotation* coordEditAnnotation = NULL;
    AnnotationOneDimensionalShape* coordEditOneDimAnnotation = NULL;
    if (selectedAnnotations.size() == 1) {
        coordEditAnnotation = selectedAnnotations[0];
        coordEditOneDimAnnotation = dynamic_cast<AnnotationOneDimensionalShape*>(coordEditAnnotation);
    }
    if (m_coordinateCenterXYWidget != NULL) m_coordinateCenterXYWidget->updateContent(selectedAnnotations);
    if (m_coordinateOneWidget != NULL) m_coordinateOneWidget->updateContent(coordEditAnnotation);
    if (coordEditOneDimAnnotation != NULL) {
        if (m_coordinateTwoWidget != NULL) m_coordinateTwoWidget->updateContent(coordEditOneDimAnnotation);
        if (m_coordinateTwoWidget != NULL) m_coordinateTwoWidget->setVisible(true);
    }
    else {
        if (m_coordinateTwoWidget != NULL) m_coordinateTwoWidget->setVisible(false);
    }
    
    if (m_backgroundTypeWidget != NULL) m_backgroundTypeWidget->updateContent(browserTabAnnotations);
    if (m_formatWidget != NULL) m_formatWidget->updateContent(selectedAnnotations);
    if (m_redoUndoWidget != NULL) m_redoUndoWidget->updateContent(selectedAnnotations);
}


