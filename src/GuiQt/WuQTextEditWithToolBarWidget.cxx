
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

#define __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_DECLARE__
#include "WuQTextEditWithToolBarWidget.h"
#undef __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_DECLARE__

#include <QAction>
#include <QFont>
#include <QTextCursor>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "UnicodeCharacterEntryDialog.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQTextEditWithToolBarWidget 
 * \brief A QTextEdit with a toolbar for entering unicode characters
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQTextEditWithToolBarWidget::WuQTextEditWithToolBarWidget()
: QWidget()
{
    QToolBar* toolbar(createToolBar());
    

    m_textEdit = new QTextEdit();
    QObject::connect(m_textEdit, &QTextEdit::textChanged,
                     this, &WuQTextEditWithToolBarWidget::textChanged);
    
    QVBoxLayout* layout(new QVBoxLayout(this));
    layout->addWidget(toolbar, 0, Qt::AlignLeft);
    layout->addWidget(m_textEdit, 100);
}

/**
 * Destructor.
 */
WuQTextEditWithToolBarWidget::~WuQTextEditWithToolBarWidget()
{
}

/**
 * Moves the cursor by performing the given operation.
 * If mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
 * This is the same effect that the user achieves when they hold down the Shift key and move the cursor with the cursor keys.
 * @param operation
 *    The move operation
 * @param mode
 *    The move mode
 */
void
WuQTextEditWithToolBarWidget::moveCursor(QTextCursor::MoveOperation operation,
                                         QTextCursor::MoveMode mode)
{
    m_textEdit->moveCursor(operation,
                           mode);
}

/**
 * @return the text of the text edit as plain text.
 */
QString
WuQTextEditWithToolBarWidget::toPlainText() const
{
    return m_textEdit->toPlainText();
}

/**
 * Changes the text of the text edit to the string text. Any previous text is removed.
 * @param text
 *    New text for editor
 */
void
WuQTextEditWithToolBarWidget::setPlainText(const QString& text)
{
    m_textEdit->setPlainText(text);
}

/*
 * Selects all text.
 */
void
WuQTextEditWithToolBarWidget::selectAll()
{
    m_textEdit->selectAll();
}

/**
 * @return New instance of insert menu
 */
QToolBar*
WuQTextEditWithToolBarWidget::createToolBar()
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
    addToolButton(createInsertUnicodeAction(0x2264,
                                            "",
                                            "Insert less than or equal to"));
    addToolButton(createInsertUnicodeAction(0x2265,
                                            "",
                                            "Insert greater than or equal to"));
    addToolButton(createInsertUnicodeAction(0x2260,
                                            "",
                                            "Insert not equal to"));
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
WuQTextEditWithToolBarWidget::addToolButton(QToolButton* toolButton)
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
WuQTextEditWithToolBarWidget::finishToolButtons(QToolBar* toolbar)
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
WuQTextEditWithToolBarWidget::insertItemSelected(const InsertItem item)
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
WuQTextEditWithToolBarWidget::createInsertItemAction(const InsertItem item,
                                                     const QString& text,
                                                     const QString& tooltip)
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
WuQTextEditWithToolBarWidget::insertUnicodeCharacterSelected(const QChar unicodeCharacter)
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
WuQTextEditWithToolBarWidget::createInsertUnicodeAction(const short unicodeValue,
                                                        const QString& optionalTextPrefix,
                                                        const QString& tooltip)
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
