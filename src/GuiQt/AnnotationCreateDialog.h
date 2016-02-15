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

#include "AnnotationCoordinateInformation.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationTypeEnum.h"
#include "UserInputModeAnnotations.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QLabel;
class QTextEdit;
class QRadioButton;

namespace caret {
    class Annotation;
    class AnnotationCoordinateSelectionWidget;
    class AnnotationFile;
    class AnnotationTwoDimensionalShape;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class MouseEvent;
    
    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* newAnnotationFromSpaceAndType(const MouseEvent& mouseEvent,
                                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                         const AnnotationTypeEnum::Enum annotationType);
        
        static AnnotationCreateDialog* newAnnotationSpaceAndType(const MouseEvent& mouseEvent,
                                                                 const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                                 const AnnotationTypeEnum::Enum annotationType,
                                                                 QWidget* parent = 0);
        
        static AnnotationCreateDialog* newAnnotationSpaceAndTypeWithBounds(const MouseEvent& mousePressEvent,
                                                                           const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                                           const AnnotationTypeEnum::Enum annotationType,
                                                                           QWidget* parent = 0);
        
        virtual ~AnnotationCreateDialog();

        virtual void okButtonClicked();
        
        Annotation* getAnnotationThatWasCreated();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void newAnnotationFileButtonClicked();
        
        void selectImageButtonClicked();
        
    private:
        /** Dialogs mode */
        enum Mode {
            /** New annotation from annotation type at mouse click */
            MODE_NEW_ANNOTATION_TYPE_CLICK,
            /** New annotation from annotation type at mouse press and release */
            MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS
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
                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                               const AnnotationTypeEnum::Enum annotationType,
                               QWidget* parent = 0);
        
        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createFileSelectionWidget();
        
        QWidget* createTextWidget();
        
        QWidget* createImageWidget();
        
        void invalidateImage();
        
//        void setAnnotationFromBoundsWidthAndHeight(Annotation* annotation);
        
        const Mode m_mode;
        
        const MouseEvent& m_mouseEvent;
        
        const AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
        
        const AnnotationTypeEnum::Enum m_annotationType;
        
        Annotation* m_annotationThatWasCreated;
        
        float m_annotationFromBoundsWidth;
        
        float m_annotationFromBoundsHeight;
        
        CaretDataFileSelectionModel* m_annotationFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_annotationFileSelectionComboBox;
        
        QRadioButton* m_sceneAnnotationFileRadioButton;
        
        QRadioButton* m_brainAnnotationFileRadioButton;
        
        QWidget* m_fileSelectionWidget;
        
        AnnotationCoordinateSelectionWidget* m_coordinateSelectionWidget;
        
        QTextEdit* m_textEdit;
        
        QLabel* m_imageFileNameLabel;
        
        std::vector<uint8_t> m_imageRgbaBytes;
        int32_t m_imageWidth;
        int32_t m_imageHeight;
        
        AnnotationCoordinateInformation m_coordInfo;
        
        AnnotationCoordinateInformation m_coordTwoInfo;
        
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
