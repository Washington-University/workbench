
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
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>

#include "Brain.h"
#include "CaretAssert.h"
#include "ConnectivityLoaderFile.h"
#include "ConnectivityLoaderManager.h"
#include "GuiManager.h"
#include "WuQFileDialog.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


static const int COLUMN_SELECTOR  = 0;
static const int COLUMN_FILE      = 1;
static const int COLUMN_FILE_TYPE = 2;
static const int COLUMN_DATA_SOURCE = 3;
static const int COLUMN_REMOVE    = 5;
    
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
    this->fileButtonsGroup = new QButtonGroup();
    QObject::connect(this->fileButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(fileButtonPressed(QAbstractButton*)));
    
    this->networkButtonsGroup = new QButtonGroup();
    QObject::connect(this->networkButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(networkButtonPressed(QAbstractButton*)));

    this->removeButtonsGroup = new QButtonGroup();
    QObject::connect(this->removeButtonsGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(removeButtonPressed(QAbstractButton*)));
    
    QLabel* selectorLabel = new QLabel("Selector");
    QLabel* fileLabel = new QLabel("File");
    QLabel* sourceLabel = new QLabel("Data Source");
    QLabel* fileTypeLabel = new QLabel("File Type");
    QLabel* removeLabel = new QLabel("Remove");
    
    this->loaderLayout = new QGridLayout();
    this->loaderLayout->addWidget(selectorLabel, 0, COLUMN_SELECTOR);
    this->loaderLayout->addWidget(fileLabel, 0, COLUMN_FILE);
    this->loaderLayout->addWidget(fileTypeLabel, 0, COLUMN_FILE_TYPE);
    this->loaderLayout->addWidget(sourceLabel, 0, COLUMN_DATA_SOURCE, 1, 2);
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
    int32_t numLoaderWidgets = static_cast<int32_t>(this->loaderNumberLabels.size());
    
    /*
     * Create new rows, as needed
     */
    const int32_t numberOfConnectivityLoaders = manager->getNumberOfConnectivityLoaderFiles();
    for (int32_t i = 0; i < numberOfConnectivityLoaders; i++) {
        if (i >= numLoaderWidgets) {
            QLabel* numberLabel = new QLabel(AString::number(i + 1));
            
            QLineEdit* fileNameLineEdit = new QLineEdit();
            fileNameLineEdit->setReadOnly(true);
            
            QLabel* fileTypeLabel = new QLabel();
            
            QToolButton* fileButton = new QToolButton();
            fileButton->setText("File");
            
            QToolButton* networkButton = new QToolButton();
            networkButton->setText("Web");
            
            QToolButton* removeButton = new QToolButton();
            removeButton->setText("X");
            
            WuQWidgetObjectGroup* widgetGroup = new WuQWidgetObjectGroup(this);
            widgetGroup = new WuQWidgetObjectGroup(this);
            widgetGroup->add(numberLabel);
            widgetGroup->add(fileNameLineEdit);
            widgetGroup->add(fileTypeLabel);
            widgetGroup->add(fileButton);
            widgetGroup->add(networkButton);
            widgetGroup->add(removeButton);
            this->rowWidgetGroups.push_back(widgetGroup);
            
            const int row = this->loaderLayout->rowCount();
            this->loaderLayout->addWidget(numberLabel, row, COLUMN_SELECTOR);
            this->loaderLayout->addWidget(fileNameLineEdit, row, COLUMN_FILE);
            this->loaderLayout->addWidget(fileButton, row, COLUMN_DATA_SOURCE);
            this->loaderLayout->addWidget(fileTypeLabel, row, COLUMN_FILE_TYPE);
            this->loaderLayout->addWidget(networkButton, row, COLUMN_DATA_SOURCE + 1);
            this->loaderLayout->addWidget(removeButton, row, COLUMN_REMOVE);
            
            this->fileButtonsGroup->addButton(fileButton);
            this->networkButtonsGroup->addButton(networkButton);
            this->removeButtonsGroup->addButton(removeButton);
            
            this->loaderNumberLabels.push_back(numberLabel);
            this->fileNameLineEdits.push_back(fileNameLineEdit);
            this->fileTypeLabels.push_back(fileTypeLabel);
            this->fileButtons.push_back(fileButton);
            this->networkButtons.push_back(networkButton);
            this->removeButtons.push_back(removeButton);
        }
    }
    
    /*
     * Update rows
     */
    numLoaderWidgets = static_cast<int32_t>(this->loaderNumberLabels.size());
    for (int32_t i = 0; i < numLoaderWidgets; i++) {
        ConnectivityLoaderFile* clf = NULL;
        if (i < numberOfConnectivityLoaders) {
            clf = manager->getConnectivityLoaderFile(i);
        }

        if (clf != NULL) {
            this->fileNameLineEdits[i]->setText(clf->getFileNameNoPath());
            this->fileTypeLabels[i]->setText(clf->getCiftiTypeName());
            this->rowWidgetGroups[i]->setVisible(true);
        }
        else {
            this->rowWidgetGroups[i]->setVisible(false);
        }
    }
    
    //this->adjustSize();
}

void 
ConnectivityLoaderControl::fileButtonPressed(QAbstractButton* button)
{
    int32_t fileIndex = -1;
    for (int32_t i = 0; i < static_cast<int32_t>(this->fileButtons.size()); i++) {
        if (this->fileButtons[i] == button) {
            fileIndex = i;
        }
    }
    CaretAssert(fileIndex >= 0);
    
    /*
     * CIFTI filters.
     */
    QStringList filenameFilterList;
    std::vector<DataFileTypeEnum::Enum> connectivityEnums;
    DataFileTypeEnum::getAllConnectivityEnums(connectivityEnums);
    for (int32_t i = 0; i < static_cast<int32_t>(connectivityEnums.size()); i++) {
        filenameFilterList.append(DataFileTypeEnum::toQFileDialogFilter(connectivityEnums[i]));
    }
    
    /*
     * Setup file selection dialog.
     */
    WuQFileDialog fd(this);
    fd.setAcceptMode(WuQFileDialog::AcceptOpen);
    fd.setNameFilters(filenameFilterList);
    fd.setFileMode(WuQFileDialog::ExistingFile);
    fd.setViewMode(WuQFileDialog::List);
    if (this->previousCiftiFileTypeFilter.isEmpty() == false) {
        fd.selectFilter(this->previousCiftiFileTypeFilter);
    }
    
    if (fd.exec()) {
        QStringList selectedFiles = fd.selectedFiles();
        this->previousCiftiFileTypeFilter = fd.selectedFilter();
        
        /*
         * Load each file.
         */
        QStringListIterator nameIter(selectedFiles);
        if (nameIter.hasNext()) {
            AString name = nameIter.next();
            ConnectivityLoaderManager* manager = GuiManager::get()->getBrain()->getConnectivityLoaderManager();
            ConnectivityLoaderFile* loaderFile = manager->getConnectivityLoaderFile(fileIndex);
            
            try {
                loaderFile->setup(name, 
                                  DataFileTypeEnum::fromQFileDialogFilter(this->previousCiftiFileTypeFilter, NULL));
            }
            catch (DataFileException e) {
                QMessageBox::critical(this, 
                                      "ERROR", 
                                      e.whatString());                
            }
        }        
    }
    
    this->updateControl();
}

void 
ConnectivityLoaderControl::networkButtonPressed(QAbstractButton* button)
{
    
    this->updateControl();
}

void 
ConnectivityLoaderControl::removeButtonPressed(QAbstractButton* button)
{
    
    this->updateControl();
}

void 
ConnectivityLoaderControl::addConnectivityLoader()
{
    Brain* brain = GuiManager::get()->getBrain();
    ConnectivityLoaderManager* manager = brain->getConnectivityLoaderManager();
    manager->addConnectivityLoaderFile();
    this->updateControl();
}
