
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <QApplication>
#include <QButtonGroup>
#include <QClipboard>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#define __SCENE_BASE_PATH_WIDGET_DECLARE__
#include "SceneBasePathWidget.h"
#undef __SCENE_BASE_PATH_WIDGET_DECLARE__

#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "FileInformation.h"
#include "SceneFile.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::SceneBasePathWidget 
 * \brief Widget for setting scene file base path.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param widget
 *     Optional parent.
 */
SceneBasePathWidget::SceneBasePathWidget(QWidget* widget)
: QWidget(widget)
{
    m_automaticRadioButton = new QRadioButton("Automatic");
    m_automaticBasePathLineEdit = new QLineEdit;
    m_automaticBasePathLineEdit->setReadOnly(true);
    
    m_customRadioButton    = new QRadioButton("Custom");
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_automaticRadioButton);
    buttGroup->addButton(m_customRadioButton);
    QObject::connect(buttGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
                     this, [=](QAbstractButton* button){ this->basePathTypeButtonGroupClicked(button); });
    
    m_customBasePathComboBox = WuQFactory::newComboBox();
    m_customBasePathComboBox->setEditable(false);
    QObject::connect(m_customBasePathComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
                     this, [=](int index){ this->customPathComboBoxActivated(index); });
    
    QPushButton* copyAutoBasePathPushButton = new QPushButton("Copy");
    copyAutoBasePathPushButton->setToolTip("Copy automatic base path to clipboard");
    QObject::connect(copyAutoBasePathPushButton, &QPushButton::clicked,
                     this, &SceneBasePathWidget::copyAutoBasePathToClipboard);
    
    QPushButton* customBrowsePushButton = new QPushButton("Browse...");
    QObject::connect(customBrowsePushButton, &QPushButton::clicked,
                     this, &SceneBasePathWidget::customBrowseButtonClicked);
    
    QPushButton* whatsThisPushButton = new QPushButton("What's this?");
    whatsThisPushButton->setSizePolicy(QSizePolicy::Fixed, whatsThisPushButton->sizePolicy().verticalPolicy());
    QObject::connect(whatsThisPushButton, &QPushButton::clicked,
                     this, &SceneBasePathWidget::whatsThisBasePath);
    
    QGroupBox* groupBox = new QGroupBox("Base Path");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int row = 0;
    gridLayout->addWidget(whatsThisPushButton,
                          row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_automaticRadioButton,
                          row, 0);
    gridLayout->addWidget(m_automaticBasePathLineEdit,
                          row, 1);
    gridLayout->addWidget(copyAutoBasePathPushButton,
                          row, 2);
    row++;
    gridLayout->addWidget(m_customRadioButton,
                          row, 0);
    gridLayout->addWidget(m_customBasePathComboBox,
                          row, 1);
    gridLayout->addWidget(customBrowsePushButton,
                          row, 2);
    
    QVBoxLayout* widgetLayout = new QVBoxLayout(this);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->addWidget(groupBox);

//    setSizePolicy(sizePolicy().horizontalPolicy(),
//                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
SceneBasePathWidget::~SceneBasePathWidget()
{
}

/**
 * Update the widget with the given scene file.
 *
 * @param sceneFile
 *     The scene file.
 */
void
SceneBasePathWidget::updateWithSceneFile(SceneFile* sceneFile)
{
    m_sceneFile = sceneFile;
    
    m_automaticBasePathLineEdit->clear();
    
    QSignalBlocker customPathSignalBlocker(m_customBasePathComboBox);
    
    if (m_sceneFile != NULL) {
        AString customBaseDirectoryName = m_sceneFile->getBalsaCustomBaseDirectory();
        m_customBasePathComboBox->clear();
        
        const std::vector<AString> dirNames = m_sceneFile->getBaseDirectoryHierarchyForDataFiles();
        if ( ! customBaseDirectoryName.isEmpty()) {
            /*
             * If the base directory is in neither the base path hierarchy
             * nor the user custom paths, add it to the user custom paths
             * so that it appears in the combo box and is selected.
             */
            if (std::find(dirNames.begin(),
                          dirNames.end(),
                          customBaseDirectoryName) == dirNames.end()) {
                if (std::find(s_userCustomBasePaths.begin(),
                              s_userCustomBasePaths.end(),
                              customBaseDirectoryName) == s_userCustomBasePaths.end()) {
                    s_userCustomBasePaths.push_back(customBaseDirectoryName);
                }
            }
        }
        for (auto name : s_userCustomBasePaths) {
            m_customBasePathComboBox->addItem(name);
        }
        for (auto name : dirNames) {
            m_customBasePathComboBox->addItem(name);
        }

        if ( ! customBaseDirectoryName.isEmpty()) {
            m_customBasePathComboBox->setCurrentText(customBaseDirectoryName);
        }

        switch (m_sceneFile->getBasePathType()) {
            case SceneFileBasePathTypeEnum::AUTOMATIC:
                m_automaticRadioButton->setChecked(true);
                m_customBasePathComboBox->setEnabled(false);
                break;
            case SceneFileBasePathTypeEnum::CUSTOM:
                m_customRadioButton->setChecked(true);
                m_customBasePathComboBox->setEnabled(true);
                break;
        }
        
        AString baseDirectoryName;
        std::vector<AString> missingFileNames;
        AString errorMessage;
        sceneFile->findBaseDirectoryForDataFiles(baseDirectoryName,
                                                 missingFileNames,
                                                 errorMessage);
        m_automaticBasePathLineEdit->setText(baseDirectoryName);
        setEnabled(true);
    }
    else {
        m_customBasePathComboBox->clear();
        setEnabled(false);
    }
}

/**
 * Copy the automatic base path to the clipboard
 */
void
SceneBasePathWidget::copyAutoBasePathToClipboard()
{
    const QString txt = m_automaticBasePathLineEdit->text().trimmed();
    if ( ! txt.isEmpty()) {
        QApplication::clipboard()->setText(txt,
                                           QClipboard::Clipboard);
    }
}

/**
 * Called when the user selects and item in the custom
 * path combo box.
 *
 * @param index
 *     Index of the item selected.
 */
void
SceneBasePathWidget::customPathComboBoxActivated(int index)
{
    const AString text = m_customBasePathComboBox->itemText(index);
    if ( ! text.isEmpty()) {
        m_sceneFile->setBalsaCustomBaseDirectory(text);
    }
}


/**
 * Gets called when the custom browse button is clicked.
 */
void
SceneBasePathWidget::customBrowseButtonClicked()
{
    const AString msg("The only reason to use this Browse button is when the Automatic mode fails.  "
                      "If you select a base path that is a not a parent directory of the "
                      "scene file and its data files, zipping of the scene file and its "
                      "data files will fail.\n\n"
                      "Do you want to continue?");
    if ( ! WuQMessageBox::warningOkCancel(this, msg)) {
        return;
    }
    
    CaretAssert(m_sceneFile);
    
    /*
     * Let user choose directory path
     */
    QString directoryName;
    FileInformation fileInfo(m_customBasePathComboBox->currentText().trimmed());
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            directoryName = fileInfo.getAbsoluteFilePath();
        }
    }
    const AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(this,
                                                                                 "Choose Base Path",
                                                                                 directoryName);
    /*
     * If user cancels,  return
     */
    if (newDirectoryName.isEmpty()) {
        return;
    }

    if (std::find(s_userCustomBasePaths.begin(),
                  s_userCustomBasePaths.end(),
                  newDirectoryName) == s_userCustomBasePaths.end()) {
        s_userCustomBasePaths.push_back(newDirectoryName);
    }
    m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::CUSTOM);
    m_sceneFile->setBalsaCustomBaseDirectory(newDirectoryName);
    updateWithSceneFile(m_sceneFile);
}

/**
 * Gets called when on of the radio buttons is clicked.
 *
 * @param button
 *     Button that was clicked.
 */
void
SceneBasePathWidget::basePathTypeButtonGroupClicked(QAbstractButton* button)
{
    if (button == m_automaticRadioButton) {
        m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::AUTOMATIC);
    }
    else if (button == m_customRadioButton) {
        m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::CUSTOM);
    }
    else {
        CaretAssertMessage(0, "Has a new SceneFileBasePathTypeEnum::Enum been added?");
    }
    
    updateWithSceneFile(m_sceneFile);
}

/**
 * @return True if the selections in the widget are valid, else false.
 *
 * @param errorMessageOut
 *     Output contains error information if false returned.
 */
bool
SceneBasePathWidget::isValid(AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    if (m_automaticRadioButton->isChecked()) {
        /* valid */
    }
    else if (m_customRadioButton->isChecked()) {
        const AString basePath = m_customBasePathComboBox->currentText().trimmed();
        if (basePath.isEmpty()) {
            errorMessageOut = "CUSTOM base path is empty.";
        }
        else {
            FileInformation fileInfo(basePath);
            if (fileInfo.exists()
                && fileInfo.isDirectory()) {
                /*
                 * Base path may not be in the scene file so update it
                 */
                m_sceneFile->setBalsaCustomBaseDirectory(basePath);
            }
            else {
                errorMessageOut = "CUSTOM base path is not a valid directory";
            }
        }
    }
    else {
        CaretAssert(0);
        errorMessageOut = "Neither AUTOMATIC nor CUSTOM is selected.";
    }
    
    return (errorMessageOut.isEmpty());
}

/**
 * Displays a dialog explaining base path options
 */
void
SceneBasePathWidget::whatsThisBasePath()
{
    const AString text("The Automatic Base Path is the \"lowest level\" path that contains the Scene File and "
                       "all data files referenced by the Scene File.  "
                       "The directory structure of all files contained in the Scene File relative to (at or below) "
                       "the Base Path will be preserved in your dataset when it is downloaded from BALSA and "
                       "unzipped by other users.  You may set a Custom path above the Automatic Base Path, but "
                       "it will add additional, unnecessary path layers to the unzipped dataset.");
    WuQMessageBox::informationOk(this,
                                 text);
    
}



