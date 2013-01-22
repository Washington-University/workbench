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
class QStringListModel;

namespace caret {

    class Border;
    class BorderFile;
    class CaretColorEnumComboBox;
    class SurfaceFile;
    
    class BorderPropertiesEditorDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        static BorderPropertiesEditorDialog*
            newInstanceFinishBorder(Border* border,
                                    SurfaceFile* surfaceFile,
                                    QWidget* parent = 0);
        
        static BorderPropertiesEditorDialog*
            newInstanceEditBorder(BorderFile* editModeBorderFile,
                                  Border* border,
                                  QWidget* parent = 0);
        
        virtual ~BorderPropertiesEditorDialog();
        
    protected:
        virtual void okButtonClicked();
        
    private slots:
        void displayNameEditor();
        
        void displayClassEditor();
    
        void borderFileSelected();
        
        void newBorderFileButtonClicked();
        
    private:
        enum Mode {
            MODE_EDIT,
            MODE_FINISH_DRAWING
        };
        
        BorderPropertiesEditorDialog(const QString& title,
                                     SurfaceFile* finishBorderSurfaceFile,
                                     const Mode mode,
                                     BorderFile* editModeBorderFile,
                                     Border* border,
                                     QWidget* parent = 0);
        
        BorderPropertiesEditorDialog(const BorderPropertiesEditorDialog&);

        BorderPropertiesEditorDialog& operator=(const BorderPropertiesEditorDialog&);
        
        BorderFile* getSelectedBorderFile();
        
        void loadBorderFileComboBox();
        
        void loadClassNameComboBox(const QString& className = "");
        
        Mode mode;
        
        BorderFile* editModeBorderFile;
        Border* border;
        
        QComboBox* borderFileSelectionComboBox;
        
        QLineEdit* nameLineEdit;
        
        QStringList m_nameCompleterStringList;
        QStringListModel* m_nameCompleterStringListModel;
        
        QCheckBox* closedCheckBox;
        
        QComboBox* classNameComboBox;

        QCheckBox* reversePointOrderCheckBox;
        
        CaretColorEnumComboBox* colorSelectionComboBox;
        
        SurfaceFile* finishBorderSurfaceFile;
        
        static bool s_previousDataValid;
        static AString previousName;
        static BorderFile* previousBorderFile;
        static bool previousClosedSelected;
        static AString previousClassName;     
        static CaretColorEnum::Enum previousCaretColor;
    };
    
#ifdef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
    bool BorderPropertiesEditorDialog::s_previousDataValid = false;
    AString BorderPropertiesEditorDialog::previousName = "Name";
    BorderFile* BorderPropertiesEditorDialog::previousBorderFile = NULL;
    bool BorderPropertiesEditorDialog::previousClosedSelected = false;
    AString BorderPropertiesEditorDialog::previousClassName = "None";
    CaretColorEnum::Enum BorderPropertiesEditorDialog::previousCaretColor;
#endif // __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

} // namespace
#endif  //__BORDER_PROPERTIES_EDITOR_DIALOG__H_
