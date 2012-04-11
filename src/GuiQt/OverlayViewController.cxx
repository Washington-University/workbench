
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

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

#define __OVERLAY_VIEW_CONTROLLER_DECLARE__
#include "OverlayViewController.h"
#undef __OVERLAY_VIEW_CONTROLLER_DECLARE__

#include "Overlay.h"

using namespace caret;


    
/**
 * \class caret::OverlayViewController 
 * \brief View Controller for an overlay.
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent object.  An instance of this should be attached
 *    to a parent, such as a QWidget or one of its subclasses so
 *    so that an instance of this is deleted when the parent is
 *    destroyed.
 * @param gridLayout
 *    Grid layout for the overlay controls.
 * @param showTopHorizontalLine
 *    If true, display a horizontal line above the controls.
 */
OverlayViewController::OverlayViewController(QObject* parent,
                                             QGridLayout* gridLayout,
                                             const bool showTopHorizontalLine)
: QObject(parent)
{
    this->overlay = NULL;
    
    QFrame* topHorizontalLineWidget = NULL;
    if (showTopHorizontalLine) {
        topHorizontalLineWidget = new QFrame();
        topHorizontalLineWidget->setLineWidth(1);
        topHorizontalLineWidget->setMidLineWidth(2);
        topHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Plain);

    }
    
    QFrame* verticalLineWidget = new QFrame();
    verticalLineWidget->setLineWidth(1);
    verticalLineWidget->setMidLineWidth(2);
    verticalLineWidget->setFrameStyle(QFrame::VLine | QFrame::Plain);
    
    this->enabledCheckBox = new QCheckBox("");
    QObject::connect(this->enabledCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(enabledCheckBoxStateChanged(int)));
    
    QLabel* fileLabel = new QLabel("File");
    this->fileComboBox = new QComboBox();
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(fileComboBoxSelected(int)));
    
    QLabel* mapLabel = new QLabel("Map");
    this->mapComboBox = new QComboBox();
    QObject::connect(this->fileComboBox, SIGNAL(activated(int)),
                     this, SLOT(mapComboBoxSelected(int)));
    
    QIcon settingsIcon;
    const bool settingsIconValid = WuQtUtilities::loadIcon(":/overlay_wrench.png",
                                                           settingsIcon);

    QAction* settingsAction = WuQtUtilities::createAction("S", 
                                                          "", 
                                                          this, 
                                                          this, 
                                                          SLOT(settingsToolButtonSelected()));
    if (settingsIconValid) {
        settingsAction->setIcon(settingsIcon);
    }
    QToolButton* settingsToolButton = new QToolButton();
    settingsToolButton->setDefaultAction(settingsAction);
    
    int row = gridLayout->rowCount();
    if (topHorizontalLineWidget != NULL) {
        gridLayout->addWidget(topHorizontalLineWidget,
                              row, 0, 1, 4);
    }
        
    row++;
    gridLayout->addWidget(this->enabledCheckBox,
                          row, 0,
                          Qt::AlignHCenter);
    gridLayout->addWidget(fileLabel,
                          row, 2);
    gridLayout->addWidget(this->fileComboBox,
                          row, 3);
    
    gridLayout->addWidget(verticalLineWidget,
                          row, 1, 2, 1);
    
    row++;
    gridLayout->addWidget(settingsToolButton,
                          row, 0,
                          Qt::AlignHCenter);
    gridLayout->addWidget(mapLabel,
                          row, 2);
    gridLayout->addWidget(this->mapComboBox,
                          row, 3);
    
    this->widgetsGroup = new WuQWidgetObjectGroup(this);
    this->widgetsGroup->addLayout(gridLayout);
}

/**
 * Destructor.
 */
OverlayViewController::~OverlayViewController()
{
    
}

/**
 * Set the visiblity of this overlay view controller.
 */
void 
OverlayViewController::setVisible(bool visible)
{
    this->widgetsGroup->setVisible(visible);
}


/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::fileComboBoxSelected(int indx)
{
    
}

/**
 * Called when a selection is made from the file combo box.
 * @parm indx
 *    Index of selection.
 */
void 
OverlayViewController::mapComboBoxSelected(int indx)
{
    
}

/**
 * Called when enabled checkbox state is changed
 * @parm state
 *    New state (selection status).
 */
void 
OverlayViewController::enabledCheckBoxStateChanged(int state)
{
    
}

/**
 * Called when the settings tool button is selected.
 */
void 
OverlayViewController::settingsToolButtonSelected()
{
    
}

/**
 * Update this view controller using the given overlay.
 * @param overlay
 *   Overlay that is used in this view controller.
 */
void 
OverlayViewController::updateViewController(Overlay* overlay)
{
    this->widgetsGroup->blockAllSignals(true);
    
    this->overlay = overlay;
    
    this->widgetsGroup->blockAllSignals(false);
}


