#ifndef __WU_Q_MACRO_COPY_DIALOG_H__
#define __WU_Q_MACRO_COPY_DIALOG_H__

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

class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QPlainTextEdit;


namespace caret {

    class WuQMacro;
    class WuQMacroGroup;
    class WuQMacroShortCutKeyComboBox;

    class WuQMacroCopyDialog : public QDialog {
        
        Q_OBJECT

    public:
        WuQMacroCopyDialog(QWidget* parent = 0);
        
        virtual ~WuQMacroCopyDialog();
        
        WuQMacroCopyDialog(const WuQMacroCopyDialog&) = delete;

        WuQMacroCopyDialog& operator=(const WuQMacroCopyDialog&) = delete;

        const WuQMacro* getMacroToCopy() const;

        // ADD_NEW_METHODS_HERE
        
    public slots:
        virtual void done(int r) override;
        
    private slots:
        void macroGroupComboBoxItemActivated(int);
        
        void macroComboBoxItemActivated(int);
        
    private:
        const WuQMacroGroup* getMacroGroup() const;
        
        std::vector<const WuQMacroGroup*> m_macroGroups;
        
        QComboBox* m_macroGroupComboBox;
        
        QComboBox* m_macroNameComboBox;
        
        QPlainTextEdit* m_macroDescriptionTextEdit;
        
        QDialogButtonBox* m_dialogButtonBox;
        
        static QString s_lastSelectedMacroGroupIdentifier;
    };
    
#ifdef __WU_Q_MACRO_COPY_DIALOG_DECLARE__
    QString WuQMacroCopyDialog::s_lastSelectedMacroGroupIdentifier = "";
#endif // __WU_Q_MACRO_COPY_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COPY_DIALOG_H__
