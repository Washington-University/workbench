
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

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

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
    
    QLabel* nameLabel = new QLabel("Annotations ");
    QWidget* modeWidget = createModeWidget();
    
//    resetLastEditedBorder();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(nameLabel);
    layout->addWidget(modeWidget);
    layout->addSpacing(10);
    layout->addStretch();
    
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
    const int selectedModeInteger = (int)m_inputModeAnnotations->getMode();
    
    const int modeComboBoxIndex = m_modeComboBox->findData(selectedModeInteger);
    CaretAssert(modeComboBoxIndex >= 0);
    m_modeComboBox->blockSignals(true);
    m_modeComboBox->setCurrentIndex(modeComboBoxIndex);
    m_modeComboBox->blockSignals(false);
}

/**
 * Gets called when the user sets a mode.
 *
 * @param indx
 *    Inde of the selected item in combo box.
 */
void
UserInputModeAnnotationsWidget::modeComboBoxSelection(int indx)
{
    const int modeInteger = m_modeComboBox->itemData(indx).toInt();
    const UserInputModeAnnotations::Mode mode = (UserInputModeAnnotations::Mode)modeInteger;
    m_inputModeAnnotations->setMode(mode);
//    resetLastEditedBorder();
}

/**
 * @return The mode widget.
 */
QWidget*
UserInputModeAnnotationsWidget::createModeWidget()
{
    m_modeComboBox = new QComboBox();
    m_modeComboBox->addItem("New",  (int)UserInputModeAnnotations::MODE_NEW);
    m_modeComboBox->addItem("Edit", (int)UserInputModeAnnotations::MODE_EDIT);
    m_modeComboBox->addItem("ROI",  (int)UserInputModeAnnotations::MODE_DELETE);
    QObject::connect(m_modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(m_modeComboBox);
    
    widget->setFixedWidth(widget->sizeHint().width());
    
    return widget;
}

