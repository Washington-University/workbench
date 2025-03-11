#ifndef __UNICODE_CHARACTER_ENTRY_DIALOG_H__
#define __UNICODE_CHARACTER_ENTRY_DIALOG_H__

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



#include <memory>

#include <QDialog>

class QLabel;
class QSpinBox;

namespace caret {

    class UnicodeCharacterEntryDialog : public QDialog {
        
        Q_OBJECT

    public:
        UnicodeCharacterEntryDialog(QWidget* parent = 0);
        
        virtual ~UnicodeCharacterEntryDialog();
        
        UnicodeCharacterEntryDialog(const UnicodeCharacterEntryDialog&) = delete;

        UnicodeCharacterEntryDialog& operator=(const UnicodeCharacterEntryDialog&) = delete;

        virtual void done(int resultCode) override;
        
        short getUnicodeValue() const;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void unicodeSpinBoxValueChanged(int value);
        
        void displayHyperlink(const QString& link);
        
    private:
        QSpinBox* m_unicodeSpinBox;
        
        QLabel* m_unicodeLabel;
        
        short m_selectedUnicodeValue = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __UNICODE_CHARACTER_ENTRY_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __UNICODE_CHARACTER_ENTRY_DIALOG_DECLARE__

} // namespace
#endif  //__UNICODE_CHARACTER_ENTRY_DIALOG_H__
