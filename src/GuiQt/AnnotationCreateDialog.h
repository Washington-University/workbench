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

#include <memory>

#include "AnnotationCoordinateInformation.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationTypeEnum.h"
#include "UserInputModeEnum.h"
#include "Vector3D.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;

namespace caret {
    class Annotation;
    class AnnotationFile;
    class BrowserTabContent;
    class GiftiMetaData;
    class MouseEvent;
    class Plane;
    class SelectionItemVoxel;
    class WuQTextEditWithToolBarWidget;

    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* newAnnotationFromSpaceAndType(const UserInputModeEnum::Enum userInputMode,
                                                         const MouseEvent& mouseEvent,
                                                         const SelectionItemVoxel* selectionItemVoxel,
                                                         const std::vector<Vector3D>& drawingCoordinates,
                                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                         const AnnotationTypeEnum::Enum annotationType,
                                                         AnnotationFile* annotationFile);
        
        static Annotation* newAnnotationFromSpaceTypeAndBounds(const UserInputModeEnum::Enum userInputMode,
                                                               const MouseEvent& mouseEvent,
                                                               const SelectionItemVoxel* selectionItemVoxel,
                                                               const std::vector<Vector3D>& drawingCoordinates,
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
        
        class NewAnnotationInfo {
        public:
            NewAnnotationInfo(const MouseEvent& mouseEvent,
                              const SelectionItemVoxel* selectionItemVoxel,
                              const std::vector<Vector3D>& drawingCoordinates,
                              const AnnotationCoordinateSpaceEnum::Enum selectedSpace,
                              const AnnotationTypeEnum::Enum annotationType,
                              const bool useBothCoordinatesFromMouseFlag,
                              AnnotationFile* annotationFile);
            
            bool isValid() const;
            
            bool isSelectedSpaceValid() const;
            
            void processTwoCoordInfo();
            
            bool adjustViewportForSurfaceMontage(BrowserTabContent* browserTabContent,
                                                 const int viewport[4],
                                                 float& widthOut,
                                                 float& heightOut);
            
            const MouseEvent& m_mouseEvent;
            
            const SelectionItemVoxel* m_selectionItemVoxel;
            
            AnnotationCoordinateSpaceEnum::Enum m_selectedSpace;
            
            const AnnotationTypeEnum::Enum m_annotationType;
            
            AnnotationFile* m_annotationFile;
            
            std::vector<AnnotationCoordinateSpaceEnum::Enum> m_validSpaces;
            
            AnnotationCoordinateInformation m_coordOneInfo;
            
            AnnotationCoordinateInformation m_coordTwoInfo;
            
            std::vector<std::unique_ptr<AnnotationCoordinateInformation>> m_coordMultiInfo;
            
            bool m_coordTwoInfoValid;
            
            float m_percentageWidth;
            
            float m_percentageHeight;
            
        };
        
        static Annotation* newAnnotationFromSpaceTypeAndCoords(const UserInputModeEnum::Enum userInputMode,
                                                               const Mode mode,
                                                               const MouseEvent& mouseEvent,
                                                               const SelectionItemVoxel* selectionItemVoxel,
                                                               const std::vector<Vector3D>& drawingCoordinates,
                                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                               const AnnotationTypeEnum::Enum annotationType,
                                                               AnnotationFile* annotationFile);

        AnnotationCreateDialog(const UserInputModeEnum::Enum userInputMode,
                               const Mode mode,
                               NewAnnotationInfo& newAnnotationInfo,
                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                               const bool annotationSpaceValidFlag,
                               QWidget* parent = 0);

        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createTextWidget();
        
        QWidget* createImageWidget();
        
        QWidget* createMetaDataEditorWidget();
        
        void invalidateImage();
        
        static Annotation* createAnnotation(const UserInputModeEnum::Enum userInputMode,
                                            NewAnnotationInfo& newAnnotationInfo,
                                            const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                            AString& errorMessageOut);
        
        static void finishAnnotationCreation(const UserInputModeEnum::Enum userInputMode,
                                             const int32_t browswerWindowIndex,
                                             const int32_t tabIndex,
                                             AnnotationFile* annotationFile,
                                             Annotation* annotation);
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const Mode m_mode;
        
        NewAnnotationInfo& m_newAnnotationInfo;
        
        const AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
        
        Annotation* m_annotationThatWasCreated = NULL;
        
        QButtonGroup* m_annotationSpaceButtonGroup = NULL;
        
        WuQTextEditWithToolBarWidget* m_textEdit = NULL;
        
        QLabel* m_imageFileNameLabel = NULL;
        
        QLabel* m_imageThumbnailLabel = NULL;
        
        std::vector<uint8_t> m_imageRgbaBytes;
        int32_t m_imageWidth;
        int32_t m_imageHeight;
                
        static const int s_MAXIMUM_THUMB_NAIL_SIZE;
                
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CREATE_DIALOG_DECLARE__
    const int AnnotationCreateDialog::s_MAXIMUM_THUMB_NAIL_SIZE = 128;
#endif // __ANNOTATION_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_CREATE_DIALOG_H__
