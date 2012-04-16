
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
ConnectivityLoaderViewController::ConnectivityLoaderViewController(const bool showTopHorizontalLine,
                                                                   QWidget* parent)
: QWidget(parent)
{
    QFrame* topHorizontalLineWidget = NULL;
    if (showTopHorizontalLine) {
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
    
    this->openAction = WuQtUtilities::createAction("Open...",
                                                      "Open a Connectivity File",
                                                      this,
                                                      this,
                                                      SLOT(openActionTriggered()));
    
    QToolButton* openToolButton = new QToolButton();
    openToolButton->setDefaultAction(openAction);
    
    this->graphCheckBox = new QCheckBox("Graph");
    QObject::connect(this->graphCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(graphCheckBoxStateChanged(int)));
    
    QLabel* timePointLabel = new QLabel("Timepoint: ");
    this->timePointSpinBox = new QSpinBox();
    
    QAction* animateAction = WuQtUtilities::createAction("Animate",
                                                         "Animate though time-series",
                                                         this,
                                                         this,
                                                         SLOT(animateActionTriggered()));
    QToolButton* animateToolButton = new QToolButton();
    animateToolButton->setDefaultAction(animateAction);
    
    QFrame* verticalLineWidget = new QFrame();
    verticalLineWidget->setLineWidth(1);
    verticalLineWidget->setMidLineWidth(2);
    verticalLineWidget->setFrameStyle(QFrame::VLine | QFrame::Plain);
    
    QHBoxLayout* rowTwoLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(rowTwoLayout, 4, 2);
    rowTwoLayout->addWidget(openToolButton);
    rowTwoLayout->addWidget(animateToolButton);
    rowTwoLayout->addWidget(timePointLabel);
    rowTwoLayout->addWidget(this->timePointSpinBox);
    rowTwoLayout->addWidget(this->graphCheckBox);
    rowTwoLayout->addStretch();
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutMargins(gridLayout, 2, 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 100);
    
    int row = gridLayout->rowCount();
    if (topHorizontalLineWidget != NULL) {
        gridLayout->addWidget(topHorizontalLineWidget,
                              row, 0, 1, 4);
    }
    row = gridLayout->rowCount();
    gridLayout->addWidget(this->enabledCheckBox, row, 0);
    gridLayout->addWidget(verticalLineWidget, row, 1, 2, 1);
    gridLayout->addWidget(fileNameLabel, row, 2);
    gridLayout->addWidget(this->fileNameLineEdit, row, 3);
    row = gridLayout->rowCount();
    gridLayout->addLayout(rowTwoLayout, row, 3);
}

/**
 * Destructor.
 */
ConnectivityLoaderViewController::~ConnectivityLoaderViewController()
{
    
}

/**
 * Update this view controller.
 * @param connectivityLoaderFile
 *    Connectivity loader file in this view controller.
 */
void 
ConnectivityLoaderViewController::updateViewController(ConnectivityLoaderFile* connectivityLoaderFile)
{
    
}

/**
 * Called when animate button is pressed.
 */
void
ConnectivityLoaderViewController::animateActionTriggered()
{
    
}

/**
 * Called when open button clicked.
 */
void 
ConnectivityLoaderViewController::openActionTriggered()
{
    
}

/**
 * Called when enabled check box changes state.
 */
void 
ConnectivityLoaderViewController::enabledCheckBoxStateChanged(int state)
{
    
}

/**
 * Called when graph check box state changes.
 */
void 
ConnectivityLoaderViewController::graphCheckBoxStateChanged(int state)
{
    
}


