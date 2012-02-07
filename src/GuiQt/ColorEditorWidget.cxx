
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <algorithm>

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#define __COLOR_EDITOR_WIDGET_DECLARE__
#include "ColorEditorWidget.h"
#undef __COLOR_EDITOR_WIDGET_DECLARE__

#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::ColorEditorWidget 
 * \brief Widget for editing RGBA colors.
 *
 */
/**
 * Constructor.
 */
ColorEditorWidget::ColorEditorWidget(const bool alphaControlEnabled,
                                     QWidget* parent)
: QWidget(parent)
{
    
    QLabel* redLabel = new QLabel("Red");
    this->redSpinBox = new QSpinBox();
    this->redSpinBox->setRange(0, 255);
    this->redSpinBox->setSingleStep(1);
    QObject::connect(this->redSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(redValueChanged(int)));
    
    QLabel* greenLabel = new QLabel("Green");
    this->greenSpinBox = new QSpinBox();
    this->greenSpinBox->setRange(0, 255);
    this->greenSpinBox->setSingleStep(1);
    QObject::connect(this->greenSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(greenValueChanged(int)));
    
    QLabel* blueLabel = new QLabel("Blue");
    this->blueSpinBox = new QSpinBox();
    this->blueSpinBox->setRange(0, 255);
    this->blueSpinBox->setSingleStep(1);
    QObject::connect(this->blueSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(blueValueChanged(int)));
    
    QLabel* alphaLabel = NULL;
    this->alphaSpinBox = NULL;
    if (alphaControlEnabled) {
        alphaLabel = new QLabel("Alpha");
        this->alphaSpinBox = new QSpinBox();
        this->alphaSpinBox->setRange(0, 255);
        this->alphaSpinBox->setSingleStep(1);
        QObject::connect(this->alphaSpinBox, SIGNAL(valueChanged(int)),
                         this, SLOT(alphaValueChanged(int)));
    }
    
    int columnCount = 0;
    const int COLUMN_NAME = columnCount++;
    const int COLUMN_SPINBOX = columnCount++;
    
    int row = 0;
    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->addWidget(redLabel, row, COLUMN_NAME);
    gridLayout->addWidget(this->redSpinBox, row, COLUMN_SPINBOX);
    row++;
    gridLayout->addWidget(greenLabel, row, COLUMN_NAME);
    gridLayout->addWidget(this->greenSpinBox, row, COLUMN_SPINBOX);
    row++;
    gridLayout->addWidget(blueLabel, row, COLUMN_NAME);
    gridLayout->addWidget(this->blueSpinBox, row, COLUMN_SPINBOX);
    row++;
    if (this->alphaSpinBox != NULL) {
        gridLayout->addWidget(alphaLabel, row, COLUMN_NAME);
        gridLayout->addWidget(this->alphaSpinBox, row, COLUMN_SPINBOX);
        row++;
    }
    
    this->controlsWidgetGroup = new WuQWidgetObjectGroup(this);
    this->controlsWidgetGroup->add(this->redSpinBox);
    this->controlsWidgetGroup->add(this->greenSpinBox);
    this->controlsWidgetGroup->add(this->blueSpinBox);
    if (this->alphaSpinBox != NULL) {
        this->controlsWidgetGroup->add(this->alphaSpinBox);
    }
}

/**
 * Destructor.
 */
ColorEditorWidget::~ColorEditorWidget()
{
    
}

/**
 * Set the color in the control with RGBA
 * values ranging 0.0 to 1.0.
 * @param rgba
 *    Input RGBA values.
 */
void 
ColorEditorWidget::setColor(const float rgba[4])
{
    const int32_t intRGBA[4] = {
        std::min(static_cast<int32_t>(rgba[0] * 255.0), 255),
        std::min(static_cast<int32_t>(rgba[1] * 255.0), 255),
        std::min(static_cast<int32_t>(rgba[2] * 255.0), 255),
        std::min(static_cast<int32_t>(rgba[3] * 255.0), 255)
    };
    
    this->setColor(intRGBA);
}

/**
 * Get the color in the control with RGBA
 * values ranging 0.0 to 1.0.
 * @param rgba
 *    Output RGBA values.
 */
void 
ColorEditorWidget::getColor(float rgba[4]) const
{
    int intRGBA[4];    
    this->getColor(intRGBA);
    
    rgba[0] = intRGBA[0] / 255.0;
    rgba[1] = intRGBA[1] / 255.0;
    rgba[2] = intRGBA[2] / 255.0;
    rgba[3] = intRGBA[3] / 255.0;
}

/**
 * Set the color in the control with RGBA
 * values ranging 0 to 255.
 * @param rgba
 *    Input RGBA values.
 */
void 
ColorEditorWidget::setColor(const int rgba[4])
{
    this->controlsWidgetGroup->blockAllSignals(true);
    
    this->redSpinBox->setValue(rgba[0]);
    this->greenSpinBox->setValue(rgba[1]);
    this->blueSpinBox->setValue(rgba[2]);
    if (this->alphaSpinBox != NULL) {
        this->alphaSpinBox->setValue(rgba[3]);
    }
    
    this->controlsWidgetGroup->blockAllSignals(false);
}

/**
 * Get the color in the control with RGBA
 * values ranging 0 to 255
 * @param rgba
 *    Output RGBA values.
 */
void 
ColorEditorWidget::getColor(int rgba[4]) const
{
    rgba[0] = this->redSpinBox->value();
    rgba[1] = this->greenSpinBox->value();
    rgba[2] = this->blueSpinBox->value();
    if (this->alphaSpinBox != NULL) {
        rgba[3] = this->alphaSpinBox->value();
    }
    else {
        rgba[3] = 255;
    }
}

/**
 * Called when a red component value is changed.
 */
void 
ColorEditorWidget::redValueChanged(int value)
{
    this->emitColorChangedSignal();
}

/**
 * Called when a green component value is changed.
 */
void 
ColorEditorWidget::greenValueChanged(int value)
{
    this->emitColorChangedSignal();
}

/**
 * Called when a blue component value is changed.
 */
void 
ColorEditorWidget::blueValueChanged(int value)
{
    this->emitColorChangedSignal();
}

/**
 * Called when a alpha component value is changed.
 */
void 
ColorEditorWidget::alphaValueChanged(int value)
{
    this->emitColorChangedSignal();
}

/**
 * Call to emit the color changed signals.
 */
void 
ColorEditorWidget::emitColorChangedSignal()
{
    float rgba[4];
    this->getColor(rgba);
    emit colorChanged(rgba);
    
    int intRgba[4];
    this->getColor(intRgba);
    emit colorChanged(intRgba);
}


