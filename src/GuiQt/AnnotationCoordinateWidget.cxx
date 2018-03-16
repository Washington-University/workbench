
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

#define __ANNOTATION_COORDINATE_WIDGET_DECLARE__
#include "AnnotationCoordinateWidget.h"
#undef __ANNOTATION_COORDINATE_WIDGET_DECLARE__

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QToolButton>
#include <QToolTip>

#include "AnnotationColorBar.h"
#include "AnnotationManager.h"
#include "AnnotationCoordinate.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
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
 * \class caret::AnnotationCoordinateWidget 
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
AnnotationCoordinateWidget::AnnotationCoordinateWidget(const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                       const WhichCoordinate whichCoordinate,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_parentWidgetType(parentWidgetType),
m_whichCoordinate(whichCoordinate),
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
    
    QLabel* surfaceVertexLabel = new QLabel("Vertex:");
    m_surfaceStructureComboBox = new StructureEnumComboBox(this);
    m_surfaceStructureComboBox->listOnlyValidStructures();
    m_surfaceStructureComboBox->getWidget()->setToolTip("Select surface structure");
    QObject::connect(m_surfaceStructureComboBox, SIGNAL(structureSelected(const StructureEnum::Enum)),
                     this, SLOT(valueChanged()));
    
    m_surfaceNodeIndexSpinBox = new QSpinBox();
    m_surfaceNodeIndexSpinBox->setRange(0, 1000000);
    m_surfaceNodeIndexSpinBox->setSingleStep(1);
    m_surfaceNodeIndexSpinBox->setToolTip("Select surface vertex");
    QObject::connect(m_surfaceNodeIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(valueChanged()));
    
    m_surfaceOffsetLengthSpinBox = new QDoubleSpinBox();
    m_surfaceOffsetLengthSpinBox->setRange(0.0, 999.0);
    m_surfaceOffsetLengthSpinBox->setSingleStep(1.0);
    m_surfaceOffsetLengthSpinBox->setToolTip("Offset of annotation from surface vertex");
    QObject::connect(m_surfaceOffsetLengthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(valueChanged()));
    
    const int digitsRightOfDecimal = 1;
    QLabel* xCoordLabel = new QLabel(" X" + colonString);
    m_xCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 100.0, 0.1, digitsRightOfDecimal,
                                                                                     this, SLOT(valueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_xCoordSpinBox,
                                         "X-coordinate of annotation\n"
                                         "   STEREOTAXIC: Stereotaxic X-Coordinate\n"
                                         "   TAB and WINDOW X-Range: [0.0%, 100.0%]\n"
                                         "      0.0% => Left side of tab/window\n"
                                         "      100.0% => Right side of tab/window\n");
    
    QLabel* yCoordLabel = new QLabel(" Y" + colonString);
    m_yCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 100.0, 0.1, digitsRightOfDecimal,
                                                                                     this, SLOT(valueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_yCoordSpinBox,
                                         "Y-coordinate of annotation\n"
                                         "   STEREOTAXIC: Stereotaxic Y-Coordinate\n"
                                         "   TAB and WINDOW Y-Range: [0.0%, 100.0%]\n"
                                         "      0.0% => Bottom of tab/window\n"
                                         "      100.0% => Top of tab/window\n");
    
    QLabel* zCoordLabel = new QLabel(" Z" + colonString);
    m_zCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-100.0, 100.0, 0.1, digitsRightOfDecimal,
                                                                                     this, SLOT(valueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_zCoordSpinBox,
                                         "Z-coordinate of annotation\n"
                                         "   STEREOTAXIC: Stereotaxic Z-Coordinate\n"
                                         "   TAB and WINDOW DEPTH Range: [0.0%, 100.0%]\n"
                                         "         0.0% => Toward's viewer\n"
                                         "       100.0% => Away from viewer\n");

    const float spinBoxMaximumWidth = 80.0f;
    m_xCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    m_yCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    m_zCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    
    m_surfaceOffsetVectorTypeComboBox = new EnumComboBoxTemplate(this);
    m_surfaceOffsetVectorTypeComboBox->setup<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
    QObject::connect(m_surfaceOffsetVectorTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(valueChanged()));
    m_surfaceOffsetVectorTypeComboBox->getWidget()->setFixedWidth(45);
    m_surfaceOffsetVectorTypeComboBox->getWidget()->setToolTip("Vector for surface offset:\n"
                                                               "   C - Centroid thru Vertex\n"
                                                               "   N - Vertex Normal");
    
    m_plusButtonToolTipText = ("Click the mouse to set the new location for the coordinate.\n"
                               "After clicking the mouse, a dialog allows selection of the\n"
                               "coordinate space.");
    

    QToolButton* setCoordinateToolButton = NULL;
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            /** disabled as change space cause grouping problems.   setCoordinateToolButton = new QToolButton(); */
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
    
    if (setCoordinateToolButton != NULL) {
        QAction* setCoordinateAction = WuQtUtilities::createAction("+",
                                                                   ("After pressing this button: \n"
                                                                    + m_plusButtonToolTipText),
                                                                   this,
                                                                   this,
                                                                   SLOT(setCoordinateActionTriggered()));
        setCoordinateToolButton->setDefaultAction(setCoordinateAction);
        WuQtUtilities::setToolButtonStyleForQt5Mac(setCoordinateToolButton);
    }
    
    m_surfaceWidget = new QWidget();
    QHBoxLayout* surfaceLayout = new QHBoxLayout(m_surfaceWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(surfaceLayout, 2, 0);
    surfaceLayout->addWidget(surfaceVertexLabel);
    surfaceLayout->addWidget(m_surfaceStructureComboBox->getWidget());
    surfaceLayout->addWidget(m_surfaceNodeIndexSpinBox);
    surfaceLayout->addWidget(m_surfaceOffsetVectorTypeComboBox->getWidget());
    surfaceLayout->addWidget(m_surfaceOffsetLengthSpinBox);
    
    m_coordinateWidget = new QWidget();
    QHBoxLayout* coordinateLayout = new QHBoxLayout(m_coordinateWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(coordinateLayout, 2, 0);
    coordinateLayout->addWidget(xCoordLabel);
    coordinateLayout->addWidget(m_xCoordSpinBox);
    coordinateLayout->addWidget(yCoordLabel);
    coordinateLayout->addWidget(m_yCoordSpinBox);
    coordinateLayout->addWidget(zCoordLabel);
    coordinateLayout->addWidget(m_zCoordSpinBox);
    
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(m_surfaceWidget);
    layout->addWidget(m_coordinateWidget);
    if (setCoordinateToolButton != NULL) {
        layout->addWidget(setCoordinateToolButton);
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationCoordinateWidget::~AnnotationCoordinateWidget()
{
}

AnnotationCoordinate*
AnnotationCoordinateWidget::getCoordinate()
{
    AnnotationCoordinate* ac = NULL;
    
    
    if (m_annotation != NULL) {
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
        AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
        
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
    }
    
    return ac;
}


/**
 * Update with the given annotation coordinate.
 *
 * @param coordinate.
 */
void
AnnotationCoordinateWidget::updateContent(Annotation* annotation)
{
    m_annotation = NULL;
    if (annotation != NULL) {
        if (annotation->testProperty(Annotation::Property::COORDINATE)) {
            m_annotation = annotation;
        }
    }
    
    m_surfaceStructureComboBox->listOnlyValidStructures();
    
    const AnnotationCoordinate* coordinate = getCoordinate();
    
    bool surfaceFlag    = false;
    
    if (coordinate != NULL) {
        float xyz[3];
        coordinate->getXYZ(xyz);
        
        bool viewportSpaceFlag = false;
        const double percentageMinimum =   0.0;
        const double percentageMaximum = 100.0;
        const double zDepthMinimum =       0.0;
        const double zDepthMaximum =     100.0;
        const double coordinateMinimum = -std::numeric_limits<float>::max();
        const double coordinateMaximum =  std::numeric_limits<float>::max();
        double xMin =  0.0;
        double xMax =  0.0;
        double yMin =  0.0;
        double yMax =  0.0;
        double zMin  = 0.0;
        double zMax  = 0.0;
        double xStep = 0.1;
        double yStep = 0.1;
        double zStep = 0.1;
        QString suffix;
        int32_t digitsRightOfDecimalX = 2;
        int32_t digitsRightOfDecimalY = 2;
        int32_t digitsRightOfDecimalZ = 2;
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
                digitsRightOfDecimalX = 3;
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
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                xMin = percentageMinimum;
                xMax = percentageMaximum;
                yMin = percentageMinimum;
                yMax = percentageMaximum;
                zMin = zDepthMinimum;
                zMax = zDepthMaximum;
                suffix = "%";
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
                viewportSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                xMin = percentageMinimum;
                xMax = percentageMaximum;
                yMin = percentageMinimum;
                yMax = percentageMaximum;
                zMin = zDepthMinimum;
                zMax = zDepthMaximum;
                suffix = "%";
                break;
        }
        
        m_xCoordSpinBox->blockSignals(true);
        m_xCoordSpinBox->setRange(xMin,
                                  xMax);
        m_xCoordSpinBox->setSingleStep(xStep);
        m_xCoordSpinBox->setSuffix(suffix);
        m_xCoordSpinBox->setValue(xyz[0]);
        m_xCoordSpinBox->setDecimals(digitsRightOfDecimalX);
        m_xCoordSpinBox->blockSignals(false);
        
        m_yCoordSpinBox->blockSignals(true);
        m_yCoordSpinBox->setRange(yMin,
                                  yMax);
        m_yCoordSpinBox->setSingleStep(yStep);
        m_yCoordSpinBox->setSuffix(suffix);
        m_yCoordSpinBox->setValue(xyz[1]);
        m_yCoordSpinBox->setDecimals(digitsRightOfDecimalY);
        m_yCoordSpinBox->blockSignals(false);
        
        m_zCoordSpinBox->blockSignals(true);
        m_zCoordSpinBox->setRange(zMin,
                                  zMax);
        m_zCoordSpinBox->setSingleStep(zStep);
        m_zCoordSpinBox->setSuffix(suffix);
        m_zCoordSpinBox->setValue(xyz[2]);
        m_zCoordSpinBox->setDecimals(digitsRightOfDecimalZ);
        m_zCoordSpinBox->blockSignals(false);
        
        if (surfaceFlag) {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfNodes  = -1;
            int32_t surfaceNodeIndex      = -1;
            float   surfaceOffsetLength   = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
            AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
            coordinate->getSurfaceSpace(structure,
                                        surfaceNumberOfNodes,
                                        surfaceNodeIndex,
                                        surfaceOffsetLength,
                                        surfaceOffsetVector);
            m_surfaceStructureComboBox->setSelectedStructure(structure);
            m_surfaceNodeIndexSpinBox->blockSignals(true);
            m_surfaceNodeIndexSpinBox->setValue(surfaceNodeIndex);
            m_surfaceNodeIndexSpinBox->blockSignals(false);

            m_surfaceOffsetVectorTypeComboBox->setSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>(surfaceOffsetVector);
            
            m_surfaceOffsetLengthSpinBox->blockSignals(true);
            m_surfaceOffsetLengthSpinBox->setValue(surfaceOffsetLength);
            m_surfaceOffsetLengthSpinBox->blockSignals(false);
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
    
    m_surfaceWidget->setVisible(surfaceFlag);
    m_coordinateWidget->setVisible( ! surfaceFlag);
}

/**
 * Gets called when a coordinate value is changed.
 */
void
AnnotationCoordinateWidget::valueChanged()
{
    const AnnotationCoordinate* coordinate = getCoordinate();
    if ((m_annotation != NULL)
        && (coordinate != NULL)) {
        bool surfaceFlag = false;
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
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
                    surfaceNodeIndex = m_surfaceNodeIndexSpinBox->value();
                    surfaceOffsetLength = m_surfaceOffsetLengthSpinBox->value();
                    
                    surfaceOffsetVector = m_surfaceOffsetVectorTypeComboBox->getSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum, AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
                    
                    coordinateCopy.setSurfaceSpace(structure,
                                                   surfaceNumberOfNodes,
                                                   surfaceNodeIndex,
                                                   surfaceOffsetLength,
                                                   surfaceOffsetVector);
                }
                else {
                    float xyz[3] = {
                        (float)m_xCoordSpinBox->value(),
                        (float)m_yCoordSpinBox->value(),
                        (float)m_zCoordSpinBox->value()
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
                }
                
                AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                switch (m_whichCoordinate) {
                    case COORDINATE_ONE:
                        undoCommand->setModeCoordinateOne(coordinateCopy,
                                                          selectedAnnotations);
                        break;
                    case COORDINATE_TWO:
                        undoCommand->setModeCoordinateTwo(coordinateCopy,
                                                          selectedAnnotations);
                        break;
                }
                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();

                AString errorMessage;
                if ( ! annMan->applyCommand(undoCommand,
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
 * Called when the set coordinate button is clicked.
 */
void
AnnotationCoordinateWidget::setCoordinateActionTriggered()
{
    QPoint middlePos(width() / 2,
                     height() / 2);
    QToolTip::showText(mapToGlobal(middlePos),
                       m_plusButtonToolTipText,
                       this);
    
    signalSelectCoordinateWithMouse();
}
