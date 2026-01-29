
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
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationScaleBar.h"
#include "AnnotationOneCoordinateShape.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlaySet.h"
#include "CziImageFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventAnnotationCoordinateSelected.h"
#include "EventBrowserWindowContent.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventOverlaySettingsEditorDialogRequest.h"
#include "GuiManager.h"
#include "HistologyOverlaySet.h"
#include "HistologySlicesFile.h"
#include "MathFunctions.h"
#include "MediaFile.h"
#include "MediaOverlaySet.h"
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

    QLabel* coordNumberLabel(new QLabel("Coord Num"));
    m_coordNumberSpinBox = new QSpinBox();
    m_coordNumberSpinBox->setFixedWidth(80);
    m_coordNumberSpinBox->setToolTip("Select coordinate number for editing");
    QObject::connect(m_coordNumberSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::coordNumberSpinBoxValueChanged);

    QVBoxLayout* coordNumberLayout(new QVBoxLayout());
    coordNumberLayout->addWidget(coordNumberLabel, 0, Qt::AlignHCenter);
    coordNumberLayout->addWidget(m_coordNumberSpinBox, 0, Qt::AlignHCenter);
    coordNumberLayout->addStretch();
    
    m_coordSurfaceWidget = createCoordinateWidgetSurface();
    m_coordXyzWidget     = createCoordinateWidgetXYZ();
    
    m_stackedLayout = new QStackedLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_stackedLayout, 2, 2);
    m_stackedLayout->addWidget(m_coordSurfaceWidget);
    m_stackedLayout->addWidget(m_coordXyzWidget);

    QHBoxLayout* layout(new QHBoxLayout(this));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addLayout(coordNumberLayout, 0);
    layout->addSpacing(10);
    layout->addLayout(m_stackedLayout, 100);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            EventManager::get()->addEventListener(this,
                                                  EventTypeEnum::EVENT_ANNOTATION_COORDINATE_SELECTED);
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            CaretAssert(0);
            break;
    }
}

/**
 * Destructor.
 */
AnnotationCoordinatesWidget::~AnnotationCoordinatesWidget()
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
AnnotationCoordinatesWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_COORDINATE_SELECTED) {
        EventAnnotationCoordinateSelected* coordEvent(dynamic_cast<EventAnnotationCoordinateSelected*>(event));
        CaretAssert(coordEvent);
        
        if (m_annotation != NULL) {
            if (m_annotation == coordEvent->getAnnotation()) {
                const int32_t coordIndex(coordEvent->getCoordinateIndex());
                if (coordIndex >= 0) {
                    int32_t coordNumber(coordIndex + 1);
                    if ((coordNumber >= m_coordNumberSpinBox->minimum())
                        && (coordNumber <= m_coordNumberSpinBox->maximum())) {
                        /*
                         * After the coordinate selected event is isused
                         * an event is issued to update the toolbar
                         */
                        QSignalBlocker blocker(m_coordNumberSpinBox);
                        m_coordNumberSpinBox->setValue(coordNumber);
                        
                        coordEvent->setEventProcessed();
                    }
                }
            }
        }
    }
}

/**
 * @return new instance of widget for XYZ coordinates
 */
QWidget*
AnnotationCoordinatesWidget::createCoordinateWidgetXYZ()
{
    QLabel* xCoordLabel = new QLabel("X");
    QLabel* yCoordLabel = new QLabel("Y");
    m_zCoordLabel = new QLabel("Z");

    m_xCoordSpinBox = createCoordinateSpinBox("X", 0);
    
    m_yCoordSpinBox = createCoordinateSpinBox("Y", 1);
    
    m_zCoordSpinBox = createCoordinateSpinBox("Z", 2);
    
    if (m_userInputMode == UserInputModeEnum::Enum::SAMPLES_EDITING) {
        const int32_t spinBoxWidth(90);
        m_xCoordSpinBox->setFixedWidth(spinBoxWidth);
        m_yCoordSpinBox->setFixedWidth(spinBoxWidth);
        m_zCoordSpinBox->setFixedWidth(spinBoxWidth);
    }
    else {
        const float spinBoxMaximumWidth = 80.0f;
        m_xCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
        m_yCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
        m_zCoordSpinBox->setMaximumWidth(spinBoxMaximumWidth);
    }

    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->setRowStretch(100, 100); /* push widgets up, space at bottom */
    layout->addWidget(xCoordLabel, 0, 0, Qt::AlignHCenter);
    layout->addWidget(m_xCoordSpinBox, 1, 0);
    layout->addWidget(yCoordLabel, 0, 1, Qt::AlignHCenter);
    layout->addWidget(m_yCoordSpinBox, 1, 1);
    layout->addWidget(m_zCoordLabel, 0, 2, Qt::AlignHCenter);
    layout->addWidget(m_zCoordSpinBox, 1, 2);
    return widget;
}

/**
 * @return new instance of widget for surface coordinates
 */
QWidget*
AnnotationCoordinatesWidget::createCoordinateWidgetSurface()
{
    QLabel* vertexLabel(new QLabel("Structure/Vertex"));
    QLabel* offsetLabel(new QLabel("Offset Type/Dist"));
    QLabel* angleDepthLabel(new QLabel("Angle/Depth"));
    
    QFont font(vertexLabel->font());
    font.setPointSizeF(font.pointSizeF() * 0.8);
    vertexLabel->setFont(font);
    offsetLabel->setFont(font);
    angleDepthLabel->setFont(font);

    m_surfaceStructureComboBox = new StructureEnumComboBox(this);
    m_surfaceStructureComboBox->listOnlyValidStructures();
    m_surfaceStructureComboBox->getWidget()->setToolTip("Select surface structure");
    QObject::connect(m_surfaceStructureComboBox, &StructureEnumComboBox::structureSelected,
                     this, &AnnotationCoordinatesWidget::surfaceVertexStructureValueChanged);
    
    m_surfaceNodeIndexSpinBox = new QSpinBox();
    m_surfaceNodeIndexSpinBox->setRange(0, 1000000);
    m_surfaceNodeIndexSpinBox->setSingleStep(1);
    m_surfaceNodeIndexSpinBox->setToolTip("Select surface vertex");
    QObject::connect(m_surfaceNodeIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::surfaceVertexIndexValueChanged);
    
    const AString lengthTT("<html>"
                           "Distance from vertex to annotation:<br>"
                           "Centroid   - Distance (mm) from vertex to annotation using vector from center of surface through vertex<br>"
                           "Normal     - Distance (mm) from vertex to annotation using vertex's normal vector<br>"
                           "Tangent    - Distance (mm) from vertex to annotation using vertex's normal vector<br>"
                           "Text->Line - Screen depth offset that moves annotation closer to viewer when text is obscured by surface"
                           "</html>");

    m_surfaceOffsetLengthSpinBox = new QDoubleSpinBox();
    m_surfaceOffsetLengthSpinBox->setRange(0.0, 999.0);
    m_surfaceOffsetLengthSpinBox->setSingleStep(0.1);
    m_surfaceOffsetLengthSpinBox->setDecimals(3);
    m_surfaceOffsetLengthSpinBox->setToolTip(lengthTT);
    QObject::connect(m_surfaceOffsetLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::surfaceVertexOffsetLengthValueChanged);
    
    const AString patt(WuQtUtilities::createWordWrappedToolTipText("Polar angle in degrees from top.  "
                                                               "Positive angle is counter-clockwise"));
    m_surfaceTextOffsetPolarAngleSpinBox = new QDoubleSpinBox();
    m_surfaceTextOffsetPolarAngleSpinBox->setRange(0.0, 360.0);
    m_surfaceTextOffsetPolarAngleSpinBox->setWrapping(true);
    m_surfaceTextOffsetPolarAngleSpinBox->setSingleStep(1.0);
    m_surfaceTextOffsetPolarAngleSpinBox->setToolTip(patt);
    QObject::connect(m_surfaceTextOffsetPolarAngleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::surfaceOffsetTextOffsetPolarAngleValueChanged);

    const AString prtt(WuQtUtilities::createWordWrappedToolTipText("Depth moves text towards viewer "
                                                                   "when text is inside the surface.  "
                                                                   "This value does not work with Tangent offset."));
    m_surfaceTextOffsetScreenDepthSpinBox = new QDoubleSpinBox();
    m_surfaceTextOffsetScreenDepthSpinBox->setRange(0.0, 100.0);
    m_surfaceTextOffsetScreenDepthSpinBox->setDecimals(3);
    m_surfaceTextOffsetScreenDepthSpinBox->setSingleStep(0.001);
    m_surfaceTextOffsetScreenDepthSpinBox->setToolTip(prtt);
    QObject::connect(m_surfaceTextOffsetScreenDepthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::surfaceOffsetTextScreenDepthValueChanged);
    
    const AString offsetTT("<html>"
                           "Offset from vertex to annotation:<br>"
                           "Centroid   - Vector from center of surface to vertex; text is in plane of screen<br>"
                           "Normal     - Vector is vertex's normal vector; text is in plane of screen<br>"
                           "Tangent    - Text is tangent (using normal vector) to the surface; text rotates with surface<br>"
                           "Text->Line - Text is offset from vertex and a line connects text to vertex; "
                           "Angle/Depth values control angle of text from vertex; depth moves text to viewer"
                           "</html>");
    m_surfaceOffsetVectorTypeComboBox = new EnumComboBoxTemplate(this);
    m_surfaceOffsetVectorTypeComboBox->setup<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
    QObject::connect(m_surfaceOffsetVectorTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &AnnotationCoordinatesWidget::surfaceVertexOffsetVectorTypeChanged);
    m_surfaceOffsetVectorTypeComboBox->getWidget()->setToolTip(offsetTT);
    
    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->setRowStretch(100, 100); /* push widgets up, space at bottom */
    layout->addWidget(vertexLabel, 0, 0, Qt::AlignHCenter);
    layout->addWidget(m_surfaceStructureComboBox->getWidget(), 1, 0);
    layout->addWidget(m_surfaceNodeIndexSpinBox, 2, 0);
    layout->addWidget(offsetLabel, 0, 1, Qt::AlignHCenter);
    layout->addWidget(m_surfaceOffsetVectorTypeComboBox->getWidget(), 1, 1);
    layout->addWidget(m_surfaceOffsetLengthSpinBox, 2, 1);
    layout->addWidget(angleDepthLabel, 0, 2, Qt::AlignHCenter);
    layout->addWidget(m_surfaceTextOffsetPolarAngleSpinBox, 1, 2);
    layout->addWidget(m_surfaceTextOffsetScreenDepthSpinBox, 2, 2);
    
    return widget;
}

/**
 * @return Selected coordinate or NULL if not available
 */
AnnotationCoordinate*
AnnotationCoordinatesWidget::getSelectedCoordinate()
{
    AnnotationCoordinate* ac(NULL);
    
    if (m_annotation != NULL) {
        const int32_t coordNumber(m_coordNumberSpinBox->value());
        const int32_t coordIndex(coordNumber - 1);
        ac = m_annotation->getCoordinate(coordIndex);
    }
    
    return ac;
}

/**
 * @return First coordinate or NULL if not available
 */
AnnotationCoordinate*
AnnotationCoordinatesWidget::getFirstCoordinate()
{
    AnnotationCoordinate* ac(NULL);
    
    if (m_annotation != NULL) {
        const int32_t coordIndex(0);
        ac = m_annotation->getCoordinate(coordIndex);
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
            
            const int32_t numCoords(m_annotation->getNumberOfCoordinates());
            m_coordNumberSpinBox->setRange(1, numCoords);
            
            if (m_annotation != m_previousAnnotation) {
                QSignalBlocker blocker(m_coordNumberSpinBox);
                m_coordNumberSpinBox->setValue(1);
            }
            
            m_coordNumberSpinBox->setEnabled(numCoords > 1);
        }
    }
    
    m_surfaceStructureComboBox->listOnlyValidStructures();

    updateCoordinate();
    bool surfaceSpaceFlag(false);
    bool viewportSpaceFlag(false);
    if (m_annotation != NULL) {
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
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
        m_stackedLayout->setCurrentWidget(m_coordSurfaceWidget);
    }
    else {
        m_stackedLayout->setCurrentWidget(m_coordXyzWidget);
    }
    
    m_previousAnnotation = m_annotation;
}

/**
 * Update the selected coordinate
 */
void
AnnotationCoordinatesWidget::updateCoordinate()
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

    QString zLabelText("Z");
    
    StructureEnum::Enum structure = StructureEnum::INVALID;
    int32_t surfaceNumberOfNodes  = -1;
    int32_t surfaceNodeIndex      = -1;
    float   surfaceOffsetLength   = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
    
    const AnnotationCoordinate* coordinate(getSelectedCoordinate());
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
                            case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
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
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            {
                xMin = coordinateMinimum;
                xMax = coordinateMaximum;
                yMin = coordinateMinimum;
                yMax = coordinateMaximum;
                zMin = coordinateMinimum;
                zMax = coordinateMaximum;
                digitsRightOfDecimalX = 1;
                digitsRightOfDecimalY = 1;
                xStep = 1.0;
                yStep = 1.0;
                
                BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
                CaretAssert(bbw);
                BrowserTabContent* browserTabContent = bbw->getBrowserTabContent();
                if (browserTabContent != NULL) {
                    ModelMedia* modelMedia = browserTabContent->getDisplayedMediaModel();
                    if (modelMedia != NULL) {
                        HistologyOverlaySet* histologyOverlaySet = browserTabContent->getHistologyOverlaySet();
                        
                        if (histologyOverlaySet != NULL) {
                            const HistologySlicesFile* histologySlicesFile(histologyOverlaySet->getBottomMostHistologySlicesFile());
                            if (histologySlicesFile != NULL) {
                                const BoundingBox bb(histologySlicesFile->getPlaneXyzBoundingBox());
                                xMin = bb.getMinX();
                                xMax = bb.getMaxX();
                                yMin = bb.getMinY();
                                yMax = bb.getMaxY();
                                zMin = bb.getMinZ();
                                zMax = bb.getMaxZ();
                            }
                            
                            if (xMax > xMin) {
                                const float range(xMax - xMin);
                                xStep = range * 0.001f;
                            }
                            if (yMax > yMin) {
                                const float range(yMax - yMin);
                                yStep = range * 0.001f;
                            }
                        }
                    }
                }
            }

                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            {
                xMin = coordinateMinimum;
                xMax = coordinateMaximum;
                yMin = coordinateMinimum;
                yMax = coordinateMaximum;
                zMin = coordinateMinimum;
                zMax = coordinateMaximum;
                digitsRightOfDecimalX = 1;
                digitsRightOfDecimalY = 1;
                xStep = 1.0;
                yStep = 1.0;
                
                BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
                CaretAssert(bbw);
                BrowserTabContent* browserTabContent = bbw->getBrowserTabContent();
                if (browserTabContent != NULL) {
                    ModelMedia* modelMedia = browserTabContent->getDisplayedMediaModel();
                    if (modelMedia != NULL) {
                        MediaOverlaySet* mediaOverlaySet = browserTabContent->getMediaOverlaySet();
                                                
                        if (mediaOverlaySet != NULL) {
                            const MediaFile* mediaFile = mediaOverlaySet->getBottomMostMediaFile();
                            const CziImageFile* cziImageFile(mediaFile->castToCziImageFile());
                            if (cziImageFile != NULL) {
                                const QRectF rect(cziImageFile->getLogicalBoundsRect());
                                xMin = rect.left();
                                yMin = rect.bottom();
                                xMax = rect.right();
                                yMax = rect.top();
                                if (yMin > yMax) {
                                    std::swap(yMin, yMax);
                                }
                            }
                            else if (mediaFile != NULL) {
                                xMin = 0;
                                xMax = mediaFile->getWidth() - 1;
                                yMin = 0;
                                yMax = mediaFile->getHeight() - 1;
                            }
                            
                            if (xMax > xMin) {
                                const float range(xMax - xMin);
                                xStep = range * 0.001f;
                            }
                            if (yMax > yMin) {
                                const float range(yMax - yMin);
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
                zMin = 0.0;
                zMax = zDepthMaximum;
                xSuffix = "%";
                ySuffix = "%";
                digitsRightOfDecimalZ = 0;
                zStep = 1.0;
                zLabelText = "Order";
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
                zMin = 0.0;
                zMax = zDepthMaximum;
                xSuffix = "%";
                ySuffix = "%";
                digitsRightOfDecimalZ = 0;
                zStep = 1.0;
                zLabelText = "Order";
                break;
        }
    }

    m_xCoordSpinBox->blockSignals(true);
    m_xCoordSpinBox->setRange(xMin,
                              xMax);
    m_xCoordSpinBox->setSingleStep(xStep);
    m_xCoordSpinBox->setSuffix(xSuffix);
    m_xCoordSpinBox->setValue(xyz[0]);
    m_xCoordSpinBox->setDecimals(digitsRightOfDecimalX);
    m_xCoordSpinBox->blockSignals(false);
    m_xCoordSpinBox->setEnabled(xyzFlag);
    
    m_yCoordSpinBox->blockSignals(true);
    m_yCoordSpinBox->setRange(yMin,
                              yMax);
    m_yCoordSpinBox->setSingleStep(yStep);
    m_yCoordSpinBox->setSuffix(ySuffix);
    m_yCoordSpinBox->setValue(xyz[1]);
    m_yCoordSpinBox->setDecimals(digitsRightOfDecimalY);
    m_yCoordSpinBox->blockSignals(false);
    m_yCoordSpinBox->setEnabled(xyzFlag);
    
    m_zCoordSpinBox->blockSignals(true);
    m_zCoordSpinBox->setRange(zMin,
                              zMax);
    m_zCoordSpinBox->setSingleStep(zStep);
    m_zCoordSpinBox->setSuffix(zSuffix);
    m_zCoordSpinBox->setValue(xyz[2]);
    m_zCoordSpinBox->setDecimals(digitsRightOfDecimalZ);
    m_zCoordSpinBox->blockSignals(false);
    m_zCoordSpinBox->setEnabled(xyzFlag);
    m_zCoordLabel->setText(zLabelText);
    
    if (m_annotation != NULL) {
        if (m_annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
            m_zCoordSpinBox->setEnabled(false);
        }
    }
    
    if (surfaceFlag) {
        coordinate->getSurfaceSpace(structure,
                                    surfaceNumberOfNodes,
                                    surfaceNodeIndex,
                                    surfaceOffsetLength,
                                    surfaceOffsetVector);
        
        m_surfaceNodeIndexSpinBox->blockSignals(true);
        m_surfaceNodeIndexSpinBox->setValue(surfaceNodeIndex);
        m_surfaceNodeIndexSpinBox->blockSignals(false);
        
        m_surfaceOffsetLengthSpinBox->blockSignals(true);
        switch (surfaceOffsetVector) {
            case AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX:
            case AnnotationSurfaceOffsetVectorTypeEnum::SURFACE_NORMAL:
            case AnnotationSurfaceOffsetVectorTypeEnum::TANGENT:
                m_surfaceOffsetLengthSpinBox->setRange(0.0, 999.0);
                m_surfaceOffsetLengthSpinBox->setSingleStep(0.1);
                break;
            case AnnotationSurfaceOffsetVectorTypeEnum::TEXT_CONNECTED_TO_LINE:
                m_surfaceOffsetLengthSpinBox->setRange(-999.0, 999.0);
                m_surfaceOffsetLengthSpinBox->setSingleStep(0.001);
                break;
        }

        m_surfaceOffsetLengthSpinBox->setValue(surfaceOffsetLength);
        m_surfaceOffsetLengthSpinBox->blockSignals(false);
        
        AnnotationCoordinate::setUserDefautlSurfaceOffsetVectorType(surfaceOffsetVector);
        AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(surfaceOffsetLength);
        
        const AnnotationCoordinate* firstCoord(getFirstCoordinate());
        if (firstCoord != NULL) {
            firstCoord->getSurfaceSpace(structure,
                                        surfaceNumberOfNodes,
                                        surfaceNodeIndex,
                                        surfaceOffsetLength,
                                        surfaceOffsetVector);

            /*
             * Structure and offset type always come from first coordinate
             */
            m_surfaceStructureComboBox->setSelectedStructure(structure);
            if (m_annotation->getType() == AnnotationTypeEnum::TEXT) {
                m_surfaceOffsetVectorTypeComboBox->setup<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>();
            }
            else {
                std::vector<AnnotationSurfaceOffsetVectorTypeEnum::Enum> types;
                types.push_back(AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX);
                types.push_back(AnnotationSurfaceOffsetVectorTypeEnum::SURFACE_NORMAL);
                types.push_back(AnnotationSurfaceOffsetVectorTypeEnum::TANGENT);
                m_surfaceOffsetVectorTypeComboBox->setupWithItems<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>(types);
            }

            m_surfaceOffsetVectorTypeComboBox->setSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum,AnnotationSurfaceOffsetVectorTypeEnum::Enum>(surfaceOffsetVector);

            QSignalBlocker angleBlocker(m_surfaceTextOffsetPolarAngleSpinBox);
            m_surfaceTextOffsetPolarAngleSpinBox->setValue(coordinate->getSurfaceTextOffsetPolarAngle());
            m_surfaceTextOffsetPolarAngleSpinBox->setEnabled(surfaceOffsetVector == AnnotationSurfaceOffsetVectorTypeEnum::TEXT_CONNECTED_TO_LINE);
            
            QSignalBlocker screenDepthBlocker(m_surfaceTextOffsetScreenDepthSpinBox);
            m_surfaceTextOffsetScreenDepthSpinBox->setValue(coordinate->getSurfaceTextOffsetScreenDepth());
        }
    }
}

/**
 * Called when surface offset value changed.
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceVertexOffsetLengthValueChanged(double value)
{
    AnnotationCoordinate::setUserDefaultSurfaceOffsetLength(value);
    valueChangedCoordinate();
}

/**
 * Called when surface offset text polar angle  value changed.
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceOffsetTextOffsetPolarAngleValueChanged(double /*value*/)
{
    valueChangedCoordinate();
}

/**
 * Called when surface offset text screen depth value changed
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceOffsetTextScreenDepthValueChanged(double /*value*/)
{
    valueChangedCoordinate();
}

/**
 * Called when surface vertex index is changed
 *
 * @param value
 *    New value.
 */
void
AnnotationCoordinatesWidget::surfaceVertexIndexValueChanged(int /*value*/)
{
    valueChangedCoordinate();
}

/**
 * Called when surface offset vector type is changed.
 */
void
AnnotationCoordinatesWidget::surfaceVertexOffsetVectorTypeChanged()
{
    CaretAssert(m_surfaceOffsetVectorTypeComboBox);
    AnnotationCoordinate::setUserDefautlSurfaceOffsetVectorType(m_surfaceOffsetVectorTypeComboBox->getSelectedItem<AnnotationSurfaceOffsetVectorTypeEnum, AnnotationSurfaceOffsetVectorTypeEnum::Enum>());
    valueChangedCoordinate();
}

/**
 * Called when surface vertex structure value is changed.
 * @param structure
 *    New structure selected
 */
void
AnnotationCoordinatesWidget::surfaceVertexStructureValueChanged(const StructureEnum::Enum /*structure*/)
{
    valueChangedCoordinate();
}

/**
 * Called when coordinate number is changed
 * @param coordNumber
 *    New coordinate number (ranges 1..N)
 */
void
AnnotationCoordinatesWidget::coordNumberSpinBoxValueChanged(int /*coordNumber*/)
{
    updateCoordinate();
}

/**
 * Called when an XYZ coordinate spin box value is changed.
 * @param value
 *    New value
 */
void
AnnotationCoordinatesWidget::coordinateSpinBoxValueChanged(double /*value*/)
{
    valueChangedCoordinate();
}

/**
 * Gets to transfer values from the widgets to the selected coordinate
 */
void
AnnotationCoordinatesWidget::valueChangedCoordinate()
{
    const AnnotationCoordinate* coordinate(getSelectedCoordinate());
    if ((m_annotation != NULL)
        && (coordinate != NULL)) {
        bool surfaceFlag = false;
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
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
                    surfaceNodeIndex = m_surfaceNodeIndexSpinBox->value();
                    surfaceOffsetLength = m_surfaceOffsetLengthSpinBox->value();
                    
                    coordinateCopy.setSurfaceSpace(structure,
                                                   surfaceNumberOfNodes,
                                                   surfaceNodeIndex,
                                                   surfaceOffsetLength,
                                                   surfaceOffsetVector);
                    
                    coordinateCopy.setSurfaceTextOffsetPolarAngle(m_surfaceTextOffsetPolarAngleSpinBox->value());
                    coordinateCopy.setSurfaceTextOffsetScreenDepth(m_surfaceTextOffsetScreenDepthSpinBox->value());
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
                    else if (ann->getType() == AnnotationTypeEnum::SCALE_BAR) {
                        AnnotationScaleBar* scaleBar = ann->castToScaleBar();
                        CaretAssert(scaleBar);
                        scaleBar->setPositionMode(AnnotationColorBarPositionModeEnum::MANUAL);
                        updateUserInterfaceFlag = true;
                    }
                }
                
                const int32_t coordinateIndex(m_coordNumberSpinBox->value() - 1);
                AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                
                undoCommand->setModeCoordinate(coordinateIndex,
                                               coordinateCopy,
                                               selectedAnnotations);
                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
                
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
        
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

/**
 * @return Spin box with axis character in tool tip
 * @param axisCharacter
 *     Character for axis
 * @param xyzIndex
 *     Index 0 (X), 1 (Y), 2(Z)
 */
QDoubleSpinBox*
AnnotationCoordinatesWidget::createCoordinateSpinBox(const QString& axisCharacter,
                                                     const int32_t xyzIndex)
{
    const int digitsRightOfDecimal = 1;
    
    QDoubleSpinBox* sb(new QDoubleSpinBox());
    sb->setRange(0, 100000.0);
    sb->setSingleStep(1.0);
    sb->setDecimals(digitsRightOfDecimal);
    QObject::connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCoordinatesWidget::coordinateSpinBoxValueChanged);
    
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
            tabWindowText = ("      Increase to move behind\n"
                             "      Decrease to move in front");
            break;
    }
    
    CaretAssert(sb);
    WuQtUtilities::setWordWrappedToolTip(sb,
                                         "   STEREOTAXIC: " + axisCharacter + "-Coordinate\n"
                                         "   TAB and WINDOW: " + coordOrderText + "\n"
                                         + tabWindowText);
    return sb;
}

