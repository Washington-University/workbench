
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_DECLARE__
#include "ExitProgramModifiedFilesDialog.h"
#undef __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_DECLARE__

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::ExitProgramModifiedFilesDialog 
 * \brief Dialog displayed when exiting wb_view with modified files
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param modifiedFilesText
 *    Text describing modified files
 * @param parent
 *    Parent widget
 */
ExitProgramModifiedFilesDialog::ExitProgramModifiedFilesDialog(const QString& modifiedFilesText,
                                                               QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Warning: Modified Files");
    
    QLabel* topLabel = new QLabel("<html><b>"
                                  "Do you want to save changes you made to these files?"
                                  "</b></html>");
    
    QPixmap warningPixmap;
    const bool warningPixmapValid(WuQtUtilities::loadPixmap(":/MessageDialog/BrainWarningImage.png",
                                                            warningPixmap));
    QLabel* warningLabel(NULL);
    if (warningPixmapValid) {
        warningLabel = new QLabel();
        warningLabel->setPixmap(warningPixmap);
    }

    /*
     * Label for storing message
     */
    QLabel* textLabel = new QLabel(modifiedFilesText);
    textLabel->setWordWrap(true);
    textLabel->setMinimumWidth(400);
    const QSize textLabelSize(textLabel->sizeHint());
    const int32_t textHeight(textLabelSize.height());
    const int32_t textWidth(textLabelSize.width());
    
    /*
     * Put label in a scroll area when there is
     * a large amount of text.  Otherwise, the
     * dialog may be too large (taller than the
     * screen) and unusable.
     */
    QWidget* textWidget(textLabel);
    if ((textWidth > s_scrollWidth)
        || (textHeight > s_scrollHeight)) {
        m_haveScrollAreaFlag = true;
        QScrollArea* scrollArea(new QScrollArea());
        scrollArea->setWidget(textLabel);
        textWidget = scrollArea;
    }

    /*
     * Buttons at bottom of dialog
     */
    m_buttonBox = new QDialogButtonBox();
    m_cancelButton = m_buttonBox->addButton(QDialogButtonBox::StandardButton::Cancel);
    m_cancelButton->setToolTip("Close this dialog while taking no action");
    m_discardButton = m_buttonBox->addButton("Don't Save", QDialogButtonBox::ButtonRole::ApplyRole);
    m_discardButton->setToolTip("DO NOT save modified files and exit program");
    m_saveButton    = m_buttonBox->addButton("Save...", QDialogButtonBox::AcceptRole);
    m_saveButton->setToolTip("Display manage files window to save modified files");
    QObject::connect(m_buttonBox, &QDialogButtonBox::clicked,
                     this, &ExitProgramModifiedFilesDialog::buttonClicked);
    
    if (warningLabel != NULL) {
        QGridLayout* layout = new QGridLayout(this);
        int row(0);
        layout->addWidget(warningLabel, row, 0, 2, 1, Qt::AlignTop);
        layout->addWidget(topLabel, row, 1, Qt::AlignLeft);
        ++row;
        layout->addWidget(textWidget, row, 1);
        ++row;
        layout->addWidget(m_buttonBox, row, 0, 1, 2);
    }
    else {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(topLabel);
        layout->addWidget(textWidget);
        layout->addWidget(m_buttonBox);
    }
}

/**
 * Destructor.
 */
ExitProgramModifiedFilesDialog::~ExitProgramModifiedFilesDialog()
{
}

/**
 * @return Size hint for this dialog
 */
QSize
ExitProgramModifiedFilesDialog::sizeHint() const
{
    /*
     * The size hint will be the default size
     * of the dialog.
     */
    QSize s(QDialog::sizeHint());
    if (m_haveScrollAreaFlag) {
        if (s.width() > s_scrollWidth) {
            s.setWidth(s_scrollWidth);
        }
        if (s.height() > s_scrollHeight) {
            s.setHeight(s_scrollHeight);
        }
    }
    return s;
}

/**
 * @return Result of user-interaction with this dialog (Save, Discard, Cancel)
 */
ExitProgramModifiedFilesDialog::Result
ExitProgramModifiedFilesDialog::getResult() const
{
    CaretAssert(m_result != Result::INVALID);
    return m_result;
}

/**
 * Called a button is clicked
 * @param button
 *    Button that was clicked
 */
void
ExitProgramModifiedFilesDialog::buttonClicked(QAbstractButton *button)
{
    m_result = Result::INVALID;
    
    if (button == m_cancelButton) {
        m_result = Result::CANCEL_EXIT;
        reject();
    }
    else if (button == m_discardButton) {
        m_result = Result::DISCARD_AND_EXIT;
        accept();
    }
    else if (button == m_saveButton) {
        m_result = Result::SAVE_AND_EXIT;
        accept();
    }
    else {
        CaretAssertMessage(0, "Invalid button.  Was new button added?");
    }
}

