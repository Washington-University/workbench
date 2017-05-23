
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

#define __BALSA_DATABASE_STUDY_TITLE_DIALOG_DECLARE__
#include "BalsaDatabaseStudyTitleDialog.h"
#undef __BALSA_DATABASE_STUDY_TITLE_DIALOG_DECLARE__

#include <QInputDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::BalsaDatabaseStudyTitleDialog 
 * \brief Dialog for selection and getting study titles with BALSA Database
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param balsaUsername
 *     Username for BALSA Database
 * @param balsaPassword
 *     Password for BALSA Database
 * @param parent
 *     Parent dialog
 */
BalsaDatabaseStudyTitleDialog::BalsaDatabaseStudyTitleDialog(const std::vector<AString> studyTitles,
                                                             const AString& balsaUsername,
                                                             const AString& balsaPassword,
                                                             QWidget* parent)
: WuQDialogModal("BALSA Study Title",
                 parent),
m_studyTitles(studyTitles),
m_balsaUsername(balsaUsername),
m_balsaPassword(balsaPassword)
{
    m_studyTitleListWidget = new QListWidget();
    m_studyTitleListWidget->setSelectionMode(QListWidget::SingleSelection);
    
    QPushButton* newStudyTitlePushButton = new QPushButton("Add New Study...");
    QObject::connect(newStudyTitlePushButton, &QPushButton::clicked,
                     this, &BalsaDatabaseStudyTitleDialog::newStudyButtonClicked);
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(m_studyTitleListWidget, 100);
    dialogLayout->addWidget(newStudyTitlePushButton, 0);
    
    setCentralWidget(dialogWidget, SCROLL_AREA_NEVER);
    
    loadStudyTitleListWidget();
}

/**
 * Destructor.
 */
BalsaDatabaseStudyTitleDialog::~BalsaDatabaseStudyTitleDialog()
{
}

/**
 * Load the list widget for the study titles
 */
void
BalsaDatabaseStudyTitleDialog::loadStudyTitleListWidget()
{
    const int32_t selectedRow = m_studyTitleListWidget->currentRow();
    m_studyTitleListWidget->clear();
    
    for (auto title : m_studyTitles) {
        m_studyTitleListWidget->addItem(title);
    }
    
    if (selectedRow >= 0) {
        m_studyTitleListWidget->setCurrentRow(selectedRow);
    }
}

/**
 * @return The selected study title or empty string if no selected title.
 */
AString
BalsaDatabaseStudyTitleDialog::getSelectedStudyTitle() const
{
    QListWidgetItem* item = m_studyTitleListWidget->currentItem();
    if (item != NULL) {
        return item->text();
    }
    return "";
}

/**
 * Called when the new study button is clicked.
 */
void
BalsaDatabaseStudyTitleDialog::newStudyButtonClicked()
{
    const AString name = QInputDialog::getText(this,
                                               "Study Title",
                                               "New Study Title");
    if ( ! name.isNull()) {
        m_studyTitles.push_back(name);
        loadStudyTitleListWidget();
        m_studyTitleListWidget->setCurrentRow(m_studyTitleListWidget->count() - 1);
    }
}

/**
 * Called when the OK button is clicked.
 */
void
BalsaDatabaseStudyTitleDialog::okButtonClicked()
{
    if ( m_studyTitleListWidget->currentItem() == NULL) {
        WuQMessageBox::errorOk(this, "No Study Title is selected");
        return;
    }
    
    WuQDialogModal::okButtonClicked();
}

