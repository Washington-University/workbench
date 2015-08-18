
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
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "AnnotationFile.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretFileDialog.h"
#include "CaretPointer.h"
#include "EventDataFileAdd.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
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
 * Get a dialog for creating a new annotation using an annotation.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotation
 *      Annotation that will be copied and added.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newAnnotation(const MouseEvent& mouseEvent,
                                      const Annotation* annotation,
                                      QWidget* parent)
{
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_ADD_NEW_ANNOTATION,
                                                                mouseEvent,
                                                                NULL,
                                                                annotation,
                                                                annotation->getType(),
                                                                parent);
    return dialog;
}

/**
 * Get a dialog for creating a new annotation using an annotation type.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newAnnotationType(const MouseEvent& mouseEvent,
                                                 const AnnotationTypeEnum::Enum annotationType,
                                                 QWidget* parent)
{
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_NEW_ANNOTATION_TYPE_CLICK,
                                                                mouseEvent,
                                                                NULL,
                                                                NULL,
                                                                annotationType,
                                                                parent);
    return dialog;
}

/**
 * Get a dialog for creating a new annotation using an annotation type.
 *
 * @param mouseEvent
 *     The mouse event.
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newAnnotationTypeWithBounds(const MouseEvent& mouseEvent,
                                          const AnnotationTypeEnum::Enum annotationType,
                                          QWidget* parent)
{
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE,
                                                                mouseEvent,
                                                                NULL,
                                                                NULL,
                                                                annotationType,
                                                                parent);
    return dialog;
}

/**
 * Get a dialog for pasting an annotation.
 *
 * @param mouseEvent
 *     The mouse event indicating where user clicked in the window
 * @param annotationFile
 *     File containing the annotation that is pasted (may be NULL).
 * @param annotation
 *     Annotation that is copied.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog*
AnnotationCreateDialog::newPasteAnnotation(const MouseEvent& mouseEvent,
                                                  const AnnotationFile* annotationFile,
                                                  const Annotation* annotation,
                                                  QWidget* parent)
{
    CaretAssert(annotation);
    AnnotationCreateDialog* dialog = new AnnotationCreateDialog(MODE_PASTE_ANNOTATION,
                                                                mouseEvent,
                                                                annotationFile,
                                                                annotation,
                                                                annotation->getType(),
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
 * @param annotationType
 *      Type of annotation that is being created.
 * @param parent
 *      Optional parent for this dialog.
 */
AnnotationCreateDialog::AnnotationCreateDialog(const Mode mode,
                                               const MouseEvent& mouseEvent,
                                               const AnnotationFile* annotationFile,
                                               const Annotation* annotation,
                                               const AnnotationTypeEnum::Enum annotationType,
                                               QWidget* parent)
: WuQDialogModal("Create Annotation",
                 parent),
m_mode(mode),
m_mouseEvent(mouseEvent),
m_annotationToPastesFile(annotationFile),
m_annotationToPaste(annotation),
m_annotationType(annotationType)
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
        case MODE_ADD_NEW_ANNOTATION:
            if (m_annotationToPaste->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW) {
                const AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(m_annotationToPaste);
                const AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<const AnnotationTwoDimensionalShape*>(m_annotationToPaste);
                
                int windowViewport[4];
                mouseEvent.getViewportContent()->getWindowViewport(windowViewport);
                
                if (oneDimShape != NULL) {
                    /*
                     * Need to convert window relative coordinate to absolute coordinate
                     */
                    float xyz[3];
                    oneDimShape->getStartCoordinate()->getXYZ(xyz);
                    windowX = xyz[0] * windowViewport[2];
                    windowY = xyz[1] * windowViewport[3];
                    oneDimShape->getEndCoordinate()->getXYZ(xyz);
                    windowTwoX = xyz[0] * windowViewport[2];
                    windowTwoY = xyz[1] * windowViewport[3];
                }
                else if (twoDimShape != NULL) {
                    /*
                     * Need to convert window relative coordinate to absolute coordinate
                     */
                    float xyz[3];
                    twoDimShape->getCoordinate()->getXYZ(xyz);
                    windowX = xyz[0] * windowViewport[2];
                    windowY = xyz[1] * windowViewport[3];
                }
            }
            break;
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
            break;
        case MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE:
            windowX = mouseEvent.getPressedX();
            windowY = mouseEvent.getPressedY();
            windowTwoX = mouseEvent.getX();
            windowTwoY = mouseEvent.getY();
            break;
        case MODE_PASTE_ANNOTATION:
            break;
    }
    
    UserInputModeAnnotations::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                             mouseEvent.getViewportContent(),
                                                             windowX,
                                                             windowY,
                                                             m_coordInfo);
    
    bool secondCoordValidFlag = false;
    if ((windowTwoX >= 0)
        && (windowTwoY >= 0)) {
        UserInputModeAnnotations::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
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
        case MODE_ADD_NEW_ANNOTATION:
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
            setWindowTitle("New Annotation");
            if (s_previousSelections.m_valid) {
                m_coordinateSelectionWidget->selectCoordinateSpace(s_previousSelections.m_coordinateSpace);
            }
            break;
        case MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE:
            setWindowTitle("New Annotation");
            if (s_previousSelections.m_valid) {
                m_coordinateSelectionWidget->selectCoordinateSpace(s_previousSelections.m_coordinateSpace);
            }
            break;
        case MODE_PASTE_ANNOTATION:
            setWindowTitle("Paste Annotation");
            m_coordinateSelectionWidget->selectCoordinateSpace(m_annotationToPaste->getCoordinateSpace());
            break;
    }
    
    QWidget* textWidget = ((m_annotationType == AnnotationTypeEnum::TEXT)
                           ? createTextWidget()
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
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_NEVER);
    
    if (m_annotationType == AnnotationTypeEnum::TEXT) {
        CaretAssert(m_textEdit);
        m_textEdit->setFocus();
        
        switch (m_mode) {
            case MODE_ADD_NEW_ANNOTATION:
                break;
            case MODE_NEW_ANNOTATION_TYPE_CLICK:
                break;
            case MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE:
                break;
            case MODE_PASTE_ANNOTATION:
            {
                const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(m_annotationToPaste);
                if (textAnn != NULL) {
                    m_textEdit->setText(textAnn->getText());
                    m_textEdit->selectAll();
                }
            }
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
        case MODE_ADD_NEW_ANNOTATION:
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
        case MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE:
            if (s_previousSelections.m_valid) {
                annotationFile = s_previousSelections.m_annotationFile;
            }
            break;
        case MODE_PASTE_ANNOTATION:
            annotationFile = const_cast<AnnotationFile*>(m_annotationToPastesFile);
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
            //WuQMessageBox::errorOk(this, "An annotation file must be selected.");
            //return;
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
        userText = m_textEdit->toPlainText().trimmed();
        if (userText.isEmpty()) {
            errorMessage.appendWithNewLine("Text is missing.");
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
        case MODE_ADD_NEW_ANNOTATION:
        {
            Annotation* annCopy = m_annotationToPaste->clone();
            AnnotationText* annText = dynamic_cast<AnnotationText*>(annCopy);
            if (annText != NULL) {
                annText->setText(userText);
            }
            annotation.grabNew(annCopy);
        }
            break;
        case MODE_NEW_ANNOTATION_TYPE_CLICK:
        case MODE_NEW_ANNOTATION_TYPE_PRESS_AND_RELEASE:
            annotation.grabNew(Annotation::newAnnotationOfType(m_annotationType));
            if (m_annotationType == AnnotationTypeEnum::TEXT) {
                AnnotationText* text = new AnnotationText();
                text->setText(userText);
                annotation.grabNew(text);
            }
            break;
        case MODE_PASTE_ANNOTATION:
        {
            Annotation* annCopy = m_annotationToPaste->clone();
            AnnotationText* annText = dynamic_cast<AnnotationText*>(annCopy);
            if (annText != NULL) {
                annText->setText(userText);
            }
            annotation.grabNew(annCopy);
        }
            break;
    }

    if ( ! m_coordinateSelectionWidget->setCoordinateForNewAnnotation(annotation,
                                                                      errorMessage)) {
        WuQMessageBox::errorOk(this, errorMessage);
        return;
    }
    
    if (annotation->getType() == AnnotationTypeEnum::TEXT) {
        AnnotationText* annText = dynamic_cast<AnnotationText*>(annotation.getPointer());
        
        if (annText != NULL) {
            BrainOpenGLViewportContent* viewportContent = m_mouseEvent.getViewportContent();
            CaretAssert(viewportContent);
            int viewport[4] = { -1, -1, -1, -1 };
            switch (annotation->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::MODEL:
                    viewportContent->getModelViewport(viewport);
                    break;
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    viewportContent->getModelViewport(viewport);
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    viewportContent->getModelViewport(viewport);
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    viewportContent->getWindowViewport(viewport);
                    break;
            }
            const float viewportHeight = viewport[3];
            annText->setViewportHeightWhenCreated(viewportHeight);
        }
    }
    
    /*
     * Need to release annotation from its CaretPointer since the
     * annotation file will take ownership of the annotation.
     */
    Annotation* annotationPointer = annotation.releasePointer();
    
    annotationFile->addAnnotation(annotationPointer);

    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    annotationManager->selectAnnotation(AnnotationManager::SELECTION_MODE_SINGLE,
                                        false,
                                        annotationPointer);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    
    WuQDialog::okButtonClicked();
}

