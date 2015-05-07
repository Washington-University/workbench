#ifndef __ANNOTATION_CREATE_DIALOG_H__
#define __ANNOTATION_CREATE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationTypeEnum.h"
#include "UserInputModeAnnotations.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QLineEdit;
class QRadioButton;

namespace caret {
    class Annotation;
    class AnnotationFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class MouseEvent;
    
    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        AnnotationCreateDialog(const MouseEvent& mouseEvent,
                               const AnnotationTypeEnum::Enum annotationType,
                               QWidget* parent = 0);
        
        virtual ~AnnotationCreateDialog();

        virtual void okButtonClicked();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void newAnnotationFileButtonClicked();
        
    private:
        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createFileSelectionWidget();
        
        QWidget* createSpaceSelectionWidget();
        
        QWidget* createTextWidget();
        
        QRadioButton* createRadioButtonForSpace(const AnnotationCoordinateSpaceEnum::Enum space);
        
        const AnnotationTypeEnum::Enum m_annotationType;
        
        CaretDataFileSelectionModel* m_annotationFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_annotationFileSelectionComboBox;
        
        QWidget* m_fileSelectionWidget;
        
        QButtonGroup* m_spaceButtonGroup;
        
        QLineEdit* m_textLineEdit;
        
        UserInputModeAnnotations::CoordinateInformation m_coordInfo;
        
        static AnnotationFile* s_previousAnnotationFile;
        
        static const QString s_SPACE_PROPERTY_NAME;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CREATE_DIALOG_DECLARE__
    AnnotationFile* AnnotationCreateDialog::s_previousAnnotationFile = NULL;
    const QString AnnotationCreateDialog::s_SPACE_PROPERTY_NAME = "SPACE_NAME";
#endif // __ANNOTATION_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_CREATE_DIALOG_H__
