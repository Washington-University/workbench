#ifndef __ANNOTATION_TEXT_EDITOR_DIALOG_H__
#define __ANNOTATION_TEXT_EDITOR_DIALOG_H__

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


#include <QDialog>

#include "UserInputModeEnum.h"

class QTextEdit;
class QToolBar;
class QToolButton;

namespace caret {

    class AnnotationText;
    
    class AnnotationTextEditorDialog : public QDialog {
        
        Q_OBJECT

    public:
        AnnotationTextEditorDialog(const UserInputModeEnum::Enum userInputMode,
                                   AnnotationText* textAnnotation,
                                   QWidget* parent);
        
        virtual ~AnnotationTextEditorDialog();
        
        virtual void done(int resultCode) override;
        
    signals:
        void textHasBeenChanged(const QString&);
        
    private:
        enum class InsertItem {
            ANNOTATION_SUBSTITUTION,
            UNICODE_CHARCTER
        };

    private slots:
        void textWasEdited();
        
        void insertItemSelected(const InsertItem item);
        
        void insertUnicodeCharacterSelected(const QChar unicodeCharacter);
        
        // ADD_NEW_METHODS_HERE

    private:        
        AnnotationTextEditorDialog(const AnnotationTextEditorDialog&);

        AnnotationTextEditorDialog& operator=(const AnnotationTextEditorDialog&);
        
        QToolBar* createToolBar();
        
        QToolButton* createInsertUnicodeAction(const short unicodeValue,
                                               const AString& optionalTextPrefix,
                                               const AString& tooltip);
        
        QToolButton* createInsertItemAction(const InsertItem item,
                                            const AString& text,
                                            const AString& tooltip);

        void addToolButton(QToolButton* toolButton);
        
        void finishToolButtons(QToolBar* toolbar);
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        AnnotationText* m_textAnnotation;
        
        QString m_uneditedText;
        
        QTextEdit* m_textEdit;
        
        std::vector<QWidget*> m_toolWidgets;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_EDITOR_DIALOG_H__
