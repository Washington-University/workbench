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
#include "WuQDialogModal.h"

class QTextEdit;
class QRadioButton;

namespace caret {
    class Annotation;
    class AnnotationCoordinateSelectionWidget;
    class AnnotationFile;
    class MouseEvent;
    
    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* newAnnotationFromSpaceAndType(const MouseEvent& mouseEvent,
                                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                         const AnnotationTypeEnum::Enum annotationType,
                                                         AnnotationFile* annotationFile);
        
        static Annotation* newAnnotationFromSpaceTypeAndBounds(const MouseEvent& mouseEvent,
                                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                               const AnnotationTypeEnum::Enum annotationType,
                                                               AnnotationFile* annotationFile);
        
        virtual ~AnnotationCreateDialog();

        virtual void okButtonClicked();
        
        Annotation* getAnnotationThatWasCreated();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void selectImageButtonClicked();
        
    private:
        /** Dialogs mode */
        enum Mode {
            /** New annotation from annotation type at mouse click */
            MODE_NEW_ANNOTATION_TYPE_CLICK,
            /** New annotation from annotation type at mouse press and release */
            MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS
        };
        
        static Annotation* newAnnotationFromSpaceTypeAndCoords(const MouseEvent& mouseEvent,
                                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                               const AnnotationTypeEnum::Enum annotationType,
                                                               const AnnotationCoordinateInformation* coordOne,
                                                               const AnnotationCoordinateInformation* coordTwo,
                                                               AnnotationFile* annotationFile);
        
        AnnotationCreateDialog(const Mode mode,
                               const MouseEvent& mouseEvent,
                               AnnotationFile* annotationFile,
                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                               const bool annotationSpaceValidFlag,
                               const AnnotationTypeEnum::Enum annotationType,
                               QWidget* parent = 0);
        
        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createTextWidget();
        
        QWidget* createImageWidget();
        
        void invalidateImage();
        
        static void finishAnnotationCreation(AnnotationFile* annotationFile,
                                             Annotation* annotation,
                                             const int32_t browswerWindowIndex);
        
        const Mode m_mode;
        
        const MouseEvent& m_mouseEvent;
        
        AnnotationFile* m_annotationFile;
        
        const AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
        
        const AnnotationTypeEnum::Enum m_annotationType;
        
        Annotation* m_annotationThatWasCreated;
        
        float m_annotationFromBoundsWidth;
        
        float m_annotationFromBoundsHeight;
        
        AnnotationCoordinateSelectionWidget* m_coordinateSelectionWidget;
        
        QTextEdit* m_textEdit;
        
        QLabel* m_imageFileNameLabel;
        
        QLabel* m_imageThumbnailLabel;
        
        std::vector<uint8_t> m_imageRgbaBytes;
        int32_t m_imageWidth;
        int32_t m_imageHeight;
        
        AnnotationCoordinateInformation m_coordInfo;
        
        AnnotationCoordinateInformation m_coordTwoInfo;
        
        static const int s_MAXIMUM_THUMB_NAIL_SIZE;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CREATE_DIALOG_DECLARE__
    const int AnnotationCreateDialog::s_MAXIMUM_THUMB_NAIL_SIZE = 300;
#endif // __ANNOTATION_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_CREATE_DIALOG_H__
