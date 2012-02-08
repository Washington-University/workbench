#ifndef __BORDER_PROPERTIES_EDITOR_DIALOG__H_
#define __BORDER_PROPERTIES_EDITOR_DIALOG__H_

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

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;

namespace caret {

    class Border;
    class BorderFile;
    class CaretColorEnumSelectionControl;
    
    class BorderPropertiesEditorDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        static BorderPropertiesEditorDialog*
            newInstanceFinishBorder(Border* border,
                                    QWidget* parent = 0);
        
        static BorderPropertiesEditorDialog*
            newInstanceEditBorder(BorderFile* editModeBorderFile,
                                  Border* border,
                                  QWidget* parent = 0);
        
        virtual ~BorderPropertiesEditorDialog();
        
    protected:
        virtual void okButtonPressed();
        
    private slots:
        void displayClassEditor();
    
        void borderFileSelected();
        
    private:
        enum Mode {
            MODE_EDIT,
            MODE_FINISH_DRAWING
        };
        
        BorderPropertiesEditorDialog(const QString& title,
                                     const Mode mode,
                                     BorderFile* editModeBorderFile,
                                     Border* border,
                                     QWidget* parent = 0);
        
        BorderPropertiesEditorDialog(const BorderPropertiesEditorDialog&);

        BorderPropertiesEditorDialog& operator=(const BorderPropertiesEditorDialog&);
        
        BorderFile* getSelectedBorderFile(bool createIfNoValidBorderFiles);
        
        void loadBorderFileComboBox();
        
        void loadClassNameComboBox(const QString& className = "");
        
        Mode mode;
        
        BorderFile* editModeBorderFile;
        Border* border;
        
        QComboBox* borderFileSelectionComboBox;
        
        QLineEdit* nameLineEdit;
        
        QCheckBox* closedCheckBox;
        
        QComboBox* classNameComboBox;

        QCheckBox* reversePointOrderCheckBox;
        
        CaretColorEnumSelectionControl* colorSelectionControl;
        
        static AString previousName;
        static BorderFile* previousBorderFile;
        static bool previousClosedSelected;
        static AString previousClassName;     
        static CaretColorEnum::Enum previousCaretColor;
    };
    
#ifdef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
    AString BorderPropertiesEditorDialog::previousName = "Name";
    BorderFile* BorderPropertiesEditorDialog::previousBorderFile = NULL;
    bool BorderPropertiesEditorDialog::previousClosedSelected = false;
    AString BorderPropertiesEditorDialog::previousClassName = "None";
    CaretColorEnum::Enum BorderPropertiesEditorDialog::previousCaretColor;
#endif // __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

} // namespace
#endif  //__BORDER_PROPERTIES_EDITOR_DIALOG__H_
