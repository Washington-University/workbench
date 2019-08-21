
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

#define __ANNOTATION_WIDTH_HEIGHT_WIDGET_DECLARE__
#include "AnnotationWidthHeightWidget.h"
#undef __ANNOTATION_WIDTH_HEIGHT_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationWidthHeightWidget 
 * \brief Widget for editing annotation coordinate, size, and rotation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentWidgetType
 *    Type of parent widget
 * @param browserWindowIndex
 *    Index of browser window.
 * @param parent
 *    Parent of this widget.
 */
AnnotationWidthHeightWidget::AnnotationWidthHeightWidget(const UserInputModeEnum::Enum userInputMode,
                                                         const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                         const int32_t browserWindowIndex,
                                                                         QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{
    QString widthLabelText;
    QString heightLabelText;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            widthLabelText  = " W:";
            heightLabelText = " H:";
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    QLabel* widthLabel = new QLabel(widthLabelText);
    m_widthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1000.0, 0.1, 1,
                                                                                    this, SLOT(widthValueChanged(double)));
    m_widthSpinBox->setSuffix("%");
    WuQtUtilities::setWordWrappedToolTip(m_widthSpinBox,
                                         "Percentage width of 2D Shapes (Box, Image, Oval)");

    QLabel* heightLabel = new QLabel(heightLabelText);
    m_heightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1000.0, 0.1, 1,
                                                                                    this, SLOT(heightValueChanged(double)));
    m_heightSpinBox->setSuffix("%");
    WuQtUtilities::setWordWrappedToolTip(m_heightSpinBox,
                                         "Percentage height of 2D Shapes (Box, Image, Oval)");

    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(widthLabel);
    layout->addWidget(m_widthSpinBox);
    layout->addWidget(heightLabel);
    layout->addWidget(m_heightSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationWidthHeightWidget::~AnnotationWidthHeightWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotations2D.
 *    Two dimensional annotation.
 */
void
AnnotationWidthHeightWidget::updateContent(std::vector<AnnotationTwoDimensionalShape*>& annotations2D)
{
    m_annotations2D = annotations2D;
    
    if ( ! m_annotations2D.empty()) {
        float widthValue = 0.0;
        bool haveMultipleWidthValuesFlag = false;
        
        float heightValue = 0.0;
        bool haveMultipleHeightValuesFlag = false;
        
        bool firstFlag = true;
        bool haveValuesFlag = false;
        
        const int32_t numAnns = static_cast<int32_t>(m_annotations2D.size());
        for (int32_t i = 0; i < numAnns; i++) {
            CaretAssertVectorIndex(m_annotations2D, i);
            if (m_annotations2D[i]->getType() == AnnotationTypeEnum::TEXT) {
                continue;
            }
            
            haveValuesFlag = true;
            
            const float width  = m_annotations2D[i]->getWidth();
            const float height = m_annotations2D[i]->getHeight();
            
            if (firstFlag) {
                widthValue  = width;
                heightValue = height;
                firstFlag = false;
            }
            else {
                if (width != widthValue) {
                    haveMultipleWidthValuesFlag = true;
                }
                if (height != heightValue) {
                    haveMultipleHeightValuesFlag = true;
                }
                
                widthValue = std::min(widthValue,
                                      width);
                heightValue = std::min(heightValue,
                                       height);
            }
        }
        
        if (haveValuesFlag) {
            m_widthSpinBox->blockSignals(true);
            m_widthSpinBox->setValue(widthValue);
            if (haveMultipleWidthValuesFlag) {
                m_widthSpinBox->setSuffix("%+");
            }
            else {
                m_widthSpinBox->setSuffix("%");
            }
            m_widthSpinBox->blockSignals(false);
            
            m_heightSpinBox->blockSignals(true);
            m_heightSpinBox->setValue(heightValue);
            if (haveMultipleHeightValuesFlag) {
                m_heightSpinBox->setSuffix("%+");
            }
            else {
                m_heightSpinBox->setSuffix("%");
            }
            m_heightSpinBox->blockSignals(false);
            

            switch (m_parentWidgetType) {
                case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
                    AnnotationTwoDimensionalShape::setUserDefaultWidth(widthValue);
                    AnnotationTwoDimensionalShape::setUserDefaultHeight(heightValue);
                    break;
                case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                    CaretAssert(0);
                    break;
            }
            
            setEnabled(true);
        }
        else {
            setEnabled(false);
        }
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when height value is changed.
 *
 * @param value
 *
 */
void
AnnotationWidthHeightWidget::heightValueChanged(double value)
{
    std::vector<Annotation*> annotations(m_annotations2D.begin(),
                                         m_annotations2D.end());
    
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeTwoDimHeight(value,
                                     annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(m_userInputMode,
                                undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            
            AnnotationTwoDimensionalShape::setUserDefaultHeight(value);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
}

/**
 * Gets called when width value is changed.
 *
 * @param value
 *
 */
void
AnnotationWidthHeightWidget::widthValueChanged(double value)
{
    std::vector<Annotation*> annotations(m_annotations2D.begin(),
                                         m_annotations2D.end());
    
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeTwoDimWidth(value,
                                    annotations);
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AString errorMessage;
    if ( ! annMan->applyCommand(m_userInputMode,
                                undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationTwoDimensionalShape::setUserDefaultWidth(value);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }

    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

