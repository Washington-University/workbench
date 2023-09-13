
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

#define __ANNOTATION_CREATE_DIALOG_TWO_DECLARE__
#include "AnnotationCreateDialogTwo.h"
#undef __ANNOTATION_CREATE_DIALOG_TWO_DECLARE__

#include <cmath>

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationManager.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPolygon.h"
#include "AnnotationPolyLine.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLWidget.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "DataFileException.h"
#include "DisplayPropertiesAnnotation.h"
#include "EnumComboBoxTemplate.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiMetaDataXmlElements.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "MetaDataCustomEditorWidget.h"
#include "ModelVolume.h"
#include "ModelSurfaceMontage.h"
#include "MouseEvent.h"
#include "SelectionItemVoxel.h"
#include "WuQtUtilities.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCreateDialogTwo
 * \brief Dialog used for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Dialog constructor.
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
AnnotationCreateDialogTwo::AnnotationCreateDialogTwo(AnnotationFile* annotationFile,
                                                     Annotation* annotation,
                                                     const int32_t viewportHeight,
                                                     const float volumeSliceThickness,
                                                     const int32_t browserWindowIndex,
                                                     const int32_t browserTabIndex,
                                                     QWidget* parent)
: WuQDialogModal("New Annotation",
                 parent),
m_annotationFile(annotationFile),
m_annotation(annotation),
m_viewportHeight(viewportHeight),
m_volumeSliceThickness(volumeSliceThickness),
m_browserWindowIndex(browserWindowIndex),
m_browserTabIndex(browserTabIndex)
{
    CaretAssert(m_annotationFile);
    CaretAssert(m_annotation);
    
    m_textEdit = NULL;
    m_annotationType = annotation->getType();
    
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);
    
    QWidget* imageWidget = ((m_annotationType == AnnotationTypeEnum::IMAGE)
                            ? createImageWidget()
                            : NULL);
    
    QWidget* polyedronWidget = ((m_annotationType == AnnotationTypeEnum::POLYHEDRON)
                                ? createPolyhedronWidget()
                                : NULL);
    
    QWidget* metaDataWidget = ((m_annotationType == AnnotationTypeEnum::POLYHEDRON)
                               ? createMetaDataEditorWidget()
                               : NULL);

    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    
    if (textWidget != NULL) {
        layout->addWidget(textWidget, 0, Qt::AlignLeft);
    }
    
    if (imageWidget != NULL) {
        layout->addWidget(imageWidget, 0, Qt::AlignLeft);
    }

    if (polyedronWidget != NULL) {
        layout->addWidget(polyedronWidget, 0, Qt::AlignLeft);
    }
    
    if (metaDataWidget != NULL) {
        layout->addWidget(metaDataWidget, 0);
    }
    
    setSizePolicy(dialogWidget->sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);

    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textEdit);
        m_textEdit->setFocus();
    }
}

/**
 * Destructor.
 */
AnnotationCreateDialogTwo::~AnnotationCreateDialogTwo()
{
}

/**
 * @return Annotation that was created by dialog (NULL if annotation NOT created).
 */
Annotation*
AnnotationCreateDialogTwo::getAnnotationThatWasCreated()
{
    return m_annotation;
}

/**
 * @return New instance of text widget.
 */
QWidget*
AnnotationCreateDialogTwo::createTextWidget()
{
    m_textEdit = new QTextEdit();
    m_textEdit->setText("");
    m_textEdit->selectAll();
    
    QGroupBox* groupBox = new QGroupBox("Text");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    layout->addWidget(m_textEdit, 100);
    
    return groupBox;
}

/**
 * @return New instance of polyhedron widget.
 */
QWidget*
AnnotationCreateDialogTwo::createPolyhedronWidget()
{
    float numberOfSlices(3.0);
    if (s_previousPolyhedronDepthValueMillimetersValidFlag) {
        float mm(s_previousPolyhedronDepthValueMillimeters);
        float absMM(std::fabs(mm));
        if (m_volumeSliceThickness > 0.0) {
            numberOfSlices = (absMM / m_volumeSliceThickness) + 1;
            if (mm < 0.0) {
                numberOfSlices = -numberOfSlices;
            }
        }
    }
    
    m_polyhedronSliceIndexDepthSpinBox = new QDoubleSpinBox();
    m_polyhedronSliceIndexDepthSpinBox->setMaximum(10000.0);
    m_polyhedronSliceIndexDepthSpinBox->setMinimum(-m_polyhedronSliceIndexDepthSpinBox->maximum());
    m_polyhedronSliceIndexDepthSpinBox->setSingleStep(1.0);
    m_polyhedronSliceIndexDepthSpinBox->setValue(numberOfSlices);
    QObject::connect(m_polyhedronSliceIndexDepthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &AnnotationCreateDialogTwo::polyhedronDepthIndexSpinBoxValueChanged);

    m_polyhedronSliceMillimetersDepthLabel = NULL;
    m_polyhedronSliceMillimetersDepthSpinBox = NULL;
    const bool useMillimetrsLabelFlag(true);
    if (useMillimetrsLabelFlag) {
        m_polyhedronSliceMillimetersDepthLabel = new QLabel("     ");
        
        /*
         * Will update slice depth label
         */
        polyhedronDepthIndexSpinBoxValueChanged(m_polyhedronSliceIndexDepthSpinBox->value());
    }
    else {
        m_polyhedronSliceMillimetersDepthSpinBox = new QDoubleSpinBox();
        m_polyhedronSliceMillimetersDepthSpinBox->setRange(-100000.0, 100000.0);
        m_polyhedronSliceMillimetersDepthSpinBox->setSingleStep(0.1);
        m_polyhedronSliceMillimetersDepthSpinBox->setDecimals(2);
        m_polyhedronSliceMillimetersDepthSpinBox->setValue(s_previousPolyhedronDepthValueMillimeters);
        QObject::connect(m_polyhedronSliceMillimetersDepthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                         this, &AnnotationCreateDialogTwo::polyhedronDepthMillimetersSpinBoxValueChanged);
        
        /* Will update slice index with appropriate value */
        polyhedronDepthMillimetersSpinBoxValueChanged(m_polyhedronSliceMillimetersDepthSpinBox->value());
    }
    
    QGroupBox* groupBox = new QGroupBox("Polyhedron Depth");
    QGridLayout* layout = new QGridLayout(groupBox);
    layout->addWidget(new QLabel("Slices"), 0, 0);
    layout->addWidget(m_polyhedronSliceIndexDepthSpinBox, 0, 1);
    layout->addWidget(new QLabel("Millimeters"), 1, 0);
    if (m_polyhedronSliceMillimetersDepthSpinBox != NULL) {
        layout->addWidget(m_polyhedronSliceMillimetersDepthSpinBox, 1, 1);
    }
    if (m_polyhedronSliceMillimetersDepthLabel != NULL) {
        layout->addWidget(m_polyhedronSliceMillimetersDepthLabel, 1, 1);
    }

    return groupBox;
}

/**
 * @return polyhedron slice depth converted to millimeters
 */
float
AnnotationCreateDialogTwo::convertPolyhedronSlicesToMillimeters() const
{
    float millimetersOut(0.0);
    const float numSlices(m_polyhedronSliceIndexDepthSpinBox->value());
    const float absNumSlices(std::fabs(numSlices));
    if (absNumSlices >= 2.0) {
        millimetersOut = (absNumSlices - 1.0) * m_volumeSliceThickness;
        if (numSlices < 0.0) {
            millimetersOut = -millimetersOut;
        }
    }
    return millimetersOut;
}

/**
 * Called when polyhedron depth index spin box value changed
 * @param value
 *    New value
 */
void
AnnotationCreateDialogTwo::polyhedronDepthIndexSpinBoxValueChanged(double /*value*/)
{
    const float millimeters(convertPolyhedronSlicesToMillimeters());
    if (m_polyhedronSliceMillimetersDepthSpinBox != NULL) {
        QSignalBlocker blocker(m_polyhedronSliceMillimetersDepthSpinBox);
        m_polyhedronSliceMillimetersDepthSpinBox->setValue(millimeters);
    }
    if (m_polyhedronSliceMillimetersDepthLabel != NULL) {
        m_polyhedronSliceMillimetersDepthLabel->setText(QString::number(millimeters, 'f', 3));
    }
}

/**
 * Called when polyhedron depth value spin box value changed
 * @param value
 *    New value
 */
void
AnnotationCreateDialogTwo::polyhedronDepthMillimetersSpinBoxValueChanged(double value)
{
    float mm(m_volumeSliceThickness);
    if (mm == 0) {
        mm = 1.0;
    }
    const float mmSize(value / mm);
    QSignalBlocker blocker(m_polyhedronSliceIndexDepthSpinBox);
    m_polyhedronSliceIndexDepthSpinBox->setValue(mmSize);
}

/**
 * @return A metadata editor widget for polyhedrons
 */
QWidget*
AnnotationCreateDialogTwo::createMetaDataEditorWidget()
{
    const bool polyhedronSamplesFlag(true);
    m_requiredMetaDataNames = Annotation::getDefaultMetaDataNamesForType(m_annotationType,
                                                                         polyhedronSamplesFlag);
    m_annotationMetaData.reset(new GiftiMetaData());
    for (const auto& name : m_requiredMetaDataNames) {
        m_annotationMetaData->set(name, "");
    }

    m_annotationMetaData->set(GiftiMetaDataXmlElements::SAMPLES_LOCATION_ID, "Choose 1 of: Desired, Actual");
    m_annotationMetaData->set(GiftiMetaDataXmlElements::METADATA_NAME_COMMENT, "");

    m_metaDataEditorWidget = new MetaDataCustomEditorWidget(m_requiredMetaDataNames,
                                                            m_annotationMetaData.get());

    m_metaDataRequiredCheckBox = new QCheckBox("Require Metadata");
    m_metaDataRequiredCheckBox->setChecked(s_previousMetaDataRequiredCheckedStatus);
    
    QGroupBox* groupBox(new QGroupBox("Metadata"));
    QVBoxLayout* groupLayout = new QVBoxLayout(groupBox);
    groupLayout->addWidget(m_metaDataEditorWidget);
    groupLayout->addWidget(m_metaDataRequiredCheckBox, 0, Qt::AlignLeft);
    
    return groupBox;
}

/**
 * @return New instance of image widget.
 */
QWidget*
AnnotationCreateDialogTwo::createImageWidget()
{
    QAction* newFileAction = WuQtUtilities::createAction("Choose Image File...",
                                                         "Choose image file using file selection dialog",
                                                         this,
                                                         this,
                                                         SLOT(selectImageButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    
    m_imageFileNameLabel = new QLabel();
    
    m_imageThumbnailLabel = new QLabel();
    m_imageThumbnailLabel->setFixedSize(s_MAXIMUM_THUMB_NAIL_SIZE,
                                        s_MAXIMUM_THUMB_NAIL_SIZE);
    
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(newFileToolButton);
    nameLayout->addWidget(m_imageFileNameLabel);
    
    QGroupBox* groupBox = new QGroupBox("Image File");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addLayout(nameLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(m_imageThumbnailLabel);
    
    return groupBox;
}

/**
 * Invalidate the image.
 */
void
AnnotationCreateDialogTwo::invalidateImage()
{
    m_imageRgbaBytes.clear();
    m_imageWidth  = 0;
    m_imageHeight = 0;
    m_imageFileNameLabel->setText("");
    m_imageThumbnailLabel->setPixmap(QPixmap());
}

/**
 * Called when the select image button is clicked.
 */
void
AnnotationCreateDialogTwo::selectImageButtonClicked()
{
    const AString fileDialogSettingsName("AnnotImageDialog");
    
    /*
     * Setup file selection dialog.
     */
    CaretFileDialog fd(CaretFileDialog::Mode::MODE_OPEN,
                       this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilterForReading(DataFileTypeEnum::IMAGE));
    fd.setFileMode(CaretFileDialog::ExistingFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.setLabelText(CaretFileDialog::Accept, "Choose"); /* OK button shows Insert */
    fd.restoreDialogSettings(fileDialogSettingsName);
    
    AString errorMessages;
    
    if (fd.exec() == CaretFileDialog::Accepted) {
        invalidateImage();
        
        fd.saveDialogSettings(fileDialogSettingsName);
        
        QStringList selectedFiles = fd.selectedFiles();
        if ( ! selectedFiles.empty()) {
            const AString imageFileName = selectedFiles.at(0);
            
            ImageFile imageFile;
            
            try {
                imageFile.readFile(imageFileName);
            }
            catch (const DataFileException& dfe) {
                WuQMessageBox::errorOk(this,
                                       dfe.whatString());
                return;
            }
            
            imageFile.getImageBytesRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                        m_imageRgbaBytes,
                                        m_imageWidth,
                                        m_imageHeight);
            if ((m_imageWidth <= 0)
                || (m_imageHeight <= 0)) {
                WuQMessageBox::errorOk(this,
                                       ("Image Width="
                                        + QString::number(m_imageWidth)
                                        + " or Height="
                                        + QString::number(m_imageHeight)
                                        + " is invalid."));
                invalidateImage();
                return;
            }
            
            const int32_t expectedNumberOfBytes = (m_imageWidth
                                                   * m_imageHeight
                                                   * 4);
            if (static_cast<int32_t>(m_imageRgbaBytes.size()) != expectedNumberOfBytes) {
                WuQMessageBox::errorOk(this,
                                       "Image bytes size should be "
                                       + QString::number(expectedNumberOfBytes)
                                       + " but is "
                                       + QString::number(m_imageRgbaBytes.size()));
                invalidateImage();
                return;
            }
            
            m_imageFileNameLabel->setText(imageFile.getFileNameNoPath());
            
            imageFile.resizeToMaximumWidthOrHeight(s_MAXIMUM_THUMB_NAIL_SIZE);
            const QImage* qImage = imageFile.getAsQImage();
            m_imageThumbnailLabel->setPixmap(QPixmap::fromImage(*qImage));
        }
    }
}


/**
 * Gets called when the OK button is clicked.
 */
void
AnnotationCreateDialogTwo::okButtonClicked()
{
    AString errorMessage;
    
    QString userText;
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textEdit);
        userText = m_textEdit->toPlainText();
        if (userText.isEmpty()) {
            errorMessage.appendWithNewLine("Text is missing.");
        }
        
    }
    
    if (m_annotationType == AnnotationTypeEnum::IMAGE) {
        if ((m_imageWidth <= 0)
            || (m_imageHeight <= 0)
            || (m_imageRgbaBytes.empty())) {
            errorMessage.appendWithNewLine("Image File is invalid.  Choose Image File.");
        }
    }
    
    float polyhedronDepthMM(0);
    if (m_annotationType == AnnotationTypeEnum::POLYHEDRON) {
        if (m_polyhedronSliceMillimetersDepthSpinBox != NULL) {
            polyhedronDepthMM = m_polyhedronSliceMillimetersDepthSpinBox->value();
        }
        else {
            polyhedronDepthMM = convertPolyhedronSlicesToMillimeters();
        }
        if (polyhedronDepthMM == 0.0) {
            errorMessage.appendWithNewLine("Polyhedron depth must not be zero.");
        }
        s_previousPolyhedronDepthValueMillimeters = polyhedronDepthMM;
        s_previousPolyhedronDepthValueMillimetersValidFlag = true;
    }
    
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
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    CaretAssert(m_annotationFile);
    
//    CaretPointer<Annotation> annotation;
//    annotation.grabNew(NULL);
    
    if (m_annotationType == AnnotationTypeEnum::POLYHEDRON) {
        AnnotationPolyhedron* polyhedron(m_annotation->castToPolyhedron());
        CaretAssert(polyhedron);
        polyhedron->setDepth(polyhedronDepthMM);
        polyhedron->setDrawingNewAnnotationStatus(false);
    }
    
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        AnnotationText* text = dynamic_cast<AnnotationText*>(m_annotation);
        CaretAssert(text);
        text->setText(userText);
    }
    AnnotationImage* annImage = dynamic_cast<AnnotationImage*>(m_annotation);
    if (annImage != NULL) {
        annImage->setImageBytesRGBA(&m_imageRgbaBytes[0],
                                    m_imageWidth,
                                    m_imageHeight);
    }
//    if (m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::POLYHEDRON) {
//        VolumeMappableInterface* underlayVolume;
//        const MouseEvent& mouseEvent(m_newAnnotationInfo.m_mouseEvent);
//        BrowserTabContent* btc(mouseEvent.getViewportContent()->getBrowserTabContent());
//        CaretAssert(btc);
//        ModelVolume* mv(btc->getDisplayedVolumeModel());
//        if (mv != NULL) {
//            underlayVolume = mv->getUnderlayVolumeFile(btc->getTabNumber());
//            if (underlayVolume == NULL) {
//                WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
//                                       "Polyhedron requires an underlay volume");
//                return;
//            }
//        }
//        else {
//            WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
//                                   "Polyhedron must be drawn on volume slices");
//            return;
//        }
//    }

    if (m_metaDataEditorWidget != NULL) {
        GiftiMetaData* annMetaData(m_annotation->getMetaData());
        CaretAssert(annMetaData);
        CaretAssert(m_annotationMetaData);
        annMetaData->replace(*m_annotationMetaData.get());
    }
    
    finishAnnotationCreation(m_annotationFile,
                             m_annotation,
                             m_browserWindowIndex,
                             m_browserTabIndex);
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->updateForNewAnnotation(m_annotation);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialog::okButtonClicked();
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
AnnotationCreateDialogTwo::finishAnnotationCreation(AnnotationFile* annotationFile,
                                                    Annotation* annotation,
                                                    const int32_t browswerWindowIndex,
                                                    const int32_t tabIndex)
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    
    /*
     * Add annotation to its file
     */
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeCreateAnnotation(annotationFile,
                                         annotation);
    
    CaretAssert(annotation);
    UserInputModeEnum::Enum inputMode = UserInputModeEnum::Enum::ANNOTATIONS;
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            inputMode = UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING;
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
    AString errorMessage;
    if ( ! annotationManager->applyCommand(inputMode,
                                           undoCommand,
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

