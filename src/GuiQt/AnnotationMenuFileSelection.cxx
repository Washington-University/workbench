
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __ANNOTATION_MENU_FILE_SELECTION_DECLARE__
#include "AnnotationMenuFileSelection.h"
#undef __ANNOTATION_MENU_FILE_SELECTION_DECLARE__

#include "AnnotationFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SamplesFile.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuFileSelection 
 * \brief Menu for selecting an annotation file.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *    The user input mode
 * @param parent
 *    The parent widget.
 */
AnnotationMenuFileSelection::AnnotationMenuFileSelection(const UserInputModeEnum::Enum userInputMode,
                                                         QWidget* parent)
: QMenu(parent),
m_userInputMode(userInputMode)
{
    m_fileMode = FileMode::INVALID;
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::INVALID:
            break;
        case UserInputModeEnum::Enum::ANNOTATIONS:
            m_fileMode = FileMode::ANNOTATIONS;
            break;
        case UserInputModeEnum::Enum::BORDERS:
            break;
        case UserInputModeEnum::Enum::FOCI:
            break;
        case UserInputModeEnum::Enum::IMAGE:
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            m_fileMode = FileMode::SAMPLES;
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            break;
        case UserInputModeEnum::Enum::VIEW:
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            break;
    }
    CaretAssert(m_fileMode != FileMode::INVALID);
    
    m_selectedAnnotationFile = NULL;
    
    QObject::connect(this, SIGNAL(aboutToShow()),
                     this, SLOT(updateMenuContents()));
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionSelected(QAction*)));
}

/**
 * Destructor.
 */
AnnotationMenuFileSelection::~AnnotationMenuFileSelection()
{
}

/**
 * @return The selected annotation file or NULL if no
 * annotation file is selected.
 */
AnnotationFile*
AnnotationMenuFileSelection::getSelectedAnnotationFile()
{
    /*
     * Ensures selected file is valid
     */
    updateMenuContents();
    
    if (m_selectedAnnotationFile != NULL) {
        switch (m_fileMode) {
            case FileMode::INVALID:
                CaretAssert(0);
                break;
            case FileMode::ANNOTATIONS:
                CaretAssert(m_selectedAnnotationFile->getDataFileType() == DataFileTypeEnum::ANNOTATION);
                break;
            case FileMode::SAMPLES:
                CaretAssert(m_selectedAnnotationFile->getDataFileType() == DataFileTypeEnum::SAMPLES);
                break;
        }
    }
    
    return m_selectedAnnotationFile;
}

/**
 * @return The selected annotation file or NULL if no
 * annotation file is selected.
 */
AString
AnnotationMenuFileSelection::getSelectedNameForToolButton()
{
    AnnotationFile* annFile = getSelectedAnnotationFile();

    AString name;
    
    switch (m_fileMode) {
        case FileMode::INVALID:
            CaretAssert(0);
            break;
        case FileMode::ANNOTATIONS:
        {
            name = "Scene";
            if (annFile != NULL) {
                if (annFile == GuiManager::get()->getBrain()->getSceneAnnotationFile()) {
                    name = "Scene";
                }
                else {
                    name = "Disk ";
                }
            }
        }
            break;
        case FileMode::SAMPLES:
        {
            name = "Select";
            if (annFile != NULL) {
                name = "Disk";
            }
        }
            break;
    }
    
    return name;
}

/**
 * Called to create a new disk file.
 */
void
AnnotationMenuFileSelection::chooseDiskFile()
{
    const AString fileDialogSettingsName("AnnotDiskFileDialog");
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_SAVE,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptSave);
    switch (m_fileMode) {
        case FileMode::INVALID:
            CaretAssert(0);
            break;
        case FileMode::ANNOTATIONS:
            fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilterForWriting(DataFileTypeEnum::ANNOTATION));
            fd.selectFile(AnnotationFile().getFileNameNoPath());
            break;
        case FileMode::SAMPLES:
            fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilterForWriting(DataFileTypeEnum::SAMPLES));
            fd.selectFile(SamplesFile().getFileNameNoPath());
            break;
    }
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.setLabelText(CaretFileDialog::Accept, "Choose"); // OK button shows Insert
    fd.restoreDialogSettings(fileDialogSettingsName);
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        fd.saveDialogSettings(fileDialogSettingsName);
        
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            const AString newFileName = selectedFiles.at(0);
            
            switch (m_fileMode) {
                case FileMode::INVALID:
                    CaretAssert(0);
                    break;
                case FileMode::ANNOTATIONS:
                {
                    AnnotationFile* newFile = new AnnotationFile();
                    newFile->setFileName(newFileName);
                    EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
                    m_selectedAnnotationFile = newFile;
                }
                    break;
                case FileMode::SAMPLES:
                {
                    SamplesFile* newFile = new SamplesFile();
                    newFile->setFileName(newFileName);
                    EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
                    m_selectedAnnotationFile = newFile;
                }
                    break;
            }
        }
    }
}

/**
 * Called when a menu action is selected.
 *
 * @param action
 *     The action that was selected.
 */
void
AnnotationMenuFileSelection::menuActionSelected(QAction* action)
{
    CaretAssert(action);
    
    switch (m_fileMode) {
        case FileMode::INVALID:
            CaretAssert(0);
            break;
        case FileMode::ANNOTATIONS:
            break;
        case FileMode::SAMPLES:
            break;
    }
    const int actionID = action->data().toInt();
    
    if (actionID == ACTION_ID_SCENE) {
        m_selectedAnnotationFile = GuiManager::get()->getBrain()->getSceneAnnotationFile();
    }
    else if (actionID == ACTION_ID_NEW_DISK_FILE) {
        chooseDiskFile();
    }
    else if (actionID >= ACTION_ID_FIRST_DISK_FILE) {
        if ((actionID >= 0)
            && (actionID < static_cast<int32_t>(m_annotationDiskFiles.size()))) {
            CaretAssertVectorIndex(m_annotationDiskFiles, actionID);
            m_selectedAnnotationFile = m_annotationDiskFiles[actionID];
        }
    }
    
    emit menuItemSelected();
}

/**
 * Update the contents of the menu.
 */
void
AnnotationMenuFileSelection::updateMenuContents()
{
    blockSignals(true);
    
    clear();
    
    bool foundSelectedFileFlag = false;
    
    Brain* brain = GuiManager::get()->getBrain();
    
    QAction* sceneAction(NULL);
    AnnotationFile* sceneAnnotationFile = brain->getSceneAnnotationFile();
    switch (m_fileMode) {
        case FileMode::INVALID:
            CaretAssert(0);
            break;
        case FileMode::ANNOTATIONS:
        {
            sceneAction = addAction("Scene Annotations");
            sceneAction->setData((int)ACTION_ID_SCENE);
            sceneAction->setCheckable(true);
            if (sceneAnnotationFile == m_selectedAnnotationFile) {
                sceneAction->setChecked(true);
                foundSelectedFileFlag = true;
            }
            
            addSeparator();
        }
            break;
        case FileMode::SAMPLES:
            break;
    }

    
    QAction* newDiskFileAction = addAction("New Disk File...");
    newDiskFileAction->setData((int)ACTION_ID_NEW_DISK_FILE);
    
    addSeparator();
    
    m_annotationDiskFiles.clear();
    
    switch (m_fileMode) {
        case FileMode::INVALID:
            CaretAssert(0);
            break;
        case FileMode::ANNOTATIONS:
            brain->getAllAnnotationFilesExcludingSceneAnnotationFile(m_annotationDiskFiles);
            break;
        case FileMode::SAMPLES:
        {
            std::vector<SamplesFile*> samplesFiles(brain->getAllSamplesFiles());
            m_annotationDiskFiles.insert(m_annotationDiskFiles.end(),
                                         samplesFiles.begin(),
                                         samplesFiles.end());
        }
            break;
    }

    
    const int32_t numDiskFiles = static_cast<int32_t>(m_annotationDiskFiles.size());
    for (int32_t i = 0; i < numDiskFiles; i++) {
        CaretAssertVectorIndex(m_annotationDiskFiles, i);
        AnnotationFile* annFile = m_annotationDiskFiles[i];
        CaretAssert(annFile);
        
        QAction* fileAction = addAction(annFile->getFileNameNoPath());
        fileAction->setData((int)ACTION_ID_FIRST_DISK_FILE + i);
        fileAction->setCheckable(true);
        if (m_selectedAnnotationFile == annFile) {
            fileAction->setChecked(true);
            foundSelectedFileFlag = true;
        }
    }
    
    if ( ! foundSelectedFileFlag) {
        if (sceneAction != NULL) {
            sceneAction->setChecked(true);
            m_selectedAnnotationFile = sceneAnnotationFile;
        }
        else if ( ! m_annotationDiskFiles.empty()) {
            CaretAssertVectorIndex(m_annotationDiskFiles, 0);
            m_selectedAnnotationFile = m_annotationDiskFiles[0];
        }
        else {
            m_selectedAnnotationFile = NULL;
        }
    }
    
    blockSignals(false);
}

