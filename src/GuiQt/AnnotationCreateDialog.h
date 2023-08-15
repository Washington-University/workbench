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
#include "Vector3D.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QDoubleSpinBox;
class QSpinBox;
class QTextEdit;

namespace caret {
    class Annotation;
    class AnnotationFile;
    class BrowserTabContent;
    class GiftiMetaData;
    class MetaDataEditorWidget;
    class MouseEvent;
    class Plane;
    class SelectionItemVoxel;

    class AnnotationCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* newAnnotationFromSpaceAndType(const MouseEvent& mouseEvent,
                                                         const SelectionItemVoxel* selectionItemVoxel,
                                                         const std::vector<Vector3D>& drawingCoordinates,
                                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                         const AnnotationTypeEnum::Enum annotationType,
                                                         AnnotationFile* annotationFile);
        
        static Annotation* newAnnotationFromSpaceTypeAndBounds(const MouseEvent& mouseEvent,
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
        
        void polyhedronDepthIndexSpinBoxValueChanged(double value);
        
        void polyhedronDepthMillimetersSpinBoxValueChanged(double value);
        
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
        
        static Annotation* newAnnotationFromSpaceTypeAndCoords(const Mode mode,
                                                               const MouseEvent& mouseEvent,
                                                               const SelectionItemVoxel* selectionItemVoxel,
                                                               const std::vector<Vector3D>& drawingCoordinates,
                                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                               const AnnotationTypeEnum::Enum annotationType,
                                                               AnnotationFile* annotationFile);

        AnnotationCreateDialog(const Mode mode,
                               NewAnnotationInfo& newAnnotationInfo,
                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                               const bool annotationSpaceValidFlag,
                               QWidget* parent = 0);

        AnnotationCreateDialog(const AnnotationCreateDialog&);

        AnnotationCreateDialog& operator=(const AnnotationCreateDialog&);
        
        QWidget* createTextWidget();
        
        QWidget* createImageWidget();
        
        QWidget* createPolyhedronWidget();
        
        MetaDataEditorWidget* createMetaDataWidget();
        
        void invalidateImage();
        
        static Annotation* createAnnotation(NewAnnotationInfo& newAnnotationInfo,
                                            const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                            const Plane& polyedronPlane,
                                            const float polyhedronDepthMM,
                                            AString& errorMessageOut);
        
        static void finishAnnotationCreation(AnnotationFile* annotationFile,
                                             Annotation* annotation,
                                             const int32_t browswerWindowIndex,
                                             const int32_t tabIndex);
        
        const Mode m_mode;
        
        NewAnnotationInfo& m_newAnnotationInfo;
        
        const AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
        
        Annotation* m_annotationThatWasCreated = NULL;
        
        QButtonGroup* m_annotationSpaceButtonGroup = NULL;
        
        QTextEdit* m_textEdit = NULL;
        
        QLabel* m_imageFileNameLabel = NULL;
        
        QLabel* m_imageThumbnailLabel = NULL;
        
        std::vector<uint8_t> m_imageRgbaBytes;
        int32_t m_imageWidth;
        int32_t m_imageHeight;
        
        QDoubleSpinBox* m_polyhedronSliceIndexDepthSpinBox = NULL;
        
        QDoubleSpinBox* m_polyhedronSliceMillimetersDepthSpinBox = NULL;
        
        MetaDataEditorWidget* m_metaEditorDataWidget = NULL;
        
        static std::unique_ptr<GiftiMetaData> s_annotationMetaData;
        
        static const int s_MAXIMUM_THUMB_NAIL_SIZE;
        
        static float s_previousPolyhedronDepthValue;
        
        static bool s_previousPolyhedronDepthValueValidFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CREATE_DIALOG_DECLARE__
    const int AnnotationCreateDialog::s_MAXIMUM_THUMB_NAIL_SIZE = 128;
    float AnnotationCreateDialog::s_previousPolyhedronDepthValue = 5;
    bool AnnotationCreateDialog::s_previousPolyhedronDepthValueValidFlag = false;
    std::unique_ptr<GiftiMetaData> AnnotationCreateDialog::s_annotationMetaData;
#endif // __ANNOTATION_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_CREATE_DIALOG_H__
