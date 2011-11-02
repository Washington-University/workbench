
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __CONNECTIVITY_LOADER_FILE_CONTROL_DECLARE__
#include "ConnectivityLoaderFileControl.h"
#undef __CONNECTIVITY_LOADER_FILE_CONTROL_DECLARE__

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "ConnectivityLoaderFile.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class ConnectivityLoaderFileControl 
 * \brief Controls one connectivity loader file.
 *
 */

/**
 * Constructor.
 */
ConnectivityLoaderFileControl::ConnectivityLoaderFileControl(QWidget* parent,
                                                             const int32_t loaderIndex)
: QObject(parent)
{
    this->loaderNumberLabel = new QLabel(AString::number(loaderIndex + 1));
    
    this->fileNameLineEdit = new QLineEdit();
    this->fileNameLineEdit->setReadOnly(true);
    
    this->fileTypeLabel = new QLabel();

    this->fileButton = new QToolButton();
    this->fileButton->setText("File");
    QObject::connect(this->fileButton, SIGNAL(triggered(QAction*)),
                     this, SLOT(fileButtonPressed()));

    this->networkButton = new QToolButton();
    this->networkButton->setText("Network");
    QObject::connect(this->networkButton, SIGNAL(triggered(QAction*)),
                     this, SLOT(networkButtonPressed()));
    
    this->removeButton = new QToolButton();
    this->removeButton->setText("Remove");
    QObject::connect(this->removeButton, SIGNAL(triggered(QAction*)),
                     this, SLOT(removeButtonPressed()));
    
    this->connectivityLoaderFile = NULL;
    
    this->widgetGroup = new WuQWidgetObjectGroup(this);
    this->widgetGroup->add(this->loaderNumberLabel);
    this->widgetGroup->add(this->fileNameLineEdit);
    this->widgetGroup->add(this->fileTypeLabel);
    this->widgetGroup->add(this->fileButton);
    this->widgetGroup->add(this->networkButton);
    this->widgetGroup->add(this->removeButton);
}

/**
 * Destructor.
 */
ConnectivityLoaderFileControl::~ConnectivityLoaderFileControl()
{
    
}

/**
 * Called when file button is pressed.
 */
void 
ConnectivityLoaderFileControl::fileButtonPressed()
{
    
}

/**
 * Called when network button is pressed.
 */
void 
ConnectivityLoaderFileControl::networkButtonPressed()
{
    
}

/**
 * Called when remove button is pressed.
 */
void 
ConnectivityLoaderFileControl::removeButtonPressed()
{
    
}

/**
 * Update the control.
 * @param clf
 *    Connectivity Loader File in this control or NULL if none.
 */
void 
ConnectivityLoaderFileControl::update(ConnectivityLoaderFile* clf)
{
    this->connectivityLoaderFile = clf;
    
    if (clf != NULL) {
        this->fileNameLineEdit->setText(clf->getFileNameNoPath());
        this->fileTypeLabel->setText(DataFileTypeEnum::toGuiName(clf->getDataFileType()));
        this->widgetGroup->setVisible(true);
    }
    else {
        this->widgetGroup->setVisible(false);
    }
}


