#ifndef __GENERAL_YOKING_GROUP_COMBO_BOX_H__
#define __GENERAL_YOKING_GROUP_COMBO_BOX_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include <QComboBox>

#include "GeneralYokingGroupEnum.h"

namespace caret {

    class GeneralYokingGroupComboBox : public QComboBox {
        
        Q_OBJECT

    public:
        /*
         * Mode for type of text to show in combo box
         */
        enum class TextMode {
            /** Arabic numerals 1, 2, ..*/
            ARABIC_NUMERAL,
            /** Lower-case roman numerals i, ii,..*/
            LOWER_CASE_ROMAN_NUMERAL,
            /** Upper-case roman numerals I, II,..*/
            UPPER_CASE_ROMAN_NUMERAL,
            /** Lower-case alphabetical a, b, ..*/
            LOWER_CASE_ALPHABETICAL,
            /** Upper-case alphabetical A, B, ..*/
            UPPER_CASE_ALPHABETICAL
        };
        
        GeneralYokingGroupComboBox(const TextMode textMode,
                                   QWidget* parent = 0);
        
        virtual ~GeneralYokingGroupComboBox();
        
        GeneralYokingGroupComboBox(const GeneralYokingGroupComboBox&) = delete;

        GeneralYokingGroupComboBox& operator=(const GeneralYokingGroupComboBox&) = delete;
        
        GeneralYokingGroupEnum::Enum getYokingGroup() const;
        
        void setYokingGroup(const GeneralYokingGroupEnum::Enum yokingGroup);
        
        // ADD_NEW_METHODS_HERE

    signals:
        void yokingGroupSelected(const GeneralYokingGroupEnum::Enum yokingGroup);
       
    private slots:
        void itemActivated(int index);
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GENERAL_YOKING_GROUP_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GENERAL_YOKING_GROUP_COMBO_BOX_DECLARE__

} // namespace
#endif  //__GENERAL_YOKING_GROUP_COMBO_BOX_H__
