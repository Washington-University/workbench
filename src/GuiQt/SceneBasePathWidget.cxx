
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
    QLabel* basePathInfoLabel = new QLabel("The Base Path is the \"lowest level\" path that contains the Scene File and "
                                           "all data files referenced by the Scene File.  It is typically used when creating a "
                                           "ZIP file.  A ZIP file may never reference files in a directory \"above\" the "
                                           "ZIP file's location.");
    basePathInfoLabel->setWordWrap(true);
    
    m_automaticRadioButton = new QRadioButton("Automatic");
    m_automaticBasePathLineEdit = new QLineEdit;
    m_automaticBasePathLineEdit->setReadOnly(true);
    
    m_customRadioButton    = new QRadioButton("Custom");
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_automaticRadioButton);
    buttGroup->addButton(m_customRadioButton);
    QObject::connect(buttGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
                     this, [=](QAbstractButton* button){ this->basePathTypeButtonGroupClicked(button); });
    
    m_basePathLineEdit = new QLineEdit;
    m_basePathLineEdit->setReadOnly(true);
    
    QPushButton* copyAutoBasePathPushButton = new QPushButton("Copy");
    copyAutoBasePathPushButton->setToolTip("Copy automatic base path to clipboard");
    QObject::connect(copyAutoBasePathPushButton, &QPushButton::clicked,
                     this, &SceneBasePathWidget::copyAutoBasePathToClipboard);
    
    QPushButton* browsePushButton = new QPushButton("Browse...");
    QObject::connect(browsePushButton, &QPushButton::clicked,
                     this, &SceneBasePathWidget::browseButtonClicked);
    
    QGroupBox* groupBox = new QGroupBox("Base Path");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    gridLayout->setColumnStretch(2, 0);
    int row = 0;
    gridLayout->addWidget(basePathInfoLabel,
                          row, 0, 1, 3);
    row++;
    gridLayout->addWidget(new QLabel(" "),
                          row, 0);
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
    gridLayout->addWidget(m_basePathLineEdit,
                          row, 1);
    gridLayout->addWidget(browsePushButton,
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
    
    if (m_sceneFile != NULL) {
        m_basePathLineEdit->setText(m_sceneFile->getBalsaBaseDirectory());
        switch (m_sceneFile->getBasePathType()) {
            case SceneFileBasePathTypeEnum::AUTOMATIC:
                m_automaticRadioButton->setChecked(true);
                m_basePathLineEdit->setEnabled(false);
                break;
            case SceneFileBasePathTypeEnum::CUSTOM:
                m_customRadioButton->setChecked(true);
                m_basePathLineEdit->setEnabled(true);
                break;
        }
        
        m_automaticBasePathLineEdit->setText(m_sceneFile->findBaseDirectoryForDataFiles());
        setEnabled(true);
    }
    else {
        m_basePathLineEdit->setText("");
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
 * Gets called when the browse button is clicked.
 */
void
SceneBasePathWidget::browseButtonClicked()
{
    CaretAssert(m_sceneFile);
    
    /*
     * Let user choose directory path
     */
    QString directoryName;
    FileInformation fileInfo(m_basePathLineEdit->text().trimmed());
    if (fileInfo.exists()) {
        if (fileInfo.isDirectory()) {
            directoryName = fileInfo.getAbsoluteFilePath();
        }
    }
    AString newDirectoryName = CaretFileDialog::getExistingDirectoryDialog(this,
                                                                           "Choose Base Path",
                                                                           directoryName);
    /*
     * If user cancels,  return
     */
    if (newDirectoryName.isEmpty()) {
        return;
    }
    
    m_sceneFile->setBalsaBaseDirectory(newDirectoryName);
    m_sceneFile->setBasePathType(SceneFileBasePathTypeEnum::CUSTOM);
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
        const AString basePath = m_basePathLineEdit->text().trimmed();
        if (basePath.isEmpty()) {
            errorMessageOut = "CUSTOM base path is empty.";
        }
        else {
            FileInformation fileInfo(basePath);
            if (fileInfo.exists()
                && fileInfo.isDirectory()) {
                /* Valid directory */
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


