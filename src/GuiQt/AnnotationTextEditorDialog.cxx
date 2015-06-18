
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__
#include "AnnotationTextEditorDialog.h"
#undef __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QVBoxLayout>

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationTextEditorDialog 
 * \brief Dialog for editing annotation text.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param textAnnotation
 *    Text annotation that will be edited.
 * @param parent
 *    Parent of this dialog.
 */
AnnotationTextEditorDialog::AnnotationTextEditorDialog(AnnotationText* textAnnotation,
                                                       QWidget* parent)
: QDialog(parent),
m_textAnnotation(textAnnotation)
{
    CaretAssert(textAnnotation);
    
    Qt::WindowFlags flags = windowFlags();
    flags |= (Qt::CustomizeWindowHint);  // disables min/max buttons
    setWindowFlags(flags);
    
    setWindowTitle("Edit Text");
    
    m_uneditedText = textAnnotation->getText();
    
    m_textEdit = new QLineEdit();
    m_textEdit->setText(textAnnotation->getText());
    m_textEdit->selectAll();
    m_textEdit->setToolTip("Press RETURN to save text editing\n"
                           "Press ESCAPE to cancel text editing");
    QObject::connect(m_textEdit, SIGNAL(textChanged(const QString&)),
                     this, SLOT(textWasEdited(const QString&)));
    QObject::connect(m_textEdit, SIGNAL(returnPressed()),
                     this, SLOT(textReturnedPressed()));
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()),
                     this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addWidget(m_textEdit);
    layout->addWidget(buttonBox);
}

/**
 * Destructor.
 */
AnnotationTextEditorDialog::~AnnotationTextEditorDialog()
{
}

/**
 * Closes the dialog.
 *
 * @param resultCode
 */
void
AnnotationTextEditorDialog::done(int resultCode)
{
    if (resultCode == QDialog::Accepted) {
    }
    else {
        m_textAnnotation->setText(m_uneditedText);
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    QDialog::done(resultCode);
}

/**
 * Called when the return key is pressed.
 */
void
AnnotationTextEditorDialog::textReturnedPressed()
{
    accept();
}

/**
 * Called when the user edits text.
 *
 * @param text
 *     Text entered by user.
 */
void
AnnotationTextEditorDialog::textWasEdited(const QString& text)
{
    m_textAnnotation->setText(text);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
