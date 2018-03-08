
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

#define __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_DECLARE__
#include "CopyPaletteColorMappingToFilesDialog.h"
#undef __COPY_PALETTE_COLOR_MAPPING_TO_FILES_DIALOG_DECLARE__

#include <algorithm>
#include <map>

#include <QCheckBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::CopyPaletteColorMappingToFilesDialog 
 * \brief Dialog for copying palette color mapping to other files
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param allPaletteMapFiles
 *     All palette mappable data files.
 * @param selectedMapFile
 *     Map file that contains palette being copied
 * @param selectedPaletteColorMapping
 *     Palette color mapping that is being copied
 * @param parent
 *     Parent widget for this dialog.
 *
 */
CopyPaletteColorMappingToFilesDialog::CopyPaletteColorMappingToFilesDialog(const std::vector<CaretMappableDataFile*>& allPaletteMapFiles,
                                                                           CaretMappableDataFile* selectedMapFile,
                                                                           const PaletteColorMapping* selectedPaletteColorMapping,
                                                                           QWidget* parent)
: WuQDialogModal("Copy Palette Color Mapping",
                 parent),
m_selectedMapFile(selectedMapFile),
m_selectedPaletteColorMapping(selectedPaletteColorMapping)
{
    CaretAssert(m_selectedMapFile);
    CaretAssert(m_selectedPaletteColorMapping);
    
    for (const auto mapFile : allPaletteMapFiles) {
        CaretAssert(mapFile);
        QCheckBox* checkBox = new QCheckBox(mapFile->getFileNameNoPath());
        if (mapFile == selectedMapFile) {
            checkBox->setStyleSheet("font : bold");
        }
        m_fileCheckBoxes.push_back(std::make_pair(checkBox, mapFile));
        
        if (s_previousCheckedFiles.find(mapFile) != s_previousCheckedFiles.end()) {
            checkBox->setChecked(true);
        }
        else {
            checkBox->setChecked(false);
        }
    }
    
    QToolButton* allOnToolButton = new QToolButton();
    allOnToolButton->setText("All On");
    QObject::connect(allOnToolButton, &QToolButton::clicked,
                     this, &CopyPaletteColorMappingToFilesDialog::allCheckBoxesOnClicked);
    
    QToolButton* allOffToolButton = new QToolButton();
    allOffToolButton->setText("All Off");
    QObject::connect(allOffToolButton, &QToolButton::clicked,
                     this, &CopyPaletteColorMappingToFilesDialog::allCheckBoxesOffClicked);
    
    QHBoxLayout* toolButtonLayout = new QHBoxLayout();
    toolButtonLayout->addWidget(allOnToolButton);
    toolButtonLayout->addWidget(allOffToolButton);
    toolButtonLayout->addStretch();
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(toolButtonLayout);
    for (auto checkFile : m_fileCheckBoxes) {
        layout->addWidget(checkFile.first);
    }
    
    setCentralWidget(widget, SCROLL_AREA_AS_NEEDED);
}

/**
 * Destructor.
 */
CopyPaletteColorMappingToFilesDialog::~CopyPaletteColorMappingToFilesDialog()
{
}

/**
 * Called when the OK button is clicked.
 */
void
CopyPaletteColorMappingToFilesDialog::okButtonClicked()
{
    {
        /*
         * Note: Cursor MUST go out of scope (be removed)
         * before calling parent's okButtonClicked()
         */
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        s_previousCheckedFiles.clear();
        
        PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
        int32_t checkedCount = 0;
        bool sourceFileFlag = false;
        
        for (auto checkFile : m_fileCheckBoxes) {
            QCheckBox* checkBox = checkFile.first;
            CaretAssert(checkBox);
            CaretMappableDataFile* mapFile = checkFile.second;
            CaretAssert(mapFile);
            
            if (checkBox->isChecked()) {
                if (mapFile != m_selectedMapFile) {
                    mapFile->setPaletteNormalizationMode(m_selectedMapFile->getPaletteNormalizationMode());
                    
                    const int32_t numMaps = mapFile->getNumberOfMaps();
                    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                        PaletteColorMapping* pcm = mapFile->getMapPaletteColorMapping(iMap);
                        pcm->copy(*m_selectedPaletteColorMapping,
                                  false);
                        mapFile->updateScalarColoringForAllMaps(paletteFile);
                        ++checkedCount;
                    }
                }
                else {
                    sourceFileFlag = true;
                }
                
                s_previousCheckedFiles.insert(mapFile);
            }
        }
        
        if (checkedCount <= 0) {
            cursor.restoreCursor();
            if (sourceFileFlag) {
                WuQMessageBox::errorOk(this,
                                       "The selected file is the 'copy from' file.  Select files for 'copying to'");
            }
            else {
                WuQMessageBox::errorOk(this,
                                       "No files are selected for palette copying.");
            }
            return;
        }
    }
    
    WuQDialogModal::okButtonClicked();
}

/**
 * Called when 'All On' checkboxes button is clicked.
 */
void
CopyPaletteColorMappingToFilesDialog::allCheckBoxesOnClicked()
{
    for (auto checkFile : m_fileCheckBoxes) {
        checkFile.first->setChecked(true);
    }
}

/**
 * Called when 'All Off' checkboxes button is clicked.
 */
void
CopyPaletteColorMappingToFilesDialog::allCheckBoxesOffClicked()
{
    for (auto checkFile : m_fileCheckBoxes) {
        checkFile.first->setChecked(false);
    }
}


/**
 * Run the dialog.
 *
 * @param selectedMapFile
 *     Map file that contains palette being copied
 * @param selectedPaletteColorMapping
 *     Palette color mapping that is being copied
 * @param parent
 *     Parent widget for this dialog.
 * @param errorMessageOut
 *     If not empty, there was an error
 * @return
 *     True if the user pressed OK and palettes were copied.
 *     False if user pressed Cancel or if there was an error (errorMessageOut
 *     will contain description of the error).
 */
bool
CopyPaletteColorMappingToFilesDialog::run(CaretMappableDataFile* selectedMapFile,
                                          const PaletteColorMapping* selectedPaletteColorMapping,
                                          QWidget* parent,
                                          AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    EventCaretMappableDataFilesGet mapFilesGet;
    EventManager::get()->sendEvent(mapFilesGet.getPointer());
    std::vector<CaretMappableDataFile*> allMapFiles;
    mapFilesGet.getAllFiles(allMapFiles);
    
    std::vector<CaretMappableDataFile*> paletteMappedFiles;
    for (auto mapFile: allMapFiles) {
        if (mapFile->isMappedWithPalette()) {
            paletteMappedFiles.push_back(mapFile);
        }
    }
    allMapFiles.clear();
    
    std::sort(paletteMappedFiles.begin(),
              paletteMappedFiles.end(),
              [](CaretMappableDataFile* m1, CaretMappableDataFile* m2) { return (m1->getFileNameNoPath().toLower()
                                                                                 < m2->getFileNameNoPath().toLower()); } );
    
    bool noFilesFlag = false;
    if (paletteMappedFiles.empty()) {
        noFilesFlag = true;
    }
    else if (paletteMappedFiles.size() == 1) {
        CaretAssert(paletteMappedFiles[0] == selectedMapFile);
        noFilesFlag = true;
    }
    if (noFilesFlag) {
        errorMessageOut = "There are no other files that accept copying of palette color mapping.";
        return false;
    }
    
    CopyPaletteColorMappingToFilesDialog dialog(paletteMappedFiles,
                                                selectedMapFile,
                                                selectedPaletteColorMapping,
                                                parent);
    
    if (dialog.exec() == CopyPaletteColorMappingToFilesDialog::Accepted) {
        return true;
    }
    
    return false;
}



