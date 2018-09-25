
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __SCENE_FILE_INFORMATION_DIALOG_DECLARE__
#include "SceneFileInformationDialog.h"
#undef __SCENE_FILE_INFORMATION_DIALOG_DECLARE__

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QTextEdit>
#include <QTreeView>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CursorDisplayScoped.h"
#include "FileInformation.h"
#include "SceneDataFileTreeItemModel.h"
#include "SceneFile.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::SceneFileInformationDialog 
 * \brief Dialog for display of scene file organization
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param sceneFile
 *     The scene file whose file information is displayed.
 * @param parent
 *     Dialog's parent widget.
 */
SceneFileInformationDialog::SceneFileInformationDialog(const SceneFile* sceneFile,
                                                       QWidget* parent)
: WuQDialogNonModal("Scene File Information",
                    parent),
m_sceneFile(sceneFile)
{
    setDeleteWhenClosed(true);
    setApplyButtonText("");
    
    m_modeComboBox = new QComboBox();
    QLabel* modeLabel = new QLabel("Show Files:");
    m_modeComboBox->addItem("With Absolute Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::AbsolutePath));
    m_modeComboBox->addItem("Relative to Base Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::RelativeToBasePath));
    m_modeComboBox->addItem("Relative to Scene File Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::RelativeToSceneFilePath));
    m_modeComboBox->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToContents);
    QObject::connect(m_modeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     this, &SceneFileInformationDialog::modeComboBoxActivated);
    
    AString basePathName;
    std::vector<AString> missingDataFiles;
    AString basePathErrorMessage;
    const bool validBasePathFlag = sceneFile->findBaseDirectoryForDataFiles(basePathName,
                                                                            missingDataFiles,
                                                                            basePathErrorMessage);
    if ( ! validBasePathFlag) {
        basePathName = basePathErrorMessage;
    }
    
    QLabel* basePathLabel = new QLabel("Base Path:");
    m_basePathLineEdit = new QLineEdit();
    m_basePathLineEdit->setReadOnly(true);
    m_basePathLineEdit->setText(basePathName);
    m_basePathLineEdit->home(true);
    
    FileInformation sceneFileInfo(sceneFile->getFileName());
    QLabel* sceneFileNameLabel = new QLabel("Scene File Name:");
    m_sceneFileNameLineEdit = new QLineEdit();
    m_sceneFileNameLineEdit->setReadOnly(true);
    m_sceneFileNameLineEdit->setText(sceneFileInfo.getFileName());
    m_sceneFileNameLineEdit->home(true);

    QLabel* sceneFilePathLabel = new QLabel("Scene File Path:");
    m_sceneFilePathLineEdit = new QLineEdit();
    m_sceneFilePathLineEdit->setReadOnly(true);
    m_sceneFilePathLineEdit->setText(sceneFileInfo.getPathName());
    m_sceneFilePathLineEdit->home(true);
    
    m_textEdit = new QTextEdit();
    
    m_sceneFileHierarchyTreeView = new QTreeView();
    m_sceneFileHierarchyTreeView->setHeaderHidden(true);
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(m_sceneFileHierarchyTreeView, "Hierarchy");
    tabWidget->addTab(m_textEdit, "List");
    
    QGridLayout* namesLayout = new QGridLayout();
    namesLayout->setColumnStretch(0, 0);
    namesLayout->setColumnStretch(1, 100);
    namesLayout->addWidget(basePathLabel, 0, 0);
    namesLayout->addWidget(m_basePathLineEdit, 0, 1);
    namesLayout->addWidget(sceneFileNameLabel, 1, 0);
    namesLayout->addWidget(m_sceneFileNameLineEdit, 1, 1);
    namesLayout->addWidget(sceneFilePathLabel, 2, 0);
    namesLayout->addWidget(m_sceneFilePathLineEdit, 2, 1);
    namesLayout->addWidget(modeLabel, 3, 0);
    namesLayout->addWidget(m_modeComboBox, 3, 1, Qt::AlignLeft);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->setSpacing(3);
    dialogLayout->addLayout(namesLayout, 0);
    dialogLayout->addWidget(tabWidget, 100);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    const SceneDataFileInfo::SortMode defaultSortMode(SceneDataFileInfo::SortMode::AbsolutePath);
    QSignalBlocker blocker(m_modeComboBox);
    m_modeComboBox->setCurrentIndex(static_cast<int>(defaultSortMode));
    displayFiles(static_cast<int32_t>(defaultSortMode));
    
    setSizeOfDialogWhenDisplayed(QSize(600, 800));
}

/**
 * Destructor.
 */
SceneFileInformationDialog::~SceneFileInformationDialog()
{
}

/**
 * Called when mode combo box is selected
 */
void
SceneFileInformationDialog::modeComboBoxActivated(int mode)
{
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    displayFiles(mode);
}

/**
 * Called when mode combo box is selected
 */
void
SceneFileInformationDialog::displayFiles(int modeInteger)
{
    const SceneDataFileInfo::SortMode sortMode = static_cast<SceneDataFileInfo::SortMode>(modeInteger);
    switch (sortMode) {
        case SceneDataFileInfo::SortMode::AbsolutePath:
            break;
        case SceneDataFileInfo::SortMode::RelativeToBasePath:
            break;
        case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
            break;
    }
    CaretAssert(m_sceneFile);
    
    std::vector<SceneDataFileInfo> fileSceneInfo = m_sceneFile->getAllDataFileInfoFromAllScenes();
    SceneDataFileInfo::sort(fileSceneInfo,
                            sortMode);
    
    if (fileSceneInfo.empty()) {
        WuQMessageBox::errorOk(this,
                               "Scene file is empty.");
        return;
    }
    
    {
        m_sceneFileHierarchyTreeModel.reset(new SceneDataFileTreeItemModel("",
                                                                           fileSceneInfo,
                                                                           sortMode));
        m_sceneFileHierarchyTreeView->setModel(m_sceneFileHierarchyTreeModel.get());
        m_sceneFileHierarchyTreeView->expandAll();
    }
    
    const AString sceneFileName = m_sceneFile->getFileName();
    AString text("<html>");
    
    AString baseDirectoryName;
    std::vector<AString> missingFileNames;
    AString errorMessage;
    const bool validBasePathFlag = m_sceneFile->findBaseDirectoryForDataFiles(baseDirectoryName,
                                                                            missingFileNames,
                                                                            errorMessage);
    text.appendWithNewLine("<b>Automatic Base Path</b>: "
                           + (validBasePathFlag ? baseDirectoryName : ("INVALID: " + errorMessage))
                           + "<p>");
    text.appendWithNewLine("<b>Scene File</b>: "
                           + sceneFileName
                           + "<p>");

    bool needListEndElementFlag = false;
    AString lastPathName("bogus ##(*&$UI()#NFGK path name");
    for (const auto& fileData : fileSceneInfo) {
        AString missingText;
        
        AString pathName;
        switch (sortMode) {
            case SceneDataFileInfo::SortMode::AbsolutePath:
                pathName = fileData.getAbsolutePath();
                break;
            case SceneDataFileInfo::SortMode::RelativeToBasePath:
                pathName = fileData.getRelativePathToBasePath();
                if (pathName.isEmpty()) {
                    pathName = "Files in Base Path";
                }
                break;
            case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
                pathName = fileData.getRelativePathToSceneFile();
                if (pathName.isEmpty()) {
                    pathName = "Files in Scene File Path";
                }
                break;
        }
        
        if (pathName != lastPathName) {
            if (needListEndElementFlag) {
                text.append("</ul>");
            }
            text.append("<BR></BR><B>"
                        + pathName
                        + "</B>");
            text.append("<ul>");
            needListEndElementFlag = true;
            lastPathName = pathName;
        }
        
        if (fileData.isMissing()) {
            missingText = "MISSING ";
        }
        
        text.append("<li> "
                    + missingText
                    + fileData.getDataFileName()
                    + " ("
                    + fileData.getSceneIndicesAsString()
                    + ")");
    }
    if (needListEndElementFlag) {
        text.append("</ul>");
    }
    text.append("</html>");
    
    m_textEdit->clear();
    m_textEdit->setHtml(text);
}

