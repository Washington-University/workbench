
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

#include "AnnotationCoordinateWidget.h"
#include "AnnotationMenuArrange.h"
#include "AnnotationColorWidget.h"
#include "AnnotationFontWidget.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationText.h"
#include "AnnotationTextAlignmentWidget.h"
#include "AnnotationTypeSpaceWidget.h"
#include "AnnotationWidthHeightRotationWidget.h"
#include "CaretAssert.h"
#include "EventAnnotation.h"
#include "EventBrainReset.h"
#include "EventManager.h"
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
m_inputModeAnnotations(inputModeAnnotations),
m_browserWindowIndex(browserWindowIndex)
{
    CaretAssert(inputModeAnnotations);
    
    m_annotationBeingEdited = NULL;
    
    m_typeSpaceWidget = new AnnotationTypeSpaceWidget();
    
    m_fontWidget = new AnnotationFontWidget(m_browserWindowIndex);
    
    m_colorWidget = new AnnotationColorWidget(m_browserWindowIndex);
    
    m_alignmentWidget = new AnnotationTextAlignmentWidget(m_browserWindowIndex);
    
    m_coordinateOneWidget = new AnnotationCoordinateWidget(m_browserWindowIndex);
    
    m_widthHeightRotationWidget = new AnnotationWidthHeightRotationWidget(m_browserWindowIndex);
    
    QWidget* arrangeToolButton = createArrangeMenuToolButton();
    
    QHBoxLayout* topRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(topRowLayout, 2, 2);
    topRowLayout->addWidget(m_typeSpaceWidget);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_colorWidget);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_fontWidget);
    topRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    topRowLayout->addWidget(m_alignmentWidget);
    topRowLayout->addStretch();
    
    QHBoxLayout* bottomRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(bottomRowLayout, 2, 2);
    bottomRowLayout->addWidget(m_coordinateOneWidget);
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    bottomRowLayout->addWidget(m_widthHeightRotationWidget);
    bottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    bottomRowLayout->addWidget(arrangeToolButton);
    bottomRowLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addLayout(topRowLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(bottomRowLayout);
    
//    setSizePolicy(QSizePolicy::Fixed,
//                  QSizePolicy::Fixed);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION);
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
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainEvent);
        
        m_annotationBeingEdited = NULL;
        
        brainEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION) {
        EventAnnotation* annotationEvent = dynamic_cast<EventAnnotation*>(event);
        CaretAssert(annotationEvent);
        
        switch (annotationEvent->getMode()) {
            case EventAnnotation::MODE_INVALID:
                break;
            case EventAnnotation::MODE_ANNOTATION_EDIT:
            {
                int32_t windowIndex = -1;
                Annotation* annotation = NULL;
                annotationEvent->getModeEditAnnotation(windowIndex,
                                                       annotation);
                if (windowIndex == m_browserWindowIndex) {
                    m_annotationBeingEdited = annotation;
                }
            }
                break;
            case EventAnnotation::MODE_DESELECT_ALL_ANNOTATIONS:
                m_annotationBeingEdited = NULL;
                break;
        }
        
        annotationEvent->setEventProcessed();
    }

    AnnotationText* textAnnotation = NULL;
    AnnotationTwoDimensionalShape* twoDimAnnotation = NULL;
    AnnotationOneDimensionalShape* oneDimAnnotation = NULL;
    
    AnnotationCoordinate* coordinateOne = NULL;
    AnnotationCoordinate* coordinateTwo = NULL;
    if (m_annotationBeingEdited != NULL) {
        textAnnotation   = dynamic_cast<AnnotationText*>(m_annotationBeingEdited);
        
        oneDimAnnotation = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotationBeingEdited);
        if (oneDimAnnotation != NULL) {
            coordinateOne = oneDimAnnotation->getStartCoordinate();
            coordinateTwo = oneDimAnnotation->getEndCoordinate();
        }
        
        twoDimAnnotation = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);
        if (twoDimAnnotation != NULL) {
            coordinateOne = twoDimAnnotation->getCoordinate();
        }
    }
    
    m_fontWidget->updateContent(textAnnotation);
    m_colorWidget->updateContent(m_annotationBeingEdited);
    m_alignmentWidget->updateContent(textAnnotation);
    
    if (m_annotationBeingEdited != NULL) {
        m_coordinateOneWidget->updateContent(m_annotationBeingEdited->getCoordinateSpace(),
                                             coordinateOne);
    }
    
    m_widthHeightRotationWidget->updateContent(twoDimAnnotation);
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
        case UserInputModeAnnotations::MODE_DELETE:
//            this->operationStackedWidget->setCurrentWidget(this->widgetDrawOperation);
//            this->setActionGroupByActionData(this->drawOperationActionGroup,
//                                             inputModeBorders->getDrawOperation());
//            resetLastEditedBorder();
            break;
        case UserInputModeAnnotations::MODE_EDIT:
//            this->operationStackedWidget->setCurrentWidget(this->widgetEditOperation);
//            this->setActionGroupByActionData(this->editOperationActionGroup,
//                                             inputModeBorders->getEditOperation());
            break;
        case UserInputModeAnnotations::MODE_NEW:
//            this->operationStackedWidget->setCurrentWidget(this->widgetRoiOperation);
//            resetLastEditedBorder();
            break;
    }
}

/**
 * @return The arrange tool button.
 */
QWidget*
UserInputModeAnnotationsWidget::createArrangeMenuToolButton()
{
    AnnotationMenuArrange* arrangeMenu = new AnnotationMenuArrange(m_browserWindowIndex);
    
    QAction* arrangeAction = new QAction("Arrange",
                                          this);
    arrangeAction->setToolTip("Arrange (align) and group annotations");
    arrangeAction->setMenu(arrangeMenu);
    
    QToolButton* arrangeToolButton = new QToolButton();
    arrangeToolButton->setDefaultAction(arrangeAction);
    
    return arrangeToolButton;
}

