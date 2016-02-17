
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

#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationCoordinateSelectionWidget.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretPointer.h"
#include "DataFileException.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "ImageFile.h"
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
    AnnotationCoordinateInformation coordInfo;
    AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent,
                                                                    coordInfo);
    
    Annotation* newAnnotation = newAnnotationFromSpaceTypeAndCoords(mouseEvent,
                                                                    annotationSpace,
                                                                    annotationType,
                                                                    &coordInfo,
                                                                    NULL,
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
    bool useAverageFlag      = false;
    bool useTextAligmentFlag = false;
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
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
        case AnnotationTypeEnum::TEXT:
            //useAverageFlag = true;
            useTextAligmentFlag = true;
            break;
    }

    float annotationWidth  = 0.0;
    float annotationHeight = 0.0;
    
    AnnotationCoordinateInformation coordOne;
    AnnotationCoordinateInformation coordTwo;
    bool useCoordTwoFlag = false;
    
    if (useAverageFlag) {
        float windowX = mouseEvent.getX();
        float windowY = mouseEvent.getY();
        float windowTwoX = mouseEvent.getPressedX();
        float windowTwoY = mouseEvent.getPressedY();
        
        if ((windowX >= 0)
            && (windowY >= 0)
            && (windowTwoX >= 0)
            && (windowTwoY >= 0)) {
            const float minX = std::min(windowX, windowTwoX);
            const float minY = std::min(windowY, windowTwoY);
            const float maxX = std::max(windowX, windowTwoX);
            const float maxY = std::max(windowY, windowTwoY);
            const float centerX = (windowX + windowTwoX) / 2.0;
            const float centerY = (windowY + windowTwoY) / 2.0;
            
            
            if (useAverageFlag) {
                /*
                 * Width and height in pixels
                 */
                annotationWidth  = maxX - minX;
                annotationHeight = maxY - minY;
                
                windowX = centerX;
                windowY = centerY;
                windowTwoX = -1;
                windowTwoY = -1;
            }
            else if (useTextAligmentFlag) {
                float textX = windowX;
                float textY = windowY;
                
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
                
                windowX = textX;
                windowY = textY;
                windowTwoX = -1;
                windowTwoY = -1;
            }
            
            if ((windowX >= 0)
                && (windowY >= 0)) {
                AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                                                mouseEvent.getViewportContent(),
                                                                                windowX,
                                                                                windowY,
                                                                                coordOne);
            }
            
            if ((windowTwoX >= 0)
                && (windowTwoY >= 0)) {
                AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                                                mouseEvent.getViewportContent(),
                                                                                windowTwoX,
                                                                                windowTwoY,
                                                                                coordTwo);
                useCoordTwoFlag = true;
            }
        }
    }
    else {
        AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent,
                                                                        coordOne);
        
        AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                                        mouseEvent.getViewportContent(),
                                                                        mouseEvent.getPressedX(),
                                                                        mouseEvent.getPressedY(),
                                                                        coordTwo);
        useCoordTwoFlag = true;
    }
    
    

    Annotation* newAnnotation = newAnnotationFromSpaceTypeAndCoords(mouseEvent,
                                                                    annotationSpace,
                                                                    annotationType,
                                                                    &coordOne,
                                                                    (useCoordTwoFlag
                                                                     ? &coordTwo
                                                                     : NULL),
                                                                    annotationFile);
    if ((annotationWidth > 0.0)
        && (annotationHeight > 0.0)) {
        AnnotationCoordinateInformation::setAnnotationFromBoundsWidthAndHeight(newAnnotation,
                                                                               mouseEvent,
                                                                               annotationWidth,
                                                                               annotationHeight);
    }
    
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
AnnotationCreateDialog::newAnnotationFromSpaceTypeAndCoords(const MouseEvent& mouseEvent,
                                                       const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                       const AnnotationTypeEnum::Enum annotationType,
                                                       const AnnotationCoordinateInformation* coordOne,
                                                       const AnnotationCoordinateInformation* coordTwo,
                                                       AnnotationFile* annotationFile)
{
    CaretAssert(coordOne);
    
    /*
     * Both coordinates must be valid for the annotation space.
     * Note: Second coordinate is optional.
     */
    bool annotationSpaceValidFlag = false;
    if (coordOne->isCoordinateSpaceValid(annotationSpace)) {
        if (coordTwo != NULL) {
            if (coordTwo->isCoordinateSpaceValid(annotationSpace)) {
                annotationSpaceValidFlag = true;
            }
        }
        else {
            annotationSpaceValidFlag = true;
        }
    }
    
    
    bool needImageOrTextFlag = false;
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssertMessage(0, "Colorbars do not get created !!!");
            break;
        case AnnotationTypeEnum::IMAGE:
            needImageOrTextFlag = true;
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::TEXT:
            needImageOrTextFlag = true;
            break;
    }
    
    CaretPointer<Annotation> annotationOut(Annotation::newAnnotationOfType(annotationType,
                                                                           AnnotationAttributesDefaultTypeEnum::USER));
    
    if (annotationSpaceValidFlag) {
        if ( ! needImageOrTextFlag) {
            Annotation* newAnn = Annotation::newAnnotationOfType(annotationType,
                                                                 AnnotationAttributesDefaultTypeEnum::USER);
            const bool validFlag = AnnotationCoordinateInformation::setAnnotationCoordinatesForSpace(newAnn,
                                                                                                     annotationSpace,
                                                                                                     coordOne,
                                                                                                     coordTwo);
            if (validFlag) {
                finishAnnotationCreation(annotationFile,
                                         newAnn,
                                         mouseEvent.getBrowserWindowIndex());
                return newAnn;
            }
            else {
                CaretAssertMessage(0, "Space should have been valid, we should never get here.");
                delete newAnn;
                annotationSpaceValidFlag = false;
            }
        }
    }
    
    AnnotationCreateDialog annDialog(MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS,
                                     mouseEvent,
                                     annotationFile,
                                     annotationSpace,
                                     annotationSpaceValidFlag,
                                     annotationType,
                                     mouseEvent.getOpenGLWidget());
    if (annDialog.exec() == AnnotationCreateDialog::Accepted) {
        return annDialog.getAnnotationThatWasCreated();
    }
    
    return NULL;
}

/**
 * Dialog constructor.
 *
 * @param mode
 *     The dialog's mode.
 * @param mouseEvent
 *     The mouse event.
 * @param annotationFile
 *     File containing the annotation that is copied (may be NULL).
 * @param annotationSpace
 *      Space of annotation that is being created.
 * @param annotationSpaceValidFlag
 *      True if annotation space is valid (do not need space selection).
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog::AnnotationCreateDialog(const Mode mode,
                                               const MouseEvent& mouseEvent,
                                               AnnotationFile* annotationFile,
                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                               const bool annotationSpaceValidFlag,
                                               const AnnotationTypeEnum::Enum annotationType,
                                               QWidget* parent)
: WuQDialogModal("New Annotation",
                 parent),
m_mode(mode),
m_mouseEvent(mouseEvent),
m_annotationFile(annotationFile),
m_annotationSpace(annotationSpace),
m_annotationType(annotationType),
m_annotationThatWasCreated(NULL),
m_annotationFromBoundsWidth(-1.0),
m_annotationFromBoundsHeight(-1.0),
m_imageWidth(0),
m_imageHeight(0)
{
    m_textEdit = NULL;
    
    /*
     * Get coordinates at the mouse location.
     */
    int32_t windowX = mouseEvent.getX();
    int32_t windowY = mouseEvent.getY();
    int32_t windowTwoX = -1;
    int32_t windowTwoY = -1;
    switch (m_mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
            break;
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
        {
            windowX = mouseEvent.getPressedX();
            windowY = mouseEvent.getPressedY();
            windowTwoX = mouseEvent.getX();
            windowTwoY = mouseEvent.getY();
            
            bool useAverageFlag      = false;
            bool useTextAligmentFlag = false;
            switch (annotationType) {
                case AnnotationTypeEnum::BOX:
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
                case AnnotationTypeEnum::TEXT:
                    //useAverageFlag = true;
                    useTextAligmentFlag = true;
                    break;
            }
            
            if ((windowX >= 0)
                && (windowY >= 0)
                && (windowTwoX >= 0)
                && (windowTwoY >= 0)) {
                const float minX = std::min(windowX, windowTwoX);
                const float minY = std::min(windowY, windowTwoY);
                const float maxX = std::max(windowX, windowTwoX);
                const float maxY = std::max(windowY, windowTwoY);
                const float centerX = (windowX + windowTwoX) / 2.0;
                const float centerY = (windowY + windowTwoY) / 2.0;
                
                if (useAverageFlag) {
                    /*
                     * Width and height in pixels
                     */
                    m_annotationFromBoundsWidth  = maxX - minX;
                    m_annotationFromBoundsHeight = maxY - minY;
                    
                    windowX = centerX;
                    windowY = centerY;
                    windowTwoX = -1;
                    windowTwoY = -1;
                }
                else if (useTextAligmentFlag) {
                    float textX = windowX;
                    float textY = windowY;
                    
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
                    
                    windowX = textX;
                    windowY = textY;
                    windowTwoX = -1;
                    windowTwoY = -1;
                }
            }
        }
            break;
    }
    
    AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                             mouseEvent.getViewportContent(),
                                                             windowX,
                                                             windowY,
                                                             m_coordInfo);
    
    bool secondCoordValidFlag = false;
    if ((windowTwoX >= 0)
        && (windowTwoY >= 0)) {
        AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                                 mouseEvent.getViewportContent(),
                                                                 windowTwoX,
                                                                 windowTwoY,
                                                                 m_coordTwoInfo);
        secondCoordValidFlag = true;
    }
    
    m_coordinateSelectionWidget = new AnnotationCoordinateSelectionWidget(m_annotationType,
                                                                          m_coordInfo,
                                                                          (secondCoordValidFlag
                                                                           ? &m_coordTwoInfo
                                                                           : NULL));
    
    
    QGroupBox* coordGroupBox = new QGroupBox("Coordinate Space");
    QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
    coordGroupLayout->setMargin(0);
    coordGroupLayout->addWidget(m_coordinateSelectionWidget);
    
    m_coordinateSelectionWidget->selectCoordinateSpace(m_annotationSpace);
    
    /*
     * If input space is valid, no need to show coordinate selection widget
     */
    bool showSpaceWidgetsFlag = true;
    if (annotationSpaceValidFlag) {
        bool validFlag = false;
        const AnnotationCoordinateSpaceEnum::Enum space = m_coordinateSelectionWidget->getSelectedCoordinateSpace(validFlag);
        if (validFlag) {
            if (m_annotationSpace == space) {
                showSpaceWidgetsFlag = false;
            }
        }
    }
    
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);
    
    QWidget* imageWidget = ((m_annotationType == AnnotationTypeEnum::IMAGE)
                            ? createImageWidget()
                            : NULL);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    if (showSpaceWidgetsFlag) {
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
    }
    else {
        coordGroupBox->setVisible(false);
        coordGroupBox->setFixedSize(0, 0);
    }
    layout->addWidget(coordGroupBox);
    if (textWidget != NULL) {
        layout->addWidget(textWidget);
    }
    if (imageWidget != NULL) {
        layout->addWidget(imageWidget);
    }
    layout->addStretch();
    
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
    m_imageFileNameLabel->setFixedSize(s_MAXIMUM_THUMB_NAIL_SIZE,
                                       s_MAXIMUM_THUMB_NAIL_SIZE);
    
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(newFileToolButton);
    nameLayout->addWidget(m_imageFileNameLabel);
    nameLayout->addStretch();
    
    QGroupBox* groupBox = new QGroupBox("Image File");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addLayout(nameLayout);
    layout->addWidget(m_imageThumbnailLabel);
    layout->addStretch();
    
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
    CaretFileDialog fd(this);
    fd.setAcceptMode(CaretFileDialog::AcceptOpen);
    fd.setNameFilter(DataFileTypeEnum::toQFileDialogFilter(DataFileTypeEnum::IMAGE));
    fd.setFileMode(CaretFileDialog::ExistingFile);
    fd.setViewMode(CaretFileDialog::List);
    fd.setLabelText(CaretFileDialog::Accept, "Insert"); // OK button shows Insert
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
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        userText = m_textEdit->toPlainText();
        if (userText.isEmpty()) {
            errorMessage.appendWithNewLine("Text is missing.");
        }
        
    }
    
    if (m_annotationType == AnnotationTypeEnum::IMAGE) {
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
    
    bool valid = false;
    m_coordinateSelectionWidget->getSelectedCoordinateSpace(valid);
    if ( ! valid) {
        const QString msg("A coordinate space has not been selected.");
        WuQMessageBox::errorOk(this,
                               msg);
        return;
    }
    CaretAssert(m_annotationFile);
    
    CaretPointer<Annotation> annotation;
    annotation.grabNew(NULL);
    
    annotation.grabNew(Annotation::newAnnotationOfType(m_annotationType,
                                                       AnnotationAttributesDefaultTypeEnum::USER));
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        AnnotationText* text = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::USER);
        CaretAssert(text);
        text->setText(userText);
        annotation.grabNew(text);
    }
    AnnotationImage* annImage = dynamic_cast<AnnotationImage*>(annotation.getPointer());
    if (annImage != NULL) {
        annImage->setImageBytesRGBA(&m_imageRgbaBytes[0],
                                    m_imageWidth,
                                    m_imageHeight);
    }

    if ( ! m_coordinateSelectionWidget->setCoordinateForNewAnnotation(annotation,
                                                                      errorMessage)) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    /*
     * Need to release annotation from its CaretPointer since the
     * annotation file will take ownership of the annotation.
     */
    Annotation* annotationPointer = annotation.releasePointer();
    
    if (m_mode == MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS) {
        AnnotationCoordinateInformation::setAnnotationFromBoundsWidthAndHeight(annotationPointer,
                                                                               m_mouseEvent,
                                                                               m_annotationFromBoundsWidth,
                                                                               m_annotationFromBoundsHeight);
    }
    
    finishAnnotationCreation(m_annotationFile,
                             annotationPointer,
                             m_mouseEvent.getBrowserWindowIndex());

    m_annotationThatWasCreated = annotationPointer;
    
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
 */
void
AnnotationCreateDialog::finishAnnotationCreation(AnnotationFile* annotationFile,
                                                 Annotation* annotation,
                                                 const int32_t browswerWindowIndex)
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    
    /*
     * Add annotation to its file
     */
    AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
    undoCommand->setModeCreateAnnotation(annotationFile,
                                         annotation);
    annotationManager->applyCommand(undoCommand);
    
    
    annotationManager->selectAnnotation(browswerWindowIndex,
                                        AnnotationManager::SELECTION_MODE_SINGLE,
                                        false,
                                        annotation);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
}

