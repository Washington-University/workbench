#ifndef __ANNOTATION_SAMPLES_CREATE_DIALOG_H__
#define __ANNOTATION_SAMPLES_CREATE_DIALOG_H__

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
#include "WuQDialogModal.h"

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;
class QTextEdit;

namespace caret {
    class Annotation;
    class AnnotationFile;
    class DingOntologyTermsFile;
    class GiftiMetaData;
    class MetaDataCustomEditorWidget;
    class MouseEvent;
    class SamplesMetaDataManager;

    class AnnotationSamplesCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        AnnotationSamplesCreateDialog(const UserInputModeEnum::Enum userInputMode,
                                  const int32_t browserWindowIndex,
                                  const int32_t browserTabIndex,
                                  AnnotationFile* annotationFile,
                                  Annotation* annotation,
                                  const int32_t viewportHeight,
                                  const float volumeSliceThickness,
                                  QWidget* parent = 0);
        
        AnnotationSamplesCreateDialog(const AnnotationSamplesCreateDialog&) = delete;
        
        AnnotationSamplesCreateDialog& operator=(const AnnotationSamplesCreateDialog&) = delete;
                
        virtual ~AnnotationSamplesCreateDialog();

        virtual void okButtonClicked();
        
        Annotation* getAnnotationThatWasCreated();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void selectImageButtonClicked();
        
        void selectDingOntologyNameButtonClicked();
        
    private:
        QWidget* createTextWidget();
        
        QWidget* createImageWidget();
        
        QWidget* createMetaDataEditorWidget();
        
        void invalidateImage();
        
        static void finishAnnotationCreation(const UserInputModeEnum::Enum userInputMode,
                                             AnnotationFile* annotationFile,
                                             Annotation* annotation,
                                             const int32_t browswerWindowIndex,
                                             const int32_t tabIndex);
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_browserTabIndex;
        
        AnnotationFile* m_annotationFile;

        Annotation* m_annotation;
        
        const int32_t m_viewportHeight;
        
        const float m_volumeSliceThickness;
        
        AnnotationTypeEnum::Enum m_annotationType;
        
        QTextEdit* m_textEdit = NULL;
        
        QLabel* m_imageFileNameLabel = NULL;
        
        QLabel* m_imageThumbnailLabel = NULL;
        
        std::vector<uint8_t> m_imageRgbaBytes;
        int32_t m_imageWidth  = 0;
        int32_t m_imageHeight = 0;
        
        MetaDataCustomEditorWidget* m_metaDataEditorWidget = NULL;
        
        QCheckBox* m_metaDataRequiredCheckBox = NULL;
        
        std::unique_ptr<GiftiMetaData> m_annotationMetaData;
        
        std::vector<AString> m_requiredMetaDataNames;
        
        SamplesMetaDataManager* m_samplesMetaDataManager = NULL;

        const DingOntologyTermsFile* m_dingOntologyTermsFile = NULL;
        
        static const int s_MAXIMUM_THUMB_NAIL_SIZE;
        
        static bool s_previousMetaDataRequiredCheckedStatus;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SAMPLES_CREATE_DIALOG_DECLARE__
    const int AnnotationSamplesCreateDialog::s_MAXIMUM_THUMB_NAIL_SIZE = 128;
    bool AnnotationSamplesCreateDialog::s_previousMetaDataRequiredCheckedStatus = true;
#endif // __ANNOTATION_SAMPLES_CREATE_DIALOG_DECLARE__;

} // namespace
#endif  //__ANNOTATION_SAMPLES_CREATE_DIALOG_H__
