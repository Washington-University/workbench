
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
#include "DataFileContentInformation.h"
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
    SceneFile* nonConstSceneFile(NULL);
    if (m_sceneFile != NULL) {
        nonConstSceneFile = const_cast<SceneFile*>(m_sceneFile);
    }
    
    setDeleteWhenClosed(true);
    setApplyButtonText("");
    
    
    QLabel* basePathLabel = new QLabel("Base Path:");
    m_basePathLineEdit = new QLineEdit();
    m_basePathLineEdit->setReadOnly(true);
    
    QLabel* sceneFileNameLabel = new QLabel("Scene File Name:");
    m_sceneFileNameLineEdit = new QLineEdit();
    m_sceneFileNameLineEdit->setReadOnly(true);

    QLabel* sceneFilePathLabel = new QLabel("Scene File Path:");
    m_sceneFilePathLineEdit = new QLineEdit();
    m_sceneFilePathLineEdit->setReadOnly(true);
    
    m_listTextEdit = new QTextEdit();
    m_listTextEdit->setLineWrapMode(QTextEdit::NoWrap);

    m_sceneTextEdit = new QTextEdit();
    if (nonConstSceneFile != NULL) {
        DataFileContentInformation dataFileInformation;
        nonConstSceneFile->addToDataFileContentInformation(dataFileInformation);
        m_sceneTextEdit->setText(dataFileInformation.getInformationInString());
        m_sceneTextEdit->setReadOnly(true);
        m_sceneTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    }

    m_sceneFileHierarchyTreeView = new QTreeView();
    m_sceneFileHierarchyTreeView->setHeaderHidden(true);
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(m_sceneFileHierarchyTreeView, "Hierarchy");
    tabWidget->addTab(m_listTextEdit, "List");
    tabWidget->addTab(m_sceneTextEdit, "Scenes");
    
    QGridLayout* namesLayout = new QGridLayout();
    int32_t nameLayoutRow(0);
    namesLayout->setColumnStretch(0, 0);
    namesLayout->setColumnStretch(1, 100);
    namesLayout->addWidget(basePathLabel, nameLayoutRow, 0);
    namesLayout->addWidget(m_basePathLineEdit, nameLayoutRow, 1);
    nameLayoutRow++;
    namesLayout->addWidget(sceneFilePathLabel, nameLayoutRow, 0);
    namesLayout->addWidget(m_sceneFilePathLineEdit, nameLayoutRow, 1);
    nameLayoutRow++;
    namesLayout->addWidget(sceneFileNameLabel, nameLayoutRow, 0);
    namesLayout->addWidget(m_sceneFileNameLineEdit, nameLayoutRow, 1);
    nameLayoutRow++;

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->setSpacing(3);
    dialogLayout->addLayout(namesLayout, 0);
    dialogLayout->addWidget(tabWidget, 100);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    setSizeOfDialogWhenDisplayed(QSize(600, 800));
    
    loadContentIntoDialog();
}

/**
 * Destructor.
 */
SceneFileInformationDialog::~SceneFileInformationDialog()
{
}

void
SceneFileInformationDialog::loadContentIntoDialog()
{
    CaretAssert(m_sceneFile);
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
    
    m_basePathLineEdit->setText(basePathName);
    m_basePathLineEdit->home(true);
    
    FileInformation sceneFileInfo(m_sceneFile->getFileName());
    m_sceneFileNameLineEdit->setText(sceneFileInfo.getFileName());
    m_sceneFileNameLineEdit->home(true);
    
    m_sceneFilePathLineEdit->setText(sceneFileInfo.getPathName());
    m_sceneFilePathLineEdit->home(true);
    
    displayFilesHierarchy();
    displayFilesList(basePathType,
                     basePathName);
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
        
        const FileInformation fileInfo(fileData.getAbsolutePathAndFileName());
        if (fileData.isMissing()) {
            missingText = "<html><font color=red>--NOT FOUND--</font></html>";
        }
        
        text.append("<li> "
                    + pathName
                    + fileData.getDataFileName()
                    + " ("
                    + fileData.getSceneIndicesAsString()
                    + ") "
                    + missingText);
    }
    
    text.append("</ul>");
    text.append("</html>");
    
    m_listTextEdit->clear();
    m_listTextEdit->setHtml(text);
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




