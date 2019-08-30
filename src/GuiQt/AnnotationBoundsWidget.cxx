
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
#include "AnnotationBoundsWidget.h"
#undef __ANNOTATION_COORDINATE_WIDGET_DECLARE__

#include <algorithm>
#include <cmath>
#include <limits>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QToolTip>

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventBrowserWindowContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationBoundsWidget
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
AnnotationBoundsWidget::AnnotationBoundsWidget(const UserInputModeEnum::Enum userInputMode,
                                                       const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
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
    
    QLabel* boundsLabel = new QLabel("Bounds");
    
    QLabel* xMinCoordLabel = new QLabel("Min X" + colonString);
    m_xMinCoordSpinBox = createSpinBox();
    QObject::connect(m_xMinCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationBoundsWidget::xMinValueChanged);
    
    QLabel* xMaxCoordLabel = new QLabel(" Max X" + colonString);
    m_xMaxCoordSpinBox = createSpinBox();
    QObject::connect(m_xMaxCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationBoundsWidget::xMaxValueChanged);

    QLabel* yMinCoordLabel = new QLabel("Min Y" + colonString);
    m_yMinCoordSpinBox = createSpinBox();
    QObject::connect(m_yMinCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationBoundsWidget::yMinValueChanged);

    QLabel* yMaxCoordLabel = new QLabel(" Max Y" + colonString);
    m_yMaxCoordSpinBox = createSpinBox();
    QObject::connect(m_yMaxCoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationBoundsWidget::yMaxValueChanged);
    
    QGridLayout* coordinateLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(coordinateLayout, 2, 0);
    int32_t row(0);
    coordinateLayout->addWidget(boundsLabel, row, 0, 1, 4, Qt::AlignHCenter);
    row++;
    coordinateLayout->addWidget(xMinCoordLabel, row, 0);
    coordinateLayout->addWidget(m_xMinCoordSpinBox, row, 1);
    coordinateLayout->addWidget(xMaxCoordLabel, row, 2);
    coordinateLayout->addWidget(m_xMaxCoordSpinBox, row, 3);
    row++;
    coordinateLayout->addWidget(yMinCoordLabel, row, 0);
    coordinateLayout->addWidget(m_yMinCoordSpinBox, row, 1);
    coordinateLayout->addWidget(yMaxCoordLabel, row, 2);
    coordinateLayout->addWidget(m_yMaxCoordSpinBox, row, 3);

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

QDoubleSpinBox*
AnnotationBoundsWidget::createSpinBox()
{
    QDoubleSpinBox* spinBox = new QDoubleSpinBox();
    spinBox->setMinimum(-100.0);
    spinBox->setMaximum( 200.0);
    spinBox->setSingleStep(0.1);
    spinBox->setDecimals(1);
    spinBox->setToolTip("0.0% => Bottom of window\n"
                        "100.0% => Top of window");
    spinBox->setSuffix("%");
    
    return spinBox;
}


/**
 * Destructor.
 */
AnnotationBoundsWidget::~AnnotationBoundsWidget()
{
}

/**
 * Update with the given annotation coordinate.
 *
 * @param annotationBrowserTabs.
 */
void
AnnotationBoundsWidget::updateContent(std::vector<AnnotationBrowserTab*>& annotationBrowserTabs)
{
    bool haveMultipleMinXValuesFlag(false);
    bool haveMultipleMaxXValuesFlag(false);
    bool haveMultipleMinYValuesFlag(false);
    bool haveMultipleMaxYValuesFlag(false);

    float valueMinX(0.0);
    float valueMaxX(0.0);
    float valueMinY(0.0);
    float valueMaxY(0.0);
    
    bool firstFlag(true);
    
    m_annotationBrowserTabs = annotationBrowserTabs;
    for (auto abt : annotationBrowserTabs) {
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        abt->getBounds2D(minX, maxX, minY, maxY);
        
        if (firstFlag) {
            valueMinX = minX;
            valueMaxX = maxX;
            valueMinY = minY;
            valueMaxY = maxY;
            firstFlag = false;
        }
        else {
            if (minX != valueMinX) {
                haveMultipleMinXValuesFlag = true;
            }
            if (maxX != valueMaxX) {
                haveMultipleMaxXValuesFlag = true;
            }
            if (minY != valueMinY) {
                haveMultipleMinYValuesFlag = true;
            }
            if (maxY != valueMaxY) {
                haveMultipleMaxYValuesFlag = true;
            }
            
            valueMinX = std::min(valueMinX, minX);
            valueMaxX = std::max(valueMaxX, maxX);
            valueMinY = std::min(valueMinY, minY);
            valueMaxY = std::max(valueMaxY, maxY);
        }
    }
    
    /*
     * If multiple annotations are selected,
     * a plus sign (+) is appeneded to the value
     */
    
    m_xMinCoordSpinBox->blockSignals(true);
    m_xMinCoordSpinBox->setValue(valueMinX);
    if (haveMultipleMinXValuesFlag) {
        m_xMinCoordSpinBox->setSuffix("%+");
    }
    else {
        m_xMinCoordSpinBox->setSuffix("%");
    }
    m_xMinCoordSpinBox->blockSignals(false);

    m_xMaxCoordSpinBox->blockSignals(true);
    m_xMaxCoordSpinBox->setValue(valueMaxX);
    if (haveMultipleMaxXValuesFlag) {
        m_xMaxCoordSpinBox->setSuffix("%+");
    }
    else {
        m_xMaxCoordSpinBox->setSuffix("%");
    }
    m_xMaxCoordSpinBox->blockSignals(false);
    
    m_yMinCoordSpinBox->blockSignals(true);
    m_yMinCoordSpinBox->setValue(valueMinY);
    if (haveMultipleMinYValuesFlag) {
        m_yMinCoordSpinBox->setSuffix("%+");
    }
    else {
        m_yMinCoordSpinBox->setSuffix("%");
    }
    m_yMinCoordSpinBox->blockSignals(false);
    
    m_yMaxCoordSpinBox->blockSignals(true);
    m_yMaxCoordSpinBox->setValue(valueMaxY);
    if (haveMultipleMaxYValuesFlag) {
        m_yMaxCoordSpinBox->setSuffix("%+");
    }
    else {
        m_yMaxCoordSpinBox->setSuffix("%");
    }
    m_yMaxCoordSpinBox->blockSignals(false);

    setEnabled( ! m_annotationBrowserTabs.empty());
}

/**
 * Gets called when x-min value is changed.
 *
 * @param value
 */
void
AnnotationBoundsWidget::xMinValueChanged(const double value)
{
    valueChangedHelper(m_xMinCoordSpinBox,
                       value);
}

/**
 * Gets called when x-max value is changed.
 *
 * @param value
 */
void
AnnotationBoundsWidget::xMaxValueChanged(const double value)
{
    valueChangedHelper(m_xMaxCoordSpinBox,
                       value);
}

/**
 * Gets called when y-min value is changed.
 *
 * @param value
 */
void
AnnotationBoundsWidget::yMinValueChanged(const double value)
{
    valueChangedHelper(m_yMinCoordSpinBox,
                       value);
}

/**
 * Gets called when y-max value is changed.
 *
 * @param value
 */
void
AnnotationBoundsWidget::yMaxValueChanged(const double value)
{
    valueChangedHelper(m_yMaxCoordSpinBox,
                       value);
}

/**
 * Helper for bounds value changed
 *
 * @param spinBox
 *     Spin box that had value changed
 * @param value
 *     New value
 */
void
AnnotationBoundsWidget::valueChangedHelper(QDoubleSpinBox* spinBox,
                        float value)
{
    if ( ! m_annotationBrowserTabs.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotationBrowserTabs.begin(),
                                             m_annotationBrowserTabs.end());
        
        if (spinBox == m_xMinCoordSpinBox) {
            undoCommand->setBoundsMinX2D(value, annotations);
        }
        else if (spinBox == m_xMaxCoordSpinBox) {
            undoCommand->setBoundsMaxX2D(value, annotations);
        }
        else if (spinBox == m_yMinCoordSpinBox) {
            undoCommand->setBoundsMinY2D(value, annotations);
        }
        else if (spinBox == m_yMaxCoordSpinBox) {
            undoCommand->setBoundsMaxY2D(value, annotations);
        }
        else {
            delete undoCommand;
            CaretAssert(0);
            return;
        }
        
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
        
        AString errorMessage;
        if ( ! annMan->applyCommand(m_userInputMode,
                                    undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        
        /*
         * Update as annotation might adjust/limit coordinates
         */
        updateContent(m_annotationBrowserTabs);
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}
