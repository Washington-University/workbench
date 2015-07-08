
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

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QToolButton>

#include "AnnotationCoordinate.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "StructureEnumComboBox.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinateWidget 
 * \brief Widget for editing annotation coordinate
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
AnnotationCoordinateWidget::AnnotationCoordinateWidget(const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_coordinate = NULL;
    m_coordinateSpace = AnnotationCoordinateSpaceEnum::TAB;
    
    QLabel* surfaceVertexLabel = new QLabel("Vertex:");
    m_surfaceStructureComboBox = new StructureEnumComboBox(this);
    m_surfaceStructureComboBox->listOnlyValidStructures();
    m_surfaceStructureComboBox->getWidget()->setToolTip("Select surface structure");
    QObject::connect(m_surfaceStructureComboBox, SIGNAL(structureSelected(const StructureEnum::Enum)),
                     this, SLOT(surfaceValueChanged()));
    
    m_surfaceNodeIndexSpinBox = new QSpinBox();
    m_surfaceNodeIndexSpinBox->setRange(0, 1000000);
    m_surfaceNodeIndexSpinBox->setSingleStep(1);
    m_surfaceNodeIndexSpinBox->setToolTip("Select surface vertex");
    QObject::connect(m_surfaceNodeIndexSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(surfaceValueChanged()));
    
    m_surfaceOffsetLengthSpinBox = new QDoubleSpinBox();
    m_surfaceOffsetLengthSpinBox->setRange(0.0, 999.0);
    m_surfaceOffsetLengthSpinBox->setSingleStep(1.0);
    m_surfaceOffsetLengthSpinBox->setToolTip("Offset of annotation from surface vertex");
    QObject::connect(m_surfaceOffsetLengthSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceValueChanged()));
    
    QLabel* xCoordLabel = new QLabel(" X:");
    m_xCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(xyzValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_xCoordSpinBox,
                                         "X-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW X-Range: [0.0, 1.0]\n"
                                         "      0.0 => Left side of tab/window\n"
                                         "      1.0 => Right side of tab/window\n");
    
    QLabel* yCoordLabel = new QLabel(" Y:");
    m_yCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(xyzValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_yCoordSpinBox,
                                         "Y-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW Y-Range: [0.0, 1.0]\n"
                                         "      0.0 => Bottom of tab/window\n"
                                         "      1.0 => Top of tab/window\n");
    
    QLabel* zCoordLabel = new QLabel(" Z:");
    m_zCoordSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-1.0, 1.0, 0.01, 2,
                                                                                     this, SLOT(xyzValueChanged()));
    WuQtUtilities::setWordWrappedToolTip(m_zCoordSpinBox,
                                         "Z-coordinate of annotation\n"
                                         "   MODEL: Stereotaxic Coordinate\n"
                                         "   TAB and WINDOW DEPTH Range: [-1.0, 1.0]\n"
                                         "      -1.0 => Toward's viewer\n"
                                         "       1.0 => Away from viewer\n");
    

    QAction* setCoordinateAction = WuQtUtilities::createAction("+",
                                                               "Set coordinate with mouse",
                                                               this,
                                                               this,
                                                               SLOT(setCoordinateActionTriggered()));
    QToolButton* setCoordinateToolButton = new QToolButton();
    setCoordinateToolButton->setDefaultAction(setCoordinateAction);
    
    m_surfaceWidget = new QWidget();
    QHBoxLayout* surfaceLayout = new QHBoxLayout(m_surfaceWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(surfaceLayout, 2, 0);
    surfaceLayout->addWidget(surfaceVertexLabel);
    surfaceLayout->addWidget(m_surfaceStructureComboBox->getWidget());
    surfaceLayout->addWidget(m_surfaceNodeIndexSpinBox);
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
    layout->addWidget(setCoordinateToolButton);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationCoordinateWidget::~AnnotationCoordinateWidget()
{
}

/**
 * Update with the given annotation coordinate.
 *
 * @param coordinate.
 */
void
AnnotationCoordinateWidget::updateContent(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                          AnnotationCoordinate* coordinate)
{
    m_coordinate = coordinate;
    m_coordinateSpace = coordinateSpace;
    
    m_surfaceStructureComboBox->listOnlyValidStructures();
    
    bool surfaceFlag    = false;
    
    if (m_coordinate != NULL) {
        double xyMin =  0.0;
        double xyMax =  1.0;
        double zMin  = -1.0;
        double zMax  =  1.0;
        double xyzStep = 0.01;
        switch (m_coordinateSpace) {
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
                surfaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        
        float xyz[3];
        m_coordinate->getXYZ(xyz);
        
        m_xCoordSpinBox->blockSignals(true);
        m_xCoordSpinBox->setRange(xyMin,
                                  xyMax);
        m_xCoordSpinBox->setSingleStep(xyzStep);
        m_xCoordSpinBox->setValue(xyz[0]);
        m_xCoordSpinBox->blockSignals(false);
        
        m_yCoordSpinBox->blockSignals(true);
        m_yCoordSpinBox->setRange(xyMin,
                                  xyMax);
        m_yCoordSpinBox->setSingleStep(xyzStep);
        m_yCoordSpinBox->setValue(xyz[1]);
        m_yCoordSpinBox->blockSignals(false);
        
        m_zCoordSpinBox->blockSignals(true);
        m_zCoordSpinBox->setRange(zMin,
                                  zMax);
        m_zCoordSpinBox->setSingleStep(xyzStep);
        m_zCoordSpinBox->setValue(xyz[2]);
        m_zCoordSpinBox->blockSignals(false);
        
        if (surfaceFlag) {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfNodes  = -1;
            int32_t surfaceNodeIndex      = -1;
            float   surfaceOffsetLength   = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
            m_coordinate->getSurfaceSpace(structure,
                                          surfaceNumberOfNodes,
                                          surfaceNodeIndex,
                                          surfaceOffsetLength);
            m_surfaceStructureComboBox->setSelectedStructure(structure);
            m_surfaceNodeIndexSpinBox->blockSignals(true);
            m_surfaceNodeIndexSpinBox->setValue(surfaceNodeIndex);
            m_surfaceNodeIndexSpinBox->blockSignals(false);
            m_surfaceOffsetLengthSpinBox->blockSignals(true);
            m_surfaceOffsetLengthSpinBox->setValue(surfaceOffsetLength);
            m_surfaceOffsetLengthSpinBox->blockSignals(false);
        }
        
        setEnabled(true);
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
AnnotationCoordinateWidget::surfaceValueChanged()
{
    if (m_coordinate != NULL) {
        bool surfaceFlag = false;
        switch (m_coordinateSpace) {
            case AnnotationCoordinateSpaceEnum::MODEL:
                break;
            case AnnotationCoordinateSpaceEnum::PIXELS:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                surfaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        
        if (surfaceFlag) {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t surfaceNumberOfNodes  = -1;
            int32_t surfaceNodeIndex      = -1;
            float surfaceOffsetLength     = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
            m_coordinate->getSurfaceSpace(structure,
                                          surfaceNumberOfNodes,
                                          surfaceNodeIndex,
                                          surfaceOffsetLength);
            structure = m_surfaceStructureComboBox->getSelectedStructure();
            surfaceNodeIndex = m_surfaceNodeIndexSpinBox->value();
            surfaceOffsetLength = m_surfaceOffsetLengthSpinBox->value();
            
            m_coordinate->setSurfaceSpace(structure,
                                          surfaceNumberOfNodes,
                                          surfaceNodeIndex,
                                          surfaceOffsetLength);
        }
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when an X, Y, or Z-coordinate value is changed.
 */
void
AnnotationCoordinateWidget::xyzValueChanged()
{
    if (m_coordinate != NULL) {
        bool surfaceFlag = false;
        switch (m_coordinateSpace) {
            case AnnotationCoordinateSpaceEnum::MODEL:
                break;
            case AnnotationCoordinateSpaceEnum::PIXELS:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                surfaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        
        if ( ! surfaceFlag) {
            float xyz[3] = {
                m_xCoordSpinBox->value(),
                m_yCoordSpinBox->value(),
                m_zCoordSpinBox->value()
            };
            m_coordinate->setXYZ(xyz);
        }
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the set coordinate button is clicked.
 */
void
AnnotationCoordinateWidget::setCoordinateActionTriggered()
{
    signalSelectCoordinateWithMouse();
}
