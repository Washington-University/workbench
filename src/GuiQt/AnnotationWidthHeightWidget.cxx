
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
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationOneCoordinateShape.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
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
                                                         const Qt::Orientation orientation,
                                                                         QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{
    QString widthLabelText;
    QString heightLabelText;
    bool includeSizeLabelFlag(true);
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            switch (userInputMode) {
                case UserInputModeEnum::Enum::ANNOTATIONS:
                    widthLabelText  = " W:";
                    heightLabelText = " H:";
                    break;
                case UserInputModeEnum::Enum::BORDERS:
                    break;
                case UserInputModeEnum::Enum::FOCI:
                    break;
                case UserInputModeEnum::Enum::IMAGE:
                    break;
                case UserInputModeEnum::Enum::INVALID:
                    break;
                case UserInputModeEnum::Enum::SAMPLES_EDITING:
                    widthLabelText  = "Width:";
                    heightLabelText = "Height:";
                    break;
                case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
                    widthLabelText  = "Width:";
                    heightLabelText = "Height:";
                    includeSizeLabelFlag = false;
                    break;
                case UserInputModeEnum::Enum::VIEW:
                    break;
                case UserInputModeEnum::Enum::VOLUME_EDIT:
                    break;
            }
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

    switch (orientation) {
        case Qt::Horizontal:
        {
            QHBoxLayout* layout = new QHBoxLayout(this);
            WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
            layout->addWidget(widthLabel);
            layout->addWidget(m_widthSpinBox);
            layout->addWidget(heightLabel);
            layout->addWidget(m_heightSpinBox);
        }
            break;
        case Qt::Vertical:
        {
            QLabel* sizeLabel(NULL);
            if (includeSizeLabelFlag) {
                sizeLabel = new QLabel("Size");
            }
            QGridLayout* layout = new QGridLayout(this);
            WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
            int32_t row(0);
            if (sizeLabel != NULL) {
                layout->addWidget(sizeLabel, row, 0, 1, 2, Qt::AlignHCenter);
                row++;
            }
            layout->addWidget(widthLabel, row, 0);
            layout->addWidget(m_widthSpinBox, row, 1);
            row++;
            layout->addWidget(heightLabel, row, 0);
            layout->addWidget(m_heightSpinBox, row, 1);
        }
            break;
    }
    
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
AnnotationWidthHeightWidget::updateContent(std::vector<AnnotationOneCoordinateShape*>& annotations2D)
{
    m_annotations2D.clear();
    for (auto a2d : annotations2D) {
        bool includeFlag(true);
        
        switch (a2d->getType()) {
            case AnnotationTypeEnum::ARROW:
                break;
            case AnnotationTypeEnum::BOX:
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                break;
            case AnnotationTypeEnum::MARKER:
                break;
            case AnnotationTypeEnum::OVAL:
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                break;
            case AnnotationTypeEnum::POLYGON:
                break;
            case AnnotationTypeEnum::POLYLINE:
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                /* Scale bar width/height not adjustable */
                includeFlag = false;
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
        if (includeFlag) {
            m_annotations2D.push_back(a2d);
        }
    }
    
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
            m_widthSpinBox->setEnabled(true);
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
                    AnnotationOneCoordinateShape::setUserDefaultWidth(widthValue);
                    AnnotationOneCoordinateShape::setUserDefaultHeight(heightValue);
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            
            AnnotationOneCoordinateShape::setUserDefaultHeight(value);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }

    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            AnnotationOneCoordinateShape::setUserDefaultWidth(value);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }

    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

