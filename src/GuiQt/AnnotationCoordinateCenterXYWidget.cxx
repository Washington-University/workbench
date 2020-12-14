
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

#define __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_DECLARE__
#include "AnnotationCoordinateCenterXYWidget.h"
#undef __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_DECLARE__

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QToolButton>

#include "AnnotationColorBar.h"
#include "AnnotationManager.h"
#include "AnnotationCoordinate.h"
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationOneCoordinateShape.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlaySet.h"
#include "EventBrowserWindowContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "ModelChartTwo.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinateCenterXYWidget
 * \brief Widget for editing annotation coordinate
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param
 * @param whichCoordinate
 *     Which coordinate, one (or only), or two
 * @param browserWindowIndex
 *     Index of browser window
 * @param parent
 *     Parent widget
 */
AnnotationCoordinateCenterXYWidget::AnnotationCoordinateCenterXYWidget(const UserInputModeEnum::Enum userInputMode,
                                                       const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                       const WhichCoordinate whichCoordinate,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
m_whichCoordinate(whichCoordinate),
m_browserWindowIndex(browserWindowIndex)
{

    QString colonString;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            colonString = ":";
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    QLabel* centerLabel = new QLabel("Center");
    const int digitsRightOfDecimal = 1;
    QLabel* xCoordLabel = new QLabel(" X" + colonString);
    m_xCoordSpinBox = new QDoubleSpinBox();
    m_xCoordSpinBox->setMinimum(-100.0);
    m_xCoordSpinBox->setMaximum( 200.0);
    m_xCoordSpinBox->setSingleStep(0.1);
    m_xCoordSpinBox->setDecimals(digitsRightOfDecimal);
    QObject::connect(m_xCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinateCenterXYWidget::xValueChanged);
    WuQtUtilities::setWordWrappedToolTip(m_xCoordSpinBox,
                                         "X-coordinate of annotation\n"
                                         "   STEREOTAXIC: Stereotaxic X-Coordinate\n"
                                         "   TAB and WINDOW X-Range: [0.0%, 100.0%]\n"
                                         "      0.0% => Left side of tab/window\n"
                                         "      100.0% => Right side of tab/window\n");
    
    QLabel* yCoordLabel = new QLabel(" Y" + colonString);
    m_yCoordSpinBox = new QDoubleSpinBox();
    m_yCoordSpinBox->setMinimum(-100.0);
    m_yCoordSpinBox->setMaximum( 200.0);
    m_yCoordSpinBox->setSingleStep(0.1);
    m_yCoordSpinBox->setDecimals(digitsRightOfDecimal);
    QObject::connect(m_yCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinateCenterXYWidget::yValueChanged);
    WuQtUtilities::setWordWrappedToolTip(m_yCoordSpinBox,
                                         "Y-coordinate of annotation\n"
                                         "   STEREOTAXIC: Stereotaxic Y-Coordinate\n"
                                         "   TAB and WINDOW Y-Range: [0.0%, 100.0%]\n"
                                         "      0.0% => Bottom of tab/window\n"
                                         "      100.0% => Top of tab/window\n");
    

    const float spinBoxMaximumWidth = 80.0f;
    m_xCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    m_yCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    int32_t row(0);
    layout->addWidget(centerLabel, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    layout->addWidget(xCoordLabel, row, 0);
    layout->addWidget(m_xCoordSpinBox, row, 1);
    row++;
    layout->addWidget(yCoordLabel, row, 0);
    layout->addWidget(m_yCoordSpinBox, row, 1);

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationCoordinateCenterXYWidget::~AnnotationCoordinateCenterXYWidget()
{
}

/**
 * Update with the given annotation coordinate.
 *
 * @param coordinate.
 */
void
AnnotationCoordinateCenterXYWidget::updateContent(std::vector<Annotation*>& selectedAnnotations)
{
    m_annotations = selectedAnnotations;
    
    bool firstFlag(true);
    bool haveMultipleXValuesFlag(false);
    bool haveMultipleYValuesFlag(false);
    float xValue(0.0);
    float yValue(0.0);
    
    if (! m_annotations.empty()) {
        for (auto ann : m_annotations) {
            AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(ann);
            AnnotationOneCoordinateShape* twoDimShape = dynamic_cast<AnnotationOneCoordinateShape*>(ann);
            
            AnnotationCoordinate* ac(NULL);
            switch (m_whichCoordinate) {
                case COORDINATE_ONE:
                    if (oneDimShape != NULL) {
                        ac = oneDimShape->getStartCoordinate();
                    }
                    else if (twoDimShape != NULL) {
                        ac = twoDimShape->getCoordinate();
                    }
                    break;
                case COORDINATE_TWO:
                    if (oneDimShape != NULL) {
                        ac = oneDimShape->getEndCoordinate();
                    }
                    break;
            }
            CaretAssert(ac);
            
            float xyz[3];
            ac->getXYZ(xyz);
            const float x = xyz[0];
            const float y = xyz[1];
            
            if (firstFlag) {
                xValue = x;
                yValue = y;
                firstFlag = false;
            }
            else {
                if (x != xValue) {
                    haveMultipleXValuesFlag = true;
                }
                if (y != yValue) {
                    haveMultipleYValuesFlag = true;
                }
                
                xValue = std::min(xValue, x);
                yValue = std::min(yValue, y);
            }
        }

        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
    
    QSignalBlocker xBlocker(m_xCoordSpinBox);
    m_xCoordSpinBox->setValue(xValue);
    if (haveMultipleXValuesFlag) {
        m_xCoordSpinBox->setSuffix("%+");
    }
    else {
        m_xCoordSpinBox->setSuffix("%");
    }
    
    QSignalBlocker yBlocker(m_yCoordSpinBox);
    m_yCoordSpinBox->setValue(yValue);
    if (haveMultipleYValuesFlag) {
        m_yCoordSpinBox->setSuffix("%+");
    }
    else {
        m_yCoordSpinBox->setSuffix("%");
    }
    

//    m_annotation = NULL;
//    if (selectedAnnotations.size() == 1) {
//        CaretAssertVectorIndex(selectedAnnotations, 0);
//        Annotation* annotation(selectedAnnotations[0]);
//        if (annotation->testProperty(Annotation::Property::COORDINATE)) {
//            m_annotation = annotation;
//        }
//    }
//
//    const AnnotationCoordinate* coordinate = getCoordinate();
//
//    if (coordinate != NULL) {
//        float xyz[3];
//        coordinate->getXYZ(xyz);
//
//        bool viewportSpaceFlag = false;
//        const double percentageMinimum =   0.0;
//        const double percentageMaximum = 100.0;
//        const double coordinateMinimum = -std::numeric_limits<float>::max();
//        const double coordinateMaximum =  std::numeric_limits<float>::max();
//        double xMin =  0.0;
//        double xMax =  0.0;
//        double yMin =  0.0;
//        double yMax =  0.0;
//        double xStep = 0.1;
//        double yStep = 0.1;
//        QString suffix;
//        int32_t digitsRightOfDecimalX = 2;
//        int32_t digitsRightOfDecimalY = 2;
//        switch (m_annotation->getCoordinateSpace()) {
//            case AnnotationCoordinateSpaceEnum::CHART:
//            {
//                xMin = coordinateMinimum;
//                xMax = coordinateMaximum;
//                yMin = coordinateMinimum;
//                yMax = coordinateMaximum;
//                digitsRightOfDecimalX = 3;
//                digitsRightOfDecimalX = 3;
//                xStep = 1.0;
//                yStep = 1.0;
//
//                BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
//                CaretAssert(bbw);
//                BrowserTabContent* browserTabContent = bbw->getBrowserTabContent();
//                if (browserTabContent != NULL) {
//                    ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
//                    const int32_t tabIndex = browserTabContent->getTabNumber();
//                    if (modelChartTwo != NULL) {
//                        ChartTwoOverlaySet* chartOverlaySet = NULL;
//                        switch (modelChartTwo->getSelectedChartTwoDataType(tabIndex)) {
//                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                                break;
//                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
//                                chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
//                                break;
//                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
//                                chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
//                                break;
//                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
//                                break;
//                        }
//
//                        if (chartOverlaySet != NULL) {
//                            float xAxisMin =  std::numeric_limits<float>::max();
//                            float xAxisMax = -std::numeric_limits<float>::max();
//                            float yAxisMin =  std::numeric_limits<float>::max();
//                            float yAxisMax = -std::numeric_limits<float>::max();
//
//                            std::vector<ChartTwoCartesianAxis*> axes;
//                            chartOverlaySet->getDisplayedChartAxes(axes);
//                            const int32_t numAxes = static_cast<int32_t>(axes.size());
//                            for (int32_t i = 0; i < numAxes; i++) {
//                                float rangeMin(0), rangeMax(0);
//                                axes[i]->getDataRange(rangeMin, rangeMax);
//
//                                if (rangeMax > rangeMin) {
//                                    const ChartAxisLocationEnum::Enum axisLocation = axes[i]->getAxisLocation();
//                                    switch (axisLocation) {
//                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
//                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
//                                            xAxisMin = std::min(xAxisMin, rangeMin);
//                                            xAxisMax = std::max(xAxisMax, rangeMax);
//                                            break;
//                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
//                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
//                                            yAxisMin = std::min(yAxisMin, rangeMin);
//                                            yAxisMax = std::max(yAxisMax, rangeMax);
//                                            break;
//                                    }
//                                }
//                            }
//
//                            if (xAxisMax > xAxisMin) {
//                                const float range(xAxisMax - xAxisMin);
//                                int32_t digits = 6 - static_cast<int32_t>(std::round(std::log10(range)));
//                                digitsRightOfDecimalX = MathFunctions::clamp(digits, 3, 6);
//                                xStep = range * 0.001f;
//                            }
//                            if (yAxisMax > yAxisMin) {
//                                const float range(yAxisMax - yAxisMin);
//                                int32_t digits = 6 - static_cast<int32_t>(std::round(std::log10(range)));
//                                digitsRightOfDecimalY = MathFunctions::clamp(digits, 3, 6);
//                                yStep = range * 0.001f;
//                            }
//                        }
//                    }
//                }
//            }
//                break;
//            case AnnotationCoordinateSpaceEnum::SPACER:
//                xMin = percentageMinimum;
//                xMax = percentageMaximum;
//                yMin = percentageMinimum;
//                yMax = percentageMaximum;
//                suffix = "%";
//                break;
//            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
//                xMin = coordinateMinimum;
//                xMax = coordinateMaximum;
//                yMin = coordinateMinimum;
//                yMax = coordinateMaximum;
//                xStep = 1.0;
//                yStep = 1.0;
//                break;
//            case AnnotationCoordinateSpaceEnum::SURFACE:
//                break;
//            case AnnotationCoordinateSpaceEnum::TAB:
//                xMin = percentageMinimum;
//                xMax = percentageMaximum;
//                yMin = percentageMinimum;
//                yMax = percentageMaximum;
//                suffix = "%";
//                break;
//            case AnnotationCoordinateSpaceEnum::VIEWPORT:
//                /*
//                 * Cannot move
//                 */
//                xMin = xyz[0];
//                xMax = xyz[0];
//                yMin = xyz[1];
//                yMax = xyz[1];
//                viewportSpaceFlag = true;
//                break;
//            case AnnotationCoordinateSpaceEnum::WINDOW:
//                xMin = percentageMinimum;
//                xMax = percentageMaximum;
//                yMin = percentageMinimum;
//                yMax = percentageMaximum;
//                suffix = "%";
//                break;
//        }
//
//        m_xCoordSpinBox->blockSignals(true);
//        m_xCoordSpinBox->setRange(xMin,
//                                  xMax);
//        m_xCoordSpinBox->setSingleStep(xStep);
//        m_xCoordSpinBox->setSuffix(suffix);
//        m_xCoordSpinBox->setValue(xyz[0]);
//        m_xCoordSpinBox->setDecimals(digitsRightOfDecimalX);
//        m_xCoordSpinBox->blockSignals(false);
//
//        m_yCoordSpinBox->blockSignals(true);
//        m_yCoordSpinBox->setRange(yMin,
//                                  yMax);
//        m_yCoordSpinBox->setSingleStep(yStep);
//        m_yCoordSpinBox->setSuffix(suffix);
//        m_yCoordSpinBox->setValue(xyz[1]);
//        m_yCoordSpinBox->setDecimals(digitsRightOfDecimalY);
//        m_yCoordSpinBox->blockSignals(false);
//
//        if (viewportSpaceFlag) {
//            setEnabled(false);
//        }
//        else {
//            setEnabled(true);
//        }
//    }
//    else {
//        setEnabled(false);
//    }
}

/**
 * Called when the X-value is changed
 *
 * @param value
 *     New value
 */
void
AnnotationCoordinateCenterXYWidget::xValueChanged(double value)
{
    processValueChanged(m_xCoordSpinBox,
                        value);
}

/**
 * Called when the Y-value is changed
 *
 * @param value
 *     New value
 */
void
AnnotationCoordinateCenterXYWidget::yValueChanged(double value)
{
    processValueChanged(m_yCoordSpinBox,
                        value);
}

/**
 * Called when an X- or Y-Value is changed
 *
 * @param spinBox
 *     The spin box that was changed
 * @param value
 *     The new value
 */
void
AnnotationCoordinateCenterXYWidget::processValueChanged(QDoubleSpinBox* spinBox,
                                                        const double value)
{
    if (! m_annotations.empty()) {
        for (auto ann : m_annotations) {
            AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(ann);
            AnnotationOneCoordinateShape* twoDimShape = dynamic_cast<AnnotationOneCoordinateShape*>(ann);
            
            AnnotationCoordinate* ac(NULL);
            switch (m_whichCoordinate) {
                case COORDINATE_ONE:
                    if (oneDimShape != NULL) {
                        ac = oneDimShape->getStartCoordinate();
                    }
                    else if (twoDimShape != NULL) {
                        ac = twoDimShape->getCoordinate();
                    }
                    break;
                case COORDINATE_TWO:
                    if (oneDimShape != NULL) {
                        ac = oneDimShape->getEndCoordinate();
                    }
                    break;
            }
            CaretAssert(ac);
            
            
            AnnotationCoordinate coordinateCopy(*ac);
            float xyz[3];
            coordinateCopy.getXYZ(xyz);
            
            if (spinBox == m_xCoordSpinBox) {
                xyz[0] = value;
            }
            else if (spinBox == m_yCoordSpinBox) {
                xyz[1] = value;
            }
            coordinateCopy.setXYZ(xyz);
            
            std::vector<Annotation*> annForCommand { ann };
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            switch (m_whichCoordinate) {
                case COORDINATE_ONE:
                    undoCommand->setModeCoordinateOne(coordinateCopy,
                                                      annForCommand);
                    break;
                case COORDINATE_TWO:
                    undoCommand->setModeCoordinateTwo(coordinateCopy,
                                                      annForCommand);
                    break;
            }
            
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
            
            AString errorMessage;
            if ( ! annMan->applyCommand(m_userInputMode,
                                        undoCommand,
                                        errorMessage)) {
                WuQMessageBox::errorOk(this,
                                       errorMessage);
            }
        }

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    }
}

