
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

#define __CONNECTIVITY_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityViewController.h"
#undef __CONNECTIVITY_VIEW_CONTROLLER_DECLARE__

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
#include "EventUserInterfaceUpdate.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ConnectivityViewController 
 * \brief View-Controller for one connectivity loader
 *
 */
/**
 * Constructor.
 */
ConnectivityViewController::ConnectivityViewController(const Qt::Orientation orientation,
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
    
    allConnectivityViewControllers.insert(this);
}

/**
 * Destructor.
 */
ConnectivityViewController::~ConnectivityViewController()
{
    allConnectivityViewControllers.erase(this);
}

/**
 * Update this view controller.
 * @param connectivityLoaderFile
 *    Connectivity loader file in this view controller.
 */
void 
ConnectivityViewController::updateViewController(ConnectivityLoaderFile* connectivityLoaderFile)
{
    this->connectivityLoaderFile = connectivityLoaderFile;
    if (this->connectivityLoaderFile != NULL) {
        Qt::CheckState enabledState = Qt::Unchecked;
        if (this->connectivityLoaderFile->isDataLoadingEnabled()) {
            enabledState = Qt::Checked;
        }
        this->enabledCheckBox->setCheckState(enabledState);
        
        this->fileNameLineEdit->setText(this->connectivityLoaderFile->getFileNameNoPath());
        
    }
}

/**
 * Update the view controller.
 */
void 
ConnectivityViewController::updateViewController()
{
    this->updateViewController(this->connectivityLoaderFile);    
}

/**
 * Called when enabled check box changes state.
 */
void 
ConnectivityViewController::enabledCheckBoxStateChanged(int state)
{
    const bool selected = (state == Qt::Checked);
    if (this->connectivityLoaderFile != NULL) {
        this->connectivityLoaderFile->setDataLoadingEnabled(selected);
        this->fileNameLineEdit->setText(this->connectivityLoaderFile->getFileNameNoPath());
    }
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
ConnectivityViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Update graphics and GUI after 
 */
void 
ConnectivityViewController::updateUserInterfaceAndGraphicsWindow()
{
    this->updateOtherConnectivityViewControllers();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    //EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update other connectivity view controllers other than 'this' instance.
 */
void 
ConnectivityViewController::updateOtherConnectivityViewControllers()
{
    for (std::set<ConnectivityViewController*>::iterator iter = allConnectivityViewControllers.begin();
         iter != allConnectivityViewControllers.end();
         iter++) {
        ConnectivityViewController* clvc = *iter;
        if (clvc != this) {
            clvc->updateViewController();
        }
    }
}




