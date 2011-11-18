#ifndef __PREFERENCES_DIALOG__H_
#define __PREFERENCES_DIALOG__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "WuQDialogNonModal.h"

class QCheckBox;
class QDoubleSpinBox;
class QComboBox;
class QRadioButton;


namespace caret {
    
    class CaretMappableDataFile;
    class PaletteColorMapping;
    class WuQWidgetObjectGroup;
    
    class PreferencesDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        PreferencesDialog(QWidget* parent);
        
        void updatePaletteEditor(CaretMappableDataFile* caretMappableDataFile,
                                 const int32_t mapIndex);
                    
        virtual ~PreferencesDialog();

    protected:
        virtual void applyButtonPressed();
        
    private:
        PreferencesDialog(const PreferencesDialog&);

        PreferencesDialog& operator=(const PreferencesDialog&);
        
    private:
        PaletteColorMapping* paletteColorMapping;
        
        QComboBox* paletteNameComboBox;
        
        QRadioButton* scaleAutoRadioButton;
        QRadioButton* scaleAutoPercentageRadioButton;
        QRadioButton* scaleFixedRadioButton;
        
        QDoubleSpinBox* scaleAutoPercentageNegativeMaximumSpinBox;
        QDoubleSpinBox* scaleAutoPercentageNegativeMinimumSpinBox;
        QDoubleSpinBox* scaleAutoPercentagePositiveMinimumSpinBox;
        QDoubleSpinBox* scaleAutoPercentagePositiveMaximumSpinBox;
        
        QDoubleSpinBox* scaleFixedNegativeMaximumSpinBox;
        QDoubleSpinBox* scaleFixedNegativeMinimumSpinBox;
        QDoubleSpinBox* scaleFixedPositiveMinimumSpinBox;
        QDoubleSpinBox* scaleFixedPositiveMaximumSpinBox;
        
        QCheckBox* displayModePositiveCheckBox;
        QCheckBox* displayModeZeroCheckBox;
        QCheckBox* displayModeNegativeCheckBox;
        
        QCheckBox* interpolateColorsCheckBox;
        
        WuQWidgetObjectGroup* widgetGroup;
    };
    
#ifdef __PREFERENCES_DIALOG__H__DECLARE__
#endif // __PREFERENCES_DIALOG__H__DECLARE__

} // namespace
#endif  //__PREFERENCES_DIALOG__H_
