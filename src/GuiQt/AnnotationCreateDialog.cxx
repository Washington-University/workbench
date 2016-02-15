
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
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
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
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
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
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "UserInputModeAnnotations.h"
#include "WuQtUtilities.h"
#include "WuQMessageBox.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCreateDialog 
 * \brief Dialog used for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Get a dialog for creating a new annotation using an annotation space and type.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 */
Annotation*
AnnotationCreateDialog::newAnnotationFromSpaceAndType(const MouseEvent& mouseEvent,
                                                      const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                      const AnnotationTypeEnum::Enum annotationType)
{
    AnnotationCoordinateInformation coordInfo;
    AnnotationCoordinateInformation::getValidCoordinateSpacesFromXY(mouseEvent,
                                                                    coordInfo);
    
    const bool needCoordinateSelectionFlag = ( ! coordInfo.isCoordinateSpaceValid(annotationSpace));

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

    if (needCoordinateSelectionFlag
        || needImageOrTextFlag) {
        
    }
    return annotationOut.getPointer();
}

/**
 * Get a dialog for creating a new annotation using an annotation space and type.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newAnnotationSpaceAndType(const MouseEvent& mouseEvent,
                                                  const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                  const AnnotationTypeEnum::Enum annotationType,
                                                  QWidget* parent)
{
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_NEW_ANNOTATION_TYPE_CLICK,
                                                                mouseEvent,
                                                                NULL,
                                                                NULL,
                                                                annotationSpace,
                                                                annotationType,
                                                                parent);
    return dialog;
}

/**
 * Get a dialog for creating a new annotation using an annotation type.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param annotationSpace
 *      Space of annotation being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newAnnotationSpaceAndTypeWithBounds(const MouseEvent& mouseEvent,
                                                            const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                            const AnnotationTypeEnum::Enum annotationType,
                                                            QWidget* parent)
{
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS,
                                                                mouseEvent,
                                                                NULL,
                                                                NULL,
                                                                annotationSpace,
                                                                annotationType,
                                                                parent);
    return dialog;
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
 * @param annotation
 *     Annotation that is copied.
 * @param annotationSpace
 *      Space of annotation that is being created.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog::AnnotationCreateDialog(const Mode mode,
                                               const MouseEvent& mouseEvent,
                                               const AnnotationFile* annotationFile,
                                               const Annotation* annotation,
                                               const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                               const AnnotationTypeEnum::Enum annotationType,
                                               QWidget* parent)
: WuQDialogModal("Create Annotation",
                 parent),
m_mode(mode),
m_mouseEvent(mouseEvent),
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
    
    m_fileSelectionWidget = createFileSelectionWidget();
    m_coordinateSelectionWidget = new AnnotationCoordinateSelectionWidget(m_annotationType,
                                                                          m_coordInfo,
                                                                          (secondCoordValidFlag
                                                                           ? &m_coordTwoInfo
                                                                           : NULL));
    QGroupBox* coordGroupBox = new QGroupBox("Coordinate Space");
    QVBoxLayout* coordGroupLayout = new QVBoxLayout(coordGroupBox);
    coordGroupLayout->setMargin(0);
    coordGroupLayout->addWidget(m_coordinateSelectionWidget);
    
    switch (m_mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
            setWindowTitle("New Annotation");
            m_coordinateSelectionWidget->selectCoordinateSpace(m_annotationSpace);
            break;
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
            setWindowTitle("New Annotation");
            m_coordinateSelectionWidget->selectCoordinateSpace(m_annotationSpace);
            break;
    }
    
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
                           : NULL);
    
    QWidget* imageWidget = ((m_annotationType == AnnotationTypeEnum::IMAGE)
                            ? createImageWidget()
                            : NULL);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dialogWidget);
    if (m_fileSelectionWidget != NULL) {
        layout->addWidget(m_fileSelectionWidget);
    }
    layout->addWidget(coordGroupBox);
    if (textWidget != NULL) {
        layout->addWidget(textWidget);
    }
    if (imageWidget != NULL) {
        layout->addWidget(imageWidget);
    }
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textEdit);
        m_textEdit->setFocus();
        
        switch (m_mode) {
            case MODE_NEW_ANNOTATION_TYPE_CLICK:
                break;
            case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
                break;
        }
    }    
}

/**
 * Destructor.
 */
AnnotationCreateDialog::~AnnotationCreateDialog()
{
    delete m_annotationFileSelectionModel;
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
 * @return New instance of the file selection widget.
 */
QWidget*
AnnotationCreateDialog::createFileSelectionWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    m_annotationFileSelectionModel = CaretDataFileSelectionModel::newInstanceForCaretDataFileType(brain,
                                                                                                  DataFileTypeEnum::ANNOTATION);
    m_annotationFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
    
    QAction* newFileAction = WuQtUtilities::createAction("New",
                                                         "Create a new annotation file",
                                                         this,
                                                         this,
                                                         SLOT(newAnnotationFileButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    
    m_sceneAnnotationFileRadioButton = new QRadioButton("Scene Annotation File");
    m_brainAnnotationFileRadioButton = new QRadioButton("File");
    
    QButtonGroup* fileButtonGroup = new QButtonGroup(this);
    fileButtonGroup->addButton(m_brainAnnotationFileRadioButton);
    fileButtonGroup->addButton(m_sceneAnnotationFileRadioButton);
    
    AnnotationFile* annotationFile = NULL;
    switch (m_mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
            if (s_previousSelections.m_valid) {
                annotationFile = s_previousSelections.m_annotationFile;
            }
            break;
    }
    if (annotationFile != NULL) {
        /*
         * Default using previous selections
         */
        if (annotationFile == brain->getSceneAnnotationFile()) {
            m_sceneAnnotationFileRadioButton->setChecked(true);
        }
        else {
            m_annotationFileSelectionModel->setSelectedFile(annotationFile);
            m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
            if (m_annotationFileSelectionModel->getSelectedFile() == annotationFile) {
                m_brainAnnotationFileRadioButton->setChecked(true);
            }
        }
    }
    else {
        /*
         * First time displayed, default to scene annotation file
         */
        m_sceneAnnotationFileRadioButton->setChecked(true);
    }
    
    QWidget* widget = new QGroupBox("Annotation File");
    QGridLayout* layout = new QGridLayout(widget);
    layout->setColumnStretch(0,   0);
    layout->setColumnStretch(1, 100);
    layout->setColumnStretch(2,   0);
    layout->addWidget(m_brainAnnotationFileRadioButton,
                      0, 0);
    layout->addWidget(m_annotationFileSelectionComboBox->getWidget(),
                      0, 1);
    layout->addWidget(newFileToolButton,
                      0, 2);
    layout->addWidget(m_sceneAnnotationFileRadioButton,
                      1, 0,
                      1, 3,
                      Qt::AlignLeft);
    
    return widget;
}

/**
 * Gets called when "New" file button is clicked.
 */
void
AnnotationCreateDialog::newAnnotationFileButtonClicked()
{
    /*
     * Let user choose a different path/name
     */
    AnnotationFile* newFile = new AnnotationFile();
    AString newFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::ANNOTATION,
                                                                       this,
                                                                       "Choose Annotation File Name",
                                                                       newFile->getFileName());
    /*
     * If user cancels, delete the new border file and return
     */
    if (newFileName.isEmpty()) {
        delete newFile;
        return;
    }
    
    /*
     * Set name of new border file, add file to brain, and make
     * file the selected annotation file.
     */
    newFile->setFileName(newFileName);
    EventManager::get()->sendEvent(EventDataFileAdd(newFile).getPointer());
    m_annotationFileSelectionModel->setSelectedFile(newFile);
    m_annotationFileSelectionComboBox->updateComboBox(m_annotationFileSelectionModel);
    m_brainAnnotationFileRadioButton->setChecked(true);
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
    
    QGroupBox* groupBox = new QGroupBox("Image File");
    QHBoxLayout* layout = new QHBoxLayout(groupBox);
    layout->addWidget(newFileToolButton);
    layout->addWidget(m_imageFileNameLabel);
    
    return groupBox;
}

void
AnnotationCreateDialog::invalidateImage()
{
    m_imageRgbaBytes.clear();
    m_imageWidth  = 0;
    m_imageHeight = 0;
    m_imageFileNameLabel->setText("");
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
    
    AnnotationFile* annotationFile = NULL;
    if (m_brainAnnotationFileRadioButton->isChecked()) {
        annotationFile = m_annotationFileSelectionModel->getSelectedFileOfType<AnnotationFile>();
        if (annotationFile == NULL) {
            errorMessage.appendWithNewLine("An annotation file must be selected.");
        }
    }
    else if (m_sceneAnnotationFileRadioButton->isChecked()) {
        annotationFile = GuiManager::get()->getBrain()->getSceneAnnotationFile();
    }
    else {
        errorMessage.appendWithNewLine("Type of anotation file is not selected.");
    }
    
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
    const AnnotationCoordinateSpaceEnum::Enum space = m_coordinateSelectionWidget->getSelectedCoordinateSpace(valid);
    if ( ! valid) {
        const QString msg("A coordinate space has not been selected.");
        WuQMessageBox::errorOk(this,
                               msg);
        return;
    }
    CaretAssert(annotationFile);
    
    /*
     * Save for next time
     */
    s_previousSelections.m_annotationFile  = annotationFile;
    s_previousSelections.m_coordinateSpace = space;
    s_previousSelections.m_valid = true;
    
    CaretPointer<Annotation> annotation;
    annotation.grabNew(NULL);
    
    switch (m_mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
        {
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
        }
            break;
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
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    
    /*
     * Add annotation to its file
     */
    switch (m_mode) {
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
        case MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS:
        {
            if (m_mode == MODE_NEW_ANNOTATION_TYPE_FROM_BOUNDS) {
                AnnotationCoordinateInformation::setAnnotationFromBoundsWidthAndHeight(annotationPointer,
                                                                                       m_mouseEvent,
                                                                                       m_annotationFromBoundsWidth,
                                                                                       m_annotationFromBoundsHeight);
            }
            
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            undoCommand->setModeCreateAnnotation(annotationFile,
                                                 annotationPointer);
            annotationManager->applyCommand(undoCommand);
        }
            break;
    }
    

    annotationManager->selectAnnotation(m_mouseEvent.getBrowserWindowIndex(),
                                        AnnotationManager::SELECTION_MODE_SINGLE,
                                        false,
                                        annotationPointer);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    m_annotationThatWasCreated = annotationPointer;
    
    WuQDialog::okButtonClicked();
}

