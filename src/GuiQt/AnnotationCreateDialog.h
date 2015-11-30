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
class QTextEdit;
class QRadioButton;

namespace caret {
    class Annotation;
    class AnnotationCoordinateSelectionWidget;
    class AnnotationFile;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class MouseEvent;
    
    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static AnnotationCreateDialog* newAnnotation(const MouseEvent& mouseEvent,
                                                     const Annotation* annotation,
                                                     QWidget* parent = 0);
        
        static AnnotationCreateDialog* newAnnotationType(const MouseEvent& mouseEvent,
                                                         const AnnotationTypeEnum::Enum annotationType,
                                                         QWidget* parent = 0);
        
        static AnnotationCreateDialog* newAnnotationTypeWithBounds(const MouseEvent& mousePressEvent,
                                                                   const AnnotationTypeEnum::Enum annotationType,
                                                                   QWidget* parent = 0);
        
        static AnnotationCreateDialog* newPasteAnnotation(const MouseEvent& mouseEvent,
                                                          const AnnotationFile* annotationFile,
                                                          const Annotation* annotation,
                                                          QWidget* parent = 0);
        
        virtual ~AnnotationCreateDialog();

        virtual void okButtonClicked();
        
        Annotation* getAnnotationThatWasCreated();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void newAnnotationFileButtonClicked();
        
    private:
        /** Dialogs mode */
        enum Mode {
            /** Adding a new annotation */
            MODE_ADD_NEW_ANNOTATION,
            /** New annotation from annotation type at mouse click */
            MODE_NEW_ANNOTATION_TYPE_CLICK,
            /** New annotation from annotation type at mouse press and release */
            MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE,
            /** Paste an annotation */
            MODE_PASTE_ANNOTATION
        };
        
        class PreviousSelections {
        public:
            PreviousSelections() {
                m_valid = false;
            }
            AnnotationFile* m_annotationFile;
            AnnotationCoordinateSpaceEnum::Enum m_coordinateSpace;
            bool m_valid;
        };
        
        AnnotationCreateDialog(const Mode mode,
                               const MouseEvent& mouseEvent,
                               const AnnotationFile* annotationFile,
                               const Annotation* annotation,
                               const AnnotationTypeEnum::Enum annotationType,
                               QWidget* parent = 0);
        
        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createFileSelectionWidget();
        
        QWidget* createTextWidget();
        
        const Mode m_mode;
        
        const MouseEvent& m_mouseEvent;
        
        const AnnotationFile* m_annotationToPastesFile;
        
        const Annotation* m_annotationToPaste;
        
        const AnnotationTypeEnum::Enum m_annotationType;
        
        Annotation* m_annotationThatWasCreated;
        
        CaretDataFileSelectionModel* m_annotationFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_annotationFileSelectionComboBox;
        
        QRadioButton* m_sceneAnnotationFileRadioButton;
        
        QRadioButton* m_brainAnnotationFileRadioButton;
        
        QWidget* m_fileSelectionWidget;
        
        AnnotationCoordinateSelectionWidget* m_coordinateSelectionWidget;
        
        QTextEdit* m_textEdit;
        
        UserInputModeAnnotations::CoordinateInformation m_coordInfo;
        
        UserInputModeAnnotations::CoordinateInformation m_coordTwoInfo;
        
        static PreviousSelections s_previousSelections;
        
        static const QString s_SPACE_PROPERTY_NAME;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CREATE_DIALOG_DECLARE__
    AnnotationCreateDialog::PreviousSelections AnnotationCreateDialog::s_previousSelections;
    const QString AnnotationCreateDialog::s_SPACE_PROPERTY_NAME = "SPACE_NAME";
#endif // __ANNOTATION_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_CREATE_DIALOG_H__
