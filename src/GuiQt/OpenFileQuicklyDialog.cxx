
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __OPEN_FILE_QUICKLY_DIALOG_DECLARE__
#include "OpenFileQuicklyDialog.h"
#undef __OPEN_FILE_QUICKLY_DIALOG_DECLARE__

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQMessageBoxTwo.h"

using namespace caret;


    
/**
 * \class caret::OpenFileQuicklyDialog 
 * \brief Dialog to quickly open file using file's path
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    Dialog's parent widget
 */
OpenFileQuicklyDialog::OpenFileQuicklyDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Open File Quickly");
    
    QLabel* instructionsLabel(new QLabel("Enter absolute path to file:"));
    
    m_filenameLineEdit = new QLineEdit();
    m_filenameLineEdit->setClearButtonEnabled(true);
    m_filenameLineEdit->setText(s_lastFilenameLineEditText);
    
    QDialogButtonBox* buttonBox(new QDialogButtonBox(QDialogButtonBox::Open
                                                     | QDialogButtonBox::Cancel));
    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     this, &OpenFileQuicklyDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     this, &OpenFileQuicklyDialog::reject);

    QVBoxLayout* dialogLayout(new QVBoxLayout(this));
    dialogLayout->addWidget(instructionsLabel);
    dialogLayout->addWidget(m_filenameLineEdit);
    dialogLayout->addWidget(buttonBox);
    
    setMinimumWidth(500);
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
OpenFileQuicklyDialog::~OpenFileQuicklyDialog()
{
}

/**
 * Called to close dialog and set result code
 */
void
OpenFileQuicklyDialog::done(int r) 
{
    if (r == QDialog::Accepted) {
        if (getFilename().isEmpty()) {
            WuQMessageBoxTwo::criticalOk(this, "Error", "Filename is empty");
            return;
        }
    }
    
    s_lastFilenameLineEditText = getFilename();
    
    QDialog::done(r);
}

/**
 * @return Name of file entered by user
 */
AString
OpenFileQuicklyDialog::getFilename() const
{
    return m_filenameLineEdit->text().trimmed();
}

