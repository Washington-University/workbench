
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

#define __ANNOTATION_COORDINATE_SIZE_ROTATE_WIDGET_DECLARE__
#include "AnnotationCoordinateSizeRotateWidget.h"
#undef __ANNOTATION_COORDINATE_SIZE_ROTATE_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>

#include "Annotation.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "StructureEnumComboBox.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinateSizeRotateWidget 
 * \brief Widget for editing annotation coordinate, size, and rotation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationCoordinateSizeRotateWidget::AnnotationCoordinateSizeRotateWidget(QWidget* parent)
: QWidget(parent)
{
    m_annotation = NULL;
    
    QLabel* surfaceVertexLabel = new QLabel("Vertex:");
    std::vector<StructureEnum::Enum> validStructures;
    validStructures.push_back(StructureEnum::CORTEX_LEFT);
    validStructures.push_back(StructureEnum::CORTEX_RIGHT);
    validStructures.push_back(StructureEnum::CEREBELLUM);
    m_surfaceStructureComboBox = new StructureEnumComboBox(this);
    m_surfaceStructureComboBox->listOnlyTheseStructures(validStructures);
    
    m_surfaceNodeIndexSpinBox = new QSpinBox();
    m_surfaceNodeIndexSpinBox->setRange(0, 1000000);
    m_surfaceNodeIndexSpinBox->setSingleStep(1);
    
    QLabel* xCoordLabel = new QLabel(" X:");
    m_xCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(coordinateValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_xCoordSpinBox,
                                         "X-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW X-Range: [0.0, 1.0]\n"
                                         "      0.0 => Left side of tab/window\n"
                                         "      1.0 => Right side of tab/window\n");
    
    QLabel* yCoordLabel = new QLabel(" Y:");
    m_yCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(coordinateValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_yCoordSpinBox,
                                         "Y-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW Y-Range: [0.0, 1.0]\n"
                                         "      0.0 => Bottom of tab/window\n"
                                         "      1.0 => Top of tab/window\n");
    
    QLabel* zCoordLabel = new QLabel(" Z:");
    m_zCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-1.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(coordinateValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_zCoordSpinBox,
                                         "Z-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW DEPTH Range: [-1.0, 1.0]\n"
                                         "      -1.0 => Toward's viewer\n"
                                         "       1.0 => Away from viewer\n");
    

    QLabel* widthLabel = new QLabel(" W:");
    m_widthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(widthValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_widthSpinBox,
                                         "Width of 2D Shapes (Box, Image, Oval)");

    QLabel* heightLabel = new QLabel(" H:");
    m_heightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(heightValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_heightSpinBox,
                                         "Height of 2D Shapes (Box, Image, Oval)");

    QLabel* lengthLabel = new QLabel(" L:");
    m_lengthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(lengthValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_lengthSpinBox,
                                         "Length of 1D Shapes (Arrow, Line)");
    
    QLabel* rotationLabel = new QLabel(" R:");
    m_rotationSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 360, 1.0, 0,
                                                                                    this, SLOT(rotationValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_rotationSpinBox,
                                         "Rotation, clockwise in degrees, of annotation");

    m_surfaceWidget = new QWidget();
    QHBoxLayout* surfaceLayout = new QHBoxLayout(m_surfaceWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(surfaceLayout, 2, 0);
    surfaceLayout->addWidget(surfaceVertexLabel);
    surfaceLayout->addWidget(m_surfaceStructureComboBox->getWidget());
    surfaceLayout->addWidget(m_surfaceNodeIndexSpinBox);
    
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
    layout->addWidget(widthLabel);
    layout->addWidget(m_widthSpinBox);
    layout->addWidget(heightLabel);
    layout->addWidget(m_heightSpinBox);
    layout->addWidget(lengthLabel);
    layout->addWidget(m_lengthSpinBox);
    layout->addWidget(rotationLabel);
    layout->addWidget(m_rotationSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
AnnotationCoordinateSizeRotateWidget::~AnnotationCoordinateSizeRotateWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationCoordinateSizeRotateWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Update with the given annotation.
 *
 * @param annotation.
 */
void
AnnotationCoordinateSizeRotateWidget::updateContent(Annotation* annotation)
{
    m_annotation = annotation;
    
    if (m_annotation != NULL) {
        double xyMin =  0.0;
        double xyMax =  1.0;
        double zMin  = -1.0;
        double zMax  =  1.0;
        double xyzStep = 0.01;
        switch (m_annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::MODEL:
                xyMax = 10000.0;
                xyMin = -xyMax;
                zMin = xyMin;
                zMax = xyMax;
                xyzStep = 1.0;
                break;
            case AnnotationCoordinateSpaceEnum::PIXELS:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        
        m_xCoordSpinBox->setRange(xyMin,
                                  xyMax);
        m_xCoordSpinBox->setSingleStep(xyzStep);
        m_yCoordSpinBox->setRange(xyMin,
                                  xyMax);
        m_yCoordSpinBox->setSingleStep(xyzStep);
        m_zCoordSpinBox->setRange(xyMin,
                                  xyMax);
        m_zCoordSpinBox->setSingleStep(xyzStep);
        
        float xyz[3];
        m_annotation->getXYZ(xyz);
        
        m_xCoordSpinBox->setValue(xyz[0]);
        m_yCoordSpinBox->setValue(xyz[1]);
        m_zCoordSpinBox->setValue(xyz[2]);
        
        m_widthSpinBox->setValue(m_annotation->getWidth2D());
        m_heightSpinBox->setValue(m_annotation->getHeight2D());
        m_lengthSpinBox->setValue(m_annotation->getLength1D());
        m_rotationSpinBox->setValue(m_annotation->getRotationAngle());
        
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when an X, Y, or Z-coordinate value is changed.
 */
void
AnnotationCoordinateSizeRotateWidget::coordinateValueChanged()
{
    if (m_annotation != NULL) {
        float xyz[3] = {
            m_xCoordSpinBox->value(),
            m_yCoordSpinBox->value(),
            m_zCoordSpinBox->value()
        };
        m_annotation->setXYZ(xyz);
    }
}

/**
 * Gets called when the width value is changed.
 *
 * @param value
 *
 */
void
AnnotationCoordinateSizeRotateWidget::widthValueChanged(double value)
{
    if (m_annotation != NULL) {
        m_annotation->setWidth2D(value);
    }
}

/**
 * Gets called when height value is changed.
 *
 * @param value
 *
 */
void
AnnotationCoordinateSizeRotateWidget::heightValueChanged(double value)
{
    if (m_annotation != NULL) {
        m_annotation->setHeight2D(value);
    }
}

/**
 * Gets called when length value is changed.
 *
 * @param value
 *
 */
void
AnnotationCoordinateSizeRotateWidget::lengthValueChanged(double value)
{
    if (m_annotation != NULL) {
        m_annotation->setLength1D(value);
    }
}

/**
 * Gets called when rotation value is changed.
 *
 * @param value
 *
 */
void
AnnotationCoordinateSizeRotateWidget::rotationValueChanged(double value)
{
    if (m_annotation != NULL) {
        m_annotation->setRotationAngle(value);
    }
}

