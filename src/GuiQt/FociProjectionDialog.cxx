
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

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#define __FOCI_PROJECTION_DIALOG_DECLARE__
#include "FociProjectionDialog.h"
#undef __FOCI_PROJECTION_DIALOG_DECLARE__

#include "Brain.h"
#include "CursorDisplayScoped.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "FociFile.h"
#include "GuiManager.h"
#include "Surface.h"
#include "SurfaceProjector.h"
#include "SurfaceSelectionViewController.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::FociProjectionDialog 
 * \brief Dialog for projection of foci.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget.
 */
FociProjectionDialog::FociProjectionDialog(QWidget* parent)
: WuQDialogModal("Project Foci",
                 parent)
{
    QWidget* surfaceWidget = NULL; //createSurfaceSelectionWidget();
    
    QWidget* fociFileWidget = createFociFileSelectionWidget();
    
    QWidget* optionsWidget = createOptionsWidget();

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    if (surfaceWidget != NULL) {
        layout->addWidget(surfaceWidget);
    }
    layout->addWidget(fociFileWidget);
    layout->addWidget(optionsWidget);
    
    setCentralWidget(widget);
}

/**
 * Destructor.
 */
FociProjectionDialog::~FociProjectionDialog()
{
    
}

/**
 * Called when the OK button is clicked.
 */
void
FociProjectionDialog::okButtonClicked()
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<const SurfaceFile*> surfaceFiles = brain->getVolumeInteractionSurfaceFiles();

    SurfaceFile* leftSurfaceFile = NULL;
    SurfaceFile* rightSurfaceFile = NULL;
    SurfaceFile* cerebellumSurfaceFile = NULL;
    
    for (std::vector<const SurfaceFile*>::iterator iter = surfaceFiles.begin();
         iter != surfaceFiles.end();
         iter++) {
        const SurfaceFile* sf = *iter;
        
        switch (sf->getStructure()) {
            case StructureEnum::CORTEX_LEFT:
                leftSurfaceFile = const_cast<SurfaceFile*>(sf);
                break;
            case StructureEnum::CORTEX_RIGHT:
                rightSurfaceFile = const_cast<SurfaceFile*>(sf);
                break;
            case StructureEnum::CEREBELLUM:
                cerebellumSurfaceFile = const_cast<SurfaceFile*>(sf);
                break;
            default:
                break;
        }
    }
    
//    if (m_leftSurfaceCheckBox != NULL) {
//        if (m_leftSurfaceCheckBox->isChecked()) {
//            const Surface* sf = m_leftSurfaceViewController->getSurface();
//            if (sf != NULL) {
//                surfaceFiles.push_back(sf);
//            }
//        }
//    }
//    if (m_rightSurfaceCheckBox != NULL) {
//        if (m_rightSurfaceCheckBox->isChecked()) {
//            const Surface* sf = m_rightSurfaceViewController->getSurface();
//            if (sf != NULL) {
//                surfaceFiles.push_back(sf);
//            }
//        }
//    }
//    if (m_cerebellumSurfaceCheckBox != NULL) {
//        if (m_cerebellumSurfaceCheckBox->isChecked()) {
//            const Surface* sf = m_cerebellumSurfaceViewController->getSurface();
//            if (sf != NULL) {
//                surfaceFiles.push_back(sf);
//            }
//        }
//    }
    
    AString errorMessages = "";
    
    const int32_t numberOfFociFiles = static_cast<int32_t>(m_fociFiles.size());
    for (int32_t i = 0; i < numberOfFociFiles; i++) {
        if (m_fociFileCheckBoxes[i]->isChecked()) {
            SurfaceProjector projector(leftSurfaceFile,
                                       rightSurfaceFile,
                                       cerebellumSurfaceFile);

            if (m_projectAboveSurfaceCheckBox->isChecked()) {
                projector.setSurfaceOffset(m_projectAboveSurfaceSpinBox->value());
            }
            
            FociFile* ff = brain->getFociFile(i);
            try {
                projector.projectFociFile(ff);
            }
            catch (const SurfaceProjectorException& spe) {
                errorMessages += (spe.whatString() + "\n");
            }
        }
    }
    
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    cursor.restoreCursor();
    
    if (errorMessages.isEmpty() == false) {
        WuQMessageBox::errorOk(this,
                               errorMessages);
    }
    
    WuQDialogModal::okButtonClicked();
}

/**
 * @return The controls for surface selection.
 */
QWidget*
FociProjectionDialog::createSurfaceSelectionWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    
    BrainStructure* leftBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_LEFT,
                                                                  false);
    BrainStructure* rightBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_RIGHT,
                                                                   false);
    BrainStructure* cerebellumBrainStructure = brain->getBrainStructure(StructureEnum::CEREBELLUM,
                                                                        false);
    
    m_leftSurfaceCheckBox = NULL;
    m_leftSurfaceViewController = NULL;
    if (leftBrainStructure != NULL) {
        m_leftSurfaceCheckBox = new QCheckBox("Left: ");
        m_leftSurfaceCheckBox->setChecked(true);
        m_leftSurfaceViewController = new SurfaceSelectionViewController(this, leftBrainStructure);
        m_leftSurfaceViewController->updateControl();
    }
    
    m_rightSurfaceCheckBox = NULL;
    m_rightSurfaceViewController = NULL;
    if (rightBrainStructure != NULL) {
        m_rightSurfaceCheckBox = new QCheckBox("Right: ");
        m_rightSurfaceCheckBox->setChecked(true);
        m_rightSurfaceViewController = new SurfaceSelectionViewController(this, rightBrainStructure);
        m_rightSurfaceViewController->updateControl();
    }
    
    m_cerebellumSurfaceCheckBox = NULL;
    m_cerebellumSurfaceViewController = NULL;
    if (cerebellumBrainStructure != NULL) {
        m_cerebellumSurfaceCheckBox = new QCheckBox("Cerebellum: ");
        m_cerebellumSurfaceCheckBox->setChecked(true);
        m_cerebellumSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                               cerebellumBrainStructure);
        m_cerebellumSurfaceViewController->updateControl();
    }
    
    QGroupBox* groupBox = new QGroupBox("Projection Surfaces");
    QGridLayout* layout = new QGridLayout(groupBox);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    int rowIndex = layout->rowCount();
    if (m_leftSurfaceCheckBox != NULL) {
        layout->addWidget(m_leftSurfaceCheckBox, rowIndex, 0);
        layout->addWidget(m_leftSurfaceViewController->getWidget(), rowIndex, 1);
        rowIndex++;
    }
    if (m_rightSurfaceCheckBox != NULL) {
        layout->addWidget(m_rightSurfaceCheckBox, rowIndex, 0);
        layout->addWidget(m_rightSurfaceViewController->getWidget(), rowIndex, 1);
        rowIndex++;
    }
    if (m_cerebellumSurfaceCheckBox != NULL) {
        layout->addWidget(m_cerebellumSurfaceCheckBox, rowIndex, 0);
        layout->addWidget(m_cerebellumSurfaceViewController->getWidget(), rowIndex, 1);
        rowIndex++;
    }
    
    return groupBox;
}

/**
 * @return The controls for foci file selection.
 */
QWidget*
FociProjectionDialog::createFociFileSelectionWidget()
{
    QGroupBox* groupBox = new QGroupBox("Foci Files for Projection");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numberOfFociFiles = brain->getNumberOfFociFiles();
    for (int32_t i = 0; i < numberOfFociFiles; i++) {
        FociFile* ff = brain->getFociFile(i);
        m_fociFiles.push_back(ff);
        QCheckBox* checkBox = new QCheckBox(ff->getFileNameNoPath());
        checkBox->setChecked(true);
        m_fociFileCheckBoxes.push_back(checkBox);
        
        layout->addWidget(checkBox);
    }
    
    return groupBox;
}

/**
 * @return The options controls.
 */
QWidget*
FociProjectionDialog::createOptionsWidget()
{
    m_projectAboveSurfaceCheckBox = new QCheckBox("Project fixed distance above surface(s)");
    
    m_projectAboveSurfaceSpinBox = new QDoubleSpinBox();
    m_projectAboveSurfaceSpinBox->setRange(-100000.0, 100000.0);
    m_projectAboveSurfaceSpinBox->setDecimals(2);
    m_projectAboveSurfaceSpinBox->setSingleStep(1.0);
    
    QObject::connect(m_projectAboveSurfaceCheckBox, SIGNAL(toggled(bool)),
                     m_projectAboveSurfaceSpinBox, SLOT(setEnabled(bool)));
    m_projectAboveSurfaceCheckBox->setChecked(false);
    
    QGroupBox* groupBox = new QGroupBox("Projection Options");
    QGridLayout* layout = new QGridLayout(groupBox);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    int rowIndex = layout->rowCount();
    
    layout->addWidget(m_projectAboveSurfaceCheckBox, rowIndex, 0);
    layout->addWidget(m_projectAboveSurfaceSpinBox, rowIndex, 1);
    rowIndex++;
    
    return groupBox;
}


