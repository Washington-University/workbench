#ifndef __WU_Q_MACRO_DIALOG_H__
#define __WU_Q_MACRO_DIALOG_H__

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

class QAbstractButton;
class QComboBox;
class QDialogButtonBox;
class QListWidget;
class QPlainTextEdit;

namespace caret {

    class WuQMacro;
    class WuQMacroGroup;

    class WuQMacroDialog : public QDialog {
        
        Q_OBJECT

    public:
        WuQMacroDialog(QWidget* parent = 0);
        
        virtual ~WuQMacroDialog();
        
        WuQMacroDialog(const WuQMacroDialog&) = delete;

        WuQMacroDialog& operator=(const WuQMacroDialog&) = delete;
        
        void updateDialogContents();

        // ADD_NEW_METHODS_HERE

    private slots:
        void macroGroupBoxActivated(int);
        
        void macrosListWidgetCurrentRowChanged(int);
        
        void buttonClicked(QAbstractButton* button);
        
        void editSelectedMacro();
        
        void runSelectedMacro();
        
    private:
        WuQMacroGroup* getSelectedMacroGroup();
        
        WuQMacro* getSelectedMacro();
        
        std::vector<WuQMacroGroup*> m_macroGroups;
        
        QComboBox* m_macroGroupComboBox;
        
        QListWidget* m_macrosListWidget;
        
        QPlainTextEdit* m_macroDescriptionTextEdit;
        
        QDialogButtonBox* m_dialogButtonBox;
        
        QAbstractButton* m_runButton;
        
        QAbstractButton* m_editButton;
        
        WuQMacro* m_macro = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_DIALOG_H__
