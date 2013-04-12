
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

#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
#include "CiftiConnectivityMatrixViewController.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSpinBox>

#include "CiftiConnectivityMatrixDataFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::CiftiConnectivityMatrixViewController 
 * \brief View-Controller for one connectivity loader
 * \ingroup GuiQt
 */
/**
 * Constructor.
 */
CiftiConnectivityMatrixViewController::CiftiConnectivityMatrixViewController(const Qt::Orientation orientation,
                                                                   QGridLayout* gridLayout,
                                                                   QObject* parent)
: QObject(parent)
{
    m_ciftiConnectivityMatrixDataFile = NULL;
    
    m_enabledCheckBox = new QCheckBox(" ");
    QObject::connect(m_enabledCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(enabledCheckBoxStateChanged(int)));
    
    m_fileNameLineEdit = new QLineEdit("                 ");
    m_fileNameLineEdit->setReadOnly(true);
    
    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                   this);
    if (orientation == Qt::Horizontal) {
        int row = m_gridLayoutGroup->rowCount();
        m_gridLayoutGroup->addWidget(m_enabledCheckBox, row, 0);
        m_gridLayoutGroup->addWidget(m_fileNameLineEdit, row, 1);
    }
    else {
        int row = m_gridLayoutGroup->rowCount();
        m_gridLayoutGroup->addWidget(m_enabledCheckBox, row, 0);
        m_gridLayoutGroup->addWidget(m_fileNameLineEdit, row, 1);
    }
    
    s_allCiftiConnectivityMatrixViewControllers.insert(this);
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixViewController::~CiftiConnectivityMatrixViewController()
{
    s_allCiftiConnectivityMatrixViewControllers.erase(this);
}

/**
 * Create the grid layout for this view controller using the given orientation.
 * @param orientation
 *    Orientation in toolbox.
 * @return
 *    GridLayout setup for this view controller.
 */
QGridLayout* 
CiftiConnectivityMatrixViewController::createGridLayout(const Qt::Orientation /*orientation*/)
{
    QGridLayout* gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutMargins(gridLayout, 2, 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    
    QLabel* onLabel = new QLabel("On");
    QLabel* fileLabel = new QLabel("File");
    
    const int row = gridLayout->rowCount();
    gridLayout->addWidget(onLabel, row, 0, Qt::AlignHCenter);
    gridLayout->addWidget(fileLabel, row, 1, Qt::AlignHCenter);
    
    return gridLayout;
}

/**
 * Update this view controller.
 * @param connectivityLoaderFile
 *    Connectivity loader file in this view controller.
 */
void 
CiftiConnectivityMatrixViewController::updateViewController(CiftiConnectivityMatrixDataFile* ciftiConnectivityMatrixFile)
{
    m_ciftiConnectivityMatrixDataFile = ciftiConnectivityMatrixFile;
    if (m_ciftiConnectivityMatrixDataFile != NULL) {
        Qt::CheckState enabledState = Qt::Unchecked;
        if (m_ciftiConnectivityMatrixDataFile->isMapDataLoadingEnabled(0)) {
            enabledState = Qt::Checked;
        }
        m_enabledCheckBox->setCheckState(enabledState);
        
        m_fileNameLineEdit->setText(m_ciftiConnectivityMatrixDataFile->getFileName());
        
    }
}

/**
 * Update the view controller.
 */
void 
CiftiConnectivityMatrixViewController::updateViewController()
{
    updateViewController(m_ciftiConnectivityMatrixDataFile);    
}

/**
 * Called when enabled check box changes state.
 */
void
CiftiConnectivityMatrixViewController::enabledCheckBoxStateChanged(int state)
{
    const bool selected = (state == Qt::Checked);
    if (m_ciftiConnectivityMatrixDataFile != NULL) {
        m_ciftiConnectivityMatrixDataFile->setMapDataLoadingEnabled(0, selected);
        m_fileNameLineEdit->setText(m_ciftiConnectivityMatrixDataFile->getFileNameNoPath());
        updateOtherCiftiConnectivityMatrixViewControllers();
    }
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
CiftiConnectivityMatrixViewController::setVisible(bool visible)
{
    m_gridLayoutGroup->setVisible(visible);
}

/**
 * Update graphics and GUI after 
 */
void 
CiftiConnectivityMatrixViewController::updateUserInterfaceAndGraphicsWindow()
{
    updateOtherCiftiConnectivityMatrixViewControllers();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update other connectivity view controllers other than 'this' instance
 * that contain the same connectivity file.
 */
void 
CiftiConnectivityMatrixViewController::updateOtherCiftiConnectivityMatrixViewControllers()
{
    if (m_ciftiConnectivityMatrixDataFile != NULL) {
        for (std::set<CiftiConnectivityMatrixViewController*>::iterator iter = s_allCiftiConnectivityMatrixViewControllers.begin();
             iter != s_allCiftiConnectivityMatrixViewControllers.end();
             iter++) {
            CiftiConnectivityMatrixViewController* clvc = *iter;
            if (clvc != this) {
                if (clvc->m_ciftiConnectivityMatrixDataFile == m_ciftiConnectivityMatrixDataFile) {
                    clvc->updateViewController();
                }
            }
        }
    }
}




