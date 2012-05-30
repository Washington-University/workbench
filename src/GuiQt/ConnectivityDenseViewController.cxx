
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

#define __CONNECTIVITY_DENSE_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityDenseViewController.h"
#undef __CONNECTIVITY_DENSE_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSpinBox>

#include "ConnectivityLoaderFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ConnectivityDenseViewController 
 * \brief View-Controller for one connectivity loader
 *
 */
/**
 * Constructor.
 */
ConnectivityDenseViewController::ConnectivityDenseViewController(const Qt::Orientation orientation,
                                                                   QGridLayout* gridLayout,
                                                                   QObject* parent)
: QObject(parent)
{
    this->connectivityLoaderFile = NULL;
    
    this->enabledCheckBox = new QCheckBox(" ");
    QObject::connect(this->enabledCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(enabledCheckBoxStateChanged(int)));
    
    this->fileNameLineEdit = new QLineEdit("                 ");
    this->fileNameLineEdit->setReadOnly(true);
    
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                   this);
    if (orientation == Qt::Horizontal) {
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox, row, 0);
        this->gridLayoutGroup->addWidget(this->fileNameLineEdit, row, 1);
    }
    else {
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox, row, 0);
        this->gridLayoutGroup->addWidget(this->fileNameLineEdit, row, 1);
    }
    
    allConnectivityDenseViewControllers.insert(this);
}

/**
 * Destructor.
 */
ConnectivityDenseViewController::~ConnectivityDenseViewController()
{
    allConnectivityDenseViewControllers.erase(this);
}

/**
 * Create the grid layout for this view controller using the given orientation.
 * @param orientation
 *    Orientation in toolbox.
 * @return
 *    GridLayout setup for this view controller.
 */
QGridLayout* 
ConnectivityDenseViewController::createGridLayout(const Qt::Orientation /*orientation*/)
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
ConnectivityDenseViewController::updateViewController(ConnectivityLoaderFile* connectivityLoaderFile)
{
    this->connectivityLoaderFile = connectivityLoaderFile;
    if (this->connectivityLoaderFile != NULL) {
        Qt::CheckState enabledState = Qt::Unchecked;
        if (this->connectivityLoaderFile->isDataLoadingEnabled()) {
            enabledState = Qt::Checked;
        }
        this->enabledCheckBox->setCheckState(enabledState);
        
        this->fileNameLineEdit->setText(this->connectivityLoaderFile->getFileName());
        
    }
}

/**
 * Update the view controller.
 */
void 
ConnectivityDenseViewController::updateViewController()
{
    this->updateViewController(this->connectivityLoaderFile);    
}

/**
 * Called when enabled check box changes state.
 */
void 
ConnectivityDenseViewController::enabledCheckBoxStateChanged(int state)
{
    const bool selected = (state == Qt::Checked);
    if (this->connectivityLoaderFile != NULL) {
        this->connectivityLoaderFile->setDataLoadingEnabled(selected);
        this->fileNameLineEdit->setText(this->connectivityLoaderFile->getFileNameNoPath());
        this->updateOtherConnectivityDenseViewControllers();
    }
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
ConnectivityDenseViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
}

/**
 * Update graphics and GUI after 
 */
void 
ConnectivityDenseViewController::updateUserInterfaceAndGraphicsWindow()
{
    this->updateOtherConnectivityDenseViewControllers();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update other connectivity view controllers other than 'this' instance
 * that contain the same connectivity file.
 */
void 
ConnectivityDenseViewController::updateOtherConnectivityDenseViewControllers()
{
    if (this->connectivityLoaderFile != NULL) {
        for (std::set<ConnectivityDenseViewController*>::iterator iter = allConnectivityDenseViewControllers.begin();
             iter != allConnectivityDenseViewControllers.end();
             iter++) {
            ConnectivityDenseViewController* clvc = *iter;
            if (clvc != this) {
                if (clvc->connectivityLoaderFile == this->connectivityLoaderFile) {
                    clvc->updateViewController();
                }
            }
        }
    }
}




