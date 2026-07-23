
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__
#include "BorderHidingDialog.h"
#undef __DATA_FILE_DRAWING_ORDER_DIALOG_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>

#include "BorderFile.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "FilePathNamePrefixCompactor.h"
#include "GuiManager.h"
#include "GroupAndNameHierarchyModel.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::BorderHidingDialog 
 * \brief Dialog for hiding borders in other files (usually overlapping)
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget
 */
BorderHidingDialog::BorderHidingDialog(QWidget* parent)
: WuQDialogModal("Hide Borders",
                 parent)
{
    m_matchByNameCheckBox = new QCheckBox("Match Names");
    m_matchByNameCheckBox->setChecked(true);
    
    m_matchCoordinatesCheckBox = new QCheckBox("Match Coordinates");

    m_dataFilesListWidget = new QListWidget();
    m_dataFilesListWidget->setDragDropMode(QListWidget::DragDropMode::InternalMove);
    
//    QObject::connect(m_dataFilesListWidget->model(), &QAbstractItemModel::rowsMoved,
//                     this, [=]() { orderOfFilesChanged(); } );
    
    const QString instructionsText("* Drag file names to change the file order\n"
                                   "* Matching borders in lower files are turned off");
    QLabel* instructionsLabel(new QLabel(instructionsText));
    
    QToolButton* turnOffBordersButton(new QToolButton());
    turnOffBordersButton->setText("Turn off hidden borders in selected files");
    QObject::connect(turnOffBordersButton, &QToolButton::clicked,
                     this, &BorderHidingDialog::turnOffHiddenBordersButtonClicked);
    
    QToolButton* turnAllBordersOnButton(new QToolButton());
    turnAllBordersOnButton->setText("Turn on all borders in selected files");
    QObject::connect(turnAllBordersOnButton, &QToolButton::clicked,
                     this, &BorderHidingDialog::turnAllBordersOnButtonClicked);

    QWidget* dialogWidget(new QWidget());
    QGridLayout* gridLayout(new QGridLayout(dialogWidget));
    gridLayout->setRowStretch(1000, 100);
    gridLayout->setColumnStretch(1, 100);
    int row(0);
    gridLayout->addWidget(m_matchByNameCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    ++row;
    gridLayout->addWidget(m_matchCoordinatesCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    ++row;
    gridLayout->addWidget(m_dataFilesListWidget, row, 0, 1, 2);
    ++row;
    gridLayout->addWidget(instructionsLabel, row, 0, 1, 2);
    ++row;
    gridLayout->addWidget(turnOffBordersButton, row, 0, 1, 2, Qt::AlignHCenter);
    ++row;
    gridLayout->addWidget(turnAllBordersOnButton, row, 0, 1, 2, Qt::AlignHCenter);
    
    
    setCentralWidget(dialogWidget, ScrollAreaStatus::SCROLL_AREA_AS_NEEDED);
    setCancelButtonText("");
    
    Brain* brain(GuiManager::get()->getBrain());
    CaretAssert(brain);
    m_borderFiles = brain->getAllBorderFiles();
    m_numBorderFiles = static_cast<int32_t>(m_borderFiles.size());

    loadBorderFiles();
}

/**
 * Destructor.
 */
BorderHidingDialog::~BorderHidingDialog()
{
}

/**
 * Called when turn off hidden borders button is clicked
 */
void
BorderHidingDialog::turnOffHiddenBordersButtonClicked()
{
    if (m_matchByNameCheckBox->isChecked()
        || m_matchCoordinatesCheckBox->isChecked()) {
        /* OK */
    }
    else {
        WuQMessageBoxTwo::critical(this, "Error", "One of the match type checkboxes must be checked");
        return;
    }
    
    std::vector<BorderFile*> borderFiles(getSelectedBorderFiles());
    
//    const int32_t numFiles(m_dataFilesListWidget->count());
//    for (int32_t iRow = (numFiles - 1); iRow >= 0; iRow--) {
//        QListWidgetItem* item(m_dataFilesListWidget->item(iRow));
//        CaretAssert(item);
//        if (item->checkState() == Qt::Checked) {
//            const AString borderFileName(item->data(Qt::UserRole).toString());
//            BorderFile* bf(getBorderFileWithName(borderFileName));
//            CaretAssert(bf);
//            borderFiles.push_back(bf);
//        }
//    }
    
    if (borderFiles.size() > 1) {
        turnOffMatchingBordersSelected(borderFiles);
    }
    else {
        WuQMessageBoxTwo::critical(this, "Error", "At least two border files must be selected");
    }
}

/**
 * @return The selected border files
 */
std::vector<BorderFile*>
BorderHidingDialog::getSelectedBorderFiles()
{
    std::vector<BorderFile*> borderFiles;
    
    const int32_t numFiles(m_dataFilesListWidget->count());
    for (int32_t iRow = (numFiles - 1); iRow >= 0; iRow--) {
        QListWidgetItem* item(m_dataFilesListWidget->item(iRow));
        CaretAssert(item);
        if (item->checkState() == Qt::Checked) {
            const AString borderFileName(item->data(Qt::UserRole).toString());
            BorderFile* bf(getBorderFileWithName(borderFileName));
            CaretAssert(bf);
            borderFiles.push_back(bf);
        }
    }

    return borderFiles;
}

/**
 * Called when a data file type is selected
 */
void
BorderHidingDialog::loadBorderFiles()
{
    const std::vector<CaretDataFile*> caretDataFiles(m_borderFiles.begin(),
                                                     m_borderFiles.end());
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(caretDataFiles,
                                                                            displayNames);
    CaretAssert(caretDataFiles.size() == displayNames.size());
    CaretAssert(caretDataFiles.size() == m_borderFiles.size());

    m_dataFilesListWidget->clear();
    for (int32_t iFile = (m_numBorderFiles - 1); iFile >= 0; iFile--) {
        CaretAssertVectorIndex(m_borderFiles, iFile);
        const BorderFile* bf(m_borderFiles[iFile]);
        CaretAssert(bf);
        CaretAssertVectorIndex(displayNames, iFile);
        QListWidgetItem* item (new QListWidgetItem(displayNames[iFile]));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setData(Qt::UserRole, bf->getFileName());
        item->setCheckState(Qt::Checked);
        m_dataFilesListWidget->addItem(item);
    }
}

/**
 * @return Pointer to border file with the given name
 * @param filename
 *    Name of border file
 */
BorderFile*
BorderHidingDialog::getBorderFileWithName(const AString& filename) const
{
    for (BorderFile* bf : m_borderFiles) {
        if (bf->getFileName() == filename) {
            return bf;
        }
    }
    return NULL;
}


/**
 * Called when the Ok button is clicked
 */
void
BorderHidingDialog::okButtonClicked()
{
    
    WuQDialogModal::okButtonClicked();
}

void
BorderHidingDialog::turnAllBordersOnButtonClicked()
{
    std::vector<BorderFile*> selectedBorderFiles(getSelectedBorderFiles());
    const int32_t numBorderFiles(selectedBorderFiles.size());
    for (int32_t iFile = 0; iFile < numBorderFiles; iFile++) {
        CaretAssertVectorIndex(selectedBorderFiles, iFile);
        BorderFile* borderFile(selectedBorderFiles[iFile]);
        CaretAssert(borderFile);
        GroupAndNameHierarchyModel* hm(borderFile->getGroupAndNameHierarchyModel());
        hm->setAllSelected(true);
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}


/**
 * Turn off matching borders in "underlapping" border files
 */
void
BorderHidingDialog::turnOffMatchingBordersSelected(std::vector<BorderFile*>& selectedBorderFiles)
{
    int32_t numOff(0);

    const int32_t numBorderFiles(selectedBorderFiles.size());
    for (int32_t iFile = (numBorderFiles - 1); iFile >= 1; iFile--) {
        CaretAssertVectorIndex(selectedBorderFiles, iFile);
        const BorderFile* borderFile(selectedBorderFiles[iFile]);
        CaretAssert(borderFile);
        for (int32_t jFile = (iFile - 1); jFile >= 0; jFile--) {
            CaretAssertVectorIndex(selectedBorderFiles, jFile);
            BorderFile* otherBorderFile(selectedBorderFiles[jFile]);
            CaretAssert(otherBorderFile);
            numOff += borderFile->turnOffMatchingBorders(otherBorderFile,
                                                         m_matchByNameCheckBox->isChecked(),
                                                         m_matchCoordinatesCheckBox->isChecked());
        }
    }

    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    const AString msg("Number of borders turned off: "
                      + AString::number(numOff));
    WuQMessageBoxTwo::information(this,
                                  "Hide Borders",
                                  msg);
}


