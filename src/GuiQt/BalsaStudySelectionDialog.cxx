
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

#define __BALSA_STUDY_SELECTION_DIALOG_DECLARE__
#include "BalsaStudySelectionDialog.h"
#undef __BALSA_STUDY_SELECTION_DIALOG_DECLARE__

#include <QHeaderView>
#include <QInputDialog>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "BalsaDatabaseManager.h"
#include "CaretAssert.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BalsaStudySelectionDialog 
 * \brief Dialog for selection and getting study titles with BALSA Database
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param studyInformation
 *     The study information.
 * @param selectedStudyName
 *     Name of study information that is selected.
 * @param balsaDatabaseManager
 *     Database manager.
 * @param databaseURL
 *     URL for database requests.
 * @param balsaUsername
 *     Username for BALSA Database
 * @param balsaPassword
 *     Password for BALSA Database
 * @param parent
 *     Parent dialog
 */
BalsaStudySelectionDialog::BalsaStudySelectionDialog(const std::vector<BalsaStudyInformation>& studyInformation,
                                                     const AString selectedStudyName,
                                                     BalsaDatabaseManager* balsaDatabaseManager,
                                                     const AString& databaseURL,
                                                     const AString& balsaUsername,
                                                     const AString& balsaPassword,
                                                     QWidget* parent)
: WuQDialogModal("BALSA Studies",
                 parent),
m_studyInformation(studyInformation),
m_balsaDatabaseManager(balsaDatabaseManager),
m_databaseURL(databaseURL),
m_balsaUsername(balsaUsername),
m_balsaPassword(balsaPassword)
{
    m_studyTableWidget = new QTableWidget();
    
    QPushButton* newStudyTitlePushButton = new QPushButton("Create New Study...");
    newStudyTitlePushButton->setSizePolicy(QSizePolicy::Fixed,
                                           newStudyTitlePushButton->sizePolicy().verticalPolicy());
    QObject::connect(newStudyTitlePushButton, &QPushButton::clicked,
                     this, &BalsaStudySelectionDialog::newStudyButtonClicked);
    
    m_editSelectedStudyPushButton = new QPushButton("Edit Selected Study Title...");
    m_editSelectedStudyPushButton->setSizePolicy(QSizePolicy::Fixed,
                                                 m_editSelectedStudyPushButton->sizePolicy().verticalPolicy());
    QObject::connect(m_editSelectedStudyPushButton, &QPushButton::clicked,
                     this, &BalsaStudySelectionDialog::editSelectedStudyButtonClicked);
    
    
    QHBoxLayout* studyButtonLayout = new QHBoxLayout();
    studyButtonLayout->addWidget(newStudyTitlePushButton);
    studyButtonLayout->addStretch();
    studyButtonLayout->addWidget(m_editSelectedStudyPushButton);
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(m_studyTableWidget, 100);
    dialogLayout->addLayout(studyButtonLayout, 0);
    
    setCentralWidget(dialogWidget, SCROLL_AREA_NEVER);
    
    loadStudyTitleTableWidget(selectedStudyName);
    
    const QSize tableSize = WuQtUtilities::estimateTableWidgetSize(m_studyTableWidget);
    
    WuQtUtilities::resizeWindow(this,
                                std::min(tableSize.width() + 40,
                                         600),
                                std::min(tableSize.height() + 40,
                                         600));
}

/**
 * Destructor.
 */
BalsaStudySelectionDialog::~BalsaStudySelectionDialog()
{
}

/**
 * Load the list widget for the studies.
 *
 * @param selectedStudyName
 *     Name of study information that is selected.
 */
void
BalsaStudySelectionDialog::loadStudyTitleTableWidget(const AString selectedStudyNameIn)
{
    AString selectedStudyName = selectedStudyNameIn;
    
    const int numRows = static_cast<int32_t>(m_studyInformation.size());
    if (selectedStudyName.isEmpty()) {
        const int32_t selectedRow = m_studyTableWidget->currentRow();
        if ((selectedRow >= 0)
            && (selectedRow < numRows)) {
            selectedStudyName = m_studyTableWidget->item(selectedRow,
                                                         COLUMN_TITLE)->text();
        }
    }
    
    m_studyTableWidget->clear();
    m_studyTableWidget->setSelectionMode(QTableWidget::SingleSelection);
    m_studyTableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    m_studyTableWidget->setRowCount(numRows);
    m_studyTableWidget->setColumnCount(COLUMN_COUNT);
    m_studyTableWidget->setHorizontalHeaderItem(COLUMN_ID,    new QTableWidgetItem("ID"));
    m_studyTableWidget->setHorizontalHeaderItem(COLUMN_TITLE, new QTableWidgetItem("Title"));
    m_studyTableWidget->setShowGrid(false);
    
    std::sort(m_studyInformation.begin(),
              m_studyInformation.end());
    
    int32_t selectedRow = 0;
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        const AString studyTitle = m_studyInformation[iRow].getStudyTitle();
        if ( ! selectedStudyName.isEmpty()) {
            if (selectedStudyName.toLower() == studyTitle.toLower()) {
                selectedRow = iRow;
            }
        }
        
        CaretAssertVectorIndex(m_studyInformation, iRow);
        QTableWidgetItem* idItem = new QTableWidgetItem(m_studyInformation[iRow].getStudyID());
        idItem->setFlags(idItem->flags() &= (~Qt::ItemIsEditable));
        m_studyTableWidget->setItem(iRow, COLUMN_ID, idItem);

        QTableWidgetItem* titleItem = new QTableWidgetItem(studyTitle);
        titleItem->setFlags(idItem->flags() &= (~Qt::ItemIsEditable));
        m_studyTableWidget->setItem(iRow, COLUMN_TITLE, titleItem);
    }
    
    if (numRows > 0) {
        if (selectedRow >= 0) {
            m_studyTableWidget->selectRow(selectedRow);
        }
        else {
            m_studyTableWidget->selectRow(0);
        }
        const int32_t rowIndex = m_studyTableWidget->currentRow();
        if (rowIndex >= 0) {
            QTableWidgetItem* item = m_studyTableWidget->item(rowIndex, 0);
            if (item != NULL) {
                m_studyTableWidget->scrollToItem(item);
            }
        }
        
        QHeaderView* horizontalHeader = m_studyTableWidget->horizontalHeader();
        for (int32_t i = 0; i < COLUMN_COUNT; i++) {
            const int logicalIndex = horizontalHeader->logicalIndex(i);
            horizontalHeader->setSectionResizeMode(logicalIndex,
                                                   QHeaderView::ResizeToContents);
        }
    }
}

/**
 * @return The selected study information.  Will return
 * and empty instance (test with isEmpty()) if no study information was selected.
 */
BalsaStudyInformation
BalsaStudySelectionDialog::getSelectedStudyInformation() const
{
    const int32_t selectedRow = m_studyTableWidget->currentRow();
    if (selectedRow < 0) {
        return BalsaStudyInformation();
    }
    
    CaretAssertVectorIndex(m_studyInformation, selectedRow);
    return m_studyInformation[selectedRow];
}

/**
 * Called when the new study button is clicked.
 */
void
BalsaStudySelectionDialog::newStudyButtonClicked()
{
    AString title = QInputDialog::getText(this,
                                               "Create New Study",
                                               "Title of New Study");
    if ( ! title.isNull()) {
        title = title.trimmed();
        AString studyID;
        AString errorMessage;
        const bool successFlag = m_balsaDatabaseManager->getStudyIDFromStudyTitle(title,
                                                                                  studyID,
                                                                                  errorMessage);
        
        if (successFlag) {
            const AString titleLower = title.toLower();
            for (auto infoIter : m_studyInformation) {
                if (infoIter.getStudyTitle().toLower() == titleLower) {
                    WuQMessageBox::errorOk(this, ("Duplicate study names are not allowed: \""
                                                  + title
                                                  + "\" "));
                    return;
                }
            }
            m_studyInformation.emplace_back(studyID,
                                            title);
            loadStudyTitleTableWidget(title);
        }
        else {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
    }
}

/**
 * Called when edit selected study title button is clicked
 */
void
BalsaStudySelectionDialog::editSelectedStudyButtonClicked()
{
    const int32_t selectedRow = m_studyTableWidget->currentRow();
    if (selectedRow < 0) {
        WuQMessageBox::errorOk(this, "No Study Title is selected");
        return;
    }
    
    CaretAssertVectorIndex(m_studyInformation, selectedRow);
    AString title = m_studyInformation[selectedRow].getStudyTitle();
    
    title = QInputDialog::getText(this,
                                  "Edit study title",
                                  "Title",
                                  QLineEdit::Normal,
                                  title);
    if ( ! title.isNull()) {
        m_studyInformation[selectedRow].setStudyTitle(title);
        loadStudyTitleTableWidget(title);
    }
}

/**
 * Called when the OK button is clicked.
 */
void
BalsaStudySelectionDialog::okButtonClicked()
{
    if ( m_studyTableWidget->currentItem() == NULL) {
        WuQMessageBox::errorOk(this, "No Study Title is selected");
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

