
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
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CursorDisplayScoped.h"
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
    QLabel* modeLabel = new QLabel("Show Files");
    m_modeComboBox->addItem("Absolute Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::AbsolutePath));
    m_modeComboBox->addItem("Relative to Base Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::RelativeToBasePath));
    m_modeComboBox->addItem("Relative to Scene File Path",
                          static_cast<int>(SceneDataFileInfo::SortMode::RelativeToSceneFilePath));
    
    QObject::connect(m_modeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     this, &SceneFileInformationDialog::modeComboBoxActivated);
    
    m_textEdit = new QTextEdit();
    
    QHBoxLayout* modeLayout = new QHBoxLayout();
    modeLayout->addWidget(modeLabel),
    modeLayout->addWidget(m_modeComboBox);
    modeLayout->addStretch();

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(modeLayout, 0);
    dialogLayout->addWidget(m_textEdit, 100);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    const SceneDataFileInfo::SortMode defaultSortMode(SceneDataFileInfo::SortMode::RelativeToSceneFilePath);
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
                            SceneDataFileInfo::SortMode::RelativeToSceneFilePath);
    
    if (fileSceneInfo.empty()) {
        WuQMessageBox::errorOk(this,
                               "Scene file is empty.");
        return;
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

