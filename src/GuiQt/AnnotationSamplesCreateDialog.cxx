
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

#define __ANNOTATION_SAMPLES_CREATE_DIALOG_DECLARE__
#include "AnnotationSamplesCreateDialog.h"
#undef __ANNOTATION_SAMPLES_CREATE_DIALOG_DECLARE__

#include <cmath>

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationMetaData.h"
#include "AnnotationMetaDataNames.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MetaDataCustomEditorWidget.h"
#include "SamplesFile.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSamplesCreateDialog
 * \brief Dialog used for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Dialog constructor.
 * This dialog will add the annotation to the file if OK is clicked or destroy the annotation if
 * Cancel is clicked.
 *
 * @param annotationFile
 *     The annotation file.
 * @param annotation
 *     The new annotation.
 * @param viewportHeight
 *      Height of viewport
 * @param volumeSliceThickness
 *     Thickness of volume slice
 * @param browserWindowIndex
 *     Index of browser window
 * @param browserTabIndex
 *     Index of tab
 * @param parent
 *     Optional parent for this dialog.
 */
AnnotationSamplesCreateDialog::AnnotationSamplesCreateDialog(const UserInputModeEnum::Enum userInputMode,
                                                     const int32_t browserWindowIndex,
                                                     const int32_t browserTabIndex,
                                                     AnnotationFile* annotationFile,
                                                     Annotation* annotation,
                                                     const int32_t viewportHeight,
                                                     const float volumeSliceThickness,
                                                     QWidget* parent)
: WuQDialogModal("New Sample",
                 parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex),
m_browserTabIndex(browserTabIndex),
m_annotationFile(annotationFile),
m_annotation(annotation),
m_viewportHeight(viewportHeight),
m_volumeSliceThickness(volumeSliceThickness)
{
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    m_annotationType = annotation->getType();
    
    initializeMetaData(annotationFile);
    
    QWidget* metaDataWidget = ((m_annotationType == AnnotationTypeEnum::POLYHEDRON)
                               ? createMetaDataEditorWidget()
                               : NULL);

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    
    if (metaDataWidget != NULL) {
        layout->addWidget(metaDataWidget, 0);
    }
    
    setSizePolicy(dialogWidget->sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);

    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
AnnotationSamplesCreateDialog::~AnnotationSamplesCreateDialog()
{
}

/**
 * @return Annotation that was created by dialog (NULL if annotation NOT created).
 */
Annotation*
AnnotationSamplesCreateDialog::getAnnotationThatWasCreated()
{
    return m_annotation;
}

/**
 * @return A metadata editor widget for polyhedrons
 */
QWidget*
AnnotationSamplesCreateDialog::createMetaDataEditorWidget()
{
    std::vector<AString> metaDataNames;
    m_annotation->getMetaData()->getMetaDataNamesForEditor(metaDataNames,
                                                           m_requiredMetaDataNames);
    m_annotationMetaData.reset(new AnnotationMetaData(m_annotationType));
    for (const auto& name : metaDataNames) {
        const auto iter(s_previousMetaDataNamesAndValues.find(name));
        AString value;
        if (iter != s_previousMetaDataNamesAndValues.end()) {
            value = iter->second;
        }
        if (name == AnnotationMetaDataNames::SAMPLES_ENTRY_DATE) {
            /*
             * Default to an today's date
             */
            value = QDate::currentDate().toString(AnnotationMetaDataNames::SAMPLES_QT_DATE_FORMAT);
        }
        
        /*
         * These items are NOT restored from previous dialog
         */
        if ((name == AnnotationMetaDataNames::SAMPLES_COMMENT)
            || (name == AnnotationMetaDataNames::SAMPLES_SAMPLE_TYPE)
            || (name == AnnotationMetaDataNames::SAMPLES_SAMPLE_NUMBER)
            || (name == AnnotationMetaDataNames::SAMPLES_DING_ABBREVIATION)
            || (name == AnnotationMetaDataNames::SAMPLES_DING_FULL_NAME)
            || (name == AnnotationMetaDataNames::SAMPLES_ALT_SHORTHAND_ID)) {
            value = "";
        }
        
        if ( ! m_annotationMetaData->isCompositeMetaDataName(name)) {
            m_annotationMetaData->set(name, value);
        }
    }

    m_metaDataEditorWidget = new MetaDataCustomEditorWidget(MetaDataCustomEditorWidget::Mode::NEW_SAMPLE_EDITING,
                                                            metaDataNames,
                                                            m_requiredMetaDataNames,
                                                            m_annotationMetaData.get());

    m_metaDataRequiredCheckBox = new QCheckBox("Require Metadata (* indicates required metadata elements)");
    m_metaDataRequiredCheckBox->setChecked(s_previousMetaDataRequiredCheckedStatus);
        
    QGroupBox* groupBox(new QGroupBox("Metadata"));
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->addWidget(m_metaDataEditorWidget);
    groupLayout->addWidget(m_metaDataRequiredCheckBox, 0, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * Initialize the metadata
 * @param toolbarSamplesFile
 *    Samples file seledted in toolbar
 */
void
AnnotationSamplesCreateDialog::initializeMetaData(AnnotationFile* toolbarSamplesFile)
{
    /*
     * The default values for a newly drawn sample default to the FIRST OF:
     *   - The most recently drawn sample since wb_view was started
     *   - The last sample in the selected sample file in the Edit Samples Toolbar
     *   - The last sample in the most recently loaded sample file (this will likely
     *     never happen since there will always be a file in the Edit Samples Toolbar).
     *   - Empty
     */
    
    /*
     * Values have been initialized
     */
    if ( ! s_previousMetaDataNamesAndValues.empty()) {
        return;
    }
    
    /*
     * Default to file in toolbar but it may be NULL
     */
    AnnotationFile* samplesFile(toolbarSamplesFile);
    
    if (samplesFile == NULL) {
        /*
         * Find the last (most recent) samples file
         */
        Brain* brain(GuiManager::get()->getBrain());
        std::vector<SamplesFile*> loadedFiles(brain->getAllSamplesFiles());
        const int32_t numLoadedFiles(loadedFiles.size());
        if (numLoadedFiles > 0) {
            CaretAssertVectorIndex(loadedFiles, numLoadedFiles - 1);
            samplesFile = loadedFiles[numLoadedFiles - 1];
        }
    }

    if (samplesFile != NULL) {
        /*
         * Find last annotation in the samples file
         */
        std::vector<Annotation*> allAnnotations;
        samplesFile->getAllAnnotations(allAnnotations);
        const int32_t numAnn(allAnnotations.size());
        if (numAnn > 0) {
            CaretAssertVectorIndex(allAnnotations, numAnn - 1);
            const Annotation* ann(allAnnotations[numAnn - 1]);
            CaretAssert(ann);
            
            std::vector<AString> metadataNames;
            std::vector<AString> requiredMetaDataNames;
            m_annotation->getMetaData()->getMetaDataNamesForEditor(metadataNames,
                                                                   requiredMetaDataNames);

            /*
             * Load metadata from annotation to the 'previous' metadata
             * used to initialize the new annotation's metadata
             */
            const GiftiMetaData* metadata(ann->getMetaData());
            for (const AString& name : metadataNames) {
                s_previousMetaDataNamesAndValues.insert(std::make_pair(name,
                                                                       metadata->get(name)));
            }
        }
    }
}


/**
 * Gets called when the OK button is clicked.
 */
void
AnnotationSamplesCreateDialog::okButtonClicked()
{
    AString errorMessage;
    
    QString userText;
    CaretAssert(m_annotation);

    if (m_metaDataEditorWidget != NULL) {
        CaretAssert(m_metaDataRequiredCheckBox);
        s_previousMetaDataRequiredCheckedStatus = m_metaDataRequiredCheckBox->isChecked();
        if (m_metaDataRequiredCheckBox->isChecked()) {
            AString message;
            if ( ! m_metaDataEditorWidget->validateAndSaveRequiredMetaData(m_requiredMetaDataNames,
                                                                           message)) {
                message.appendWithNewLine("\nUncheck \""
                                          + m_metaDataRequiredCheckBox->text()
                                          + "\" to finish metadata entry later");
                errorMessage.appendWithNewLine(message);
            }
        }
        else {
            m_metaDataEditorWidget->saveMetaData();
        }
    }
    
    if (m_metaDataEditorWidget != NULL) {
        s_previousMetaDataNamesAndValues = m_annotationMetaData->getAsMap();
    }
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    CaretAssert(m_annotationFile);
    
    m_annotation->setDrawingNewAnnotationStatus(false);
    
    if (m_metaDataEditorWidget != NULL) {
        GiftiMetaData* annMetaData(m_annotation->getMetaData());
        CaretAssert(annMetaData);
        CaretAssert(m_annotationMetaData);
        annMetaData->replace(*m_annotationMetaData.get());
    }
    
    AnnotationPolyhedron* polyhedron(m_annotation->castToPolyhedron());
    if (polyhedron != NULL) {
        polyhedron->resetPlaneOneTwoNameStereotaxicXYZ();
    }
    
    finishAnnotationCreation(m_userInputMode,
                             m_annotationFile,
                             m_annotation,
                             m_browserWindowIndex,
                             m_browserTabIndex);
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->updateForNewAnnotation(m_annotation);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialogModal::okButtonClicked();
}

/**
 * Called if user clicks the cancel button
 */
void
AnnotationSamplesCreateDialog::cancelButtonClicked()
{
    const AString msg("Cancel finishing and return to drawing?");
    if ( ! WuQMessageBox::warningYesNo(this, msg)) {
        /*
         * Did NOT cancel
         */
        return;
    }
    
    delete m_annotation;
    m_annotation = NULL;
    
    WuQDialogModal::cancelButtonClicked();
}

/**
 * Finish the creation of an annotation.
 *
 * @param annotationFile
 *     File to which annotation is added.
 * @param annotation
 *     Annotation that was created.
 * @param browserWindowIndex
 *     Index of window in which annotation was created.
 * @param tabIndex
 *     Index of tab in which annotation was created.
 */
void
AnnotationSamplesCreateDialog::finishAnnotationCreation(const UserInputModeEnum::Enum userInputMode,
                                                    AnnotationFile* annotationFile,
                                                    Annotation* annotation,
                                                    const int32_t browswerWindowIndex,
                                                    const int32_t tabIndex)
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(userInputMode);
    
    CaretAssert(annotation);
    annotation->setDrawingNewAnnotationStatus(false);
    
    /*
     * Add annotation to its file
     */
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeCreateAnnotation(annotationFile,
                                         annotation);
    
    AString errorMessage;
    if ( ! annotationManager->applyCommand(undoCommand,
                                           errorMessage)) {
        WuQMessageBox::errorOk(GuiManager::get()->getBrowserWindowByWindowIndex(browswerWindowIndex),
                               errorMessage);
    }

    
    annotationManager->selectAnnotationForEditing(browswerWindowIndex,
                                        AnnotationManager::SELECTION_MODE_SINGLE,
                                        false,
                                        annotation);
    
    /*
     * A new chart annotation is displayed only in the tab in which it was created
     */
    if (annotation->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::CHART) {
        annotation->setItemDisplaySelectedInOneTab(tabIndex);
        annotation->setItemDisplaySelected(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                           tabIndex,
                                           TriStateSelectionStatusEnum::SELECTED);
    }
}

