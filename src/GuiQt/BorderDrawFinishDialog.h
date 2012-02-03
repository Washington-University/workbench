#ifndef __BORDER_DRAW_FINISH_DIALOG__H_
#define __BORDER_DRAW_FINISH_DIALOG__H_

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

#include "CaretColorEnum.h"
#include "WuQDialogModal.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QRadioButton;
class QLineEdit;

namespace caret {

    class Border;
    class BorderFile;
    class CaretColorEnumSelectionControl;
    
    class BorderDrawFinishDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        BorderDrawFinishDialog(Border* border,
                               QWidget* parent = 0);
        
        virtual ~BorderDrawFinishDialog();
        
    protected:
        virtual void okButtonPressed();
        
    private slots:
        void displayColorEditor();
    
    private:
        BorderDrawFinishDialog(const BorderDrawFinishDialog&);

        BorderDrawFinishDialog& operator=(const BorderDrawFinishDialog&);
        
        Border* border;
        
        QComboBox* borderFileSelectionComboBox;
        
        QLineEdit* nameLineEdit;
        
        QRadioButton* openRadioButton;
        QRadioButton* closedRadioButton;
        
        QLineEdit* classNameLineEdit;

        CaretColorEnumSelectionControl* colorSelectionControl;
        
        static AString previousName;
        static BorderFile* previousBorderFile;
        static bool previousOpenTypeSelected;
        static AString previousClassName;     
        static CaretColorEnum::Enum previousCaretColor;
    };
    
#ifdef __BORDER_DRAW_FINISH_DIALOG_DECLARE__
    AString BorderDrawFinishDialog::previousName = "Name";
    BorderFile* BorderDrawFinishDialog::previousBorderFile = NULL;
    bool BorderDrawFinishDialog::previousOpenTypeSelected = true;
    AString BorderDrawFinishDialog::previousClassName = "None";
    CaretColorEnum::Enum BorderDrawFinishDialog::previousCaretColor;
#endif // __BORDER_DRAW_FINISH_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_DRAW_FINISH_DIALOG__H_
