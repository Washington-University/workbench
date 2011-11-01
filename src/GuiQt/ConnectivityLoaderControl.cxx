
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

#define __CONNECTIVITY_LOADER_CONTROL_DECLARE__
#include "ConnectivityLoaderControl.h"
#undef __CONNECTIVITY_LOADER_CONTROL_DECLARE__

#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

#include "Brain.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderFileControl.h"
#include "ConnectivityLoaderManager.h"
#include "GuiManager.h"

using namespace caret;


static const int COLUMN_SELECTOR  = 0;
static const int COLUMN_FILE      = 1;
static const int COLUMN_FILE_TYPE = 2;
static const int COLUMN_BROWSE    = 3;
static const int COLUMN_NETWORK   = 4;
static const int COLUMN_CLEAR     = 5;
static const int COLUMN_REMOVE    = 6;
    
/**
 * \class ConnectivityLoaderControl 
 * \brief User interface for connectivity selection.
 *
 * Creates a user-interface for connectivity selection 
 * including adding and removing connectivity loaders.
 */
/**
 * Constructor.
 */
ConnectivityLoaderControl::ConnectivityLoaderControl(QWidget* parent)
: QWidget(parent)
{
    QLabel* selectorLabel = new QLabel("Selector");
    QLabel* fileLabel = new QLabel("File");
    QLabel* browseButton = new QLabel("Browse");
    QLabel* networkButton = new QLabel("Network");
    QLabel* fileTypeLabel = new QLabel("File Type");
    QLabel* removeLabel = new QLabel("Remove");
    
    this->loaderLayout = new QGridLayout();
    this->loaderLayout->addWidget(selectorLabel, 0, COLUMN_SELECTOR);
    this->loaderLayout->addWidget(fileLabel, 0, COLUMN_FILE);
    this->loaderLayout->addWidget(browseButton, 0, COLUMN_BROWSE);
    this->loaderLayout->addWidget(networkButton, 0, COLUMN_NETWORK);
    this->loaderLayout->addWidget(fileTypeLabel, 0, COLUMN_FILE_TYPE);
    this->loaderLayout->addWidget(removeLabel, 0, COLUMN_REMOVE);
    this->loaderLayout->setColumnStretch(COLUMN_FILE, 100);
    
    QPushButton* addPushButton = new QPushButton("Add Connectivity Loader");
    addPushButton->setFixedSize(addPushButton->sizeHint());
    QObject::connect(addPushButton, SIGNAL(clicked()),
                     this, SLOT(addConnectivityLoader()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(this->loaderLayout);
    layout->addWidget(addPushButton);
    layout->addStretch();
}

/**
 * Destructor.
 */
ConnectivityLoaderControl::~ConnectivityLoaderControl()
{
    
}

void
ConnectivityLoaderControl::updateControl()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    int32_t numLoaderWidgets = static_cast<int32_t>(this->fileLoaderControls.size());
    
    /*
     * Create new rows, as needed
     */
    const int32_t numberOfConnectivityLoaders = manager->getNumberOfConnectivityLoaderFiles();
    for (int32_t i = 0; i < numberOfConnectivityLoaders; i++) {
        if (i >= numLoaderWidgets) {
            ConnectivityLoaderFileControl* clfc = new ConnectivityLoaderFileControl(this, i);
            this->fileLoaderControls.push_back(clfc);
            const int row = this->loaderLayout->rowCount();
            this->loaderLayout->addWidget(clfc->loaderNumberLabel, row, COLUMN_SELECTOR);
            this->loaderLayout->addWidget(clfc->fileNameLineEdit, row, COLUMN_FILE);
            this->loaderLayout->addWidget(clfc->browseButton, row, COLUMN_BROWSE);
            this->loaderLayout->addWidget(clfc->networkButton, row, COLUMN_NETWORK);
            this->loaderLayout->addWidget(clfc->fileTypeLabel, row, COLUMN_FILE_TYPE);
            this->loaderLayout->addWidget(clfc->clearButton, row, COLUMN_CLEAR);
            this->loaderLayout->addWidget(clfc->removeButton, row, COLUMN_REMOVE);
        }
    }
    
    /*
     * Update rows
     */
    numLoaderWidgets = static_cast<int32_t>(this->fileLoaderControls.size());
    for (int32_t i = 0; i < numLoaderWidgets; i++) {
        ConnectivityLoaderFile* clf = NULL;
        if (i < numberOfConnectivityLoaders) {
            clf = manager->getConnectivityLoaderFile(i);
        }
        this->fileLoaderControls[i]->update(clf);
    }
    
    this->adjustSize();
}

void 
ConnectivityLoaderControl::addConnectivityLoader()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    manager->addConnectivityLoaderFile();
    this->updateControl();
}
