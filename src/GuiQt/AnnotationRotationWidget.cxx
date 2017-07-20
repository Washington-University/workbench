
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

#include <algorithm>
#include <cmath>

#define __ANNOTATION_ROTATION_WIDGET_DECLARE__
#include "AnnotationRotationWidget.h"
#undef __ANNOTATION_ROTATION_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>


#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGetViewportSize.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationRotationWidget 
 * \brief Widget for adjusting annotation's rotation angle.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window.
 * @param parent
 *    Parent of this widget.
 */
AnnotationRotationWidget::AnnotationRotationWidget(const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* rotationLabel = new QLabel(" R:");
    m_rotationSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 359, 1.0, 0,
                                                                                       this, SLOT(rotationValueChanged(double)));
    m_rotationSpinBox->setWrapping(true);
    WuQtUtilities::setWordWrappedToolTip(m_rotationSpinBox,
                                         "Rotation, clockwise in degrees");
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(rotationLabel);
    layout->addWidget(m_rotationSpinBox);
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationRotationWidget::~AnnotationRotationWidget()
{
}

/**
 * Convert the given annotation to a one-dimensional annotation.
 * 
 * @param annotation
 *     The annotation.
 * @return
 *     Non-null if it is a one-dimensional annotation in a compatible
 *     stereotaxic space for rotation angle.
 *
 */
AnnotationOneDimensionalShape*
AnnotationRotationWidget::getValidOneDimAnnotation(Annotation* annotation)
{
    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
    
    if (oneDimAnn != NULL) {
        bool validSpaceFlag = false;
        switch (oneDimAnn->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                validSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                validSpaceFlag = true;
                break;
        }
        
        if ( ! validSpaceFlag) {
            oneDimAnn = NULL;
        }
    }
    
    return oneDimAnn;
}

/**
 * Update with the given annotation.
 *
 * @param annotations.
 *    The annotation.
 */
void
AnnotationRotationWidget::updateContent(std::vector<Annotation*>& annotations)
{
    m_annotations.clear();
    
    if ( ! annotations.empty()) {
        float rotationAngle = 0.0;
        bool rotationAngleValid = false;
        bool haveMultipleRotationAnglesFlag = false;
        
        const int32_t numAnns = static_cast<int32_t>(annotations.size());
        for (int32_t i = 0; i < numAnns; i++) {
            CaretAssertVectorIndex(annotations, i);
            Annotation* ann = annotations[i];
            if (ann->testProperty(Annotation::Property::ROTATION)) {
                if (ann->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
                    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(ann);
                    AnnotationOneDimensionalShape* oneDimAnn = getValidOneDimAnnotation(ann);
                    
                    float angle = 0.0;
                    float angleValid = false;
                    if (twoDimAnn != NULL) {
                        angle = twoDimAnn->getRotationAngle();
                        angleValid = true;
                    }
                    else if (oneDimAnn != NULL) {
                        int32_t viewport[4] = { 0, 0, 0, 0 };
                        bool viewportValidFlag = false;
                        switch (oneDimAnn->getCoordinateSpace()) {
                            case AnnotationCoordinateSpaceEnum::CHART:
                                break;
                            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                                break;
                            case  AnnotationCoordinateSpaceEnum::SURFACE:
                                break;
                            case AnnotationCoordinateSpaceEnum::TAB:
                            {
                                const int tabIndex = oneDimAnn->getTabIndex();
                                EventGetViewportSize vpSizeEvent(EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX,
                                                                 tabIndex);
                                EventManager::get()->sendEvent(vpSizeEvent.getPointer());
                                if (vpSizeEvent.isViewportSizeValid()) {
                                    vpSizeEvent.getViewportSize(viewport);
                                    viewportValidFlag = true;
                                }
                            }
                                break;
                            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                                break;
                            case AnnotationCoordinateSpaceEnum::WINDOW:
                            {
                                const int windowIndex = oneDimAnn->getWindowIndex();
                                EventGetViewportSize vpSizeEvent(EventGetViewportSize::MODE_WINDOW_INDEX,
                                                                 windowIndex);
                                EventManager::get()->sendEvent(vpSizeEvent.getPointer());
                                if (vpSizeEvent.isViewportSizeValid()) {
                                    vpSizeEvent.getViewportSize(viewport);
                                    viewportValidFlag = true;
                                }
                            }
                                break;
                        }
                        
                        if (viewportValidFlag) {
                            angle = oneDimAnn->getRotationAngle(viewport[2], viewport[3]);
                            angleValid = true;
                        }
                    }
                    
                    if (angleValid) {
                        if (angle < 0.0) {
                            angle += 360.0;
                        }
                        else if (angle > 360.0) {
                            angle -= 360.0;
                        }
                        
                        if (rotationAngleValid) {
                            if (rotationAngle != angle) {
                                haveMultipleRotationAnglesFlag = true;
                            }
                            rotationAngle = std::min(rotationAngle,
                                                     angle);
                        }
                        else {
                            rotationAngle = angle;
                            rotationAngleValid = true;
                        }
                        
                        m_annotations.push_back(ann);
                    }
                }
            }
        }
        
        if (rotationAngleValid) {
            m_rotationSpinBox->blockSignals(true);
            m_rotationSpinBox->setValue(rotationAngle);
            if (haveMultipleRotationAnglesFlag) {
                m_rotationSpinBox->setSuffix("+");
            }
            else {
                m_rotationSpinBox->setSuffix("");
            }
            m_rotationSpinBox->blockSignals(false);
        }
        
        setEnabled(rotationAngleValid);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when rotation value is changed.
 *
 * @param value
 *
 */
void
AnnotationRotationWidget::rotationValueChanged(double value)
{
    if ( ! m_annotations.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        undoCommand->setModeRotationAngle(value,
                                          m_annotations);
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}
