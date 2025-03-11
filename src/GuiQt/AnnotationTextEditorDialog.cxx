
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
#include <QMenu>
#include <QTextCursor>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
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
    flags |= (Qt::CustomizeWindowHint);  // disables min/max buttons
    setWindowFlags(flags);
    
    setWindowTitle("Edit Annotation Text");
    
    QToolBar* toolbar(createToolBar());
    
    m_uneditedText = textAnnotation->getText();
    
    m_textEdit = new QTextEdit();
    const QString text(textAnnotation->getText());
    m_textEdit->setText(text);
    if ( ! text.isEmpty()) {
        QTextCursor textCursor(m_textEdit->textCursor());
        textCursor.movePosition(QTextCursor::End,
                                QTextCursor::MoveAnchor);
        m_textEdit->setTextCursor(textCursor);
    }
    m_textEdit->setToolTip("Press OK to save text changes and close dialog\n"
                           "Press CANCEL to revert changes and close dialog");
    QObject::connect(m_textEdit, SIGNAL(textChanged()),
                     this, SLOT(textWasEdited()));
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    QObject::connect(buttonBox, SIGNAL(accepted()),
                     this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()),
                     this, SLOT(reject()));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
    layout->addWidget(toolbar);
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
 * @return New instance of insert menu
 */
QToolBar* 
AnnotationTextEditorDialog::createToolBar()
{
    QToolBar* toolbar(new QToolBar());
    addToolButton(createInsertItemAction(InsertItem::ANNOTATION_SUBSTITUTION,
                                              "AS", "Insert annotation substitution"));
    addToolButton(createInsertUnicodeAction(0x00B2,
                                                 "x",
                                                 "Insert superscript 2"));
    addToolButton(createInsertUnicodeAction(0x00B3,
                                                 "x",
                                                 "Insert superscript 3"));
    addToolButton(createInsertItemAction(InsertItem::UNICODE_CHARCTER,
                                              "U", "Insert unicode character by entering hexadecimal value"));

    finishToolButtons(toolbar);
    
    return toolbar;
}

/**
 * Add a toolbutton to the toolbar
 * @param toolButton
 *    Toolbutton added to toolbar
 */
void
AnnotationTextEditorDialog::addToolButton(QToolButton* toolButton)
{
    /*
     * Increase font size as some unicode symbols such
     * as superscripts are very small
     */
    QFont font(toolButton->font());
    font.setPointSizeF(font.pointSizeF() * 1.5f);
    toolButton->setFont(font);
    
    m_toolWidgets.push_back(toolButton);
}

void 
AnnotationTextEditorDialog::finishToolButtons(QToolBar* toolbar)
{
    WuQtUtilities::matchWidgetSizes(m_toolWidgets);
    
    for (QWidget* w : m_toolWidgets) {
        toolbar->addWidget(w);
    }
}


/**
 * Called when an item is selected for insertion
 * @param item
 *    Item for insertion
 */
void
AnnotationTextEditorDialog::insertItemSelected(const InsertItem item)
{
    QString text;
    switch (item) {
        case InsertItem::ANNOTATION_SUBSTITUTION:
            text = "$<file>@<column>$";
            break;
        case InsertItem::UNICODE_CHARCTER:
            {
                UnicodeCharacterEntryDialog unicodeDialog(this);
                if (unicodeDialog.exec() == UnicodeCharacterEntryDialog::Accepted) {
                    QChar unicodeChar(unicodeDialog.getUnicodeValue());
                    insertUnicodeCharacterSelected(unicodeChar);
                }
            }
            break;
    }
    
    QTextCursor textCursor(m_textEdit->textCursor());
    textCursor.insertText(text);
}

/**
 * Create an action for inserting an item
 * @param item
 *    Item for insertion
 * @param text
 *    Text for item
 * @param tooltip
 *    Tooltip for item
 * @return
 *    Toolbutton setup to insert text for the item
 */
QToolButton*
AnnotationTextEditorDialog::createInsertItemAction(const InsertItem item,
                                                   const AString& text,
                                                   const AString& tooltip)
{
    QAction* action(new QAction(text));
    action->setToolTip(tooltip);
    QObject::connect(action, &QAction::triggered,
                     [=]() { insertItemSelected(item); });
    
    QToolButton* toolButton(new QToolButton());
    toolButton->setDefaultAction(action);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    
    return toolButton;
}

/**
 * Insert the given unicode character
 * @param unicodeCharacter
 *    Unicode character to insert
 */
void
AnnotationTextEditorDialog::insertUnicodeCharacterSelected(const QChar unicodeCharacter)
{
    QString text(unicodeCharacter);
    QTextCursor textCursor(m_textEdit->textCursor());
    textCursor.insertText(text);
}


/**
 * Create an action for inserting an unicode character
 * @param unicodeValue
 *    The unicode value
 * @param optionalTextPrefix
 *    Optional text prefix inserted before the unicode character (if not empty)
 * @param tooltip
 *    Tooltip for item
 * @return
 *    Toolbutton setup to insert the unicode character
 */
QToolButton*
AnnotationTextEditorDialog::createInsertUnicodeAction(const short unicodeValue,
                                                      const AString& optionalTextPrefix,
                                                      const AString& tooltip)
{
    const QChar unicodeCharacter(unicodeValue);
    const QString text(optionalTextPrefix
                       + unicodeCharacter);
    QAction* action(new QAction(text));
    action->setToolTip(tooltip);
    QObject::connect(action, &QAction::triggered,
                     [=]() { insertUnicodeCharacterSelected(unicodeCharacter); });
    
    QToolButton* toolButton(new QToolButton());
    toolButton->setDefaultAction(action);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
        
    return toolButton;
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
        m_textEdit->setText(m_uneditedText);
    }
  
    textWasEdited();
    
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
