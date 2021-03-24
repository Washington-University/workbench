#ifndef __WU_Q_TEXT_EDITOR_DIALOG_H__
#define __WU_Q_TEXT_EDITOR_DIALOG_H__

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



#include <memory>

#include <QDialog>

class QTextEdit;


namespace caret {
    class WuQTextEditorDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class TextMode {
            HTML,
            PLAIN
        };
        
        enum class WrapMode {
            NO,
            YES
        };

        enum class TextReadOnlyMode {
            NO,
            YES
        };
        
        static void runModal(const QString& dialogTitle,
                             const QString& text,
                             const TextMode textMode,
                             const WrapMode wrapMode,
                             QWidget* parent);

        static void runNonModal(const QString& dialogTitle,
                                const QString& text,
                                const TextMode textMode,
                                const WrapMode wrapMode,
                                QWidget* parent);
        
        WuQTextEditorDialog(const QString& dialogTitle,
                            const QString& text,
                            const TextReadOnlyMode readOnlyMode,
                            const TextMode textMode,
                            const WrapMode wrapMode,
                            QWidget* parent = 0);
        
        virtual ~WuQTextEditorDialog();
        

        // ADD_NEW_METHODS_HERE

    private:
        WuQTextEditorDialog(const WuQTextEditorDialog&);

        WuQTextEditorDialog& operator=(const WuQTextEditorDialog&);
        
        QTextEdit* m_textEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TEXT_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TEXT_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_TEXT_EDITOR_DIALOG_H__
