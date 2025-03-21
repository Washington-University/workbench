#ifndef __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_H__
#define __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_H__

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



#include <vector>

#include <QTextCursor>
#include <QWidget>

class QTextEdit;
class QToolBar;
class QToolButton;

namespace caret {

    class WuQTextEditWithToolBarWidget : public QWidget {
        
        Q_OBJECT

    public:
        WuQTextEditWithToolBarWidget();
        
        virtual ~WuQTextEditWithToolBarWidget();
        
        WuQTextEditWithToolBarWidget(const WuQTextEditWithToolBarWidget&) = delete;

        WuQTextEditWithToolBarWidget& operator=(const WuQTextEditWithToolBarWidget&) = delete;

        void moveCursor(QTextCursor::MoveOperation operation,
                        QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);
        
        QString toPlainText() const;

        // ADD_NEW_METHODS_HERE

    signals:
        void textChanged();
        
    public slots:
        void selectAll();
        
        void setPlainText(const QString& text);
        
    private:
        enum class InsertItem {
            ANNOTATION_SUBSTITUTION,
            UNICODE_CHARCTER
        };

    private slots:
        void insertItemSelected(const InsertItem item);
        
        void insertUnicodeCharacterSelected(const QChar unicodeCharacter);
        

    private:
        QToolBar* createToolBar();
        
        QToolButton* createInsertUnicodeAction(const short unicodeValue,
                                               const QString& optionalTextPrefix,
                                               const QString& tooltip);
        
        QToolButton* createInsertItemAction(const InsertItem item,
                                            const QString& text,
                                            const QString& tooltip);
        
        void addToolButton(QToolButton* toolButton);
        
        void finishToolButtons(QToolBar* toolbar);
        
        QTextEdit* m_textEdit;
        
        std::vector<QWidget*> m_toolWidgets;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_TEXT_EDIT_WITH_TOOL_BAR_WIDGET_H__
