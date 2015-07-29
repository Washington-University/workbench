
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
 * @param browserWindowIndex
 *    Index of browser window.
 * @param parent
 *    Parent of this widget.
 */
AnnotationWidthHeightWidget::AnnotationWidthHeightWidget(const int32_t browserWindowIndex,
                                                                         QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_annotation2D = NULL;
    
    QLabel* widthLabel = new QLabel(" W:");
    m_widthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 3,
                                                                                    this, SLOT(widthValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_widthSpinBox,
                                         "Width of 2D Shapes (Box, Image, Oval)");

    QLabel* heightLabel = new QLabel(" H:");
    m_heightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0, 1.0, 0.01, 2,
                                                                                    this, SLOT(heightValueChanged(double)));
    WuQtUtilities::setWordWrappedToolTip(m_heightSpinBox,
                                         "Height of 2D Shapes (Box, Image, Oval)");

    {
        AnnotationBox box;
        if (box.isUseHeightAsAspectRatio()) {
            heightLabel->setText(" A:");
            m_heightSpinBox->setMaximum(1000.0);
            m_heightSpinBox->setDecimals(6);
            const QString toolTipText("Aspect ratio of shape\n"
                                      "height = width * aspect ratio");
            m_heightSpinBox->setToolTip(toolTipText);
        }
    }
    
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
 * @param annotation2D.
 *    Two dimensional annotation.
 */
void
AnnotationWidthHeightWidget::updateContent(AnnotationTwoDimensionalShape* annotation2D)
{
    m_annotation2D = annotation2D;
    
    if (m_annotation2D != NULL) {
        if (m_annotation2D->getType() != AnnotationTypeEnum::TEXT) {
            m_widthSpinBox->blockSignals(true);
            m_widthSpinBox->setValue(m_annotation2D->getWidth());
            m_widthSpinBox->blockSignals(false);
            m_heightSpinBox->blockSignals(true);
            m_heightSpinBox->setValue(m_annotation2D->getHeight());
            m_heightSpinBox->blockSignals(false);
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeTwoDimHeight(value,
                                     annMan->getSelectedAnnotations());
    annMan->applyCommand(undoCommand);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
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
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeTwoDimWidth(value,
                                     annMan->getSelectedAnnotations());
    annMan->applyCommand(undoCommand);
    
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

