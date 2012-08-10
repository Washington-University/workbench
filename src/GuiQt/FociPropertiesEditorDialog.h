#ifndef __FOCI_PROPERTIES_EDITOR_DIALOG__H_
#define __FOCI_PROPERTIES_EDITOR_DIALOG__H_

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
class QTextEdit;

namespace caret {

    class Focus;
    class FociFile;
    class CaretColorEnumComboBox;
    
    class FociPropertiesEditorDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        FociPropertiesEditorDialog(const QString& title,
                                   FociFile* fociFile,
                                   Focus* focus,
                                   const bool allowFociFileSelection,
                                   QWidget* parent = 0);
        
        virtual ~FociPropertiesEditorDialog();
        
        FociFile* getSelectedFociFile();
        
        void loadFromFocusDataIntoDialog(const Focus* focus);

        void loadFromDialogIntoFocusData(Focus* focus) const;
        
    protected:
        virtual void okButtonClicked();
        
    private slots:
        void displayClassEditor();
    
        void fociFileSelected();
        
        void newFociFileButtonClicked();
        
    private:
        FociPropertiesEditorDialog(const FociPropertiesEditorDialog&);

        FociPropertiesEditorDialog& operator=(const FociPropertiesEditorDialog&);
        
        void loadFociFileComboBox(const FociFile* fociFile);
        
        void loadClassNameComboBox(const QString& className = "");
        
        Focus* m_focus;
        
        QComboBox* m_fociFileSelectionComboBox;
        
        QLineEdit* m_nameLineEdit;
        
        QComboBox* m_classNameComboBox;

        CaretColorEnumComboBox* m_colorSelectionComboBox;
        
        QDoubleSpinBox* m_xCoordSpinBox;
        QDoubleSpinBox* m_yCoordSpinBox;
        QDoubleSpinBox* m_zCoordSpinBox;
        
        QTextEdit* m_commentTextEdit;
        
        QLineEdit* m_areaLineEdit;
        
        QLineEdit* m_geographyLineEdit;
        
        QDoubleSpinBox* m_extentSpinBox;
        
        QLineEdit* m_regionOfInterestLineEdit;
        
        QLineEdit* m_statisticLineEdit;
    };
    
#ifdef __FOCI_PROPERTIES_EDITOR_DIALOG__DECLARE__
#endif // __FOCI_PROPERTIES_EDITOR_DIALOG__DECLARE__

} // namespace
#endif  //__FOCI_PROPERTIES_EDITOR_DIALOG__H_
