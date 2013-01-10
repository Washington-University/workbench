
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __ADJUST_VIEW_DIALOG_DECLARE__
#include "AdjustViewDialog.h"
#undef __ADJUST_VIEW_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

using namespace caret;


    
/**
 * \class caret::AdjustViewDialog 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
AdjustViewDialog::AdjustViewDialog(BrowserTabContent* browserTabContent,
                                   QWidget* parent)
: WuQDialogModal("Adjust View",
                 parent)
{
    m_browserTabContent = browserTabContent;
    
    QLabel* xLabel = new QLabel("X");
    QLabel* yLabel = new QLabel("Y");
    QLabel* zLabel = new QLabel("Z");
    
    QLabel* translateLabel = new QLabel("Translation:");
    m_xTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_xTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_xTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_xTranslateDoubleSpinBox->setSingleStep(5.0);
    m_xTranslateDoubleSpinBox->setDecimals(1);
    QObject::connect(m_xTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));
    m_yTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_yTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_yTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_yTranslateDoubleSpinBox->setSingleStep(5.0);
    m_yTranslateDoubleSpinBox->setDecimals(1);
    QObject::connect(m_yTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));
    m_zTranslateDoubleSpinBox = new QDoubleSpinBox;
    m_zTranslateDoubleSpinBox->setMinimum(-100000.0);
    m_zTranslateDoubleSpinBox->setMaximum( 100000.0);
    m_zTranslateDoubleSpinBox->setSingleStep(5.0);
    m_zTranslateDoubleSpinBox->setDecimals(1);
    QObject::connect(m_zTranslateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));
    
    //
    // Rotation
    //
    QLabel* rotateLabel = new QLabel("Rotation:");
    m_xRotateDoubleSpinBox = new QDoubleSpinBox;
    m_xRotateDoubleSpinBox->setMinimum(-180.0);
    m_xRotateDoubleSpinBox->setMaximum(180.0);
    m_xRotateDoubleSpinBox->setSingleStep(5.0);
    m_xRotateDoubleSpinBox->setDecimals(2);
    QObject::connect(m_xRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));
    m_yRotateDoubleSpinBox = new QDoubleSpinBox;
    m_yRotateDoubleSpinBox->setMinimum(-180.0);
    m_yRotateDoubleSpinBox->setMaximum(180.0);
    m_yRotateDoubleSpinBox->setSingleStep(5.0);
    m_yRotateDoubleSpinBox->setDecimals(2);
    QObject::connect(m_yRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));
    m_zRotateDoubleSpinBox = new QDoubleSpinBox;
    m_zRotateDoubleSpinBox->setMinimum(-180.0);
    m_zRotateDoubleSpinBox->setMaximum(180.0);
    m_zRotateDoubleSpinBox->setSingleStep(5.0);
    m_zRotateDoubleSpinBox->setDecimals(2);
    QObject::connect(m_zRotateDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));

    //
    // Scale
    //
    QLabel* scaleLabel = new QLabel("Scaling:");
    m_scaleDoubleSpinBox = new QDoubleSpinBox;
    m_scaleDoubleSpinBox->setMinimum(0.001);
    m_scaleDoubleSpinBox->setMaximum(10000.0);
    m_scaleDoubleSpinBox->setSingleStep(0.1);
    m_scaleDoubleSpinBox->setDecimals(3);
    QObject::connect(m_scaleDoubleSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(slotViewChanged()));

    int column = 0;
    const int COLUMN_LABEL = column++;
    const int COLUMN_X     = column++;
    const int COLUMN_Y     = column++;
    const int COLUMN_Z     = column++;
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    
    int row = gridLayout->rowCount();
    gridLayout->addWidget(xLabel,
                          row,
                          COLUMN_X,
                          Qt::AlignCenter);
    gridLayout->addWidget(yLabel,
                          row,
                          COLUMN_Y,
                          Qt::AlignCenter);
    gridLayout->addWidget(zLabel,
                          row,
                          COLUMN_Z,
                          Qt::AlignCenter);
    row++;
    
    gridLayout->addWidget(translateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xTranslateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yTranslateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zTranslateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(rotateLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_xRotateDoubleSpinBox,
                          row,
                          COLUMN_X);
    gridLayout->addWidget(m_yRotateDoubleSpinBox,
                          row,
                          COLUMN_Y);
    gridLayout->addWidget(m_zRotateDoubleSpinBox,
                          row,
                          COLUMN_Z);
    row++;
    
    gridLayout->addWidget(scaleLabel,
                          row,
                          COLUMN_LABEL);
    gridLayout->addWidget(m_scaleDoubleSpinBox,
                          row,
                          COLUMN_X);
    row++;
    setCentralWidget(widget);
}

/**
 * Destructor.
 */
AdjustViewDialog::~AdjustViewDialog()
{
    
}

/**
 * called when float spin box changed.
 */
void
AdjustViewDialog::slotViewChanged()
{
//    if (creatingDialog) {
//        return;
//    }
//    if (brainModel == NULL) {
//        return;
//    }
//    
//    float t[3] = {
//        xTranslateDoubleSpinBox->value(),
//        yTranslateDoubleSpinBox->value(),
//        zTranslateDoubleSpinBox->value()
//    };
//    brainModel->setTranslation(viewNumber, t);
//    
//    float r[3] = {
//        xRotateDoubleSpinBox->value(),
//        yRotateDoubleSpinBox->value(),
//        zRotateDoubleSpinBox->value()
//    };
//    vtkTransform* matrix = brainModel->getRotationTransformMatrix(viewNumber);
//    matrix->Identity();
//    matrix->RotateZ(r[2]);
//    matrix->RotateX(r[0]);
//    matrix->RotateY(r[1]);
//    
//    brainModel->setScaling(viewNumber,
//                           xScaleDoubleSpinBox->value(),
//                           yScaleDoubleSpinBox->value(),
//                           zScaleDoubleSpinBox->value());
//    if (viewNumber == BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW) {
//        theMainWindow->updateTransformationMatrixEditor(NULL);
//    }
//    GuiBrainModelOpenGL::updateAllGL();
}

