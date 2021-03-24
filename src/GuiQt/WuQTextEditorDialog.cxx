
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

#define __WU_Q_TEXT_EDITOR_DIALOG_DECLARE__
#include "WuQTextEditorDialog.h"
#undef __WU_Q_TEXT_EDITOR_DIALOG_DECLARE__

#include <QPushButton>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>


#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQTextEditorDialog 
 * \brief Dialog that shows a text editor.
 * \ingroup GuiQt
 */

/**
 * Run the dialog in modal (blocking) mode.
 * Since modal and no way to retrieve text, the
 * text is not editable.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param text
 *    Text displayed in text editor.
 * @param textMode
 *    Text mode (html or plain)
 * @param wrapMode
 *    Text wrapping (no, yes)
 * @param parent
 *    The parent widget on which dialog is displayed.
 */
void
WuQTextEditorDialog::runModal(const QString& dialogTitle,
                              const QString& text,
                              const TextMode textMode,
                              const WrapMode wrapMode,
                              QWidget* parent)
{
    WuQTextEditorDialog dialog(dialogTitle,
                               text,
                               TextReadOnlyMode::YES,
                               textMode,
                               wrapMode,
                               parent);

    dialog.exec();
}

/**
 * Run the dialog in non-modal (non-blocking) mode.
 * Since non-modal and no way to retrieve text, the
 * text is not editable.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param text
 *    Text displayed in text editor.
 * @param textMode
 *    Text mode (html or plain)
 * @param wrapMode
 *    Text wrapping (no, yes)
 * @param parent
 *    The parent widget on which dialog is displayed.
 */
void
WuQTextEditorDialog::runNonModal(const QString& dialogTitle,
                                 const QString& text,
                                 const TextMode textMode,
                                 const WrapMode wrapMode,
                                 QWidget* parent)
{
    WuQTextEditorDialog* dialog = new WuQTextEditorDialog(dialogTitle,
                                                          text,
                                                          TextReadOnlyMode::YES,
                                                          textMode,
                                                          wrapMode,
                                                          parent);
    
    /*
     * Since dialog is non-modal, setting this attribute
     * will destroy the dialog when the dialog is closed
     * and prevent a memory leak.
     */
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    
    dialog->setVisible(true);
    dialog->show();
    dialog->activateWindow();
    dialog->raise();
}

/**
 * Constructor.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param text
 *    Text displayed in text editor.
 * @param readOnlyMode
 *    Read only mode (no, yes)
 * @param textMode
 *    Text mode (html or plain)
 * @param wrapMode
 *    Text wrapping (no, yes)
 * @param parent
 *    The parent widget on which dialog is displayed.
 */
WuQTextEditorDialog::WuQTextEditorDialog(const QString& dialogTitle,
                                         const QString& text,
                                         const TextReadOnlyMode readOnlyMode,
                                         const TextMode textMode,
                                         const WrapMode wrapMode,
                                         QWidget* parent)
: QDialog(parent)
{
    setWindowTitle(dialogTitle);
    
    m_textEdit = new QTextEdit();
    switch (textMode) {
        case TextMode::HTML:
            m_textEdit->setHtml(text);
            break;
        case TextMode::PLAIN:
            m_textEdit->setPlainText(text);
            break;
    }
    
    switch (readOnlyMode) {
        case TextReadOnlyMode::NO:
            m_textEdit->setReadOnly(false);
            break;
        case TextReadOnlyMode::YES:
            m_textEdit->setReadOnly(true);
            break;
    }
    
    QPushButton* closeButton = new QPushButton("Close");
    QObject::connect(closeButton, &QPushButton::clicked,
                     this, &QDialog::accept);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_textEdit, 100);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
    
    
    QTextDocument* document = m_textEdit->document();
    
    QTextOption textOption = document->defaultTextOption();
    switch (wrapMode) {
        case WrapMode::NO:
            textOption.setWrapMode(QTextOption::NoWrap);
            break;
        case WrapMode::YES:
            textOption.setWrapMode(QTextOption::WordWrap);
            break;
    }
    
    document->setDefaultTextOption(textOption);
    
    document->adjustSize();
    m_textEdit->adjustSize();
    
    int32_t dialogWidth = document->size().width() + 70;
    if (dialogWidth < 300) {
        dialogWidth = 300;
    }
    const int32_t dialogHeight = document->size().height() + 100;
    
    WuQtUtilities::resizeWindow(this,
                                dialogWidth,
                                dialogHeight);
    
}

/**
 * Destructor.
 */
WuQTextEditorDialog::~WuQTextEditorDialog()
{
}

