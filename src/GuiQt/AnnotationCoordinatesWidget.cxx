
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

#define __ANNOTATION_COORDINATES_WIDGET_DECLARE__
#include "AnnotationCoordinatesWidget.h"
#undef __ANNOTATION_COORDINATES_WIDGET_DECLARE__

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QStackedLayout>
#include <QToolButton>
#include <QToolTip>

#include "AnnotationColorBar.h"
#include "AnnotationManager.h"
#include "AnnotationCoordinate.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationScaleBar.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlaySet.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "ModelChartTwo.h"
#include "StructureEnumComboBox.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinatesWidget
 * \brief Widget for editing annotation coordinate
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *  The user input mode
 * @param parentWidgetType
 *     Type of parent widget
 * @param browserWindowIndex
 *     Index of browser window
 * @param parent
 *     Parent widget
 */
AnnotationCoordinatesWidget::AnnotationCoordinatesWidget(const UserInputModeEnum::Enum userInputMode,
                                                       const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{

    m_annotation = NULL;
    
    QString colonString;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            colonString = ":";
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    QLabel* xCoordLabel = new QLabel(" X" + colonString);
    QLabel* yCoordLabel = new QLabel(" Y" + colonString);
    m_zCoordLabel = new QLabel(" Z" + colonString);
    QLabel* surfaceVertexLabel = new QLabel("Vertex:");

    createCoordinateWidgets(0);
    createCoordinateWidgets(1);
    
    m_surfaceWidget = new QWidget();
    QGridLayout* surfaceLayout = new QGridLayout(m_surfaceWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(surfaceLayout, 2, 0);
    surfaceLayout->addWidget(surfaceVertexLabel, 0, 0);
    surfaceLayout->addWidget(m_surfaceStructureComboBox->getWidget(), 0, 1);
    surfaceLayout->addWidget(m_surfaceNodeIndexSpinBox[0], 0, 2);
    surfaceLayout->addWidget(m_surfaceNodeIndexSpinBox[1], 1, 2);
    if (m_surfaceOffsetVectorTypeComboBox != NULL) {
        surfaceLayout->addWidget(m_surfaceOffsetVectorTypeComboBox->getWidget(), 0, 3);
    }
    surfaceLayout->addWidget(m_surfaceOffsetLengthSpinBox[0], 0, 4);
    surfaceLayout->addWidget(m_surfaceOffsetLengthSpinBox[1], 1, 4);
    surfaceLayout->setColumnStretch(surfaceLayout->columnCount(), 100);

    m_coordinateWidget = new QWidget();
    QGridLayout* coordinateLayout = new QGridLayout(m_coordinateWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(coordinateLayout, 2, 0);
    coordinateLayout->addWidget(xCoordLabel, 0, 0);
    coordinateLayout->addWidget(m_xCoordSpinBox[0], 0, 1);
    coordinateLayout->addWidget(m_xCoordSpinBox[1], 1, 1);
    coordinateLayout->addWidget(yCoordLabel, 0, 2);
    coordinateLayout->addWidget(m_yCoordSpinBox[0], 0, 3);
    coordinateLayout->addWidget(m_yCoordSpinBox[1], 1, 3);
    coordinateLayout->addWidget(m_zCoordLabel, 0, 4);
    coordinateLayout->addWidget(m_zCoordSpinBox[0], 0, 5);
    coordinateLayout->addWidget(m_zCoordSpinBox[1], 1, 5);
    coordinateLayout->setColumnStretch(coordinateLayout->columnCount(), 100);

    m_stackedLayout = new QStackedLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(m_stackedLayout, 2, 2);
    m_stackedLayout->addWidget(m_surfaceWidget);
    m_stackedLayout->addWidget(m_coordinateWidget);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationCoordinatesWidget::~AnnotationCoordinatesWidget()
{
}

/**
 * Create the widgets for the given coordinate index
 * @param coordinateIndex
 *   Index of the coordinate
 */
void
AnnotationCoordinatesWidget::createCoordinateWidgets(const int32_t coordinateIndex)
{
    if (coordinateIndex == 0) {
        m_surfaceStructureComboBox = new StructureEnumComboBox(this);
        m_surfaceStructureComboBox->listOnlyValidStructures();
        m_surfaceStructureComboBox->getWidget()->setToolTip("Select surface structure");
        QObject::connect(m_surfaceStructureComboBox, SIGNAL(structureSelected(const StructureEnum::Enum)),
                         this, SLOT(valueChangedCoordinateOne()));
    }
    
    m_surfaceNodeIndexSpinBox[coordinateIndex] = new QSpinBox();
    m_surfaceNodeIndexSpinBox[coordinateIndex]->setRange(0, 1000000);
    m_surfaceNodeIndexSpinBox[coordinateIndex]->setSingleStep(1);
    m_surfaceNodeIndexSpinBox[coordinateIndex]->setToolTip("Select surface vertex");
    switch (coordinateIndex) {
        case 0:
            QObject::connect(m_surfaceNodeIndexSpinBox[coordinateIndex], SIGNAL(valueChanged(int)),
                             this, SLOT(valueChangedCoordinateOne()));
            break;
        case 1:
            QObject::connect(m_surfaceNodeIndexSpinBox[coordinateIndex], SIGNAL(valueChanged(int)),
                             this, SLOT(valueChangedCoordinateTwo()));
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    m_surfaceOffsetLengthSpinBox[coordinateIndex] = new QDoubleSpinBox();
    m_surfaceOffsetLengthSpinBox[coordinateIndex]->setRange(0.0, 999.0);
    m_surfaceOffsetLengthSpinBox[coordinateIndex]->setSingleStep(0.1);
    m_surfaceOffsetLengthSpinBox[coordinateIndex]->setToolTip("Offset of annotation from surface vertex");
    switch (coordinateIndex) {
        case 0:
            QObject::connect(m_surfaceOffsetLengthSpinBox[coordinateIndex], SIGNAL(valueChanged(double)),
                             this, SLOT(surfaceOffsetLengthValueOneChanged(double)));
            break;
        case 1:
            QObject::connect(m_surfaceOffsetLengthSpinBox[coordinateIndex], SIGNAL(valueChanged(double)),
                             this, SLOT(surfaceOffsetLengthValueTwoChanged(double)));
            break;
        default:
            CaretAssert(0);
            break;
    }

    m_xCoordSpinBox[coordinateIndex] = createCoordinateSpinBox(coordinateIndex, "X", 0);
    
    m_yCoordSpinBox[coordinateIndex] = createCoordinateSpinBox(coordinateIndex, "Y", 1);
    
    m_zCoordSpinBox[coordinateIndex] = createCoordinateSpinBox(coordinateIndex, "Z", 2);
    
    const float spinBoxMaximumWidth = 80.0f;
    m_xCoordSpinBox[coordinateIndex]->setMaximumWidth(spinBoxMaximumWidth);
    m_yCoordSpinBox[coordinateIndex]->setMaximumWidth(spinBoxMaximumWidth);
    m_zCoordSpinBox[coordinateIndex]->setMaximumWidth(spinBoxMaximumWidth);
    
    if (coordinateIndex == 0) {
        m_surfaceOffsetVectorTypeComboBox = new EnumComboBoxTemplate(this);
        m_surfaceOffsetVectorTypeComboBox->setup<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
        QObject::connect(m_surfaceOffsetVectorTypeComboBox, SIGNAL(itemActivated()),
                         this, SLOT(surfaceOffsetVectorTypeChanged()));
        m_surfaceOffsetVectorTypeComboBox->getWidget()->setFixedWidth(55);
        m_surfaceOffsetVectorTypeComboBox->getWidget()->setToolTip("Vector for surface offset:\n"
                                                                   "   C - Centroid thru Vertex, Faces Viewer\n"
                                                                   "   N - Vertex Normal, Faces Viewer\n"
                                                                   "   T - Tangent, Rotates with Surface");
    }
}

/**
 * @return Coordinate for given indx
 * @param const int32_t coordinateIndex
 *    Index of coordinate
 */
AnnotationCoordinate*
AnnotationCoordinatesWidget::getCoordinate(const int32_t coordinateIndex)
{
    AnnotationCoordinate* ac = NULL;
    
    
    if (m_annotation != NULL) {
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
        AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
        
        switch (coordinateIndex) {
            case 0:
                if (oneDimShape != NULL) {
                    ac = oneDimShape->getStartCoordinate();
                }
                else if (twoDimShape != NULL) {
                    ac = twoDimShape->getCoordinate();
                }
                break;
            case 1:
                if (oneDimShape != NULL) {
                    ac = oneDimShape->getEndCoordinate();
                }
                break;
            default:
                CaretAssert(0);
                break;
        }
    }
    
    return ac;
}


/**
 * Update with the given annotation coordinate.
 *
 * @param coordinate.
 */
void
AnnotationCoordinatesWidget::updateContent(Annotation* annotation)
{
    m_annotation = NULL;
    if (annotation != NULL) {
        if (annotation->testProperty(Annotation::Property::COORDINATE)) {
            m_annotation = annotation;
        }
    }
    
    m_surfaceStructureComboBox->listOnlyValidStructures();
    
    updateCoordinate(0,
                      getCoordinate(0));
    updateCoordinate(1,
                     getCoordinate(1));

    bool surfaceSpaceFlag(false);
    bool viewportSpaceFlag(false);
    if (m_annotation != NULL) {
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                surfaceSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                viewportSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        if (viewportSpaceFlag) {
            setEnabled(false);
        }
        else {
            setEnabled(true);
        }
    }
    else {
        setEnabled(false);
    }
    
    if (surfaceSpaceFlag) {
        m_stackedLayout->setCurrentWidget(m_surfaceWidget);
    }
    else {
        m_stackedLayout->setCurrentWidget(m_coordinateWidget);
    }
}

/**
 * Update coordinate for given index
 *
 * @param coordinateIndex
 *    Index of coordinate
 * @param coordinate
 *    The coordinate
 */
void
AnnotationCoordinatesWidget::updateCoordinate(const int32_t coordinateIndex,
                                              const AnnotationCoordinate* coordinate)
{
    bool surfaceFlag(false);
    bool xyzFlag(false);
    
    double xMin =  0.0;
    double xMax =  0.0;
    double yMin =  0.0;
    double yMax =  0.0;
    double zMin  = 0.0;
    double zMax  = 0.0;
    double xStep = 0.1;
    double yStep = 0.1;
    double zStep = 0.1;
    QString xSuffix;
    QString ySuffix;
    QString zSuffix;
    float xyz[3] { 0.0, 0.0, 0.0 };

    int32_t digitsRightOfDecimalX = 2;
    int32_t digitsRightOfDecimalY = 2;
    int32_t digitsRightOfDecimalZ = 2;

    QString zLabelText(" Z:");
    
    StructureEnum::Enum structure = StructureEnum::INVALID;
    int32_t surfaceNumberOfNodes  = -1;
    int32_t surfaceNodeIndex      = -1;
    float   surfaceOffsetLength   = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    
    if (coordinate != NULL) {
        xyzFlag = true;
        coordinate->getXYZ(xyz);
        
        const double percentageMinimum =   0.0;
        const double percentageMaximum = 100.0;
        const double zDepthMinimum =   -1000.0;
        const double zDepthMaximum =    1000.0;
        const double coordinateMinimum = -std::numeric_limits<float>::max();
        const double coordinateMaximum =  std::numeric_limits<float>::max();
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
            {
                xMin = coordinateMinimum;
                xMax = coordinateMaximum;
                yMin = coordinateMinimum;
                yMax = coordinateMaximum;
                zMin = coordinateMinimum;
                zMax = coordinateMaximum;
                digitsRightOfDecimalX = 3;
                digitsRightOfDecimalY = 3;
                xStep = 1.0;
                yStep = 1.0;
                
                BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
                CaretAssert(bbw);
                BrowserTabContent* browserTabContent = bbw->getBrowserTabContent();
                if (browserTabContent != NULL) {
                    ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
                    const int32_t tabIndex = browserTabContent->getTabNumber();
                    if (modelChartTwo != NULL) {
                        ChartTwoOverlaySet* chartOverlaySet = NULL;
                        switch (modelChartTwo->getSelectedChartTwoDataType(tabIndex)) {
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                                chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                                chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                                break;
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                                break;
                        }
                        
                        if (chartOverlaySet != NULL) {
                            float xAxisMin =  std::numeric_limits<float>::max();
                            float xAxisMax = -std::numeric_limits<float>::max();
                            float yAxisMin =  std::numeric_limits<float>::max();
                            float yAxisMax = -std::numeric_limits<float>::max();
                            
                            std::vector<ChartTwoCartesianAxis*> axes;
                            chartOverlaySet->getDisplayedChartAxes(axes);
                            const int32_t numAxes = static_cast<int32_t>(axes.size());
                            for (int32_t i = 0; i < numAxes; i++) {
                                float rangeMin(0), rangeMax(0);
                                axes[i]->getDataRange(rangeMin, rangeMax);
                                
                                if (rangeMax > rangeMin) {
                                    const ChartAxisLocationEnum::Enum axisLocation = axes[i]->getAxisLocation();
                                    switch (axisLocation) {
                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                                            xAxisMin = std::min(xAxisMin, rangeMin);
                                            xAxisMax = std::max(xAxisMax, rangeMax);
                                            break;
                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                                        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                                            yAxisMin = std::min(yAxisMin, rangeMin);
                                            yAxisMax = std::max(yAxisMax, rangeMax);
                                            break;
                                    }
                                }
                            }
                            
                            if (xAxisMax > xAxisMin) {
                                const float range(xAxisMax - xAxisMin);
                                int32_t digits = 6 - static_cast<int32_t>(std::round(std::log10(range)));
                                digitsRightOfDecimalX = MathFunctions::clamp(digits, 3, 6);
                                xStep = range * 0.001f;
                            }
                            if (yAxisMax > yAxisMin) {
                                const float range(yAxisMax - yAxisMin);
                                int32_t digits = 6 - static_cast<int32_t>(std::round(std::log10(range)));
                                digitsRightOfDecimalY = MathFunctions::clamp(digits, 3, 6);
                                yStep = range * 0.001f;
                            }
                        }
                    }
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                xMin = percentageMinimum;
                xMax = percentageMaximum;
                yMin = percentageMinimum;
                yMax = percentageMaximum;
                zMin = zDepthMinimum;
                zMax = zDepthMaximum;
                xSuffix = "%";
                ySuffix = "%";
                zSuffix = "%";
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                xMin = coordinateMinimum;
                xMax = coordinateMaximum;
                yMin = coordinateMinimum;
                yMax = coordinateMaximum;
                zMin = coordinateMinimum;
                zMax = coordinateMaximum;
                xStep = 1.0;
                yStep = 1.0;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                surfaceFlag = true;
                xyzFlag     = false;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                xMin = percentageMinimum;
                xMax = percentageMaximum;
                yMin = percentageMinimum;
                yMax = percentageMaximum;
                zMin = zDepthMinimum;
                zMax = zDepthMaximum;
                xSuffix = "%";
                ySuffix = "%";
                digitsRightOfDecimalZ = 0;
                zStep = 1.0;
                zLabelText = " O:";
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                /*
                 * Cannot move
                 */
                xMin = xyz[0];
                xMax = xyz[0];
                yMin = xyz[1];
                yMax = xyz[1];
                zMin = xyz[2];
                zMax = xyz[2];
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                xMin = percentageMinimum;
                xMax = percentageMaximum;
                yMin = percentageMinimum;
                yMax = percentageMaximum;
                zMin = zDepthMinimum;
                zMax = zDepthMaximum;
                xSuffix = "%";
                ySuffix = "%";
                digitsRightOfDecimalZ = 0;
                zStep = 1.0;
                zLabelText = " O:";
                break;
        }
    }

    m_xCoordSpinBox[coordinateIndex]->blockSignals(true);
    m_xCoordSpinBox[coordinateIndex]->setRange(xMin,
                              xMax);
    m_xCoordSpinBox[coordinateIndex]->setSingleStep(xStep);
    m_xCoordSpinBox[coordinateIndex]->setSuffix(xSuffix);
    m_xCoordSpinBox[coordinateIndex]->setValue(xyz[0]);
    m_xCoordSpinBox[coordinateIndex]->setDecimals(digitsRightOfDecimalX);
    m_xCoordSpinBox[coordinateIndex]->blockSignals(false);
    m_xCoordSpinBox[coordinateIndex]->setEnabled(xyzFlag);
    
    m_yCoordSpinBox[coordinateIndex]->blockSignals(true);
    m_yCoordSpinBox[coordinateIndex]->setRange(yMin,
                              yMax);
    m_yCoordSpinBox[coordinateIndex]->setSingleStep(yStep);
    m_yCoordSpinBox[coordinateIndex]->setSuffix(ySuffix);
    m_yCoordSpinBox[coordinateIndex]->setValue(xyz[1]);
    m_yCoordSpinBox[coordinateIndex]->setDecimals(digitsRightOfDecimalY);
    m_yCoordSpinBox[coordinateIndex]->blockSignals(false);
    m_yCoordSpinBox[coordinateIndex]->setEnabled(xyzFlag);
    
    m_zCoordSpinBox[coordinateIndex]->blockSignals(true);
    m_zCoordSpinBox[coordinateIndex]->setRange(zMin,
                              zMax);
    m_zCoordSpinBox[coordinateIndex]->setSingleStep(zStep);
    m_zCoordSpinBox[coordinateIndex]->setSuffix(zSuffix);
    m_zCoordSpinBox[coordinateIndex]->setValue(xyz[2]);
    m_zCoordSpinBox[coordinateIndex]->setDecimals(digitsRightOfDecimalZ);
    m_zCoordSpinBox[coordinateIndex]->blockSignals(false);
    m_zCoordSpinBox[coordinateIndex]->setEnabled(xyzFlag);
    if (coordinateIndex == 0) {
        m_zCoordLabel->setText(zLabelText);
    }
    
    if (m_annotation != NULL) {
        if (m_annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
            m_zCoordSpinBox[coordinateIndex]->setEnabled(false);
        }
    }
    
    if (surfaceFlag) {
        coordinate->getSurfaceSpace(structure,
                                    surfaceNumberOfNodes,
                                    surfaceNodeIndex,
                                    surfaceOffsetLength,
                                    surfaceOffsetVector);
        if (coordinateIndex == 0) {
            m_surfaceStructureComboBox->setSelectedStructure(structure);
            m_surfaceOffsetVectorTypeComboBox->setSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>(surfaceOffsetVector);
        }
        
        m_surfaceNodeIndexSpinBox[coordinateIndex]->blockSignals(true);
        m_surfaceNodeIndexSpinBox[coordinateIndex]->setValue(surfaceNodeIndex);
        m_surfaceNodeIndexSpinBox[coordinateIndex]->blockSignals(false);
        
        m_surfaceOffsetLengthSpinBox[coordinateIndex]->blockSignals(true);
        m_surfaceOffsetLengthSpinBox[coordinateIndex]->setValue(surfaceOffsetLength);
        m_surfaceOffsetLengthSpinBox[coordinateIndex]->blockSignals(false);
        
        AnnotationCoordinate::setUserDefautlSurfaceOffsetVectorType(surfaceOffsetVector);
        AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(surfaceOffsetLength);
    }
    if (coordinateIndex == 0) {
        m_surfaceStructureComboBox->getWidget()->setEnabled(surfaceFlag);
        m_surfaceOffsetVectorTypeComboBox->getWidget()->setEnabled(surfaceFlag);
    }
    m_surfaceNodeIndexSpinBox[coordinateIndex]->setEnabled(surfaceFlag);
    m_surfaceOffsetLengthSpinBox[coordinateIndex]->setEnabled(surfaceFlag);
}

/**
 * Called when surface offset one value changed.
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceOffsetLengthValueOneChanged(double value)
{
    AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(value);
    valueChangedCoordinateOne();
}

/**
 * Called when surface offset Two value changed.
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceOffsetLengthValueTwoChanged(double value)
{
    AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(value);
    valueChangedCoordinateTwo();
}

/**
 * Called when surface offset vector type is changed.
 */
void
AnnotationCoordinatesWidget::surfaceOffsetVectorTypeChanged()
{
    CaretAssert(m_surfaceOffsetVectorTypeComboBox);
    AnnotationCoordinate::setUserDefautlSurfaceOffsetVectorType(m_surfaceOffsetVectorTypeComboBox->getSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum, AnnotationSurfaceOffsetVectorTypeEnum::Enum>());
    valueChangedCoordinateOne();
    valueChangedCoordinateTwo();
}

/**
 * Gets called when a coordinate value is changed.
 */
void
AnnotationCoordinatesWidget::valueChangedCoordinateOne()
{
    valueChangedCoordinate(0);
}
/**
 * Gets called when a coordinate value is changed.
 */
void
AnnotationCoordinatesWidget::valueChangedCoordinateTwo()
{
    valueChangedCoordinate(1);
}

/**
 * Gets called when a coordinate value is changed.
 * @param coordinateIndex
 *    Index of coordinate
 */
void
AnnotationCoordinatesWidget::valueChangedCoordinate(const int32_t coordinateIndex)
{
    const AnnotationCoordinate* coordinate = getCoordinate(coordinateIndex);
    if ((m_annotation != NULL)
        && (coordinate != NULL)) {
        bool surfaceFlag = false;
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                surfaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        
        switch (m_parentWidgetType) {
            case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            {
                AnnotationCoordinate coordinateCopy(*coordinate);
                
                if (surfaceFlag) {
                    StructureEnum::Enum structure = StructureEnum::INVALID;
                    int32_t surfaceNumberOfNodes  = -1;
                    int32_t surfaceNodeIndex      = -1;
                    float surfaceOffsetLength     = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
                    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
                    coordinate->getSurfaceSpace(structure,
                                                surfaceNumberOfNodes,
                                                surfaceNodeIndex,
                                                surfaceOffsetLength,
                                                surfaceOffsetVector);
                    structure = m_surfaceStructureComboBox->getSelectedStructure();
                    surfaceOffsetVector = m_surfaceOffsetVectorTypeComboBox->getSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum, AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
                    surfaceNodeIndex = m_surfaceNodeIndexSpinBox[coordinateIndex]->value();
                    surfaceOffsetLength = m_surfaceOffsetLengthSpinBox[coordinateIndex]->value();
                    
                    coordinateCopy.setSurfaceSpace(structure,
                                                   surfaceNumberOfNodes,
                                                   surfaceNodeIndex,
                                                   surfaceOffsetLength,
                                                   surfaceOffsetVector);
                }
                else {
                    float xyz[3] = {
                        (float)m_xCoordSpinBox[coordinateIndex]->value(),
                        (float)m_yCoordSpinBox[coordinateIndex]->value(),
                        (float)m_zCoordSpinBox[coordinateIndex]->value()
                    };
                    coordinateCopy.setXYZ(xyz);
                }
                
                std::vector<Annotation*> selectedAnnotations;
                selectedAnnotations.push_back(m_annotation);
                
                bool updateUserInterfaceFlag = false;
                for (std::vector<Annotation*>::iterator annIter = selectedAnnotations.begin();
                     annIter != selectedAnnotations.end();
                     annIter++) {
                    Annotation* ann = *annIter;
                    if (ann->getType() == AnnotationTypeEnum::COLOR_BAR) {
                        AnnotationColorBar* colorBar = dynamic_cast<AnnotationColorBar*>(ann);
                        CaretAssert(colorBar);
                        colorBar->setPositionMode(AnnotationColorBarPositionModeEnum::MANUAL);
                        updateUserInterfaceFlag = true;
                    }
                    else if (ann->getType() == AnnotationTypeEnum::SCALE_BAR) {
                        AnnotationScaleBar* scaleBar = ann->castToScaleBar();
                        CaretAssert(scaleBar);
                        scaleBar->setPositionMode(AnnotationColorBarPositionModeEnum::MANUAL);
                        updateUserInterfaceFlag = true;
                    }
                }
                
                AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                switch (coordinateIndex) {
                    case 0:
                        undoCommand->setModeCoordinateOne(coordinateCopy,
                                                          selectedAnnotations);
                        break;
                    case 1:
                        undoCommand->setModeCoordinateTwo(coordinateCopy,
                                                          selectedAnnotations);
                        break;
                    default:
                        CaretAssert(0);
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
                
                EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
                if (updateUserInterfaceFlag) {
                    EventManager::get()->sendEvent(EventOverlaySettingsEditorDialogRequest(EventOverlaySettingsEditorDialogRequest::MODE_UPDATE_ALL,
                                                                                           m_browserWindowIndex,
                                                                                           NULL,
                                                                                           (CaretMappableDataFile*)NULL,
                                                                                           -1).getPointer());
                }
            }
                break;
            case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
                CaretAssert(0);
                break;
        }
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * @return Spin box with axis character in tool tip
 * @param coordinateIndex
 *     Index of coordinate
 * @param axisCharacter
 *     Character for axis
 * @param xyzIndex
 *     Index 0 (X), 1 (Y), 2(Z)
 */
QDoubleSpinBox*
AnnotationCoordinatesWidget::createCoordinateSpinBox(const int32_t coordinateIndex,
                                                     const QString& axisCharacter,
                                                     const int32_t xyzIndex)
{
    const int digitsRightOfDecimal = 1;
    
    QDoubleSpinBox* sb(NULL);
    switch (coordinateIndex) {
        case 0:
            sb = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                                100.0,
                                                                                0.1,
                                                                                digitsRightOfDecimal,
                                                                                this,
                                                                                SLOT(valueChangedCoordinateOne()));
            break;
        case 1:
            sb = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                                100.0,
                                                                                0.1,
                                                                                digitsRightOfDecimal,
                                                                                this,
                                                                                SLOT(valueChangedCoordinateTwo()));
            break;
        default:
            break;
    }
    
    AString tabWindowText;
    AString coordOrderText;
    switch (xyzIndex) {
        case 0:
            coordOrderText = (axisCharacter + "-Coordinate");
            tabWindowText = ("      0.0% => Left side of tab/window\n"
                             "      100.0% => Right side of tab/window");
            break;
        case 1:
            coordOrderText = (axisCharacter + "-Coordinate");
            tabWindowText = ("      0.0% => Bottom of tab/window\n"
                             "      100.0% => Top of tab/window");
            break;
        case 2:
            coordOrderText = "Order";
            tabWindowText = ("      0 => Closer to viewer\n"
                             "      100 => Further from viewer");
            break;
    }
    
    CaretAssert(sb);
    WuQtUtilities::setWordWrappedToolTip(sb,
                                         "   STEREOTAXIC: " + axisCharacter + "-Coordinate\n"
                                         "   TAB and WINDOW: " + coordOrderText + "\n"
                                         + tabWindowText);
    return sb;
}

