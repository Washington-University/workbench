#ifndef __INFORMATION_DISPLAY_OPTIONS_DIALOG_H__
#define __INFORMATION_DISPLAY_OPTIONS_DIALOG_H__

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


#include "WuQDialogNonModal.h"

#include <QDoubleSpinBox>

namespace caret {

    class CaretColorEnumComboBox;
    class WuQTrueFalseComboBox;
    
    class InformationDisplayPropertiesDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        InformationDisplayPropertiesDialog(QWidget* parent,
                                        Qt::WindowFlags f = Qt::WindowFlags());
        
        virtual ~InformationDisplayPropertiesDialog();
        
        void updateDialog();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void informationPropertyChanged();
    
    private:
        InformationDisplayPropertiesDialog(const InformationDisplayPropertiesDialog&);

        InformationDisplayPropertiesDialog& operator=(const InformationDisplayPropertiesDialog&);
        
        CaretColorEnumComboBox* m_idColorComboBox;
        
        CaretColorEnumComboBox* m_idContralateralColorComboBox;
        
        QDoubleSpinBox* m_symbolSizeSpinBox;
        
        QDoubleSpinBox* m_mostRecentSymbolSizeSpinBox;
        
        WuQTrueFalseComboBox* m_surfaceIdentificationSymbolComboBox;
        
        WuQTrueFalseComboBox* m_volumeIdentificationSymbolComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __INFORMATION_DISPLAY_OPTIONS_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __INFORMATION_DISPLAY_OPTIONS_DIALOG_DECLARE__

} // namespace
#endif  //__INFORMATION_DISPLAY_OPTIONS_DIALOG_H__
