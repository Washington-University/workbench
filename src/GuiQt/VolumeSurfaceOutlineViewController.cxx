
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

#include <stdint.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__
#include "VolumeSurfaceOutlineViewController.h"
#undef __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__

#include "VolumeSurfaceOutlineColorOrTabViewController.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineViewController 
 * \brief View controller for volume surface outline
 *
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineViewController::VolumeSurfaceOutlineViewController(QWidget* parent)
: QWidget(parent)
{
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 100);
    
    for (int32_t i = 0; i < 10; i++) {
        QCheckBox* enabledCheckBox = new QCheckBox(" ");
        
        //VolumeSurfaceOutlineColorOrTabViewController* vsoc = new VolumeSurfaceOutlineColorOrTabViewController(parentPage,
        //                                                                                    vsos->getColorOrTabModel());
        
        QComboBox* surfaceComboBox = new QComboBox();
        surfaceComboBox->addItem("Surface");
        QComboBox* colorComboBox = new QComboBox();
        colorComboBox->addItem("Color");
        
        const float minLineWidth = 0.5;
        const float maxLineWidth = 100.0;
        const float stepSize = 0.5;
        QDoubleSpinBox* thicknessSpinBox = new QDoubleSpinBox();
        thicknessSpinBox->setRange(minLineWidth, 
                                         maxLineWidth);
        thicknessSpinBox->setSingleStep(stepSize);
        thicknessSpinBox->setFixedWidth(100);
        
        int row = gridLayout->rowCount();
        gridLayout->addWidget(enabledCheckBox, row, 0);
        gridLayout->addWidget(surfaceComboBox, row, 1, 1, 2);
        row = gridLayout->rowCount();
        gridLayout->addWidget(colorComboBox, row, 1);        
        gridLayout->addWidget(thicknessSpinBox, row, 2, Qt::AlignLeft);
    }
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineViewController::~VolumeSurfaceOutlineViewController()
{
    
}

