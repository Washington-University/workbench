
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
#include <QVBoxLayout>

#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "UnicodeCharacterEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQTextEditWithToolBarWidget.h"
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
 * @param userInputMode
 *    The user input mode
 * @param textAnnotation
 *    Text annotation that will be edited.
 * @param parent
 *    Parent of this dialog.
 */
AnnotationTextEditorDialog::AnnotationTextEditorDialog(const UserInputModeEnum::Enum userInputMode,
                                                       AnnotationText* textAnnotation,
                                                       QWidget* parent)
: QDialog(parent),
m_userInputMode(userInputMode),
m_textAnnotation(textAnnotation)
{
    CaretAssert(textAnnotation);
    
    Qt::WindowFlags flags = windowFlags();
    flags |= (Qt::CustomizeWindowHint);  /* disables min/max buttons */
    setWindowFlags(flags);
    
    setWindowTitle("Edit Annotation Text");
    
    m_uneditedText = textAnnotation->getText();
    
    m_textEdit = new WuQTextEditWithToolBarWidget();
    const QString text(textAnnotation->getText());
    m_textEdit->setPlainText(text);
    m_textEdit->moveCursor(QTextCursor::End);
    m_textEdit->setToolTip("Press OK to save text changes and close dialog\n"
                           "Press CANCEL to revert changes and close dialog");
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()),
                     this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addWidget(m_textEdit, 100);
    layout->addWidget(buttonBox, 0);
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
        const QString text = m_textEdit->toPlainText();
        if (text != m_uneditedText) {
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            std::vector<Annotation*> annotationVector;
            annotationVector.push_back(m_textAnnotation);
            undoCommand->setModeTextCharacters(text,
                                               annotationVector);
            AString errorMessage;
            if ( ! annMan->applyCommand(undoCommand,
                                        errorMessage)) {
                WuQMessageBox::errorOk(this,
                                       errorMessage);
            }
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        }
    }

    QDialog::done(resultCode);
}

/**
 * Called when the user edits text.
 *
 * @param text
 *     Text entered by user.
 */
void
AnnotationTextEditorDialog::textWasEdited()
{
    const QString text = m_textEdit->toPlainText();
    AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode);
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    std::vector<Annotation*> annotationVector;
    annotationVector.push_back(m_textAnnotation);
    undoCommand->setModeTextCharacters(text,
                                       annotationVector);
    AString errorMessage;
    if ( ! annMan->applyCommand(undoCommand,
                                errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}
