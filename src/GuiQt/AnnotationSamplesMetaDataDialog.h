#ifndef __ANNOTATION_SAMPLES_META_DATA_DIALOG_H__
#define __ANNOTATION_SAMPLES_META_DATA_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include "UserInputModeEnum.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QComboBox;
class QDateEdit;
class QGridLayout;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QTextEdit;

namespace caret {

    class Annotation;
    class AnnotationFile;
    class AnnotationPolyhedron;
    class AnnotationSampleMetaData;
    class GiftiMetaData;
    
    class AnnotationSamplesMetaDataDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static void deleteStaticMembers();
        
        AnnotationSamplesMetaDataDialog(AnnotationPolyhedron* polyhedron,
                                        QWidget* parent = 0);
        
        AnnotationSamplesMetaDataDialog(const UserInputModeEnum::Enum userInputMode,
                                        const int32_t browserWindowIndex,
                                        const int32_t browserTabIndex,
                                        AnnotationFile* annotationFile,
                                        AnnotationPolyhedron* polyhedron,
                                        const int32_t viewportHeight,
                                        const float volumeSliceThickness,
                                        QWidget* parent = 0);
        
        virtual ~AnnotationSamplesMetaDataDialog();
        
        AnnotationSamplesMetaDataDialog(const AnnotationSamplesMetaDataDialog&) = delete;

        AnnotationSamplesMetaDataDialog& operator=(const AnnotationSamplesMetaDataDialog&) = delete;
        
        virtual void okButtonClicked() override;
        
        virtual void cancelButtonClicked() override;
        
        Annotation* getAnnotationThatWasCreated();

        
        // ADD_NEW_METHODS_HERE

    private:
        enum class ChooseButtonEnum {
            ALTERNATE_PARCELLATION,
            BORDER,
            DING_ABBREVIATION,
            FOCUS,
            PRIMARY_PARCELLATION
        };
        
    private slots:
        void chooseButtonClicked(const ChooseButtonEnum chooseButton,
                                 const int32_t focusBorderIndex);
        
    private:
        enum class DialogMode {
            CREATE_NEW_SAMPLE,
            EDIT_EXISTING_SAMPLE
        };
        
        void createDialog();
        
        QWidget* createPrimaryTabWidget();
        
        QWidget* createBorderFociTabWidget();
        
        QWidget* createSpecimenPortalTabWidget();
        
        QWidget* createAdvancedTabWidget();
        
        void addWidget(const AString& labelText,
                       const AString& toolTip,
                       QGridLayout* gridLayout,
                       QWidget* widget);
        
        QLineEdit* addLineEdit(const AString& labelText,
                               const AString& toolTip,
                               QGridLayout* gridLayout);
        
        QLineEdit* addLineEdit(const AString& labelText,
                               const AString& toolTip,
                               const ChooseButtonEnum chooseButton,
                               QGridLayout* gridLayout);
        
        template <typename PointerToMemberFunction> QLineEdit* addLineEdit(const AString& labelText,
                               const AString& toolTip,
                               PointerToMemberFunction slot,
                               QGridLayout* gridLayout);

        QComboBox* addComboBox(const AString& labelText,
                               const AString& toolTip,
                               const std::vector<AString>& comboBoxTextSelections,
                               QGridLayout* gridLayout);
        
        QDateEdit* addDateEdit(const AString& labelText,
                               const AString& toolTip,
                               QGridLayout* gridLayout);
        
        QSpinBox* addSpinBox(const AString& labelText,
                             const AString& toolTip,
                             const int32_t minimumValue,
                             const int32_t maximumValue,
                             QGridLayout* gridLayout);
        
        AString getDateAsText(QDateEdit* dateEdit);
        
        void setComboBoxSelection(QComboBox* comboBox,
                                  const AString& text);
        
        void setDateEditSelection(QDateEdit* dateEdit,
                                  const AString& text);
        
        void loadMetaDataIntoDialog();
        
        void loadLocalSlabID();

        void loadLocalSampleID();
        
        void readMetaDataFromDialog();
        
        AString getLabelFileAndMap();
        
        bool finishCreatingNewSample();
        
        bool finishEditingExistingSample();
        
        void saveCreatingAnnotationMetaDataForNextTime();
        
        void createSampleNumber();
        
        const DialogMode m_dialogMode;
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_browserTabIndex;
        
        AnnotationFile* m_annotationFile = NULL;
        
        AnnotationPolyhedron* m_polyhedron = NULL;
        
        const int32_t m_viewportHeight;
        
        const float m_volumeSliceThickness;

        /**
         * POINTS to a metadata instance NEVER DESTROY it
         */
        AnnotationSampleMetaData* m_sampleMetaData = NULL;
        
        std::unique_ptr<GiftiMetaData> m_editModeMetaDataCopy;
        
        std::unique_ptr<AnnotationSampleMetaData> m_editModeSampleMetaDataCopy;
        
        QLineEdit* m_subjectNameLineEdit;
        
        QLineEdit* m_allenLocalNameLineEdit;
        
        QComboBox* m_hemisphereComboBox;
        
        QComboBox* m_allenTissueTypeComboBox;
        
        QLineEdit* m_allenSlabNumberLineEdit;
        
        QLineEdit* m_localSlabIdLineEdit;
        
        QComboBox* m_slabFaceComboBox;
        
        QComboBox* m_sampleTypeComboBox;
        
        QDateEdit* m_desiredSampleEntryDateEdit;
        
        QDateEdit* m_actualSampleEntryDateEdit;
        
        QLineEdit* m_hmbaParcelDingAbbreviationLineEdit;
        
        QLineEdit* m_hmbaParcelDingFullNameLineEdit;
        
        QLineEdit* m_sampleNameLineEdit;
        
        QLineEdit* m_alternativeSampleNameLineEdit;
        
        QLineEdit* m_sampleNumberLineEdit;
        
        QLineEdit* m_localSampleIdLineEdit;
        
        QLineEdit* m_primaryParcellationLineEdit;
        
        QLineEdit* m_alternativeParcellationLineEdit;
        
        QTextEdit* m_commentTextEdit;
        
        QCheckBox* m_requireMetaDataCheckBox;
        
        
        QLineEdit* m_bicanDonorIdLineEdit;
        
        QLineEdit* m_nhashSlabIdLineEdit;
        
        
        std::vector<QLineEdit*> m_borderFileNameLineEdits;
        
        std::vector<QLineEdit*> m_borderClassLineEdits;

        std::vector<QLineEdit*> m_borderIdLineEdits;
        
        std::vector<QLineEdit*> m_borderNameLineEdits;
        
        
        std::vector<QLineEdit*> m_focusFileNameLineEdits;
        
        std::vector<QLineEdit*> m_focusClassLineEdits;
        
        std::vector<QLineEdit*> m_focusIdLineEdits;
        
        std::vector<QLineEdit*> m_focusNameLineEdits;
        

        QSpinBox* m_firstLeftHemisphereAllenSlabNumberSpinBox;
        
        QSpinBox* m_firstRightHemisphereAllenSlabNumberSpinBox;
        
        QSpinBox* m_lastRightHemisphereAllenSlabNumberSpinBox;
        
        static std::unique_ptr<GiftiMetaData> s_previousCreateMetaData;
        
        /* Note This wraps a GiftiMetaData instance */
        static std::unique_ptr<AnnotationSampleMetaData> s_previousCreateSampleMetaData;
        
        static bool s_previousRequireMetaDataCheckedFlag;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SAMPLES_META_DATA_DIALOG_DECLARE__
    std::unique_ptr<GiftiMetaData> AnnotationSamplesMetaDataDialog::s_previousCreateMetaData;
    
    std::unique_ptr<AnnotationSampleMetaData> AnnotationSamplesMetaDataDialog::s_previousCreateSampleMetaData;
    
    bool AnnotationSamplesMetaDataDialog::s_previousRequireMetaDataCheckedFlag = true;
#endif // __ANNOTATION_SAMPLES_META_DATA_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_SAMPLES_META_DATA_DIALOG_H__
