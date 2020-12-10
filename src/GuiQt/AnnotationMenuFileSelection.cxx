
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

using namespace caret;


    
/**
 * \class caret::AnnotationMenuFileSelection 
 * \brief Menu for selecting an annotation file.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget.
 */
AnnotationMenuFileSelection::AnnotationMenuFileSelection(QWidget* parent)
: QMenu(parent)
{
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
    
    return m_selectedAnnotationFile;
}

/**
 * @return The selected annotation file or NULL if no
 * annotation file is selected.
 */
AString
AnnotationMenuFileSelection::getSelectedNameForToolButton()
{
    AString name("Scene");
    
    AnnotationFile* annFile = getSelectedAnnotationFile();
    if (annFile != NULL) {
        if (annFile == GuiManager::get()->getBrain()->getSceneAnnotationFile()) {
            name = "Scene";
        }
        else {
            name = "Disk ";
        }
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
    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilterForWriting(DataFileTypeEnum::ANNOTATION));
    fd.setFileMode(CaretFileDialog::AnyFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.setLabelText(CaretFileDialog::Accept, "Choose"); // OK button shows Insert
    fd.restoreDialogSettings(fileDialogSettingsName);
    fd.selectFile(AnnotationFile().getFileNameNoPath());
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        fd.saveDialogSettings(fileDialogSettingsName);
        
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            const AString annotationFileName = selectedFiles.at(0);
            
            AnnotationFile* newFile = new AnnotationFile();
            newFile->setFileName(annotationFileName);
            EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
            m_selectedAnnotationFile = newFile;
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
    AnnotationFile* sceneAnnotationFile = brain->getSceneAnnotationFile();
    
    QAction* sceneAction = addAction("Scene Annotations");
    sceneAction->setData((int)ACTION_ID_SCENE);
    sceneAction->setCheckable(true);
    if (sceneAnnotationFile == m_selectedAnnotationFile) {
        sceneAction->setChecked(true);
        foundSelectedFileFlag = true;
    }
    
    addSeparator();
    
    QAction* newDiskFileAction = addAction("New Disk File...");
    newDiskFileAction->setData((int)ACTION_ID_NEW_DISK_FILE);
    
    addSeparator();
    
    m_annotationDiskFiles.clear();
    brain->getAllAnnotationFilesExcludingSceneAnnotationFile(m_annotationDiskFiles);
    
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
        sceneAction->setChecked(true);
        m_selectedAnnotationFile = sceneAnnotationFile;
    }
    
    blockSignals(false);
}

