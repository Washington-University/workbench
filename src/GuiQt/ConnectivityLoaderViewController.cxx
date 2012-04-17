
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

#define __CONNECTIVITY_LOADER_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityLoaderViewController.h"
#undef __CONNECTIVITY_LOADER_VIEW_CONTROLLER_DECLARE__

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
 * \class caret::ConnectivityLoaderViewController 
 * \brief View-Controller for one connectivity loader
 *
 */
/**
 * Constructor.
 */
ConnectivityLoaderViewController::ConnectivityLoaderViewController(const Qt::Orientation orientation,
                                                                   QGridLayout* gridLayout,
                                                                   const bool showTopHorizontalBar,
                                                                   QObject* parent)
: QObject(parent)
{
    this->connectivityLoaderFile = NULL;
    
    QFrame* topHorizontalLineWidget = NULL;
    if (showTopHorizontalBar) {
        topHorizontalLineWidget = new QFrame();
        topHorizontalLineWidget->setLineWidth(1);
        topHorizontalLineWidget->setMidLineWidth(2);
        topHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Plain);
        
    }
    this->enabledCheckBox = new QCheckBox(" ");
    QObject::connect(this->enabledCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(enabledCheckBoxStateChanged(int)));
    
    QLabel* fileNameLabel = new QLabel("File");
    this->fileNameLineEdit = new QLineEdit("                 ");
    
    this->openFileAction = WuQtUtilities::createAction("Open...",
                                                      "Open a Connectivity File on Disk",
                                                      this,
                                                      this,
                                                      SLOT(openFileActionTriggered()));
    
    QToolButton* openFileToolButton = new QToolButton();
    openFileToolButton->setDefaultAction(openFileAction);
    
    this->openWebAction = WuQtUtilities::createAction("Web",
                                                       "Open a Connectivity File from Web",
                                                       this,
                                                       this,
                                                       SLOT(openWebActionTriggered()));
    
    QToolButton* openWebToolButton = new QToolButton();
    openWebToolButton->setDefaultAction(openWebAction);
    
    QFrame* verticalLineWidget = new QFrame();
    verticalLineWidget->setLineWidth(1);
    verticalLineWidget->setMidLineWidth(2);
    verticalLineWidget->setFrameStyle(QFrame::VLine | QFrame::Plain);
    
    QWidget* rowTwoWidget = new QWidget();
    QHBoxLayout* rowTwoLayout = new QHBoxLayout(rowTwoWidget);
    WuQtUtilities::setLayoutMargins(rowTwoLayout, 4, 2);
    rowTwoLayout->addWidget(openFileToolButton);
    rowTwoLayout->addWidget(openWebToolButton);
    rowTwoLayout->addStretch();
    
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                   this);
    if (orientation == Qt::Horizontal) {
        
    }
    else {
        int row = this->gridLayoutGroup->rowCount();
        if (topHorizontalLineWidget != NULL) {
            this->gridLayoutGroup->addWidget(topHorizontalLineWidget,
                                             row, 0, 1, 4);
        }
        row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->enabledCheckBox, row, 0);
        this->gridLayoutGroup->addWidget(verticalLineWidget, row, 1, 2, 1);
        this->gridLayoutGroup->addWidget(fileNameLabel, row, 2);
        this->gridLayoutGroup->addWidget(this->fileNameLineEdit, row, 3);
        row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(rowTwoWidget, row, 3);
    }
    
    allConnectivityViewControllers.insert(this);
}

/**
 * Destructor.
 */
ConnectivityLoaderViewController::~ConnectivityLoaderViewController()
{
    allConnectivityViewControllers.erase(this);
}

/**
 * Update this view controller.
 * @param connectivityLoaderFile
 *    Connectivity loader file in this view controller.
 */
void 
ConnectivityLoaderViewController::updateViewController(ConnectivityLoaderFile* connectivityLoaderFile)
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
ConnectivityLoaderViewController::updateViewController()
{
    this->updateViewController(this->connectivityLoaderFile);    
}

/**
 * Called when open file button clicked.
 */
void 
ConnectivityLoaderViewController::openFileActionTriggered()
{
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when open web button clicked.
 */
void 
ConnectivityLoaderViewController::openWebActionTriggered()
{
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when enabled check box changes state.
 */
void 
ConnectivityLoaderViewController::enabledCheckBoxStateChanged(int state)
{
    const bool selected = (state == Qt::Checked);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
ConnectivityLoaderViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
    
    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Update graphics and GUI after 
 */
void 
ConnectivityLoaderViewController::updateUserInterfaceAndGraphicsWindow()
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
ConnectivityLoaderViewController::updateOtherConnectivityViewControllers()
{
    for (std::set<ConnectivityLoaderViewController*>::iterator iter = allConnectivityViewControllers.begin();
         iter != allConnectivityViewControllers.end();
         iter++) {
        ConnectivityLoaderViewController* clvc = *iter;
        if (clvc != this) {
            clvc->updateViewController();
        }
    }
}




