
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

#define __ANNOTATION_CREATE_DIALOG_DECLARE__
#include "AnnotationCreateDialog.h"
#undef __ANNOTATION_CREATE_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationManager.h"
#include "AnnotationPercentSizeText.h"
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
#include "GuiManager.h"
#include "ImageFile.h"
#include "ModelSurfaceMontage.h"
#include "MouseEvent.h"
#include "WuQtUtilities.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCreateDialog 
 * \brief Dialog used for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Creating a new annotation using an annotation space and type.
 * A dialog may be displayed when the coordinate space is not valid
 * for the window location or for text and image annotations.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param annotationFile
 *      File to which new annotation is added.
 * @return
 *      Pointer to annotation that was created or NULL
 *      if annotation not created (user cancelled).
 */
Annotation*
AnnotationCreateDialog::newAnnotationFromSpaceAndType(const MouseEvent& mouseEvent,
                                                      const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                      const AnnotationTypeEnum::Enum annotationType,
                                                      AnnotationFile* annotationFile)
{
    Annotation* newAnnotation = newAnnotationFromSpaceTypeAndCoords(MODE_NEW_ANNOTATION_TYPE_CLICK,
                                                                    mouseEvent,
                                                                    annotationSpace,
                                                                    annotationType,
                                                                    annotationFile);

    return newAnnotation;
}

/**
 * Creating a new annotation using an annotation space and type.
 * A dialog may be displayed when the coordinate space is not valid
 * for the window location or for text and image annotations.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param annotationFile
 *      File to which new annotation is added.
 * @return
 *      Pointer to annotation that was created or NULL
 *      if annotation not created (user cancelled).
 */
Annotation*
AnnotationCreateDialog::newAnnotationFromSpaceTypeAndBounds(const MouseEvent& mouseEvent,
                                                       const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                       const AnnotationTypeEnum::Enum annotationType,
                                                       AnnotationFile* annotationFile)
{
    Annotation* newAnnotation = newAnnotationFromSpaceTypeAndCoords(MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS,
                                                                     mouseEvent,
                                                                     annotationSpace,
                                                                     annotationType,
                                                                     annotationFile);
    return newAnnotation;
}

/**
 * Creating a new annotation using an annotation space and type.
 * A dialog may be displayed when the coordinate space is not valid
 * for the window location or for text and image annotations.
 *
 * @param mode
 *     The mode.
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param coordOne
 *      The first coordinate MUST BE VALID (NOT NULL).
 * @param coordTwo
 *      Optional second coordinate (NULL if invalid).
 * @param annotationFile
 *      File to which new annotation is added.
 * @return
 *      Pointer to annotation that was created or NULL
 *      if annotation not created (user cancelled).
 */
Annotation*
AnnotationCreateDialog::newAnnotationFromSpaceTypeAndCoords(const Mode mode,
                                                            const MouseEvent& mouseEvent,
                                                            const AnnotationCoordinateSpaceEnum::Enum annotationSpaceIn,
                                                            const AnnotationTypeEnum::Enum annotationType,
                                                            AnnotationFile* annotationFile)
{
    bool useBothFlag = false;
    switch (mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
            break;
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
            useBothFlag = true;
            break;
    }
    
    NewAnnotationInfo newInfo(mouseEvent,
                              annotationSpaceIn,
                              annotationType,
                              useBothFlag,
                              annotationFile);
    
    if ( ! newInfo.isValid()) {
        CaretAssert(0);
        WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                               "PROGRAM ERROR: Failed to create coordinate information.");
        return NULL;
    }
    
    
    bool needImageOrTextFlag = false;
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            CaretAssertMessage(0, "Browser Tabs do not get created !!!");
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssertMessage(0, "Color bars do not get created !!!");
            break;
        case AnnotationTypeEnum::IMAGE:
            needImageOrTextFlag = true;
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLY_LINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            CaretAssertMessage(0, "Scale bars do not get created !!!");
            break;
        case AnnotationTypeEnum::TEXT:
            needImageOrTextFlag = true;
            break;
    }
    
    bool needToLaunchDialogFlag = false;
    if (newInfo.isSelectedSpaceValid()) {
        if (needImageOrTextFlag) {
            needToLaunchDialogFlag = true;
        }
        else {
            AString errorMessage;
            Annotation* newAnn = createAnnotation(newInfo, newInfo.m_selectedSpace, /*annotationSpace,*/ errorMessage);
            if (newAnn != NULL) {
                DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
                dpa->updateForNewAnnotation(newAnn);
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
            
                return newAnn;
            }
            if ( ! errorMessage.isEmpty()) {
                WuQMessageBox::errorOk(mouseEvent.getOpenGLWidget(),
                                       errorMessage);
                return NULL;
            }
            
            needToLaunchDialogFlag = true;
        }
    }
    else {
        needToLaunchDialogFlag = true;
    }
    
    if (needToLaunchDialogFlag) {
        AnnotationCreateDialog annDialog(mode,
                                         newInfo,
                                         newInfo.m_selectedSpace,
                                         newInfo.isSelectedSpaceValid(),
                                         mouseEvent.getOpenGLWidget());
        if (annDialog.exec() == AnnotationCreateDialog::Accepted) {
            return annDialog.getAnnotationThatWasCreated();
        }
        
    }

    return NULL;
}

/**
 * Create a new annotation.
 *
 * @param newAnnotationInfo
 *     Information about the new annotation.
 * @param annotationSpace
 *     Coordinate space for new annotaiton.
 * @param errorMessageOut
 *     Output with error message.
 * @return
 *     Pointer to new annotation or NULL if creating annotation failed.
 */
Annotation*
AnnotationCreateDialog::createAnnotation(NewAnnotationInfo& newAnnotationInfo,
                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                         AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    Annotation* newAnnotation = NULL;
    if (newAnnotationInfo.m_annotationType == AnnotationTypeEnum::TEXT) {
        newAnnotation = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::USER);
        
        /*
         * In surface montage, percentage size text may need alteration
         */
        bool adjustTextPctSizeFlag = false;
        switch (annotationSpace) {
            case AnnotationCoordinateSpaceEnum::CHART:
                adjustTextPctSizeFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                adjustTextPctSizeFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                adjustTextPctSizeFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        if (adjustTextPctSizeFlag) {
            AnnotationPercentSizeText* pctText = dynamic_cast<AnnotationPercentSizeText*>(newAnnotation);
            if (pctText != NULL) {
                const int32_t browserWindowIndex = newAnnotationInfo.m_mouseEvent.getBrowserWindowIndex();
                BrowserTabContent* browserTabContent = GuiManager::get()->getBrowserTabContentForBrowserWindow(browserWindowIndex,
                                                                                                               false);
                if (browserTabContent != NULL) {
                    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
                    if (msm != NULL) {
                        const int32_t tabIndex = browserTabContent->getTabNumber();
                        int32_t rowCount = 1;
                        int32_t columnCount = 1;
                        msm->getSurfaceMontageNumberOfRowsAndColumns(tabIndex,
                                                                     rowCount,
                                                                     columnCount);
                        if (rowCount > 0) {
                            float pctSize = pctText->getFontPercentViewportSize();
                            pctSize *= static_cast<float>(rowCount);
                            pctText->setFontPercentViewportSize(pctSize);
                        }
                    }
                }
            }
        }
    }
    else {
        newAnnotation = Annotation::newAnnotationOfType(newAnnotationInfo.m_annotationType,
                                                         AnnotationAttributesDefaultTypeEnum::USER);
    }
    
    AnnotationCoordinateInformation* coordTwo = NULL;
    if (newAnnotationInfo.m_coordTwoInfoValid) {
        coordTwo = &newAnnotationInfo.m_coordTwoInfo;
    }
    else {
        bool threeDimSpaceFlag = false;
        switch (annotationSpace) {
            case AnnotationCoordinateSpaceEnum::CHART:
                threeDimSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                threeDimSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                threeDimSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                break;
        }
        if (threeDimSpaceFlag) {
            switch (newAnnotationInfo.m_annotationType) {
                case AnnotationTypeEnum::BOX:
                    break;
                case AnnotationTypeEnum::BROWSER_TAB:
                    break;
                case AnnotationTypeEnum::COLOR_BAR:
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    delete newAnnotation;
                    newAnnotation = NULL;
                    errorMessageOut = ("A line annotation cannot be created from a mouse click in "
                                       + AnnotationCoordinateSpaceEnum::toGuiName(annotationSpace)
                                       + " coordinate space.   Hold the mouse down, drag the mouse, and then release the mouse.");
                    return NULL;
                    break;
                case AnnotationTypeEnum::OVAL:
                    break;
                case AnnotationTypeEnum::POLY_LINE:
                    delete newAnnotation;
                    newAnnotation = NULL;
                    errorMessageOut = ("A poly line annotation cannot be created from a mouse click in "
                                       + AnnotationCoordinateSpaceEnum::toGuiName(annotationSpace)
                                       + " coordinate space.   Hold the mouse down, drag the mouse, and then release the mouse.");
                    return NULL;
                    break;
                case AnnotationTypeEnum::SCALE_BAR:
                    break;
                case AnnotationTypeEnum::TEXT:
                    break;
            }
        }
    }
    const bool validFlag = AnnotationCoordinateInformation::setAnnotationCoordinatesForSpace(newAnnotation,
                                                                                             annotationSpace,
                                                                                             &newAnnotationInfo.m_coordOneInfo,
                                                                                             coordTwo);
    if (validFlag) {
        if ((newAnnotationInfo.m_percentageWidth > 0)
            && (newAnnotationInfo.m_percentageHeight > 0)) {
            AnnotationOneCoordinateShape* twoDimShape = dynamic_cast<AnnotationOneCoordinateShape*>(newAnnotation);
            if (twoDimShape != NULL) {
                twoDimShape->setWidth(newAnnotationInfo.m_percentageWidth);
                twoDimShape->setHeight(newAnnotationInfo.m_percentageHeight);
            }
        }
        
        finishAnnotationCreation(newAnnotationInfo.m_annotationFile,
                                 newAnnotation,
                                 newAnnotationInfo.m_mouseEvent.getBrowserWindowIndex(),
                                 newAnnotationInfo.m_coordOneInfo.m_tabSpaceInfo.m_index);
        return newAnnotation;
    }
    
    else {
        CaretAssertMessage(0, "Space should have been valid, we should never get here.");
        delete newAnnotation;
        return NULL;
    }
}


/**
 * Dialog constructor.
 *
 * @param mode
 *     The dialog's mode.
 * @param newAnnotationInfo
 *     Information for creating new annotation.
 * @param annotationSpace
 *      Space of annotation that is being created.
 * @param annotationSpaceValidFlag
 *      True if annotation space is valid (do not need space selection).
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog::AnnotationCreateDialog(const Mode mode,
                                               NewAnnotationInfo& newAnnotationInfo,
                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                               const bool annotationSpaceValidFlag,
                                               QWidget* parent)
: WuQDialogModal("New Annotation",
                 parent),
m_mode(mode),
m_newAnnotationInfo(newAnnotationInfo),
m_annotationSpace(annotationSpace),
m_annotationThatWasCreated(NULL),
m_imageWidth(0),
m_imageHeight(0)
{
    m_annotationSpaceButtonGroup = NULL;
    m_textEdit = NULL;
    
    QGroupBox* coordGroupBox = NULL;
    if ( ! annotationSpaceValidFlag) {
        coordGroupBox = new QGroupBox("Coordinate Space");
        QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
        coordGroupLayout->setMargin(0);
        
        m_annotationSpaceButtonGroup = new QButtonGroup(this);
        for (std::vector<AnnotationCoordinateSpaceEnum::Enum>::iterator iter = m_newAnnotationInfo.m_validSpaces.begin();
             iter != m_newAnnotationInfo.m_validSpaces.end();
             iter++) {
            const AnnotationCoordinateSpaceEnum::Enum space = *iter;
            QRadioButton* rb = new QRadioButton(AnnotationCoordinateSpaceEnum::toGuiName(space));
            if (space == AnnotationCoordinateSpaceEnum::SPACER) {
                /*
                 * Spacer and Tab are presented as 'TAB' to the user.  So show 'TAB' but
                 * use the integer code for 'SPACER'.
                 */
                rb->setText(AnnotationCoordinateSpaceEnum::toGuiName(AnnotationCoordinateSpaceEnum::TAB));
            }
            m_annotationSpaceButtonGroup->addButton(rb,
                                                    AnnotationCoordinateSpaceEnum::toIntegerCode(space));
            coordGroupLayout->addWidget(rb);
        }

    }
    
    QWidget* textWidget = ((m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);
    
    QWidget* imageWidget = ((m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::IMAGE)
                            ? createImageWidget()
                            : NULL);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    
    if (coordGroupBox != NULL) {
        const QString message("The location for the new annotation is incompatible with the "
                              "coordinate space selected in the toolbar.  "
                              "Choose one of the coordinate "
                              "spaces below to create the annotation or press Cancel to cancel creation "
                              "of the annotation.");
        QLabel* messageLabel = new QLabel(message);
        messageLabel->setWordWrap(true);
        
        QLabel* spaceLabel = new QLabel("Space selected in Toolbar: "
                                        + AnnotationCoordinateSpaceEnum::toGuiName(annotationSpace));
        
        spaceLabel->setWordWrap(false);
        layout->addWidget(spaceLabel);
        layout->addSpacing(10);
        layout->addWidget(messageLabel);
        layout->addSpacing(10);
        
        layout->addWidget(coordGroupBox);
    }

    if (textWidget != NULL) {
        layout->addWidget(textWidget);
    }
    
    if (imageWidget != NULL) {
        layout->addWidget(imageWidget);
    }

    dialogWidget->setSizePolicy(dialogWidget->sizePolicy().horizontalPolicy(),
                                QSizePolicy::Fixed);
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    if (m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textEdit);
        m_textEdit->setFocus();
    }
}

/**
 * Destructor.
 */
AnnotationCreateDialog::~AnnotationCreateDialog()
{
}

/**
 * @return Annotation that was created by dialog (NULL if annotation NOT created).
 */
Annotation*
AnnotationCreateDialog::getAnnotationThatWasCreated()
{
    return m_annotationThatWasCreated;
}

/**
 * @return New instance of text widget.
 */
QWidget*
AnnotationCreateDialog::createTextWidget()
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
 * @return New instance of image widget.
 */
QWidget*
AnnotationCreateDialog::createImageWidget()
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
AnnotationCreateDialog::invalidateImage()
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
AnnotationCreateDialog::selectImageButtonClicked()
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
    fd.setLabelText(CaretFileDialog::Accept, "Choose"); // OK button shows Insert
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
AnnotationCreateDialog::okButtonClicked()
{
    AString errorMessage;
    
    QString userText;
    if (m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::TEXT) {
        userText = m_textEdit->toPlainText();
        if (userText.isEmpty()) {
            errorMessage.appendWithNewLine("Text is missing.");
        }
        
    }
    
    if (m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::IMAGE) {
        if ((m_imageWidth <= 0)
            || (m_imageHeight <= 0)
            || (m_imageRgbaBytes.empty())) {
            errorMessage = "Image File is invalid.  Choose Image File.";
        }
    }
    
    if ( ! errorMessage.isEmpty()) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
        return;
    }
    
    AnnotationCoordinateSpaceEnum::Enum space = m_newAnnotationInfo.m_selectedSpace;
    if (m_annotationSpaceButtonGroup != NULL) {
        const int id = m_annotationSpaceButtonGroup->checkedId();
        bool validFlag = false;
        space = AnnotationCoordinateSpaceEnum::fromIntegerCode(id, &validFlag);
        if ( ! validFlag) {
            WuQMessageBox::errorOk(this, "No Space is selected.");
            return;
        }
    }
    
    CaretAssert(m_newAnnotationInfo.m_annotationFile);
    
    CaretPointer<Annotation> annotation;
    annotation.grabNew(NULL);
    
    annotation.grabNew(createAnnotation(m_newAnnotationInfo, space, errorMessage));
    if (annotation == NULL) {
        if (errorMessage.isEmpty()) {
            WuQMessageBox::errorOk(this,
                                   "Failed to create annotation in space: "
                                   + AnnotationCoordinateSpaceEnum::toGuiName(space));
        }
        else {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        return;
    }
    
    if (m_newAnnotationInfo.m_annotationType == AnnotationTypeEnum::TEXT) {
        AnnotationText* text = dynamic_cast<AnnotationText*>(annotation.getPointer());
        CaretAssert(text);
        text->setText(userText);
    }
    AnnotationImage* annImage = dynamic_cast<AnnotationImage*>(annotation.getPointer());
    if (annImage != NULL) {
        annImage->setImageBytesRGBA(&m_imageRgbaBytes[0],
                                    m_imageWidth,
                                    m_imageHeight);
    }

    /*
     * Need to release annotation from its CaretPointer since the
     * annotation file will take ownership of the annotation.
     */
    Annotation* annotationPointer = annotation.releasePointer();
    
    m_annotationThatWasCreated = annotationPointer;
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->updateForNewAnnotation(m_annotationThatWasCreated);
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
AnnotationCreateDialog::finishAnnotationCreation(AnnotationFile* annotationFile,
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
            inputMode = UserInputModeEnum::Enum::TILE_TABS_MANUAL_LAYOUT_EDITING;
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLY_LINE:
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

/**
 * Constructor for information used to create a new annotation.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param selectedSpace
 *     The space selected by the user.
 * @param annotationType
 *     The annotation type.
 * @param useBothCoordinatesFromMouseFlag
 *     Use both coords (X/Y and pressed X/Y)
 * @param annotationFile
 *     File to which annotation is added.
 */
AnnotationCreateDialog::NewAnnotationInfo::NewAnnotationInfo(const MouseEvent& mouseEvent,
                                                             const AnnotationCoordinateSpaceEnum::Enum selectedSpace,
                                                             const AnnotationTypeEnum::Enum annotationType,
                                                             const bool useBothCoordinatesFromMouseFlag,
                                                             AnnotationFile* annotationFile)
: m_mouseEvent(mouseEvent),
m_selectedSpace(selectedSpace),
m_annotationType(annotationType),
m_annotationFile(annotationFile)
{
    CaretAssert(annotationFile);
    
    m_validSpaces.clear();
    m_coordOneInfo.reset();
    m_coordTwoInfo.reset();
    m_coordTwoInfoValid = false;
    m_percentageWidth  = -1;
    m_percentageHeight = -1;
    
    
    if (useBothCoordinatesFromMouseFlag) {
        AnnotationCoordinateInformation::createCoordinateInformationFromXY(mouseEvent,
                                                                           mouseEvent.getX(),
                                                                           mouseEvent.getY(),
                                                                           m_coordTwoInfo);
        AnnotationCoordinateInformation::createCoordinateInformationFromXY(mouseEvent,
                                                                           mouseEvent.getPressedX(),
                                                                           mouseEvent.getPressedY(),
                                                                           m_coordOneInfo);
        
        AnnotationCoordinateInformation::getValidCoordinateSpaces(&m_coordOneInfo,
                                                                  &m_coordTwoInfo,
                                                                  m_validSpaces);
        
        if (isValid()) {
            m_coordTwoInfoValid = true;
            
            processTwoCoordInfo();
        }
    }
    else {
        AnnotationCoordinateInformation::createCoordinateInformationFromXY(mouseEvent,
                                                                           mouseEvent.getX(),
                                                                           mouseEvent.getY(),
                                                                           m_coordOneInfo);
        
        /*
         * For one-dimensional annotations (line), we need a second coordinate
         */
        bool addSecondCoordFlag = true;
        switch (annotationType) {
            case AnnotationTypeEnum::BOX:
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                addSecondCoordFlag = true;
                break;
            case AnnotationTypeEnum::OVAL:
                break;
            case AnnotationTypeEnum::POLY_LINE:
                CaretAssertToDoWarning();
                addSecondCoordFlag = true;
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                break;
        }
        
        if (addSecondCoordFlag) {
            
        }
        
        AnnotationCoordinateInformation::getValidCoordinateSpaces(&m_coordOneInfo,
                                                                  NULL,
                                                                  m_validSpaces);
    }
    
    /*
     * There is not a selection in the GUI for the user to choose 'SPACER' coordinate space.
     * Instead the user uses 'TAB' space in the GUI.  So, if TAB space is NOT valid, but
     * SPACER space is valid, change the requested space to 'SPACER'
     */
    if (m_selectedSpace == AnnotationCoordinateSpaceEnum::TAB) {
        const bool haveTabFlag = (std::find(m_validSpaces.begin(),
                                            m_validSpaces.end(),
                                            AnnotationCoordinateSpaceEnum::TAB) != m_validSpaces.end());
        const bool haveSpacerFlag = (std::find(m_validSpaces.begin(),
                                               m_validSpaces.end(),
                                               AnnotationCoordinateSpaceEnum::SPACER) != m_validSpaces.end());
        
        if (haveSpacerFlag) {
            if ( ! haveTabFlag) {
                m_selectedSpace = AnnotationCoordinateSpaceEnum::SPACER;
            }
        }
    }
}

/**
 * When the user drags to create an annotation, two points are
 * used at opposite corners.  For non-linear annotations, we
 * need a center, width, and height.  So for these types,
 * convert the two points to one point with center, width, 
 * and height.
 */
void
AnnotationCreateDialog::NewAnnotationInfo::processTwoCoordInfo()
{
    if ((m_coordOneInfo.m_windowSpaceInfo.m_index >= 0)
        && (m_coordTwoInfo.m_windowSpaceInfo.m_index >= 0)) {
    
        bool useAverageFlag      = false;
        bool useTextAligmentFlag = false;
        switch (m_annotationType) {
            case AnnotationTypeEnum::BOX:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::IMAGE:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::OVAL:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::LINE:
                useAverageFlag = false;
                break;
            case AnnotationTypeEnum::POLY_LINE:
                useAverageFlag = false;
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                useAverageFlag = true;
                break;
            case AnnotationTypeEnum::TEXT:
                useTextAligmentFlag = true;
                break;
        }
        
        if (useAverageFlag
            || useTextAligmentFlag) {
            int32_t windowPixelX    = m_coordOneInfo.m_windowSpaceInfo.m_pixelXYZ[0];
            int32_t windowPixelY    = m_coordOneInfo.m_windowSpaceInfo.m_pixelXYZ[1];
            int32_t windowTwoPixelX = m_coordTwoInfo.m_windowSpaceInfo.m_pixelXYZ[0];
            int32_t windowTwoPixelY = m_coordTwoInfo.m_windowSpaceInfo.m_pixelXYZ[1];
            
            if ((windowPixelX >= 0)
                && (windowPixelY >= 0)
                && (windowTwoPixelX >= 0)
                && (windowTwoPixelY >= 0)) {
                const float minX = std::min(windowPixelX, windowTwoPixelX);
                const float minY = std::min(windowPixelY, windowTwoPixelY);
                const float maxX = std::max(windowPixelX, windowTwoPixelX);
                const float maxY = std::max(windowPixelY, windowTwoPixelY);
                const float centerX = (windowPixelX + windowTwoPixelX) / 2.0;
                const float centerY = (windowPixelY + windowTwoPixelY) / 2.0;
                
                if (useAverageFlag) {
                    /*
                     * Width and height in pixels
                     */
                    const float pixelWidth  = maxX - minX;
                    const float pixelHeight = maxY - minY;
                    
                    float viewportWidth  = 0.0;
                    float viewportHeight = 0.0;
                    switch (m_selectedSpace) {
                        case AnnotationCoordinateSpaceEnum::CHART:
                        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                        case AnnotationCoordinateSpaceEnum::SURFACE:
                        {
                            int viewport[4];
                            m_mouseEvent.getViewportContent()->getModelViewport(viewport);
                            viewportWidth = viewport[2];
                            viewportHeight = viewport[3];
                            
                            float subWidth  = 0.0;
                            float subHeight = 0.0;
                            if (adjustViewportForSurfaceMontage(m_mouseEvent.getViewportContent()->getBrowserTabContent(),
                                                                viewport,
                                                                subWidth,
                                                                subHeight)) {
                                
                                viewportWidth  = subWidth;
                                viewportHeight = subHeight;
                            }
                            break;
                        }
                        case AnnotationCoordinateSpaceEnum::SPACER:
                        {
                            int viewport[4];
                            m_mouseEvent.getViewportContent()->getModelViewport(viewport);
                            viewportWidth = viewport[2];
                            viewportHeight = viewport[3];
                        }
                            break;
                        case AnnotationCoordinateSpaceEnum::TAB:
                        {
                            int viewport[4];
                            m_mouseEvent.getViewportContent()->getModelViewport(viewport);
                            viewportWidth = viewport[2];
                            viewportHeight = viewport[3];
                        }
                            break;
                        case AnnotationCoordinateSpaceEnum::VIEWPORT:
                            CaretAssert(0);
                            break;
                        case AnnotationCoordinateSpaceEnum::WINDOW:
                        {
                            int viewport[4];
                            m_mouseEvent.getViewportContent()->getWindowViewport(viewport);
                            viewportWidth = viewport[2];
                            viewportHeight = viewport[3];
                        }
                            break;
                    }
                    
                    if ((viewportWidth > 0.0)
                        && (viewportHeight > 0.0)) {
                        m_percentageWidth  = (pixelWidth  / viewportWidth)  * 100.0;
                        m_percentageHeight = (pixelHeight / viewportHeight) * 100.0;
                    }
                    windowPixelX = centerX;
                    windowPixelY = centerY;
                    windowTwoPixelX = -1;
                    windowTwoPixelY = -1;
                    
                    /* 25 Mar 2016
                     * Note: windowPixelX,Y has origin in locked viewport
                     * but we need X,Y in window as if there was no locked
                     * viewport.
                     */
                    int wvp[4];
                    m_mouseEvent.getViewportContent()->getWindowViewport(wvp);
                    windowPixelX = centerX + wvp[0];
                    windowPixelY = centerY + wvp[1];
                }
                else if (useTextAligmentFlag) {
                    float textX = windowPixelX;
                    float textY = windowPixelY;
                    
                    AnnotationPercentSizeText textAnn(AnnotationAttributesDefaultTypeEnum::USER);
                    switch (textAnn.getHorizontalAlignment()) {
                        case AnnotationTextAlignHorizontalEnum::CENTER:
                            textX = centerX;
                            break;
                        case AnnotationTextAlignHorizontalEnum::LEFT:
                            textX = minX;
                            break;
                        case AnnotationTextAlignHorizontalEnum::RIGHT:
                            textX = maxX;
                            break;
                    }
                    
                    switch (textAnn.getVerticalAlignment()) {
                        case AnnotationTextAlignVerticalEnum::BOTTOM:
                            textY = minY;
                            break;
                        case AnnotationTextAlignVerticalEnum::MIDDLE:
                            textY = centerY;
                            break;
                        case AnnotationTextAlignVerticalEnum::TOP:
                            textY = maxY;
                            break;
                    }
                    
                    windowPixelX = textX;
                    windowPixelY = textY;
                    windowTwoPixelX = -1;
                    windowTwoPixelY = -1;
                    
                    /* 25 Mar 2016
                     * Note: windowPixelX,Y has origin in locked viewport
                     * but we need X,Y in window as if there was no locked
                     * viewport.
                     */
                    int wvp[4];
                    m_mouseEvent.getViewportContent()->getWindowViewport(wvp);
                    windowPixelX = textX + wvp[0];
                    windowPixelY = textY + wvp[1];
                }
                
                AnnotationCoordinateInformation::createCoordinateInformationFromXY(m_mouseEvent,
                                                                                   windowPixelX,
                                                                                   windowPixelY,
                                                                                   m_coordOneInfo);
                
                m_coordTwoInfo.reset();
                m_coordTwoInfoValid = false;

                AnnotationCoordinateInformation::getValidCoordinateSpaces(&m_coordOneInfo,
                                                                          NULL,
                                                                          m_validSpaces);
            }
        }
    }
}

/**
 * Adjust the viewport for surface montage.  A surface montage is a composite of multiple
 * smaller viewports.  For proper annotation sizing in stereotaxic or surface space,
 * we need to find the with of the model area in the viewport.
 *
 * @param browserTabContent
 *     Content of browser tab.
 * @param viewport
 *     The tab's viewport.
 * @param widthOut
 *     Output with width.
 * @param heightOut
 *     Output with height.
 * @return
 *     True if the width and height are valid, else false.
 */
bool
AnnotationCreateDialog::NewAnnotationInfo::adjustViewportForSurfaceMontage(BrowserTabContent* browserTabContent,
                                                                           const int viewport[4],
                                                                           float& widthOut,
                                                                           float& heightOut)
{
    if (browserTabContent == NULL) {
        return false;
    }
    
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    if (msm == NULL) {
        return false;
    }
    
    int32_t numRows = -1;
    int32_t numCols = -1;
    msm->getSurfaceMontageNumberOfRowsAndColumns(browserTabContent->getTabNumber(),
                                                 numRows,
                                                 numCols);
    
    if ((numRows > 0)
        && (numCols > 0)) {
        /*
         * All viewports within a surface montage are same dimensions
         */
        const float viewportWidth  = viewport[2];
        const float viewportHeight = viewport[3];
        widthOut  = viewportWidth  / static_cast<float>(numCols);
        heightOut = viewportHeight / static_cast<float>(numRows);
        
        if ((widthOut > 0.0)
            && (heightOut > 0.0)) {
            return true;
        }
    }
    
    return false;
}


/**
 * @return Is the new annotation information valid (has at least one valid space)?
 */
bool
AnnotationCreateDialog::NewAnnotationInfo::isValid() const
{
    return ( ! m_validSpaces.empty());
}

/**
 * @return Is the selected space valid?
 */
bool
AnnotationCreateDialog::NewAnnotationInfo::isSelectedSpaceValid() const
{
    if (std::find(m_validSpaces.begin(),
                  m_validSpaces.end(),
                  m_selectedSpace) != m_validSpaces.end()) {
        return true;
    }
    
    return false;
}


