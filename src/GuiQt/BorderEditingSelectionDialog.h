#ifndef __BORDER_EDITING_SELECTION_DIALOG_H__
#define __BORDER_EDITING_SELECTION_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "WuQDialogModal.h"

class QCheckBox;

namespace caret {

    class BorderEditingSelectionDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BorderEditingSelectionDialog(const AString& modeDescription,
                                     const std::vector<AString>& borderNames,
                                     QWidget* parent);
        
        virtual ~BorderEditingSelectionDialog();
        

        // ADD_NEW_METHODS_HERE
        
        bool isBorderNameSelected(const int32_t borderNameIndex) const;
        
    private slots:
        void allOnPushButtonClicked();
        
        void allOffPushButtonClicked();
        
    private:
        BorderEditingSelectionDialog(const BorderEditingSelectionDialog&);

        BorderEditingSelectionDialog& operator=(const BorderEditingSelectionDialog&);
        
        void setAllCheckBoxesChecked(const bool status);
        
        std::vector<QCheckBox*> m_borderNameCheckBoxes;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BORDER_EDITING_SELECTION_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_EDITING_SELECTION_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_EDITING_SELECTION_DIALOG_H__
