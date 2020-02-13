
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
: WuQDialogNonModal("Files and Folders",
                    parent),
m_sceneFile(sceneFile)
{
    setDeleteWhenClosed(true);
    setApplyButtonText("");
    
    AString basePathName;
    SceneFileBasePathTypeEnum::Enum basePathType = SceneFileBasePathTypeEnum::AUTOMATIC;
    {
        AString errorMessage;
        const bool validFlag = m_sceneFile->getSelectedBasePathTypeAndName(basePathType,
                                                                           basePathName,
                                                                           errorMessage);
        if ( ! validFlag) {
            basePathName = ("INVALID " + errorMessage);
        }
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
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->setSpacing(3);
    dialogLayout->addLayout(namesLayout, 0);
    dialogLayout->addWidget(tabWidget, 100);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    displayFilesHierarchy();
    displayFilesList(basePathType,
                     basePathName);
    
    setSizeOfDialogWhenDisplayed(QSize(600, 800));
}

/**
 * Destructor.
 */
SceneFileInformationDialog::~SceneFileInformationDialog()
{
}

/**
 * Setup the list of files
 */
void
SceneFileInformationDialog::displayFilesList(const SceneFileBasePathTypeEnum::Enum basePathType,
                                             const AString& basePathName)
{
    const SceneDataFileInfo::SortMode sortMode = SceneDataFileInfo::SortMode::RelativeToSceneFilePath;
    CaretAssert(m_sceneFile);
    
    std::vector<SceneDataFileInfo> fileSceneInfo = m_sceneFile->getAllDataFileInfoFromAllScenes();
    SceneDataFileInfo::sort(fileSceneInfo,
                            sortMode);
    
    if (fileSceneInfo.empty()) {
        WuQMessageBox::errorOk(this,
                               "Scene file is empty.");
        return;
    }
    
    AString text("<html>");
    const AString sceneFileName = m_sceneFile->getFileName();

    text.appendWithNewLine("<b>"
                           + (SceneFileBasePathTypeEnum::toGuiName(basePathType) + " Base Path")
                           + "</b>: "
                           + basePathName
                           + "<p>");

    text.appendWithNewLine("<b>Scene File</b>: "
                           + sceneFileName
                           + "<p>");
    
    AString pathName;
    switch (sortMode) {
        case SceneDataFileInfo::SortMode::AbsolutePath:
            pathName = "Absolute File Paths";
            break;
        case SceneDataFileInfo::SortMode::RelativeToBasePath:
            pathName = "Files in Base Path";
            break;
        case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
            pathName = "Data File paths relative to Scene File";
            break;
    }
    text.append("<b>"
                + pathName
                + "</b>");
    
    text.append("<ul>");
    
    for (const auto& fileData : fileSceneInfo) {
        AString missingText;
        
        AString pathName;
        switch (sortMode) {
            case SceneDataFileInfo::SortMode::AbsolutePath:
                pathName = fileData.getAbsolutePath();
                break;
            case SceneDataFileInfo::SortMode::RelativeToBasePath:
                pathName = fileData.getRelativePathToBasePath();
                break;
            case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
                pathName = fileData.getRelativePathToSceneFile();
                break;
        }
        if (! pathName.isEmpty()) {
            pathName.append("/ ");
        }
        
        if (fileData.isMissing()) {
            missingText = "<NOT FOUND> ";
        }
        
        text.append("<li> "
                    + missingText
                    + pathName
                    + fileData.getDataFileName()
                    + " ("
                    + fileData.getSceneIndicesAsString()
                    + ")");
    }
    
    text.append("</ul>");
    text.append("</html>");
    
    m_textEdit->clear();
    m_textEdit->setHtml(text);
}

/**
 * Setup the hierarchy of files
 */
void
SceneFileInformationDialog::displayFilesHierarchy()
{
    AString baseDirectoryName;
    std::vector<AString> missingFileNames;
    AString errorMessage;
    const bool validBasePathFlag = m_sceneFile->findBaseDirectoryForDataFiles(baseDirectoryName,
                                                                              missingFileNames,
                                                                              errorMessage);
    if ( ! validBasePathFlag) {
        return;
    }
    CaretAssert(m_sceneFile);
    
    std::vector<SceneDataFileInfo> fileSceneInfo = m_sceneFile->getAllDataFileInfoFromAllScenes();
    SceneDataFileInfo::sort(fileSceneInfo,
                            SceneDataFileInfo::SortMode::AbsolutePath);

    m_sceneFileHierarchyTreeModel.reset(new SceneDataFileTreeItemModel(m_sceneFile->getFileName(),
                                                                       baseDirectoryName,
                                                                       fileSceneInfo,
                                                                       SceneDataFileInfo::SortMode::AbsolutePath));
    m_sceneFileHierarchyTreeView->setModel(m_sceneFileHierarchyTreeModel.get());
    m_sceneFileHierarchyTreeView->expandAll();
}



