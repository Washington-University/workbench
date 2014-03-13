#ifndef __BORDER_PROPERTIES_EDITOR_DIALOG__H_
#define __BORDER_PROPERTIES_EDITOR_DIALOG__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QStringListModel;

namespace caret {

    class Border;
    class BorderFile;
    class GiftiLabelTableSelectionComboBox;
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
        
        void loadNameComboBox(const QString& name = "");
        
        void loadClassComboBox(const QString& className = "");
        
        Mode m_mode;
        
        BorderFile* m_editModeBorderFile;
        Border* m_border;
        
        QComboBox* m_borderFileSelectionComboBox;
        
        GiftiLabelTableSelectionComboBox* m_nameComboBox;
        
        QStringList m_nameCompleterStringList;
        QStringListModel* m_nameCompleterStringListModel;
        
        QCheckBox* m_closedCheckBox;
        
        GiftiLabelTableSelectionComboBox* m_classComboBox;

        QCheckBox* m_reversePointOrderCheckBox;
        
        SurfaceFile* m_finishBorderSurfaceFile;
        
        static bool s_previousDataValid;
        static AString s_previousName;
        static BorderFile* s_previousBorderFile;
        static bool s_previousClosedSelected;
        static AString s_previousClassName;
    };
    
#ifdef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
    bool BorderPropertiesEditorDialog::s_previousDataValid = false;
    AString BorderPropertiesEditorDialog::s_previousName = "Name";
    BorderFile* BorderPropertiesEditorDialog::s_previousBorderFile = NULL;
    bool BorderPropertiesEditorDialog::s_previousClosedSelected = false;
    AString BorderPropertiesEditorDialog::s_previousClassName = "None";
#endif // __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

} // namespace
#endif  //__BORDER_PROPERTIES_EDITOR_DIALOG__H_
