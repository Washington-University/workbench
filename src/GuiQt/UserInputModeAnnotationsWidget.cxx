
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

#include "AnnotationCoordinateSizeRotateWidget.h"
#include "AnnotationMenuArrange.h"
#include "AnnotationColorWidget.h"
#include "AnnotationFontWidget.h"
#include "AnnotationTextAlignmentWidget.h"
#include "AnnotationTypeSpaceWidget.h"
#include "CaretAssert.h"
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
 */
UserInputModeAnnotationsWidget::UserInputModeAnnotationsWidget(UserInputModeAnnotations* inputModeAnnotations)
: QWidget(),
m_inputModeAnnotations(inputModeAnnotations)
{
    CaretAssert(inputModeAnnotations);
    
    m_typeSpaceWidget = new AnnotationTypeSpaceWidget();
    
    m_fontWidget = new AnnotationFontWidget();
    
    m_colorWidget = new AnnotationColorWidget();
    
    m_alignmentWidget = new AnnotationTextAlignmentWidget();
    
    m_coordSizeRotateWidget = new AnnotationCoordinateSizeRotateWidget();
    
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
    bottomRowLayout->addWidget(m_coordSizeRotateWidget);
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
        
        brainEvent->setEventProcessed();
    }
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
    AnnotationMenuArrange* arrangeMenu = new AnnotationMenuArrange();
    
    QAction* arrangeAction = new QAction("Arrange",
                                          this);
    arrangeAction->setToolTip("Arrange (align) and group annotations");
    arrangeAction->setMenu(arrangeMenu);
    
    QToolButton* arrangeToolButton = new QToolButton();
    arrangeToolButton->setDefaultAction(arrangeAction);
    
    return arrangeToolButton;
}

